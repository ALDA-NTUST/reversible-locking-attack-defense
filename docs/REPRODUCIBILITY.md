# Reproducing the paper

The implementation accompanies Feng-Jie Chao and Yung-Chih Chen, [Decomposition Attack on Structural Logic Locking of Reversible Circuits](https://doi.org/10.1109/ICCD65941.2025.00098), ICCD 2025. The original RevC-Lock method is credited separately in the [README](../README.md).

## Run Table I

After installation, run from the repository root:

```bash
python3 reproduce.py
```

The command downloads the 11 RevLib inputs, verifies their SHA-256 values, runs RevC-Lock and Control Obfuscation, attacks both outputs, and checks the results against the archived baseline. Both locking modes include final ESOP rewriting, with random seed 2.

| Output | Contents |
| --- | --- |
| `outputs/iccd-table1/comparison.csv` | Per-case input/gate counts, timings, circuit hashes, equivalence outcomes, and acceptance checks |
| `outputs/iccd-table1/summary.json` | Completed case count and overall acceptance result |
| `outputs/iccd-table1/locked/`, `defense/` | Generated encrypted circuits |
| `outputs/iccd-table1/logs/` | Operation and solver logs |

All cases should report `passed: true`. RevC-Lock attack results should be equivalent to the original; defense attack results should match the recorded non-equivalence outcomes. CPU timings are measured again and are not required to equal the paper's times.

## Included baselines

- `_internal/experiments/iccd/table1.json`: the 11 paper cases and published counts/overheads.
- `_internal/experiments/iccd/benchmarks.json`: URLs and SHA-256 values for 245 archived original inputs.
- `_internal/experiments/iccd/expected_all.json`: expected counts, encrypted-circuit hashes, and attack outcomes for those 245 cases.

These are reproduction inputs, not temporary test results. Original RevLib circuit files are downloaded separately and are not assigned the laboratory software license.

## Verification scope

The recorded full archived-input validation passed 245/245 cases in both modes and matched all 490 encrypted-circuit hashes. Clean Ubuntu 22.04 online installation tests passed the seven automated test methods, all 11 Table I cases, and all 22 encrypted-file hashes. Historical run logs are kept outside the source distribution. Current build, test, and Table I results are available in the repository's Actions tab.

The 245-case validation used the exact archived input set. Online downloads of the 11 Table I inputs were exercised. Download or checksum failures stop the run; changed upstream data are not silently accepted. For alternate output directories, archive import, all-case runs, and resume options, see [USAGE.md](USAGE.md).

Verification follows the original miter/MiniSat construction and line-order semantics. The defense outcomes describe the paper's decomposition-attack experiments. Tested environments are 64-bit glibc Linux, including Ubuntu 20.04 and Ubuntu 22.04; Docker tests share the host Linux kernel.
