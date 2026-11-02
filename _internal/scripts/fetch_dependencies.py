#!/usr/bin/env python3
"""Fetch exact upstream commits, or export them from an existing Git cache."""
from pathlib import Path
import argparse
import io
import json
import os
import shutil
import subprocess
import tarfile
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def verify_files(name, directory):
    integrity = json.loads((ROOT / 'dependencies.integrity.json').read_text())
    import hashlib
    for relative, expected in integrity[name].items():
        path = directory / relative
        if not path.is_file() or hashlib.sha256(path.read_bytes()).hexdigest() != expected:
            raise RuntimeError('Pinned source file is missing or modified: ' + str(path))


def git(args, cwd=None):
    try:
        r = subprocess.run(['git'] + list(map(str, args)), cwd=str(cwd) if cwd else None,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=180)
    except (OSError, subprocess.TimeoutExpired) as e:
        raise RuntimeError('Git could not finish: ' + str(e))
    if r.returncode:
        raise RuntimeError(r.stderr.decode('utf-8', errors='replace').strip())
    return r.stdout


def extract(data, destination):
    root = destination.resolve()
    with tarfile.open(fileobj=io.BytesIO(data)) as archive:
        for member in archive.getmembers():
            target = (root / member.name).resolve()
            if target != root and root not in target.parents:
                raise RuntimeError('Unsafe path in dependency archive')
            if member.issym() or member.islnk():
                link = ((target.parent if member.issym() else root) / member.linkname).resolve()
                if link != root and root not in link.parents:
                    raise RuntimeError('Unsafe link in dependency archive')
            elif not member.isfile() and not member.isdir():
                raise RuntimeError('Unsupported dependency archive entry')
        archive.extractall(str(root))


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--cache-root', type=Path, help='Optional existing CirKit Git checkout for offline exports')
    p.add_argument('--destination', type=Path, default=ROOT / '.deps')
    args = p.parse_args(argv)
    lock = json.loads((ROOT / 'dependencies.lock.json').read_text())
    args.destination.mkdir(parents=True, exist_ok=True)
    for name, info in lock.items():
        target = args.destination / name
        receipt = target / '.revc-source.json'
        if target.exists():
            if receipt.is_file() and json.loads(receipt.read_text()).get('commit') == info['commit']:
                verify_files(name, target)
                print(name, info['commit'][:12], 'already fetched', flush=True)
                continue
            raise RuntimeError('Unverified dependency directory {}; use a fresh destination'.format(target))
        with tempfile.TemporaryDirectory(prefix='revc-fetch-') as tmp:
            if args.cache_root:
                source = args.cache_root if name == 'cirkit' else args.cache_root / 'lib' / name
                actual = git(['rev-parse', info['commit'] + '^{commit}'], source).decode().strip()
            else:
                source = Path(tmp) / 'repository'
                git(['init', '--quiet', source])
                print('Fetching', name, info['commit'], flush=True)
                git(['fetch', '--quiet', '--depth', '1', info['url'], info['commit']], source)
                actual = git(['rev-parse', 'FETCH_HEAD'], source).decode().strip()
            if actual != info['commit']: raise RuntimeError('Unexpected Git revision for ' + name)
            data = git(['archive', info['commit']], source)
            staging = args.destination / ('.' + name + '.staging')
            if staging.exists(): raise RuntimeError('Stale staging directory: ' + str(staging))
            staging.mkdir()
            try:
                extract(data, staging)
                verify_files(name, staging)
                (staging / '.revc-source.json').write_text(json.dumps(info, indent=2))
                os.replace(str(staging), str(target))
            except Exception:
                shutil.rmtree(str(staging))
                raise
        print(name, info['commit'][:12], 'ready', flush=True)
    return 0


if __name__ == '__main__':
    try: raise SystemExit(main())
    except RuntimeError as e: raise SystemExit('Dependency fetch failed: ' + str(e))
