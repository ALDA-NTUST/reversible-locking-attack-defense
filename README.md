# Reversible Logic Locking: Attack and Defense

**Maintained by [Advanced Logic Design Automation Lab (ALDA)](https://sites.google.com/view/ntust-alda), National Taiwan University of Science and Technology.** Laboratory-owned code is released under the [MIT License](LICENSE).

This repository provides the companion implementation for:

> **[Decomposition Attack on Structural Logic Locking of Reversible Circuits](https://doi.org/10.1109/ICCD65941.2025.00098)**
>
> **Feng-Jie Chao and Yung-Chih Chen**
>
> *2025 IEEE 43rd International Conference on Computer Design (ICCD)*, pp. 646-653, 2025.
>
> DOI: `10.1109/ICCD65941.2025.00098`

It includes the RevC-Lock reimplementation, the paper's Decomposition Attack, and the Control Obfuscation defense. The implementation and its third-party notices are retained under `_internal/`.

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

Copyright (c) 2026 Advanced Logic Design Automation Lab (ALDA), NTUST. Laboratory-owned code is distributed under the [MIT License](LICENSE). Paper authors and original research contributions are credited in [AUTHORS.md](_internal/AUTHORS.md). Third-party code retains its original licenses and notices; see [third-party notices](_internal/THIRD_PARTY_NOTICES.md). RevLib benchmark data are obtained separately and are not assigned the laboratory software license.

On 2026-11-02 the supplied implementation passed a clean Ubuntu 22.04 Docker test with online installation, all six pinned GitHub dependencies and all 11 RevLib Table I inputs downloaded from their upstream sources. All cases passed and all 22 encrypted-file checksums matched. See the [Docker report](_internal/validation/docker_online_report.md). GitHub Actions execution is separate from this Docker evidence. Changed benchmark content is rejected by checksum verification.

## Related research

The `lock` command is a research reimplementation of **RevC-Lock**, introduced in:

> **Nimisha Limaye, Muhammad Yasin, and Ozgur Sinanoglu.** [Revisiting Logic Locking for Reversible Computing](https://doi.org/10.1109/ETS.2019.8791550). *2019 IEEE European Test Symposium (ETS)*, pp. 1-6, 2019. DOI: `10.1109/ETS.2019.8791550`.

RevC-Lock is the earlier locking method used as the experimental baseline. The `attack` command implements Decomposition Attack, and `defend` implements Control Obfuscation, from Chao and Chen's ICCD 2025 paper identified above. The earlier RevC-Lock method is credited to its original authors.

## Citation

If you use this implementation in research, please cite the associated paper:

```bibtex
@inproceedings{chao2025decomposition,
  author    = {Feng-Jie Chao and Yung-Chih Chen},
  title     = {Decomposition Attack on Structural Logic Locking of Reversible Circuits},
  booktitle = {2025 IEEE 43rd International Conference on Computer Design (ICCD)},
  year      = {2025},
  pages     = {646--653},
  doi       = {10.1109/ICCD65941.2025.00098}
}
```

The same paper is recorded in [CITATION.cff](CITATION.cff). The research contributions and upstream tools are credited separately in [AUTHORS.md](_internal/AUTHORS.md) and [THIRD_PARTY_NOTICES.md](_internal/THIRD_PARTY_NOTICES.md).

Developers can enter `_internal/` and use the [technical README](_internal/README.md), build configuration, and maintenance scripts.
