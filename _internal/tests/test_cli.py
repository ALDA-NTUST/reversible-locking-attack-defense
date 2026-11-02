#!/usr/bin/env python3
"""End-to-end tests of independent operations, file handling and isolation."""
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor
import argparse
import json
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--build-dir', type=Path, default=ROOT / 'build')
ARGS, remaining = parser.parse_known_args()


class ToolTests(unittest.TestCase):
    def setUp(self):
        # Exercise spaces and Unicode paths using an escaped accent.
        self.temporary = tempfile.TemporaryDirectory(prefix='revc cli unicode \u00e9 ')
        self.work = Path(self.temporary.name)
        self.original = self.work / 'original \u00e9 input.real'
        self.original.write_bytes((ROOT.parent / 'examples/demo.real').read_bytes())

    def tearDown(self): self.temporary.cleanup()

    def invoke(self, *args, **kwargs):
        cmd = [sys.executable, str(ROOT.parent / 'revc-tool')] + list(map(str, args))
        cmd += ['--build-dir', str(ARGS.build_dir), '--json']
        result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
        self.assertEqual(result.returncode, kwargs.get('status', 0), result.stdout + result.stderr)
        return json.loads(result.stdout)

    def test_lock_attack_verify_and_defend(self):
        locked, recovered, defended = [self.work / name for name in ['locked.real', 'recovered.real', 'defended.real']]
        result = self.invoke('lock', self.original, locked)
        self.assertEqual(result['inputs'], 14)
        result = self.invoke('attack', locked, recovered)
        self.assertNotIn('equivalence', result)
        self.assertEqual(self.invoke('verify', self.original, recovered)['equivalence'], 'equivalent')
        self.assertEqual(self.invoke('attack', locked, recovered, '--force', '--reference', self.original)['equivalence'], 'equivalent')
        self.assertEqual(self.invoke('defend', self.original, defended)['inputs'], 14)
        self.invoke('attack', defended, self.work / 'defense_recovered.real', '--reference', self.original)

    def test_non_equivalence_exit_status(self):
        other = self.work / 'other.real'
        other.write_text(self.original.read_text().replace('t1 a', 't1 b'))
        self.assertEqual(self.invoke('verify', self.original, other, status=1)['equivalence'], 'non_equivalent')

    def test_reject_invalid_inputs_and_preserve_output(self):
        bad = self.work / 'bad.real'; bad.write_text('not a circuit')
        output = self.work / 'out.real'
        self.invoke('lock', bad, output, status=2)
        self.assertFalse(output.exists())
        self.invoke('attack', self.original, output, status=2)
        self.invoke('lock', self.original, self.original, '--force', status=2)
        self.invoke('lock', self.original, output)
        data = output.read_bytes()
        self.invoke('defend', self.original, output, status=2)
        self.assertEqual(data, output.read_bytes())
        self.invoke('lock', self.work / 'missing.real', self.work / 'unused.real', status=2)

    def test_parallel_jobs_are_deterministic(self):
        paths = [self.work / 'run1.real', self.work / 'run2.real']
        with ThreadPoolExecutor(max_workers=2) as executor:
            reports = list(executor.map(lambda p: self.invoke('lock', self.original, p), paths))
        self.assertEqual(reports[0]['sha256'], reports[1]['sha256'])


if __name__ == '__main__': unittest.main(argv=[sys.argv[0]] + remaining)
