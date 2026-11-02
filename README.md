# Reversible Logic Locking: Attack and Defense

**Private preparation version maintained by ALDA-NTUST.** MIT is selected; copyright attribution remains pending. See [publication status](PUBLICATION_STATUS.md).

The original ICCD implementation of RevC-Lock, Decomposition Attack and Control Obfuscation, associated with the manuscript by Feng-Jie Chao and Yung-Chih Chen. This user-facing layout retains the validated implementation and its third-party notices under `_internal/`.

這份是舊版工具的使用入口。一般使用者只需看本文件，並使用 `setup.py`、`revc-tool`、`reproduce.py`。

## 1. Install / 安裝

Use 64-bit glibc Linux. Install the system build tools on Ubuntu/Debian:

```bash
sudo apt-get update
sudo apt-get install -y git cmake g++ make python3 ca-certificates \
  libboost-filesystem-dev libboost-system-dev zlib1g-dev
python3 setup.py
python3 revc-tool doctor
```

The installer fetches fixed upstream versions, builds the native tools and runs tests. Python needs no pip packages. Offline source-cache setup is available as `python3 setup.py --cache-root /path/to/cirkit`.

## 2. Use the three functions / 三個功能

```bash
# RevC-Lock encryption / 加密
python3 revc-tool lock examples/demo.real outputs/locked.real

# Attack / 攻擊還原
python3 revc-tool attack outputs/locked.real outputs/recovered.real

# Verify the recovered circuit / 驗證還原結果
python3 revc-tool verify examples/demo.real outputs/recovered.real

# Legacy paper defense / 論文防禦，輸入原始電路
python3 revc-tool defend examples/demo.real outputs/defended.real
```

Replace `examples/demo.real` with your original `.real` circuit. Each operation writes to the output path you specify. Existing files are preserved; use `--force` to overwrite. `attack --reference ORIGINAL.real` also performs the equivalence check. `--json` provides structured output.

## 3. Reproduce Table I / 重現論文

```bash
python3 reproduce.py
```

This obtains the 11 RevLib input circuits, verifies their archived SHA-256 values, runs both encryption modes and the attack, and compares against the paper. Results are written to `outputs/iccd-table1/comparison.csv` and `summary.json`.

If you have the original research ZIP, use the tested offline data-import path:

```bash
python3 reproduce.py --from-archive /path/to/original-research.zip
```

For all 245 inputs add `--suite all` and select a fresh `--output`. To continue an interrupted run, add `--resume`.

## Where things are / 檔案放哪裡

| Item | Purpose |
| --- | --- |
| `setup.py` | Install/build once / 安裝 |
| `revc-tool` | `lock`, `attack`, `defend`, `verify` / 操作 |
| `reproduce.py` | Fetch/import inputs and rerun the paper / 重現論文 |
| `examples/` | Small demonstration circuit / 範例 |
| `outputs/` | Your generated results; created on use / 執行後產生的結果 |
| `_internal/` | Implementation, fixed dependencies, tests, attribution and validation / 程式與技術紀錄 |

The algorithm, dependency versions and acceptance baseline are unchanged. The original preparation validated 245 cases in both encryption modes and matched all 490 encrypted files byte-for-byte; see [validation](_internal/validation/VALIDATION.md). CPU timings vary by machine.

Original-developer permission to laboratory publication is confirmed. MIT has been selected, with final copyright attribution pending; see [LICENSE](LICENSE) and [publication status](PUBLICATION_STATUS.md). Third-party licenses and original authors remain preserved in [third-party notices](_internal/THIRD_PARTY_NOTICES.md) and [authors](_internal/AUTHORS.md). RevLib benchmark data are obtained separately and are not bundled as laboratory-owned data.

On 2026-11-02 the supplied implementation passed a clean Ubuntu 22.04 Docker test with online installation, all six pinned GitHub dependencies and all 11 RevLib Table I inputs downloaded from their upstream sources. All cases passed and all 22 encrypted-file checksums matched. See the [Docker report](_internal/validation/docker_online_report.md). GitHub Actions execution is separate from this Docker evidence. Changed benchmark content is rejected by checksum verification.

Developers can enter `_internal/` and use its original [technical README](_internal/README.md), build configuration and maintenance scripts.
