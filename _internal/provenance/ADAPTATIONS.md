# Adaptations from the original experiment

`original_experiment.cpp` is byte-identical to the root experiment source in the original research ZIP. `legacy_changes.patch` records changes to `src/legacy_experiment.cpp`.

Changes to the laboratory experiment source:

1. Remove unused includes and the hard-coded `main()`; add explicit command dispatch in `src/native_main.cpp`.
2. Keep `Encryption_original_version()`, `Encryption_new_version()` and the Decomposition Attack bodies and their fixed seed=2 behavior.
3. Make the original-circuit reference optional for the post-attack equivalence check. The attack's segmentation/decomposition stage is unchanged.
4. Replace `delete` with `delete[]` for the two encryption arrays, and initialize the equivalence timing accumulator.
5. For line counts >=31, explicitly reproduce the legacy GCC/x86 integer key bound instead of relying on an overflowing float-to-int cast. This preserves the archived 245-case encrypted bytes while avoiding that undefined conversion. It does not enlarge the legacy key representation.

`third_party_adapters/cirkit/legacy_esop.cpp` is derived from these files at CirKit commit `8a098dae1f09f9e8af6e132b1fa76f05948689ed`:

- `src/classical/optimization/exorcism_minimization.cpp`: `exorcism_processor` and the cube-vector `exorcism_minimization()` overload.
- `addons/cirkit-addon-reversible/src/reversible/optimization/simplify.cpp`: `control_polarity_mask()`, `make_controls()`, `exorcism_merge_heuristic_add_cubes()` and `exorcism_merge_heuristic()`.

Their original MIT notice is retained. Unrelated BDD/GIA interfaces and the statistics-only timer were removed. The default scratch file changed from the global `/tmp/test.esop` to relative `exorcism.esop`; every process runs in its own temporary directory. The EXORCISM input conversion, parameters (quality=2, maximum starting cubes=20000, quantum-cost mode off), rewritten cube parsing and gate reconstruction remain the same.

Only the required upstream C++ sources and six ABC EXORCISM source files are compiled by the new project-owned CMake build. It does not invoke the historical mixed-license CirKit build scripts, and does not compile or link PUMA, Z3, CUDD implementation code or the full ABC shell. CUDD's two public headers are present only because the upstream reader's type declarations reference them. All actual dependency files are recorded in `dependencies.integrity.json`.

The historical local `simplify.cpp` contained an unrelated debug print in another function; that modification is not part of this adapter. No journal/restore-free code or Web wrapper was imported.
