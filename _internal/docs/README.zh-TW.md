# 舊版工具使用說明

這份目錄只包含原始 ZIP 對應的 ICCD 舊版，不含 journal 的 restore-free、新攻擊版本或 Web 系統。原始演算法的來源與介面調整保存在 `provenance/`，第三方改寫程式則獨立放在 `third_party_adapters/`。

## 安裝

目前實際驗證的是 64-bit glibc Linux、GCC 9.4、Boost 1.71。先依根目錄 README 安裝系統套件，再執行：

```bash
python3 scripts/setup.py
python3 revc-tool doctor
```

這會取得固定 commit 的第三方來源、檢查使用到的檔案雜湊、編譯並執行測試。Python 不需要額外 pip 套件。若無網路，可透過 `--cache-root /path/to/cirkit` 從含有指定 commit 的 Git 快取取得來源；不會借用該目錄已編好的函式庫。

## 三個研究功能與驗證

```bash
# 原版 RevC-Lock，含最後 ESOP 改寫
python3 revc-tool lock examples/demo.real outputs/locked.real

# 原始 Decomposition Attack，不必提供原始電路
python3 revc-tool attack outputs/locked.real outputs/recovered.real

# 用原始電路確認還原結果
python3 revc-tool verify examples/demo.real outputs/recovered.real

# 舊版論文的 Control Obfuscation 防禦，含最後 ESOP 改寫
python3 revc-tool defend examples/demo.real outputs/defended.real
```

也能在 attack 指令加上 `--reference examples/demo.real` 一起驗證。Defense 的輸入是原始電路，不是現成的 RevC-Lock 電路。`verify` 使用原論文 miter 的線路順序與比較語意；它不是任意量子電路的通用驗證器。

`--json` 輸出機器可讀的結果，`--log-dir` 保存紀錄，`--timeout` 調整預設 120 秒時限。輸出檔存在時會拒絕覆寫，需明確使用 `--force`。每次工作有獨立程序與暫存目錄，避免 MiniSat 或 EXORCISM 檔案碰撞。

## 論文重現

RevLib 原始電路及其衍生電路不隨此來源包散布；資料來源、檔名與原始 SHA-256 在 `experiments/iccd/benchmarks.json`。可由上游下載，或匯入手上的原始研究 ZIP：

```bash
python3 scripts/fetch_benchmarks.py --from-archive /path/to/original-research.zip
python3 scripts/reproduce.py --output outputs/table1
```

重跑全部 245 個原始案例：

```bash
python3 scripts/fetch_benchmarks.py --suite all --from-archive /path/to/original-research.zip
python3 scripts/reproduce.py --suite all --output outputs/all-cases
```

結果會核對加密前後的 input／gate 數、兩種加密模式產出的電路雜湊，以及攻擊的等價結果。中斷後可使用同一輸出目錄並加 `--resume`；已完成的加密電路會重新檢查雜湊。

本次全 245 個案例的兩種模式都已通過，Table I 的 11 列也全部一致。實際驗證紀錄在 `validation/`。執行時間受環境影響，不要求與論文毫秒數相同。

## 發布範圍與待確認事項

`src/` 是實驗室研究程式與入口，`third_party_adapters/` 明確保留 CirKit 授權，`.deps/` 是固定版本下載的上游來源，不放入發布 ZIP。PUMA 與 Z3 沒有編譯或連結到此工具。

使用者已確認：撰寫三個功能的學長同意由實驗室公開，這項許可已記錄為確認。目前只待選定主專案 LICENSE 的具體條款；尚未自行指定授權，也沒有直接上傳 GitHub。詳細狀態見 `PUBLICATION_STATUS.md`。Docker 與線上下載路徑受準備環境限制，尚未完成真實外部環境測試；離線來源建置、原始 ZIP 匯入及實驗重現已實測。
