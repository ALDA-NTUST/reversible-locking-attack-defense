#!/usr/bin/env python3
"""Check source-package structure, dependency records, and reproduction inputs."""
from pathlib import Path
import argparse
import json
import re

ROOT = Path(__file__).resolve().parents[1]
REPOSITORY = ROOT.parent
DEPENDENCIES = {'cirkit', 'abc', 'minisat', 'cudd', 'kitty', 'fmt'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--technical-only', action='store_true', help='Check structure without checking the project license text')
    args = parser.parse_args()
    problems = []
    required = [REPOSITORY / name for name in [
        'README.md', 'LICENSE', 'CITATION.cff', 'setup.py', 'revc-tool', 'reproduce.py',
        'examples/demo.real', 'docs/USAGE.md', 'docs/MAINTENANCE.md', 'docs/REPRODUCIBILITY.md']]
    required += [ROOT / name for name in [
        'AUTHORS.md', 'ADAPTATIONS.md', 'THIRD_PARTY_NOTICES.md', 'CMakeLists.txt',
        'dependencies.lock.json', 'dependencies.integrity.json',
        'tests/test_cli.py', 'tests/test_fetch.py']]
    required += [ROOT / 'licenses' / (name + '.txt') for name in [
        'abc', 'cirkit-mit', 'cudd', 'fmt', 'kitty', 'minisat']]
    for path in required:
        if not path.is_file() or not path.stat().st_size:
            problems.append('Missing or empty source-package file: ' + str(path.relative_to(REPOSITORY)))

    def load(relative):
        try:
            return json.loads((ROOT / relative).read_text())
        except (OSError, ValueError) as error:
            problems.append('Cannot read {}: {}'.format(relative, error))
            return None

    lock = load('dependencies.lock.json')
    integrity = load('dependencies.integrity.json')
    if not isinstance(lock, dict) or set(lock) != DEPENDENCIES:
        problems.append('Dependency lock does not contain the six expected source dependencies')
    elif any(not isinstance(info, dict) or not re.fullmatch(r'[0-9a-f]{40}', str(info.get('commit', '')))
             or not str(info.get('url', '')).startswith('https://github.com/') for info in lock.values()):
        problems.append('Dependency lock has an invalid revision or source URL')
    if not isinstance(integrity, dict) or set(integrity) != DEPENDENCIES:
        problems.append('Dependency source-integrity records are incomplete')
    else:
        for name, files in integrity.items():
            if not isinstance(files, dict) or not files:
                problems.append('Missing source-file hashes for ' + name)
                continue
            for relative, digest in files.items():
                if not re.fullmatch(r'[0-9a-f]{64}', str(digest)):
                    problems.append('Invalid dependency source hash: ' + name + '/' + relative)
                if 'puma' in relative.lower() or 'z3' in relative.lower():
                    problems.append('Excluded dependency in selected source files: ' + relative)

    def rows(relative, count):
        data = load(relative)
        if not isinstance(data, list) or len(data) != count or any(
                not isinstance(row, dict) or not isinstance(row.get('benchmark'), str) for row in data):
            problems.append('Expected {} benchmark records in {}'.format(count, relative))
            return {}
        indexed = {row['benchmark']: row for row in data}
        if len(indexed) != count:
            problems.append('Duplicate benchmark names in ' + relative)
        return indexed

    inputs = rows('experiments/iccd/benchmarks.json', 245)
    expected = rows('experiments/iccd/expected_all.json', 245)
    paper = rows('experiments/iccd/table1.json', 11)
    if set(inputs) != set(expected):
        problems.append('Input manifest and expected benchmark sets differ')
    if set(paper) != {name for name, row in inputs.items() if row.get('table1')}:
        problems.append('Table I selection differs from the input manifest')
    count_fields = ['original_inputs', 'original_gates', 'locked_inputs', 'locked_gates',
                    'defense_inputs', 'defense_gates']
    for name, row in inputs.items():
        if not re.fullmatch(r'[0-9a-f]{64}', str(row.get('sha256', ''))):
            problems.append('Missing input checksum: ' + name)
    for name, row in expected.items():
        if any(not isinstance(row.get(field), int) or row[field] < 0 for field in count_fields):
            problems.append('Missing or invalid expected circuit counts: ' + name)
        if any(not re.fullmatch(r'[0-9a-f]{64}', str(row.get(field, '')))
               for field in ['locked_sha256', 'defense_sha256']):
            problems.append('Missing encrypted-circuit baseline hashes: ' + name)
        if name in paper and any(paper[name].get(field) != row.get(field) for field in count_fields):
            problems.append('Table I and full-suite expected counts differ: ' + name)
    if not args.technical_only:
        license_path = REPOSITORY / 'LICENSE'
        if license_path.is_file() and not license_path.read_text().startswith('MIT License\n'):
            problems.append('Project LICENSE is not the finalized MIT license text')
    for problem in problems:
        print('ERROR:', problem)
    if not problems:
        print('Source release checks passed. Run the automated tests and reproduce.py to verify behavior.')
    return 1 if problems else 0


if __name__ == '__main__':
    raise SystemExit(main())
