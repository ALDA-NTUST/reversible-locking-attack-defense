/* CirKit: A circuit toolkit
 * Copyright (C) 2009-2015  University of Bremen
 * Copyright (C) 2015-2017  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// Adapted from the pinned CirKit functions; see provenance/ADAPTATIONS.md.
#include "legacy_esop.hpp"
#include <core/io/pla_parser.hpp>
#include <core/utils/bitset_utils.hpp>
#include <reversible/functions/add_gates.hpp>
#include <reversible/functions/copy_metadata.hpp>
#include <reversible/target_tags.hpp>
#include <base/exor/exor.h>
#include <misc/vec/vecInt.h>
#include <misc/vec/vecWec.h>
namespace abc { int Abc_ExorcismMain(Vec_Wec_t*,int,int,char*,int,int,int,int); }
namespace cirkit {
class exorcism_processor : public pla_processor
{
public:
  exorcism_processor( const cube_function_t& on_cube_f ) : on_cube_f( on_cube_f ) {}

  void on_cube( const std::string& in, const std::string& out )
  {
    /* only one output functions are supported */
    assert( out.size() == 1u );

    unsigned n = in.size();
    boost::dynamic_bitset<> values( n );
    boost::dynamic_bitset<> care( n );

    for ( unsigned i = 0u; i < n; ++i )
    {
      values[i] = ( in[i] == '1' );
      care[i]   = ( in[i] != '-' );
    }

    cube_t cube = std::make_pair( values, care );

    if ( on_cube_f )
    {
      on_cube_f( cube );
    }

    ++_cube_count;
    _literal_count += care.count();
  }

  unsigned cube_count() const
  {
    return _cube_count;
  }

  unsigned literal_count() const
  {
    return _literal_count;
  }

private:
  const cube_function_t& on_cube_f;
  unsigned _cube_count = 0u;
  unsigned _literal_count = 0u;
};

void exorcism_minimization( const cube_vec_t& cubes, const properties::ptr& settings, const properties::ptr& statistics )
{
  const auto verbose      = get( settings, "verbose",      false );
  const auto on_cube      = get( settings, "on_cube",      cube_function_t() );
  const auto esopname     = get( settings, "esopname",     std::string( "exorcism.esop" ) );
  const auto skip_parsing = get( settings, "skip_parsing", false );



  if ( cubes.empty() )
  {
    return;
  }

  abc::Vec_Wec_t *esop = abc::Vec_WecAlloc( 0u );

  for ( const auto& cube : cubes )
  {
    auto * level = abc::Vec_WecPushLevel( esop );

    for ( auto i = 0u; i < cube.length(); ++i )
    {
      if ( !cube.care()[i] ) { continue; }

      abc::Vec_IntPush( level, ( i << 1u ) | !cube.bits()[i] );
    }
    abc::Vec_IntPush( level, -1 );
  }

  if ( verbose )
  {
    abc::Vec_WecPrint( esop, 0 );
  }

  /* redict STDOUT because of one output line in Abc_ExorcismMain */
  abc::Abc_ExorcismMain( esop, cubes.front().length(), 1, const_cast<char*>( esopname.c_str() ), 2, verbose, 20000, 0 );

  abc::Vec_WecFree( esop );

  /* Parse */
  if ( !skip_parsing )
  {
    exorcism_processor p( on_cube );
    pla_parser( esopname, p );

    set( statistics, "cube_count", p.cube_count() );
    set( statistics, "literal_count", p.literal_count() );
  }
}

std::pair<boost::dynamic_bitset<>, boost::dynamic_bitset<>> control_polarity_mask( const gate::control_container& controls, unsigned n )
{
  boost::dynamic_bitset<> cs( n ), ps( n );

  for ( const auto& c : controls )
  {
    assert( c.line() < n );

    cs[c.line()] = 1;
    ps[c.line()] = c.polarity();
  }

  return {cs, ps};
}

gate::control_container make_controls( const boost::dynamic_bitset<>& cs, const boost::dynamic_bitset<>& ps )
{
  gate::control_container controls;

  foreach_bit( cs, [&controls, &ps]( unsigned pos ) {
      controls.push_back( make_var( pos, ps[pos] ) );
    } );

  return controls;
}

void exorcism_merge_heuristic_add_cubes( circuit& circ, const cube_vec_t& cubes, int current_target )
{
  if ( cubes.empty() ) { return; }

  /* just one gate, directly add it */
  if ( cubes.size() == 1u )
  {
    append_toffoli( circ, make_controls( cubes.front().care(), cubes.front().bits() ), current_target );
    return;
  }

  /* we call exorcism */
  auto f = [&circ, current_target]( const cube_t& c ) {
    /* c.first is bits, c.second is care */
    append_toffoli( circ, make_controls( c.second, c.first ), current_target );
  };
  auto settings = std::make_shared<properties>();
  settings->set( "on_cube", cube_function_t( f ) );
  settings->set( "verbose", false );
  exorcism_minimization( cubes, settings );
}

circuit exorcism_merge_heuristic( const circuit& base )
{
  circuit circ;
  circ.set_lines( base.lines() );
  copy_metadata( base, circ );

  auto current_target = -1;
  cube_vec_t cubes;

  for ( const auto& gate : base )
  {
    /* works only for Toffoli gates for now */
    if ( !is_toffoli( gate ) )
    {
      exorcism_merge_heuristic_add_cubes( circ, cubes, current_target );
      cubes.clear();
      current_target = -1;
      circ.append_gate() = gate;
      continue;
    }

    const auto target = gate.targets().front();

    /* apply current cubes */
    if ( static_cast<int>( target ) != current_target )
    {
      exorcism_merge_heuristic_add_cubes( circ, cubes, current_target );
      cubes.clear();
      current_target = target;
    }

    /* add new cube */
    const auto mask = control_polarity_mask( gate.controls(), circ.lines() );
    cubes.push_back( cube( mask.second, mask.first ) );
  }

  /* leftovers? */
  exorcism_merge_heuristic_add_cubes( circ, cubes, current_target );

  return circ;
}


}
