#!/usr/bin/env python3
"""Check validated source, package integrity, attribution, and mirrored notices."""
from pathlib import Path
import argparse
import hashlib
import json
import sys

ROOT = Path(__file__).resolve().parents[1]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--technical-only', action='store_true')
    args = p.parse_args()
    problems = []
    for name in ['README.md', 'LICENSE', 'AUTHORS.md', 'CITATION.cff', 'THIRD_PARTY_NOTICES.md',
                 'dependencies.lock.json', 'dependencies.integrity.json', 'CMakeLists.txt']:
        if not (ROOT / name).is_file(): problems.append('Missing ' + name)
    integrity = json.loads((ROOT / 'dependencies.integrity.json').read_text())
    for package, paths in integrity.items():
        if package not in {'cirkit', 'abc', 'minisat', 'cudd', 'fmt', 'kitty'}:
            problems.append('Unexpected dependency: ' + package)
        for name in paths:
            if 'puma' in name.lower() or 'z3' in name.lower(): problems.append('Excluded dependency used: ' + name)
    summary_path = ROOT / 'validation/full245_summary.json'
    if not summary_path.is_file(): problems.append('Missing full 245-case validation')
    else:
        summary = json.loads(summary_path.read_text())
        if summary.get('cases') != 245 or summary.get('passed') != 245 or not summary.get('all_passed'):
            problems.append('245-case validation did not pass')
    seal = ROOT / 'provenance/source_hashes.json'
    if not seal.is_file(): problems.append('Missing validated source snapshot')
    else:
        for name, expected in json.loads(seal.read_text()).items():
            path = ROOT / name
            if not path.is_file() or hashlib.sha256(path.read_bytes()).hexdigest() != expected:
                problems.append('Source changed since validation: ' + name)
    if ROOT.name == '_internal':
        repository = ROOT.parent
        manifest = ROOT / 'USER_PACKAGE_MANIFEST.json'
        if not manifest.is_file():
            problems.append('Missing package manifest')
        else:
            for name, expected in json.loads(manifest.read_text()).items():
                relative = Path(name)
                if relative.is_absolute() or '..' in relative.parts:
                    problems.append('Unsafe package manifest path: ' + name)
                    continue
                path = repository / relative
                if not path.is_file() or hashlib.sha256(path.read_bytes()).hexdigest() != expected:
                    problems.append('Package file is missing or changed: ' + name)
        for name in ['LICENSE', 'CITATION.cff', 'examples/demo.real']:
            public, internal = repository / name, ROOT / name
            if not public.is_file() or not internal.is_file() or public.read_bytes() != internal.read_bytes():
                problems.append('Root and internal copies differ: ' + name)
    license_text = (ROOT / 'LICENSE').read_text()
    license_pending = any(marker in license_text for marker in ['LICENSE TERMS NOT SELECTED', 'COPYRIGHT HOLDER NOT CONFIRMED'])
    if license_pending and not args.technical_only:
        problems.append('Finalize the selected project license with confirmed copyright attribution; developer publication permission is already confirmed')
    for problem in problems: print('BLOCKER:', problem)
    if not problems:
        print('Technical preparation checks passed.' if args.technical_only else 'Recorded release checks passed; retain all third-party conditions.')
        if license_pending: print('Developer publication permission is confirmed; final license/copyright attribution is pending.')
    return 1 if problems else 0


if __name__ == '__main__': raise SystemExit(main())
