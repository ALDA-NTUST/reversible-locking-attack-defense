cmake_minimum_required(VERSION 3.16)
project(legacy_revc_lock VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
if(NOT UNIX OR APPLE OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR "This legacy reproduction currently supports 64-bit Linux.")
endif()
set(DEPS "${CMAKE_CURRENT_SOURCE_DIR}/.deps" CACHE PATH "Pinned dependency source directory")
foreach(name cirkit abc kitty minisat cudd fmt)
  if(NOT IS_DIRECTORY "${DEPS}/${name}")
    message(FATAL_ERROR "Missing ${name}. Run: python3 scripts/fetch_dependencies.py")
  endif()
endforeach()
set(Boost_NO_BOOST_CMAKE ON)
if(NOT DEFINED BOOST_ROOT AND EXISTS "/usr/include/boost/version.hpp")
  set(BOOST_ROOT /usr)
  set(BOOST_INCLUDEDIR /usr/include)
  set(BOOST_LIBRARYDIR "/usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}")
  set(Boost_NO_SYSTEM_PATHS ON)
endif()
find_package(Boost 1.65 REQUIRED COMPONENTS filesystem system)
find_package(ZLIB REQUIRED)
find_package(Python3 3.6 REQUIRED COMPONENTS Interpreter)
# Prevent unrelated /usr/local Boost headers from shadowing the selected version.
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/selected-include")
file(CREATE_LINK "${Boost_INCLUDE_DIRS}/boost" "${CMAKE_BINARY_DIR}/selected-include/boost" SYMBOLIC)
set(CK "${DEPS}/cirkit")
set(REV "${CK}/addons/cirkit-addon-reversible/src")
set(CORE_SOURCES
  core/properties.cpp core/cube.cpp core/version.cpp core/io/pla_parser.cpp
  core/utils/bitset_utils.cpp core/utils/conversion_utils.cpp core/utils/string_utils.cpp
  classical/utils/truth_table_utils.cpp classical/utils/expression_parser.cpp)
list(TRANSFORM CORE_SOURCES PREPEND "${CK}/src/")
set(REV_SOURCES
  circuit.cpp gate.cpp variable.cpp target_tags.cpp meta/bus_collection.cpp
  functions/add_gates.cpp functions/add_circuit.cpp functions/add_line_to_circuit.cpp
  functions/clear_circuit.cpp functions/copy_circuit.cpp functions/copy_metadata.cpp
  io/read_realization.cpp io/write_realization.cpp io/revlib_parser.cpp
  io/revlib_processor.cpp io/print_circuit.cpp)
list(TRANSFORM REV_SOURCES PREPEND "${REV}/reversible/")
set(EXOR_SOURCES exor.c exorBits.c exorCubes.c exorLink.c exorList.c exorUtil.c)
list(TRANSFORM EXOR_SOURCES PREPEND "${DEPS}/abc/src/base/exor/")
set_source_files_properties(${EXOR_SOURCES} PROPERTIES LANGUAGE CXX)
add_executable(revc-native src/native_main.cpp third_party_adapters/cirkit/legacy_esop.cpp
  ${CORE_SOURCES} ${REV_SOURCES} ${EXOR_SOURCES})
target_include_directories(revc-native BEFORE PRIVATE
  "${CMAKE_BINARY_DIR}/selected-include" "${CMAKE_SOURCE_DIR}/third_party_adapters/cirkit" "${CK}/src" "${REV}"
  "${DEPS}/abc/src" "${DEPS}/kitty/include" "${DEPS}/fmt"
  "${DEPS}/cudd/cudd" "${DEPS}/cudd/cplusplus" "${DEPS}/cudd/mtr")
target_compile_definitions(revc-native PRIVATE ABC_NAMESPACE=abc LIN64
  SIZEOF_VOID_P=8 SIZEOF_LONG=8 SIZEOF_INT=4 BOOST_RESULT_OF_USE_DECLTYPE FMT_HEADER_ONLY=1)
target_compile_options(revc-native PRIVATE -ffunction-sections -fdata-sections -fpermissive -w)
target_link_options(revc-native PRIVATE -Wl,--gc-sections)
target_link_libraries(revc-native PRIVATE Boost::filesystem Boost::system)

# Build only the MIT-licensed MiniSat solver sources; no upstream build tree needed.
add_executable(minisat
  "${DEPS}/minisat/minisat/utils/Options.cc"
  "${DEPS}/minisat/minisat/utils/System.cc"
  "${DEPS}/minisat/minisat/core/Solver.cc"
  "${DEPS}/minisat/minisat/simp/SimpSolver.cc"
  "${DEPS}/minisat/minisat/simp/Main.cc")
target_include_directories(minisat PRIVATE "${DEPS}/minisat")
target_compile_definitions(minisat PRIVATE __STDC_FORMAT_MACROS __STDC_LIMIT_MACROS)
target_compile_options(minisat PRIVATE -fpermissive -w)
target_link_libraries(minisat PRIVATE ZLIB::ZLIB)

enable_testing()
add_test(NAME cli_smoke COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tests/test_cli.py"
  --build-dir "${CMAKE_BINARY_DIR}")
add_test(NAME benchmark_import COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tests/test_fetch.py")
