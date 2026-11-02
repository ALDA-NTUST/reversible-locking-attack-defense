# Maintainer guide

Run the commands below from the repository root. The user workflow is documented once in [README](../README.md) and the [usage guide](USAGE.md).

## Source map

| Path | Contents |
| --- | --- |
| `setup.py`, `revc-tool`, `reproduce.py` | Public installation, operation, and reproduction entry points |
| `_internal/src/` | Laboratory research implementation and native dispatch |
| `_internal/revc_lock/` | Python command handling and process isolation |
| `_internal/scripts/` | Dependency acquisition, build, reproduction, and release checks |
| `_internal/third_party_adapters/` | CirKit ESOP adaptations with original notices |
| `_internal/experiments/iccd/` | Input manifests and numerical acceptance baselines |
| `_internal/provenance/` | Archived original source, adaptation patch, source hashes, and attribution record |
| `_internal/licenses/`, `_internal/THIRD_PARTY_NOTICES.md` | Original third-party licenses and source attribution |
| `_internal/validation/` | Current evidence index and aggregate results; older records are under `history/` |

## Build and check

```bash
python3 setup.py
python3 _internal/scripts/check_release.py
```

Setup runs the CLI and benchmark-import tests through CTest. The complete release check also verifies the recorded implementation hashes, package manifest, and synchronized notices. The GitHub Actions workflow runs setup and the complete release check on Ubuntu 22.04.

The copies of `LICENSE`, `CITATION.cff`, and `examples/demo.real` under `_internal/` support the native build and internal tools. They are checked against the root versions; edit the corresponding pair together. User instructions always use the root entry points.

## Reproduction and changes

```bash
python3 reproduce.py --output outputs/review-table1
```

Use the 245-case command in the [usage guide](USAGE.md) when an algorithm or dependency change requires checking the full archived baseline. Preserve numerical comparisons and timing provenance. Refresh validated source hashes only after the affected implementation has been checked; a changed hash by itself is not evidence that the behavior is correct.

`_internal/USER_PACKAGE_MANIFEST.json` records source and documentation SHA-256 values relative to the repository root, excluding the manifest itself. Update it after a reviewed change. Exclude `.git/`, `.deps/`, build outputs, downloaded inputs, generated circuits, and Python caches. Preserve the package-authored demo as the only bundled circuit.

## Attribution and distribution

The project is maintained and attributed to Advanced Logic Design Automation Lab (ALDA), NTUST. [AUTHORS.md](../_internal/AUTHORS.md) preserves the paper authors and original archive provenance. [CITATION.cff](../CITATION.cff) records the ICCD 2025 paper and the earlier RevC-Lock paper. Third-party notices and copied license texts remain separate from the laboratory MIT grant.

The laboratory attribution decision is recorded in [RELEASE_RECORD.md](../_internal/provenance/RELEASE_RECORD.md) and `publication_permission.json`. Historical preparation statements are not current installation or licensing instructions.

GitHub's source download contains the repository layout, including the public entry points. Earlier preparation-only archive scripts and the unvalidated optional Dockerfile were removed during final cleanup. The clean-environment Docker tests are documented as test evidence; this source release does not distribute a container image or prebuilt executables.

Repository visibility and tagged releases are separate publication actions. The cleanup does not change visibility or create a release tag.
