# Publication status

The source release is maintained by **Advanced Logic Design Automation Lab (ALDA), National Taiwan University of Science and Technology**, through the **ALDA-NTUST** GitHub account.

## Copyright, authors, and license

- Laboratory-owned project code is attributed to Advanced Logic Design Automation Lab (ALDA), NTUST, and distributed under the [MIT License](../LICENSE).
- This attribution follows the laboratory user's 2026-11-02 report of the professor's instruction to attribute the project to the laboratory. The developer's agreement to laboratory publication was already confirmed on 2026-10-31.
- Paper authors **Feng-Jie Chao** and **Yung-Chih Chen** remain credited in the README, [author record](../AUTHORS.md), and [citation file](../CITATION.cff). The original research archive remains credited to Feng-Jie Chao.
- Third-party copyright statements, licenses, and adaptation records remain in place. Laboratory attribution does not replace upstream ownership or apply to RevLib benchmark data. See [third-party notices](../THIRD_PARTY_NOTICES.md).
- The copyright year 2026 identifies this source release. The associated paper was published in 2025.

## Associated paper

**Feng-Jie Chao and Yung-Chih Chen.** [Decomposition Attack on Structural Logic Locking of Reversible Circuits](https://doi.org/10.1109/ICCD65941.2025.00098). In *2025 IEEE 43rd International Conference on Computer Design (ICCD)*, pp. 646-653, 2025. DOI: `10.1109/ICCD65941.2025.00098`.

The title, authors, venue, year, pages, and DOI were checked against the publisher's registered Crossref metadata and the supplied manuscript. The laboratory website also lists this ICCD 2025 paper.

## Verification and scope

The legacy ICCD research functions, dependency revisions, fixed seed, and numerical acceptance baselines are unchanged. The clean Ubuntu 22.04 Docker test installed tools and downloaded all six dependencies and all 11 Table I inputs online. All cases passed; all 22 encrypted circuits matched the archived original-program hashes. The separate 245-case validation is retained.

Only source, documentation, notices, and verification records are provided. Original RevLib inputs and generated derivatives are not included, apart from the package-authored demonstration circuit. See the validation directory for the recorded scope; CPU timings vary by machine.

The repository remains private. Completing attribution and licensing does not change repository visibility or publish a binary or container image.
