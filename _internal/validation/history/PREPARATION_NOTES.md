# Historical preparation notes

This is an archived record of earlier preparation stages. Statements about pending attribution or unavailable network/Docker access describe those stages. Use the [current validation record](../VALIDATION.md), repository LICENSE, and current user guide for the present release.

---

# Completed validation record

Preparation date: 2026-10-31. Scope: the legacy ICCD program only.

## Acceptance results

| Check | Result |
| --- | --- |
| Full archived input set | 245/245 cases passed |
| Original RevC-Lock line/gate counts | 245/245 match the archived experiment |
| Legacy defense line/gate counts | 245/245 match the archived experiment |
| Original RevC-Lock encrypted-file bytes | 245/245 SHA-256 matches |
| Legacy defense encrypted-file bytes | 245/245 SHA-256 matches |
| Attack on original RevC-Lock | 245/245 MiniSat-equivalent to the original |
| Attack on legacy defense | 245/245 MiniSat-non-equivalent, as expected |
| Published Table I | 11/11 original, locked and defended counts and overheads match |
| Fresh source-package relocation | Passed independent dependency export, build and CLI tests in a path containing spaces |
| Table I using that relocated build | 11/11 cases passed again, including both modes' encrypted-file checksums |
| CLI / data-import tests | 7 test methods in 2 CTest suites passed |
| PUMA / Z3 linkage | Neither is compiled or linked |

Per-case evidence:

- [full245_comparison.csv](../full245_comparison.csv): all fresh counts, timings, hashes and equivalence outcomes.
- [full245_summary.json](../full245_summary.json): completed full-suite summary.
- [table1_comparison.csv](table1_comparison.csv): the 11 paper cases with historical paper timings alongside fresh timings.
- [relocated_table1_summary.json](relocated_table1_summary.json): the second source-package build's Table I result.
- [relocated_table1_comparison.csv](relocated_table1_comparison.csv): relocated build's actual runs.
- [native_linkage.txt](native_linkage.txt): actual native executable shared-library dependencies.
- [environment.txt](environment.txt): compiler and CMake versions.

The full suite executes two encryption operations and two attack-plus-verification operations per input: 490 generated encrypted circuits and 490 attack verifications. The 490 encrypted outputs match the earlier archived-source baseline byte-for-byte, not merely in gate count. The original RevC-Lock baseline comes from the previous full original-source audit; the defense baseline also matches the original ZIP's saved encrypted circuits.

## Build isolation

The native executables were compiled in a fresh project build directory from exact upstream Git source snapshots. No previous CirKit `.so`, `.a`, object file, `flags.make`, `link.txt`, or old MiniSat executable was used. The relocated source package was extracted into a separate directory containing spaces, fetched the same source objects through the offline-cache option, and completed a second build. Its native executable and MiniSat hashes matched those of the initial new build.

The selected upstream dependency boundary is 55 CirKit files, 20 ABC files, 22 MiniSat files, 2 CUDD headers, 16 kitty headers and 2 fmt headers. All selected CirKit files were checked for their retained MIT permission text. Exact paths and file hashes are in `dependencies.integrity.json`. This identifies the selected build, not a blanket license assertion about all files in the historical CirKit repository.

## Reproduction semantics

- Counts follow the paper's `.numvars` / gate-count convention; `.numvars` includes original, key and ancillary lines in locked circuits.
- The random seed remains 2. Both encryption functions include final ESOP rewriting before their output is measured.
- The original miter / MiniSat result is the equivalence criterion. The attack without a reference is also independently exercised by the CLI tests.
- Timings use fresh runs on this machine and are not expected to equal the published milliseconds.
- Published Table I counts and rounded overheads are separately checked against the archived experiment metadata.

## What has not been claimed

The environment lacked working external DNS and access to its Docker daemon. Live GitHub/RevLib fetching, GitHub Actions execution and Docker image execution were not tested here. Dependency acquisition from a Git source cache, benchmark import from the original ZIP, narrow native builds, source relocation, CLI handling and the numerical reproduction were tested. Unit tests also cover the network-download byte-verification logic with a mock response, including rejection of changed data; this is not a live-server test.

The laboratory user has confirmed the original developer's agreement to laboratory publication. The specific project-wide license terms have not yet been selected, so the candidate retains a status-only LICENSE while recording publication permission as confirmed. All third-party material remains separately attributed. Benchmark redistribution permissions are not assumed; input and derived `.real` files are excluded from the source package except for the newly authored small demonstration example. No legal guarantee or new claim of cryptographic security is made by this technical validation.


## 2026-11-02 update

The earlier network/Docker limitations above describe the original preparation run. They are superseded for online installation and Table I by [docker_online_report.md](../docker_online_report.md). MIT has since been selected; copyright attribution remains pending. The research implementation and dependency versions remain unchanged. The release checker now also detects the explicit pending-copyright marker, and its validated source seal was updated after checking that technical mode succeeds and full-release mode still blocks unfinished attribution.


## 2026-11-02 attribution completion

The laboratory user subsequently reported the professor's instruction to attribute laboratory-owned project code to Advanced Logic Design Automation Lab (ALDA), NTUST, while preserving the paper authors. The formal MIT LICENSE is now applied. README, AUTHORS.md and CITATION.cff identify the associated ICCD 2025 paper, its authors and DOI 10.1109/ICCD65941.2025.00098, verified against publisher-registered metadata. Earlier pending-attribution statements above describe historical preparation stages; they are no longer the current project status. The research code, upstream dependency notices and acceptance baselines are unchanged.
