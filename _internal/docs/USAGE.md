# Legacy tool usage

This guide covers the legacy ICCD implementation. Source adaptations are recorded in `provenance/`; attributed upstream adaptations are stored in `third_party_adapters/`. Run the commands below from `_internal/`. The repository root provides simpler wrappers documented in the main README.

## Install

The tool has been tested on 64-bit glibc Linux, including Ubuntu 20.04 and a clean Ubuntu 22.04 Docker environment. Install the system packages listed in the repository README, then run:

```bash
python3 scripts/setup.py
python3 revc-tool doctor
```

Setup downloads pinned upstream commits, verifies the selected source-file hashes, compiles the tools, and runs the tests. No Python pip packages are required. For offline installation, add `--cache-root /path/to/cirkit` to export the specified revisions from an existing Git source cache. This does not reuse compiled libraries from that cache.

## Research functions and verification

```bash
# RevC-Lock reimplementation, including the final ESOP rewriting
python3 revc-tool lock examples/demo.real outputs/locked.real

# Decomposition Attack; the original circuit is optional
python3 revc-tool attack outputs/locked.real outputs/recovered.real

# Compare the recovered circuit with the original
python3 revc-tool verify examples/demo.real outputs/recovered.real

# Control Obfuscation defense, including the final ESOP rewriting
python3 revc-tool defend examples/demo.real outputs/defended.real
```

Add `--reference examples/demo.real` to `attack` to check equivalence in the same invocation. The input to `defend` is the original circuit. Verification follows the original experiment's miter construction, line ordering, and comparison semantics.

Use `--json` for structured results, `--log-dir` to retain operation logs, and `--timeout` to change the default 120-second limit. Existing output files are preserved unless `--force` is specified. Each operation uses its own process and temporary directory to isolate MiniSat and EXORCISM scratch files.

## Reproduce the paper results

RevLib input circuits and their generated derivatives are obtained separately. Source URLs, filenames, and archived SHA-256 values are recorded in `experiments/iccd/benchmarks.json`. Download the Table I inputs and run the experiment:

```bash
python3 scripts/fetch_benchmarks.py
python3 scripts/reproduce.py --output outputs/table1
```

To import the original research archive, add `--from-archive /path/to/original-research.zip` to the fetch command. To run all 245 archived cases:

```bash
python3 scripts/fetch_benchmarks.py --suite all --from-archive /path/to/original-research.zip
python3 scripts/reproduce.py --suite all --output outputs/all-cases
```

Reproduction checks input and gate counts, encrypted-circuit hashes for both modes, and attack equivalence outcomes. To continue an interrupted run, reuse the output directory and add `--resume`; hashes of completed encrypted circuits are checked again.

The archived 245-case validation passed in both modes. A separate clean Ubuntu 22.04 Docker test downloaded all Table I inputs online and passed all 11 cases, matching all 22 encrypted-file hashes. See `validation/` for the evidence. CPU timings depend on the machine and are not required to equal the paper's milliseconds.

## Attribution and publication status

Research code and the native entry point are under `src/`. CirKit adaptations retain their original licenses under `third_party_adapters/`. Downloaded dependencies are stored in `.deps/` and excluded from the repository. PUMA and Z3 are not compiled or linked.

Original-developer permission to laboratory publication is confirmed. The user selected MIT and ALDA-NTUST laboratory-account maintenance; final copyright attribution remains pending. Third-party licenses remain separate. See [publication status](PUBLICATION_STATUS.md) and `../THIRD_PARTY_NOTICES.md`.
