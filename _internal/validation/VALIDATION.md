# Validation evidence

Scope: the legacy ICCD 2025 implementation. The random seed is fixed at 2. Both encryption modes include the final ESOP rewriting before counts and output hashes are recorded.

## Recorded results

| Validation | Environment and inputs | Result |
| --- | --- | --- |
| Full archived suite, 2026-10-31 | Fresh source build; 245 exact archived inputs | 245/245 cases pass; 490 encrypted outputs match the original-program baseline |
| Clean source relocation, 2026-10-31 | New source directory containing spaces; independent build | 11/11 Table I cases pass with both modes' hashes matching |
| Clean Docker online installation, 2026-11-02 | Official Ubuntu 22.04 base image; tools installed through apt; six dependencies and 11 inputs downloaded online | Build and 7 tests pass; Table I 11/11; 22 encrypted-file hashes match |
| CLI and import tests | Two CTest suites, 7 test methods | Separate commands, equivalence outcomes, invalid inputs, overwrite protection, Unicode/space paths, concurrent jobs, and input checksums exercised |
| GitHub Actions | Ubuntu 22.04, repository checkout | Build and CLI/import tests; CI results are available in the repository's Actions tab |

The 245-case run applies both encryption modes and attacks each output: 490 encrypted circuits and 490 attack verifications. Attacks on RevC-Lock recover an equivalent function for all recorded cases; attacks on the defense are non-equivalent, as expected by the archived experiment.

## Current evidence files

- [Full 245-case summary](full245_summary.json) and [per-case comparison](full245_comparison.csv).
- [Clean Docker report](docker_online_report.md), [summary](docker_online_summary.json), [Table I comparison](docker_online_comparison.csv), and [image identity](docker_online_image.json).
- [Earlier preparation records](history/): source relocation, initial environment/linkage records, user-entry checks, and the original chronological notes. These are historical evidence, not current setup or licensing instructions.

## Acceptance criteria and limits

- Published Table I input/gate counts and rounded overheads are checked separately from timings.
- Encrypted-file SHA-256 values are compared against the archived original-program baseline.
- The original miter/MiniSat construction defines the reported equivalence semantics.
- CPU times depend on the machine; equality with paper timings is not required.
- Online downloads of the 11 Table I inputs were exercised. The recorded full 245-case run used exact archived inputs.
- Clean Docker testing shares the host Linux kernel. It does not establish support for every operating system or compiler combination.
- The paper's defense outcomes concern this decomposition attack on the recorded cases.

## Source boundary

The selected dependency boundary is 55 CirKit files, 20 ABC files, 22 MiniSat files, 2 CUDD headers, 16 kitty headers, and 2 fmt headers. Exact paths and hashes are in `../dependencies.integrity.json`; versions are in `../dependencies.lock.json`. The selected CirKit files retain their MIT permission notices. PUMA and Z3 are not compiled or linked, and no old build artifacts are reused.

Current copyright, authorship, and licensing are recorded in the repository LICENSE, [AUTHORS.md](../AUTHORS.md), [CITATION.cff](../CITATION.cff), and [third-party notices](../THIRD_PARTY_NOTICES.md). The archived original source and adaptation patch remain under `../provenance/`.
