#!/usr/bin/env python3
"""Reproduce the original ICCD experiment: both encryption modes and attack CEC."""
from pathlib import Path
import argparse
import csv
import datetime
import hashlib
import json
import math
import platform
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from revc_lock.runner import operate, binary_paths
from revc_lock.real import read_real


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--suite', choices=['table1', 'all'], default='table1')
    p.add_argument('--benchmark-dir', type=Path, default=ROOT / 'data/benchmarks')
    p.add_argument('--output', type=Path, default=ROOT / 'outputs/iccd-table1')
    p.add_argument('--build-dir', type=Path, default=ROOT / 'build')
    p.add_argument('--timeout', type=float, default=120)
    p.add_argument('--resume', action='store_true', help='Resume verified results in an existing output directory')
    args = p.parse_args()
    expected = json.loads((ROOT / 'experiments/iccd/expected_all.json').read_text())
    paper = json.loads((ROOT / 'experiments/iccd/table1.json').read_text())
    by_name = {r['benchmark']: r for r in expected}
    for row in paper:
        baseline = by_name[row['benchmark']]
        for key in ['original_inputs', 'original_gates', 'locked_inputs', 'locked_gates', 'defense_inputs', 'defense_gates']:
            if baseline[key] != row[key]: p.error('Published Table I and baseline disagree: ' + row['benchmark'])
        if math.ceil(10000.0 * (row['defense_gates'] - row['locked_gates']) / row['locked_gates']) / 100.0 != row['overhead_percent']:
            p.error('Published overhead baseline is inconsistent')
    selected = [by_name[r['benchmark']] for r in paper] if args.suite == 'table1' else expected
    manifests = {r['benchmark']: r for r in json.loads((ROOT / 'experiments/iccd/benchmarks.json').read_text())}
    for e in selected:
        source = args.benchmark_dir / (e['benchmark'] + '.real')
        if not source.is_file():
            p.error('Missing {}; run python3 scripts/fetch_benchmarks.py'.format(source))
        if hashlib.sha256(source.read_bytes()).hexdigest() != manifests[e['benchmark']]['sha256']:
            p.error('Benchmark bytes differ from the archived baseline: ' + str(source))
    if args.output.exists() and not args.resume: p.error('Use a fresh --output directory or --resume: ' + str(args.output))
    args.output.mkdir(parents=True, exist_ok=True)
    progress = args.output / 'progress.json'
    rows = json.loads(progress.read_text()) if args.resume and progress.exists() else []
    cached = {r['benchmark']: r for r in rows}
    if any(name not in {e['benchmark'] for e in selected} for name in cached):
        p.error('Existing output belongs to a different suite')
    for index, e in enumerate(selected, 1):
        name = e['benchmark']; source = args.benchmark_dir / (name + '.real')
        if name in cached:
            if not cached[name]['passed']: p.error('Cannot resume a failed case: ' + name)
            for label in ['locked', 'defense']:
                artifact = args.output / label / (name + '.real')
                if not artifact.is_file() or hashlib.sha256(artifact.read_bytes()).hexdigest() != e[label + '_sha256']:
                    p.error('Resume artifact missing or changed: ' + str(artifact))
            continue
        counts = read_real(source)
        result = dict(benchmark=name, original_inputs=counts['inputs'], original_gates=counts['gates'])
        result['original_match'] = counts['inputs'] == e['original_inputs'] and counts['gates'] == e['original_gates']
        for operation, label in [('lock', 'locked'), ('defend', 'defense')]:
            artifact = args.output / label / (name + '.real')
            report = operate(operation, source, artifact, build_dir=args.build_dir,
                             timeout=args.timeout, log_dir=args.output / 'logs' / name / operation)
            attack = operate('attack', artifact, args.output / (label + '_recovered') / (name + '.real'),
                             reference=source, build_dir=args.build_dir, timeout=args.timeout,
                             log_dir=args.output / 'logs' / name / (operation + '_attack'))
            result[label + '_inputs'] = report['inputs']; result[label + '_gates'] = report['gates']
            result[label + '_sha256'] = report['sha256']
            result[label + '_encryption_ms'] = report['encryption_seconds'] * 1000
            result[label + '_attack_ms'] = attack['attack_seconds'] * 1000
            result[label + '_equivalence'] = attack['equivalence']
            result[label + '_counts_match'] = report['inputs'] == e[label + '_inputs'] and report['gates'] == e[label + '_gates']
            result[label + '_bytes_match'] = report['sha256'] == e[label + '_sha256']
        result['overhead_percent'] = math.ceil(10000.0 * (result['defense_gates'] - result['locked_gates']) / result['locked_gates']) / 100.0
        result['passed'] = (result['original_match'] and result['locked_counts_match'] and result['defense_counts_match']
                            and result['locked_bytes_match'] and result['defense_bytes_match']
                            and result['locked_equivalence'] == e['old_equivalence']
                            and result['defense_equivalence'] == e['defense_equivalence'])
        rows.append(result)
        progress.write_text(json.dumps(rows, indent=2))
        with (args.output / 'comparison.csv').open('w', newline='') as f:
            w = csv.DictWriter(f, fieldnames=list(rows[0])); w.writeheader(); w.writerows(rows)
        print('{}/{} {}: {} (lock {}, defense {}, CEC {}/{})'.format(index, len(selected), name,
              'PASS' if result['passed'] else 'FAIL', result['locked_gates'], result['defense_gates'],
              result['locked_equivalence'], result['defense_equivalence']), flush=True)
    native, solver = binary_paths(args.build_dir)
    summary = dict(suite=args.suite, cases=len(rows), passed=sum(r['passed'] for r in rows),
        all_passed=all(r['passed'] for r in rows), python=platform.python_version(),
        platform=platform.platform(), completed_at=datetime.datetime.now().astimezone().isoformat(),
        native_sha256=hashlib.sha256(native.read_bytes()).hexdigest(),
        minisat_sha256=hashlib.sha256(solver.read_bytes()).hexdigest(),
        timing_note='Fresh CPU times are hardware dependent; timing equality is not an acceptance criterion.')
    (args.output / 'summary.json').write_text(json.dumps(summary, indent=2))
    print(json.dumps(summary, indent=2))
    return 0 if summary['all_passed'] else 1


if __name__ == '__main__':
    try: raise SystemExit(main())
    except (ValueError, OSError) as e:
        print('Reproduction stopped: {}. Completed results are retained; inspect logs or resume after fixing the cause.'.format(e), file=sys.stderr)
        raise SystemExit(2)
