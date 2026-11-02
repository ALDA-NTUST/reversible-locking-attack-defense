# Third-party sources and license scope

Laboratory-owned project code is attributed to Advanced Logic Design Automation Lab (ALDA), NTUST, and distributed under the root MIT License. Upstream code and benchmark inputs retain their original ownership and applicable terms.

| Component | Source and locked version | Role | License evidence |
| --- | --- | --- | --- |
| CirKit / RevKit 2.x | [msoeken/cirkit](https://github.com/msoeken/cirkit/tree/8a098dae1f09f9e8af6e132b1fa76f05948689ed), `8a098dae1f09f9e8af6e132b1fa76f05948689ed` | Circuit representation, RevLib IO and small utilities; attributed ESOP adapter | Retained MIT notices in the selected source files and [licenses/cirkit-mit.txt](licenses/cirkit-mit.txt) |
| ABC EXORCISM-4 | [msoeken/abc](https://github.com/msoeken/abc), `8a7293ce2ee021eb1c51c0b48a0a602c46c936fb` | Six EXORCISM implementation files plus required headers | [licenses/abc.txt](licenses/abc.txt), UC Regents permission; Alan Mishchenko credited in EXORCISM headers |
| MiniSat | [msoeken/minisat](https://github.com/msoeken/minisat), `01fae76b900ed3b6c90635dad5ba536475c8a52a` | Standalone SAT solver, compiled from the five selected source files | [licenses/minisat.txt](licenses/minisat.txt), original Een / Sorensson notices |
| CUDD | [msoeken/cudd-1](https://github.com/msoeken/cudd-1), `c2c820e1f4bb1554adeb14c77551dbd7a7c8140b` | Two public headers; no CUDD library is linked | [licenses/cudd.txt](licenses/cudd.txt), BSD-style conditions; individual headers also retain notices |
| kitty | [msoeken/kitty](https://github.com/msoeken/kitty), `77303a0402b07faf1e7c2bdb867f19aadc5ce49c` | Headers needed by upstream truth-table type declarations | [licenses/kitty.txt](licenses/kitty.txt), MIT |
| fmt (historical version) | [fmtlib/fmt](https://github.com/fmtlib/fmt), `398343897f98b88ade80bbebdcbe82a36c65a980` | Header-only formatting support | [licenses/fmt.txt](licenses/fmt.txt), historical BSD-style terms |
| Boost | System development package | Filesystem and C++ headers | [Boost Software License 1.0](https://www.boost.org/LICENSE_1_0.txt); supplied by system packages |
| zlib | System development package | MiniSat compressed-file input | [zlib license](https://zlib.net/zlib_license.html); supplied by system packages |

`dependencies.lock.json` fixes exact revisions. `dependencies.integrity.json` fixes every upstream file actually used by the validated build. The dependency fetcher checks these hashes and refuses altered files. `.deps/` is ignored and excluded from the source release archive.

The fetched CirKit source history contains other components and build scripts; their presence upstream is not a claim that they are laboratory-owned or covered by the same license. This package does not copy or invoke its historical GPL-marked CMake scripts. The narrow build does not compile or link PUMA (whose redistribution grant was unresolved), Z3, Readline, linenoise, or unrelated toolkits. It uses a new CMake configuration and retains the MIT notice on the explicitly adapted CirKit routines. This describes the selected build; it is not a blanket relicensing of the full CirKit repository.

RevLib input data are separately attributed in `experiments/iccd/benchmarks.json`. They are not bundled in the source release, not assigned the laboratory's software license, and not uploaded with generated derivatives while data redistribution terms remain unconfirmed. The manifest/import/download scripts allow reproduction using independently acquired data. The bundled `examples/demo.real` is a small demonstration circuit written for this package.

Research attribution: RevC-Lock was introduced by Nimisha Limaye, Muhammad Yasin, and Ozgur Sinanoglu, [Revisiting Logic Locking for Reversible Computing](https://doi.org/10.1109/ETS.2019.8791550), ETS 2019, pp. 1-6. This project provides a research reimplementation of that baseline. The supplied ICCD manuscript describes the reimplementation, Decomposition Attack and Control Obfuscation. CirKit/RevKit, EXORCISM, MiniSat and RevLib should also be credited in research using them.

Before distributing precompiled executables or container images, review the complete selected binary and system-library distribution requirements. This repository distributes source and verification records; it does not distribute a prebuilt image or executable.
