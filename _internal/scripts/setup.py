#!/usr/bin/env python3
"""Fetch pinned source dependencies and build the two small native executables."""
from pathlib import Path
import argparse
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--cache-root', type=Path, help='Optional CirKit source checkout for offline setup')
    p.add_argument('--build-dir', type=Path, default=ROOT / 'build')
    p.add_argument('--jobs', type=int, default=2)
    args = p.parse_args()
    if args.jobs < 1: p.error('--jobs must be positive')
    for command in ['cmake', 'g++', 'git']:
        if not shutil.which(command): p.error('Missing {}. See README.md system packages.'.format(command))
    fetch = [sys.executable, str(ROOT / 'scripts/fetch_dependencies.py')]
    if args.cache_root: fetch += ['--cache-root', str(args.cache_root.resolve())]
    commands = [fetch, ['cmake', '-S', str(ROOT), '-B', str(args.build_dir.resolve()), '-DCMAKE_BUILD_TYPE=Release'],
                ['cmake', '--build', str(args.build_dir.resolve()), '--parallel', str(args.jobs)]]
    for command in commands:
        result = subprocess.run(command)
        if result.returncode: return result.returncode
    result = subprocess.run(['ctest', '--output-on-failure'], cwd=str(args.build_dir.resolve()))
    if result.returncode: return result.returncode
    print('Ready. Try: python3 revc-tool lock examples/demo.real outputs/demo_locked.real')
    return 0


if __name__ == '__main__': raise SystemExit(main())
