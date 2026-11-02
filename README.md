# Reversible Logic Locking: Attack and Defense

**Private preparation version maintained by ALDA-NTUST.** MIT is selected; copyright attribution remains pending. See [publication status](PUBLICATION_STATUS.md).

The original ICCD implementation of RevC-Lock, Decomposition Attack and Control Obfuscation, associated with the manuscript by Feng-Jie Chao and Yung-Chih Chen. This user-facing layout retains the validated implementation and its third-party notices under `_internal/`.

Use this guide and the three entry points: `setup.py`, `revc-tool`, and `reproduce.py`.

## 1. Install

Use 64-bit glibc Linux. Install the system build tools on Ubuntu/Debian:

```bash
sudo apt-get update
sudo apt-get install -y git cmake g++ make python3 ca-certificates \
  libboost-filesystem-dev libboost-system-dev zlib1g-dev
python3 setup.py
python3 revc-tool doctor
```

The installer fetches fixed upstream versions, builds the native tools and runs tests. Python needs no pip packages. Offline source-cache setup is available as `python3 setup.py --cache-root /path/to/cirkit`.

## 2. Use the three functions

```bash
# RevC-Lock encryption
python3 revc-tool lock examples/demo.real outputs/locked.real

# Decomposition Attack
python3 revc-tool attack outputs/locked.real outputs/recovered.real

# Verify the recovered circuit
python3 revc-tool verify examples/demo.real outputs/recovered.real

# Control Obfuscation defense, applied to the original circuit
python3 revc-tool defend examples/demo.real outputs/defended.real
```

Replace `examples/demo.real` with your original `.real` circuit. Each operation writes to the output path you specify. Existing files are preserved; use `--force` to overwrite. `attack --reference ORIGINAL.real` also performs the equivalence check. `--json` provides structured output.

## 3. Reproduce Table I

```bash
python3 reproduce.py
```

This obtains the 11 RevLib input circuits, verifies their archived SHA-256 values, runs both encryption modes and the attack, and compares against the paper. Results are written to `outputs/iccd-table1/comparison.csv` and `summary.json`.

If you have the original research ZIP, use the tested offline data-import path:

```bash
python3 reproduce.py --from-archive /path/to/original-research.zip
```

For all 245 inputs add `--suite all` and select a fresh `--output`. To continue an interrupted run, add `--resume`.

## Where things are

| Item | Purpose |
| --- | --- |
| `setup.py` | Install and build once |
| `revc-tool` | Run `lock`, `attack`, `defend`, or `verify` |
| `reproduce.py` | Fetch or import inputs and reproduce the paper results |
| `examples/` | Small demonstration circuit |
| `outputs/` | Generated results; created when commands run |
| `_internal/` | Implementation, pinned dependencies, tests, attribution, and validation |

The algorithm, dependency versions and acceptance baseline are unchanged. The original preparation validated 245 cases in both encryption modes and matched all 490 encrypted files byte-for-byte; see [validation](_internal/validation/VALIDATION.md). CPU timings vary by machine.

Original-developer permission to laboratory publication is confirmed. MIT has been selected, with final copyright attribution pending; see [LICENSE](LICENSE) and [publication status](PUBLICATION_STATUS.md). Third-party licenses and original authors remain preserved in [third-party notices](_internal/THIRD_PARTY_NOTICES.md) and [authors](_internal/AUTHORS.md). RevLib benchmark data are obtained separately and are not bundled as laboratory-owned data.

On 2026-11-02 the supplied implementation passed a clean Ubuntu 22.04 Docker test with online installation, all six pinned GitHub dependencies and all 11 RevLib Table I inputs downloaded from their upstream sources. All cases passed and all 22 encrypted-file checksums matched. See the [Docker report](_internal/validation/docker_online_report.md). GitHub Actions execution is separate from this Docker evidence. Changed benchmark content is rejected by checksum verification.

Developers can enter `_internal/` and use its original [technical README](_internal/README.md), build configuration and maintenance scripts.
