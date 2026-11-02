# Usage and troubleshooting

Run every command in this guide from the repository root, alongside `setup.py`, `revc-tool`, and `reproduce.py`. See the [README](../README.md) for installation and the basic workflow.

## Supported inputs and outputs

- Input circuits use flat RevLib `.real` format with `tN` NOT, CNOT, Toffoli, or MCT gates. Negative controls and bus metadata are supported.
- Encryption accepts 3 to 249 circuit lines. The legacy random seed is fixed at 2 for reproduction.
- `lock` and `defend` take an original circuit. Both include the final ESOP rewriting.
- `attack` takes a circuit in this implementation's locked layout. It can run without the original circuit.
- `verify` uses the paper's miter construction and MiniSat, including the original line ordering and extra-line handling.
- Output paths are chosen by the caller. Spaces and Unicode characters in paths are supported.

The implementation covers the legacy ICCD paper. The later journal variants are outside this package.

## Command options

| Option | Purpose |
| --- | --- |
| `--json` | Return structured operation results |
| `--timeout SECONDS` | Change the per-operation limit from the default 120 seconds |
| `--log-dir DIRECTORY` | Save operation output and available MiniSat results |
| `--build-dir DIRECTORY` | Select an alternative native build directory |
| `--force` | Replace an existing output file for `lock`, `attack`, or `defend` |
| `--reference ORIGINAL.real` | Also verify the recovered circuit when running `attack` |

For example:

```bash
python3 revc-tool attack outputs/locked.real outputs/recovered.real \
  --reference examples/demo.real --json --log-dir outputs/attack-log
```

`verify` returns exit code 0 for `equivalent`, 1 for `non_equivalent`, and 2 for an input or execution error. A completed attack can produce `non_equivalent`, especially against the defense; inspect the reported equivalence field separately from process completion.

Each operation uses its own subprocess and temporary directory. Use a different `--log-dir` for each concurrent job.

## Reproduction options

The default command downloads the 11 Table I inputs from RevLib and verifies their archived SHA-256 values:

```bash
python3 reproduce.py
```

Use a fresh result directory and a larger per-operation timeout when needed:

```bash
python3 reproduce.py --output outputs/table1-run2 --timeout 300
```

To continue an interrupted run:

```bash
python3 reproduce.py --output outputs/table1-run2 --timeout 300 --resume
```

If you have the original research archive, the exact archived inputs can be imported instead of downloaded:

```bash
python3 reproduce.py --from-archive /path/to/original-research.zip
```

To reproduce all 245 archived cases:

```bash
python3 reproduce.py --suite all --from-archive /path/to/original-research.zip \
  --output outputs/all-cases
```

Without `--from-archive`, the same `--suite all` option downloads inputs individually. The recorded clean Docker run exercised the online Table I downloads; the recorded 245-case validation used the exact archived inputs. Downloads stop if data are unavailable or checksums differ.

Inputs are cached under `data/benchmarks/`. Results default to `outputs/iccd-table1/`. Counts and circuit hashes are compared with the baseline; identical CPU times across machines are not an acceptance criterion. The recorded non-equivalence of the defense under this attack describes the paper's experiment.

## Offline source installation

An existing CirKit Git checkout can supply the exact pinned dependency revisions:

```bash
python3 setup.py --cache-root /path/to/cirkit
```

The cache must contain the commits in `_internal/dependencies.lock.json`, including the dependency checkouts under its `lib/` directory. Only source objects are exported; existing compiled libraries are not reused. A normal online setup needs no such cache.

## Troubleshooting

| Problem | Next step |
| --- | --- |
| Missing native executable | Run `python3 setup.py`, then `python3 revc-tool doctor` |
| Output already exists | Choose a new output path or add `--force` |
| Operation times out | Increase `--timeout` and keep logs with `--log-dir` |
| Build or download fails | Check the system packages and network access; keep the complete error output |
| Benchmark checksum differs | Preserve the error and use the exact original archive if available |
| Input format is rejected | Check the supported circuit format and line limits above |

For a bug report, include the command, OS, `python3 revc-tool doctor` output, and relevant logs. See [validation](../_internal/validation/VALIDATION.md) for the tested environments.
