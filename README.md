# Reversible Logic Locking: Attack and Defense

Code for **[Decomposition Attack on Structural Logic Locking of Reversible Circuits](https://doi.org/10.1109/ICCD65941.2025.00098)**, by **Feng-Jie Chao and Yung-Chih Chen**, *ICCD 2025*, pp. 646-653.

Maintained by [Advanced Logic Design Automation Lab (ALDA)](https://sites.google.com/view/ntust-alda), National Taiwan University of Science and Technology.

## Install

Use 64-bit glibc Linux; clean installation has been tested on Ubuntu 20.04 and Ubuntu 22.04. Run all commands below from the extracted repository's root directory.

```bash
sudo apt-get update
sudo apt-get install -y git cmake g++ make python3 ca-certificates \
  libboost-filesystem-dev libboost-system-dev zlib1g-dev
python3 setup.py
python3 revc-tool doctor
```

Setup downloads pinned source dependencies, compiles the tools, and runs the tests. No Python pip packages are required.

## Use

```bash
# RevC-Lock reimplementation
python3 revc-tool lock examples/demo.real outputs/locked.real

# Decomposition Attack
python3 revc-tool attack outputs/locked.real outputs/recovered.real

# Check the recovered circuit against the original
python3 revc-tool verify examples/demo.real outputs/recovered.real

# Control Obfuscation defense: start from the original circuit
python3 revc-tool defend examples/demo.real outputs/defended.real
```

Replace `examples/demo.real` with your original `.real` circuit. Existing output files are preserved; use `--force` to replace one. See the [usage guide](docs/USAGE.md) for supported circuits, options, and troubleshooting.

## Reproduce Table I

```bash
python3 reproduce.py
```

This downloads and verifies the 11 RevLib inputs, runs both locking modes and the attack, and compares counts, circuit hashes, and equivalence outcomes with the archived baseline. Results are written to `outputs/iccd-table1/comparison.csv` and `summary.json`.

The clean Ubuntu 22.04 Docker test passed all 11 cases and matched all 22 encrypted-circuit hashes. A separate archived-input validation passed all 245 cases in both modes. CPU timings vary by machine. See the [validation record](docs/REPRODUCIBILITY.md) for the exact scope and evidence.

## Research attribution

| Implementation | Research source |
| --- | --- |
| `lock`: RevC-Lock reimplementation and experimental baseline | Nimisha Limaye, Muhammad Yasin, and Ozgur Sinanoglu, [Revisiting Logic Locking for Reversible Computing](https://doi.org/10.1109/ETS.2019.8791550), ETS 2019, pp. 1-6 |
| `attack`: Decomposition Attack | Feng-Jie Chao and Yung-Chih Chen, ICCD 2025 paper linked above |
| `defend`: Control Obfuscation | Feng-Jie Chao and Yung-Chih Chen, ICCD 2025 paper linked above |

RevC-Lock is credited to its original authors. The source provenance and upstream adaptations are recorded in [AUTHORS.md](_internal/AUTHORS.md) and [ADAPTATIONS.md](_internal/ADAPTATIONS.md).

## Citation and license

If you use this implementation in research, please cite:

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

[CITATION.cff](CITATION.cff) also records the paper and the original RevC-Lock reference. Laboratory-owned code is copyright (c) 2026 Advanced Logic Design Automation Lab (ALDA), NTUST, and distributed under the [MIT License](LICENSE). Third-party files retain their original licenses and copyright notices; see [third-party notices](_internal/THIRD_PARTY_NOTICES.md).

RevLib inputs are downloaded separately and are not assigned the laboratory software license. The bundled demo was written for this package.

## Further information

- [Usage and troubleshooting](docs/USAGE.md)
- [Maintainer guide](docs/MAINTENANCE.md)
- [Experimental reproduction](docs/REPRODUCIBILITY.md)

Report problems through this repository's Issues page, including the command, environment, and error output.
