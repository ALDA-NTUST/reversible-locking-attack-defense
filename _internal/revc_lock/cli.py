from pathlib import Path
import argparse
import json
import platform
import sys

from . import __version__
from .runner import ROOT, binary_paths, operate


def main(argv=None):
    parser = argparse.ArgumentParser(description='ICCD legacy RevC-Lock, attack and defense tools.')
    parser.add_argument('--version', action='version', version=__version__)
    commands = parser.add_subparsers(dest='command')
    for name in ['lock', 'defend', 'attack']:
        p = commands.add_parser(name)
        p.add_argument('input', type=Path)
        p.add_argument('output', type=Path)
        if name == 'attack': p.add_argument('--reference', type=Path, help='Optional original circuit for equivalence verification')
        p.add_argument('--force', action='store_true')
        _common(p)
    p = commands.add_parser('verify')
    p.add_argument('input', type=Path)
    p.add_argument('reference', type=Path)
    _common(p)
    p = commands.add_parser('doctor', help='Check the local runtime')
    p.add_argument('--build-dir', type=Path)
    p.add_argument('--json', action='store_true')
    args = parser.parse_args(argv)
    if not args.command:
        parser.print_help(); return 2
    try:
        if args.command == 'doctor':
            native, solver = binary_paths(args.build_dir)
            result = dict(python=platform.python_version(), platform=platform.platform(),
                          native=str(native), native_found=native.is_file(),
                          minisat=str(solver), minisat_found=solver.is_file())
            status = 0 if result['native_found'] and result['minisat_found'] else 1
        else:
            result = operate(args.command, args.input, getattr(args, 'output', None),
                getattr(args, 'reference', None), args.build_dir, args.timeout,
                getattr(args, 'force', False), args.log_dir)
            status = 1 if args.command == 'verify' and result['equivalence'] != 'equivalent' else 0
        print(json.dumps(result, indent=2) if args.json else '\n'.join('{}: {}'.format(k, v) for k, v in result.items()))
        return status
    except (ValueError, OSError) as e:
        if args.json: print(json.dumps(dict(error=str(e))))
        else: print('Error: ' + str(e), file=sys.stderr)
        return 2


def _common(parser):
    parser.add_argument('--build-dir', type=Path, help='Native build directory (default: project/build)')
    parser.add_argument('--timeout', type=float, default=120)
    parser.add_argument('--json', action='store_true')
    parser.add_argument('--log-dir', type=Path)


if __name__ == '__main__': raise SystemExit(main())
