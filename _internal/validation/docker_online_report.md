# Clean Docker online installation: passed

On 2026-11-02 the user source ZIP with SHA-256 `153478cba5527141360e48c82c91dffa7eb89b781d3342186cc1eae1450c64ed` was unpacked into a fresh official Ubuntu 22.04 container. Git, CMake, G++, Make and Python were confirmed absent before apt installed them. No research archive, source cache, host home or previous binaries were supplied.

All six pinned dependencies were downloaded from GitHub and compiled. Both CTest suites (seven Python tests) passed; doctor, lock, attack, independent equivalence checking and defend passed. All 11 Table I circuits were downloaded from RevLib and verified against the archived source hashes. Table I passed 11/11 and all 22 encrypted outputs matched the archived original-program hashes. CPU timings were measured, not required to equal the manuscript.

See docker_online_summary.json, docker_online_comparison.csv and docker_online_image.json. The historical 245-case test is separate and was not repeated in this Docker run.

The GitHub preparation adds publication documentation and updates the release checker's pending-attribution diagnostic; it does not change the research code, CLI runtime, installer, dependency pins or numerical baselines. No GitHub Actions success is claimed here.
