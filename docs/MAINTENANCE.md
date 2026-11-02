# Maintainer guide

Run commands from the repository root. User instructions are in [README](../README.md) and [USAGE.md](USAGE.md).

## Source map

| Path | Contents |
| --- | --- |
| `setup.py`, `revc-tool`, `reproduce.py` | Public installation, operation, and reproduction commands |
| `_internal/src/` | Research implementation and native dispatch |
| `_internal/revc_lock/` | Python command handling and process isolation |
| `_internal/scripts/` | Build, dependency acquisition, reproduction, and release checks |
| `_internal/tests/` | Automated CLI and benchmark-import tests |
| `_internal/experiments/iccd/` | Input-source manifest and expected experimental results |
| `_internal/third_party_adapters/` | Attributed CirKit ESOP adaptations |
| `_internal/licenses/` | Original third-party license texts |

## Verify a change

```bash
python3 setup.py
python3 _internal/scripts/check_release.py
python3 reproduce.py --output outputs/review-table1
```

Setup compiles the tools and runs seven test methods through CTest. The release checker checks source-package structure, dependency records, attribution, and the completeness of the reproduction baselines. It does not substitute for executing tests. CI builds the source, runs the tests, checks the package, and reproduces Table I on Ubuntu 22.04.

Use the full 245-case command in [USAGE.md](USAGE.md) when an algorithm or dependency change requires checking the archived baseline. Review any change to expected counts or circuit hashes against the paper before updating acceptance data.

## Dependencies and attribution

The six upstream revisions are fixed in `_internal/dependencies.lock.json`; selected upstream files are verified against `_internal/dependencies.integrity.json`. Keep source changes and license notices together when reviewing dependency updates.

The laboratory MIT license is in the root [LICENSE](../LICENSE). [AUTHORS.md](../_internal/AUTHORS.md), [ADAPTATIONS.md](../_internal/ADAPTATIONS.md), and [third-party notices](../_internal/THIRD_PARTY_NOTICES.md) preserve research and source attribution. [CITATION.cff](../CITATION.cff) records the associated paper and original RevC-Lock reference.

## Distribution

GitHub's source download contains the public entry points, implementation, essential automated tests, expected results, and notices. Preparation diaries, historical run reports, duplicate original-source snapshots, downloaded dependencies, and generated experiment outputs are not part of the source package.

Local builds use `_internal/build/` and `_internal/.deps/`. Input downloads use `data/`; experiment results use `outputs/`. These paths are ignored by Git. The package-authored `examples/demo.real` is the only bundled circuit.
