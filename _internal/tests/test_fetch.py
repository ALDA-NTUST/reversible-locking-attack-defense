#!/usr/bin/env python3
from pathlib import Path
from unittest import mock
import hashlib
import importlib.util
import io
import json
import sys
import tempfile
import unittest
import zipfile

PROJECT = Path(__file__).resolve().parents[1]


class FetchTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='revc-data-test-')
        self.root = Path(self.temporary.name)
        (self.root / 'experiments/iccd').mkdir(parents=True)
        self.data = (PROJECT / 'examples/demo.real').read_bytes()
        info = [dict(filename='demo.real', sha256=hashlib.sha256(self.data).hexdigest(),
                     table1=True, url='https://example.invalid/demo.real')]
        (self.root / 'experiments/iccd/benchmarks.json').write_text(json.dumps(info))
        spec = importlib.util.spec_from_file_location('benchmark_fetch', str(PROJECT / 'scripts/fetch_benchmarks.py'))
        self.module = importlib.util.module_from_spec(spec); spec.loader.exec_module(self.module)
        self.module.ROOT = self.root

    def tearDown(self): self.temporary.cleanup()

    def test_archive_prefers_original_over_nested_build_copy(self):
        archive = self.root / 'research.zip'
        with zipfile.ZipFile(str(archive), 'w') as z:
            z.writestr('research/benchmark/demo.real', self.data)
            z.writestr('research/cirkit/build/benchmark/demo.real', b'wrong nested copy')
        with mock.patch.object(sys, 'argv', ['fetch', '--from-archive', str(archive)]):
            self.assertEqual(self.module.main(), 0)
        self.assertEqual((self.root / 'data/benchmarks/demo.real').read_bytes(), self.data)

    def test_download_verifies_bytes_before_publishing(self):
        with mock.patch.object(sys, 'argv', ['fetch']), mock.patch.object(
                self.module.urllib.request, 'urlopen', return_value=io.BytesIO(self.data)):
            self.assertEqual(self.module.main(), 0)
        self.assertEqual((self.root / 'data/benchmarks/demo.real').read_bytes(), self.data)

    def test_wrong_download_is_rejected(self):
        with mock.patch.object(sys, 'argv', ['fetch']), mock.patch.object(
                self.module.urllib.request, 'urlopen', return_value=io.BytesIO(b'changed')):
            with self.assertRaises(ValueError): self.module.main()
        self.assertFalse((self.root / 'data/benchmarks/demo.real').exists())


if __name__ == '__main__': unittest.main()
