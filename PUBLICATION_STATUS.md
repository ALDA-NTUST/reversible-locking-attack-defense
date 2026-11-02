# Publication status

This repository is the private preparation version maintained by **ALDA-NTUST**.

- The laboratory user confirmed the original developer's agreement to publication on 2026-10-31. That permission is recorded, not pending.
- On 2026-11-02 the user selected **MIT** for laboratory-owned code and confirmed publication and maintenance through the laboratory account.
- The exact copyright holder and copyright years have not yet been confirmed. The user asked to leave them pending. The hosting account is not automatically the copyright holder.
- Accordingly, `LICENSE` is currently a status notice, not a completed MIT grant. The MIT draft is retained under `_internal/docs/MIT_DRAFT.txt` for final attribution.
- Third-party files retain their own licenses, copyright statements and source attribution. The laboratory license selection does not replace them.
- The supplied manuscript is attributed to Feng-Jie Chao and Yung-Chih Chen. The original research archive is credited to Feng-Jie Chao. No institutional rights transfer, DOI or publication year is invented.

## Verified implementation

The legacy ICCD implementation is separated from the journal and Web versions. The three research functions, dependency revisions, fixed seed and numerical acceptance baselines are unchanged from the tested source ZIP.

The 2026-11-02 clean Ubuntu 22.04 Docker test installed development tools from Ubuntu, downloaded all six pinned dependencies from GitHub, compiled from source, passed all CLI/import tests, downloaded all 11 Table I inputs from RevLib, and passed every case. All 22 encrypted circuits matched the archived original-program checksums. See `_internal/validation/docker_online_report.md`.

The historical 245-case validation is also retained. Timings depend on the machine. GitHub Actions execution is separate from the local Docker evidence.

## Completing the public release

Fill in the confirmed rights-holder names/years and the laboratory's preferred formal maintenance name, finalize the MIT LICENSE, synchronize the notices/manifests, and run the complete release check. This does not ask for the original developer's publication permission again.

Raw RevLib circuits and their generated derivatives are not uploaded. Only the package-authored demo is included. Any future benchmark or binary distribution needs its own applicable distribution conditions.
