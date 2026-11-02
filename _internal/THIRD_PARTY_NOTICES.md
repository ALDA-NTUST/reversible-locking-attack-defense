# Third-party sources and license scope

This repository distinguishes the laboratory research code from upstream infrastructure. The laboratory user has confirmed the original developer's permission to publish and the professor's instruction to attribute laboratory-owned code to Advanced Logic Design Automation Lab (ALDA), NTUST. That code is distributed under MIT. This does not alter any third-party license.

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

## Build and test tools

The following tools support installation, compilation, or verification. They are separate from the six pinned research dependencies above. On Ubuntu, Python, CMake, GCC/G++, GNU Make, and Git are installed from Ubuntu package repositories using the README's `apt-get` commands; the links below identify their upstream projects. These tools and the operating system are not included in this source repository.

| Tool or service | Official upstream source | Role |
| --- | --- | --- |
| Python / CPython | [python/cpython](https://github.com/python/cpython) | Command-line wrappers, dependency downloads, reproduction, and tests; standard library only |
| CMake / CTest | [Kitware CMake](https://gitlab.kitware.com/cmake/cmake) | Native build configuration and test orchestration |
| GCC / G++ | [GNU Compiler Collection](https://gcc.gnu.org/) | Compile the native research tool and MiniSat |
| GNU Make | [GNU Make](https://www.gnu.org/software/make/) | Execute the generated build rules |
| Git | [Git project](https://git-scm.com/) | Retrieve the exact upstream dependency revisions |
| Ubuntu | [Ubuntu](https://ubuntu.com/) and [official Ubuntu container image](https://hub.docker.com/_/ubuntu) | System packages and the clean Ubuntu 22.04 test environment |
| Docker Engine | [Docker Engine documentation](https://docs.docker.com/engine/) | Run the clean installation test; optional for ordinary use |
| GitHub Actions / checkout | [GitHub Actions](https://docs.github.com/en/actions) and [actions/checkout](https://github.com/actions/checkout) | CI compilation and CLI tests; `.github/workflows/build.yml` uses `ubuntu-22.04` and `actions/checkout@v4` |

System package versions depend on the selected Ubuntu repositories; they are not all pinned by `dependencies.lock.json`. The six research dependency commits and selected source-file hashes are pinned and verified. Docker image information and validation scope are recorded in `validation/docker_online_image.json` and `validation/docker_online_report.md`.

`dependencies.lock.json` fixes exact revisions. `dependencies.integrity.json` fixes every upstream file actually used by the validated build. The dependency fetcher checks these hashes and refuses altered files. `.deps/` is ignored and excluded from the source release archive.

The fetched CirKit source history contains other components and build scripts; their presence upstream is not a claim that they are laboratory-owned or covered by the same license. This package does not copy or invoke its historical GPL-marked CMake scripts. The narrow build does not compile or link PUMA (whose redistribution grant was unresolved), Z3, Readline, linenoise, or unrelated toolkits. It uses a new CMake configuration and retains the MIT notice on the explicitly adapted CirKit routines. This describes the selected build; it is not a blanket relicensing of the full CirKit repository.

RevLib input data are separately attributed in `experiments/iccd/benchmarks.json`. They are not bundled in the source release, not assigned the laboratory's software license, and not uploaded with generated derivatives while data redistribution terms remain unconfirmed. The manifest/import/download scripts allow reproduction using independently acquired data. The bundled `examples/demo.real` is a small demonstration circuit written for this package.

Research attribution: RevC-Lock is from N. Limaye et al., *Revisiting logic locking for reversible computing*, ETS 2019. The supplied ICCD manuscript describes the reimplementation, Decomposition Attack and Control Obfuscation. CirKit/RevKit, EXORCISM, MiniSat and RevLib should also be credited in research using them.

Before distributing precompiled executables or container images, review the complete selected binary and system-library distribution requirements. This candidate supplies source and a Docker recipe, not a prebuilt image or externally published binary.
