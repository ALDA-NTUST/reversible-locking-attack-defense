# Legacy RevC-Lock: ICCD experiment reproduction

The legacy research implementation associated with **Decomposition Attack on Structural Logic Locking of Reversible Circuits**, by Feng-Jie Chao and Yung-Chih Chen. This package provides the reimplementation of RevC-Lock, the paper's Decomposition Attack, and its Control Obfuscation defense. The later journal / restore-free variants and the Web application are outside this package.

**Maintained by Advanced Logic Design Automation Lab (ALDA), NTUST.** Laboratory-owned code is distributed under the [MIT License](LICENSE), with the original paper authors credited below and in [AUTHORS.md](AUTHORS.md). The associated paper appeared at ICCD 2025, pp. 646-653: [DOI 10.1109/ICCD65941.2025.00098](https://doi.org/10.1109/ICCD65941.2025.00098). Technical reproduction and clean Docker online installation have been validated. See [publication status](docs/PUBLICATION_STATUS.md). Third-party licenses remain separate.

## Quick start

Supported validation platform: 64-bit glibc Linux, GCC 9.4, Boost 1.71, CMake 3.24. Python tooling uses only the standard library and was tested with Python 3.6 and 3.10. The Ubuntu 22.04 Docker/CI definitions are supplied but were not executable in the preparation environment; see [validation](validation/VALIDATION.md).

On Ubuntu/Debian, install system build dependencies:

```bash
sudo apt-get update
sudo apt-get install -y git cmake g++ make python3 ca-certificates \
  libboost-filesystem-dev libboost-system-dev zlib1g-dev
```

From this directory:

```bash
python3 scripts/setup.py
python3 revc-tool doctor
python3 revc-tool lock examples/demo.real outputs/demo_locked.real
python3 revc-tool attack outputs/demo_locked.real outputs/demo_recovered.real
python3 revc-tool verify examples/demo.real outputs/demo_recovered.real
python3 revc-tool defend examples/demo.real outputs/demo_defended.real
```

`setup.py` downloads exact upstream commits, verifies source checksums, performs a fresh CMake build, and runs CLI tests. No prebuilt CirKit installation, Web server, Python packages, PUMA, or Z3 is required. CUDD headers and fmt/kitty headers are obtained as source dependencies; this build does not link a CUDD or Z3 library. `--jobs 2` is the default; use a larger number if memory allows.

For offline setup with an existing CirKit Git checkout containing the locked commits:

```bash
python3 scripts/setup.py --cache-root /path/to/cirkit
```

The cache supplies upstream source objects only; no libraries or build flags are reused from that checkout.

## Commands

| Command | Input | Output / behavior |
| --- | --- | --- |
| `lock ORIGINAL LOCKED` | Original flat MCT circuit | Original RevC-Lock, including final ESOP rewriting |
| `defend ORIGINAL DEFENDED` | Original flat MCT circuit | The legacy paper's Control Obfuscation defense, including final ESOP rewriting |
| `attack LOCKED RECOVERED` | Legacy RevC-Lock / defense circuit | Apply the original Decomposition Attack; no original reference is required |
| `verify FIRST SECOND` | Two circuits | Original MiniSat miter equivalence check; exit 0 if equivalent, 1 if different |
| `doctor` | None | Check local executable paths |

`attack --reference ORIGINAL` also runs equivalence checking. A completed attack can report `non_equivalent`, especially against the defense; this is a result, not a process crash. `verify` follows the original miter's line ordering and checks all represented outputs with extra lines padded as in the legacy code; it is not a general quantum/ancilla-aware equivalence checker.

All operation commands support `--json`, `--build-dir`, and `--log-dir`. The default time limit is 120 seconds; change it with `--timeout`. Existing outputs are preserved unless `--force` is given. Input and output paths may contain spaces or non-ASCII characters. Each operation runs in an isolated process and temporary directory, including the EXORCISM scratch file.

The legacy seed is fixed at **2** for exact reproduction. The original implementation uses integer keys; this package makes its tested large-input integer behavior explicit instead of retaining an overflowing float-to-int conversion. This is a reproduction tool, not a claim of broader security or a new cryptographic design. See [adaptations](provenance/ADAPTATIONS.md).

Supported format: flat RevLib `.real` files using NOT/CNOT/Toffoli/MCT `tN` gates, optionally with negative controls and bus metadata. Encryption supports 3..249 circuit lines. The attack recognizes this paper's legacy restore-unit layout; other locking schemes and arbitrary gate libraries are rejected.

## Reproduce Table I

Original benchmark data are from RevLib and are **not bundled or relicensed as laboratory-owned data**. Fetch the 11 Table I inputs separately:

```bash
python3 scripts/fetch_benchmarks.py
python3 scripts/reproduce.py
```

Every download must match the archived paper-input SHA-256. If the upstream copy is unavailable or has changed, the command stops with a clear error. All 11 Table I downloads were verified against the live RevLib server in the clean Ubuntu 22.04 Docker test on 2026-11-02. An exact source-archive import was tested:

```bash
python3 scripts/fetch_benchmarks.py --from-archive /path/to/original-research.zip
python3 scripts/reproduce.py --output outputs/my-table1
```

For all 245 archived inputs:

```bash
python3 scripts/fetch_benchmarks.py --suite all --from-archive /path/to/original-research.zip
python3 scripts/reproduce.py --suite all --output outputs/my-all-cases
```

Both encryption modes and the attack on each output are executed. The report checks original/locked/defended line and gate counts, expected equivalence outcomes, and byte-identical encrypted outputs against the validated archived baseline. CPU timings are measured separately; identical milliseconds across machines are not expected. Use `--resume` to continue an interrupted run; completed encrypted artifacts are checksum-checked before reuse.

`outputs/.../comparison.csv` contains per-case results; `summary.json` contains the acceptance result. The 11 paper rows and full baseline are in `experiments/iccd/`. See [validation/VALIDATION.md](validation/VALIDATION.md) for the actual completed verification, including 245/245 cases in each mode.

## Source and credit

- `src/`: laboratory research implementation and the native command dispatch.
- `revc_lock/`, `scripts/`, `tests/`: CLI, installation, reproduction, validation and tests added during publication preparation.
- `third_party_adapters/cirkit/`: explicitly attributed MIT-licensed adaptations of upstream CirKit ESOP routines; these are not claimed as laboratory-original algorithms.
- `.deps/`: ignored, externally fetched upstream sources at locked commits.
- `provenance/`: the original research file, adaptation diff and authorship history.
- `licenses/`, `THIRD_PARTY_NOTICES.md`: third-party license texts and scope.

RevC-Lock itself is a method from Limaye et al., *Revisiting logic locking for reversible computing*, ETS 2019; this package supplies the research reimplementation described by the ICCD manuscript. Circuit infrastructure is from CirKit/RevKit. ESOP rewriting uses Alan Mishchenko's ABC EXORCISM-4. Equivalence checking uses MiniSat. Original benchmarks are from RevLib. See [AUTHORS.md](AUTHORS.md), [CITATION.cff](CITATION.cff), and [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

## Build and troubleshooting

- Missing native binaries: run `python3 scripts/setup.py`.
- Network unavailable: use the offline Git cache and original-archive import above.
- Source checksum mismatch: do not bypass it; restore the pinned source revision or review the change and rerun the full baseline.
- Output exists: choose a fresh path or explicitly use `--force`.
- Slow SAT case: increase `--timeout`; inspect `--log-dir` output.
- This release uses its own CMake targets and never reads a previous installation's `flags.make` or `link.txt`.

The optional Docker recipe is `docker build -t legacy-revc-lock .`. Mount a working directory and invoke commands through its entrypoint. The clean Ubuntu Docker installation test passed on 2026-11-02; this optional Dockerfile recipe itself is a separate deployment path and has not been executed. See validation/docker_online_report.md.
