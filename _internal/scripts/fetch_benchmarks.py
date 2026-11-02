#!/usr/bin/env python3
"""Fetch separately attributed RevLib data, or import the exact research archive."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import tempfile
import urllib.request
import zipfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--suite', choices=['table1', 'all'], default='table1')
    p.add_argument('--from-archive', type=Path, help='Import exact benchmark files from the original research ZIP')
    p.add_argument('--destination', type=Path, default=ROOT / 'data/benchmarks')
    args = p.parse_args()
    manifest = json.loads((ROOT / 'experiments/iccd/benchmarks.json').read_text())
    selected = [r for r in manifest if args.suite == 'all' or r['table1']]
    args.destination.mkdir(parents=True, exist_ok=True)
    archive = zipfile.ZipFile(str(args.from_archive)) if args.from_archive else None
    try:
        for info in selected:
            target = args.destination / info['filename']
            if target.exists():
                if hashlib.sha256(target.read_bytes()).hexdigest() != info['sha256']:
                    raise ValueError('Existing data differ from the paper baseline: ' + str(target))
                print(info['filename'], 'already verified', flush=True)
                continue
            if archive:
                matches = [n for n in archive.namelist() if n == 'benchmark/' + info['filename'] or n.endswith('/benchmark/' + info['filename'])]
                # The historical ZIP also contains build/benchmark copies.
                # Prefer the original top-level benchmark directory.
                if matches:
                    depth = min(n.count('/') for n in matches)
                    matches = [n for n in matches if n.count('/') == depth]
                if len(matches) != 1: raise ValueError('Archive is missing an unambiguous ' + info['filename'])
                data = archive.read(matches[0])
            else:
                print('Downloading', info['url'], flush=True)
                try:
                    with urllib.request.urlopen(info['url'], timeout=30) as response: data = response.read()
                except Exception as e:
                    raise ValueError('Download failed for {}: {}. You can import the original ZIP with --from-archive.'.format(info['filename'], e))
            if hashlib.sha256(data).hexdigest() != info['sha256']:
                raise ValueError('Checksum mismatch for {}; the upstream copy may have changed. Import the original research ZIP with --from-archive; no unverified data were accepted.'.format(info['filename']))
            fd, tmp = tempfile.mkstemp(prefix='.benchmark-', dir=str(args.destination))
            try:
                with os.fdopen(fd, 'wb') as handle: handle.write(data)
                os.replace(tmp, str(target))
            finally:
                if os.path.exists(tmp): os.unlink(tmp)
            print(info['filename'], 'verified', flush=True)
    finally:
        if archive: archive.close()
    print('Ready:', len(selected), 'benchmarks; source: RevLib, not authored by this laboratory.')
    return 0


if __name__ == '__main__':
    try: raise SystemExit(main())
    except (ValueError, OSError, zipfile.BadZipFile) as e: raise SystemExit(str(e))
