#!/usr/bin/env python3
"""Package only reviewed source, notices and aggregate validation records."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import zipfile

ROOT = Path(__file__).resolve().parents[1]
ROOT_FILES = ['README.md', 'LICENSE', 'AUTHORS.md', 'CITATION.cff', 'THIRD_PARTY_NOTICES.md',
              'CMakeLists.txt', 'dependencies.lock.json', 'dependencies.integrity.json',
              'revc-tool', 'Dockerfile', '.gitignore', '.dockerignore']
DIRECTORIES = ['src', 'revc_lock', 'third_party_adapters', 'scripts', 'tests', 'examples',
               'experiments', 'docs', 'licenses', 'provenance', 'validation', '.github']


def selected_files():
    result = [ROOT / name for name in ROOT_FILES]
    for directory in DIRECTORIES:
        result += [p for p in (ROOT / directory).rglob('*')
                   if p.is_file() and '__pycache__' not in p.parts and p.suffix not in {'.pyc', '.pyo'}]
    for path in result:
        if path.is_symlink(): raise ValueError('Symlinks are not part of the source package: ' + str(path))
        if path.suffix == '.real' and path.relative_to(ROOT).as_posix() != 'examples/demo.real':
            raise ValueError('Unreviewed benchmark/derived circuit would be packaged: ' + str(path))
        if path.suffix in {'.o', '.so', '.a', '.zip'}:
            raise ValueError('Build/data artifact would be packaged: ' + str(path))
    return sorted(set(result))


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output', type=Path, required=True)
    args = p.parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    files = selected_files()
    manifest = {f.relative_to(ROOT).as_posix(): hashlib.sha256(f.read_bytes()).hexdigest() for f in files}
    temporary = args.output.with_suffix('.tmp')
    with zipfile.ZipFile(str(temporary), 'w', zipfile.ZIP_DEFLATED) as archive:
        for f in files: archive.write(str(f), 'legacy-revc-lock/' + f.relative_to(ROOT).as_posix())
        archive.writestr('legacy-revc-lock/PACKAGE_MANIFEST.json', json.dumps(manifest, sort_keys=True, indent=2))
    with zipfile.ZipFile(str(temporary)) as archive:
        if archive.testzip() is not None: raise ValueError('Source archive failed integrity check')
        for name, expected in manifest.items():
            if hashlib.sha256(archive.read('legacy-revc-lock/' + name)).hexdigest() != expected:
                raise ValueError('Source archive content mismatch: ' + name)
    os.replace(str(temporary), str(args.output))
    checksum = hashlib.sha256(args.output.read_bytes()).hexdigest()
    args.output.with_suffix(args.output.suffix + '.sha256').write_text(checksum + '  ' + args.output.name + '\n')
    print(args.output, len(files), 'reviewed files;', args.output.stat().st_size, 'bytes')


if __name__ == '__main__': main()
