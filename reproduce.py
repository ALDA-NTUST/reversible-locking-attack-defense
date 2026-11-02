#!/usr/bin/env python3
"""Fetch/import the paper inputs and reproduce the legacy ICCD experiment."""
from pathlib import Path
import argparse
import subprocess
import sys

root = Path(__file__).resolve().parent
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--from-archive', type=Path, help='Optional original research ZIP for exact offline benchmark import')
p.add_argument('--suite', choices=['table1', 'all'], default='table1')
p.add_argument('--benchmark-dir', type=Path, default=root / 'data/benchmarks')
p.add_argument('--output', type=Path, default=root / 'outputs/iccd-table1')
p.add_argument('--build-dir', type=Path, default=root / '_internal/build')
p.add_argument('--timeout', type=float, default=120)
p.add_argument('--resume', action='store_true')
a = p.parse_args()
fetch = [sys.executable, str(root / '_internal/scripts/fetch_benchmarks.py'),
         '--suite', a.suite, '--destination', str(a.benchmark_dir.resolve())]
if a.from_archive: fetch += ['--from-archive', str(a.from_archive.resolve())]
r = subprocess.call(fetch)
if r: raise SystemExit(r)
command = [sys.executable, str(root / '_internal/scripts/reproduce.py'),
           '--suite', a.suite, '--benchmark-dir', str(a.benchmark_dir.resolve()),
           '--output', str(a.output.resolve()), '--build-dir', str(a.build_dir.resolve()),
           '--timeout', str(a.timeout)]
if a.resume: command += ['--resume']
raise SystemExit(subprocess.call(command))
