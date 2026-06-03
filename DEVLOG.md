# 疑難排解與開發日誌 (Troubleshooting Log)

## 紀錄時間：2026-06
## 事件：VMware 虛擬機 VMX 進程卡死與網路斷線修復

### 🚨 問題描述 (Issue)
1. 虛擬機內部網路突然斷線，`ping 8.8.8.8` 顯示 `Destination Host Unreachable`。
2. 嘗試使用 `sudo reboot` 強制重啟後，VMware 崩潰。
3. 再次啟動虛擬機時，跳出嚴重錯誤：**「VMX 進程已提前退出 (VMX process has exited prematurely)」**，虛擬機徹底被鎖死無法開機。

### 🔍 根本原因 (Root Cause)
* **網路斷線起因：** Windows 實體機休眠喚醒或切換網卡，導致 VMware 的虛擬網卡 (NAT) 失去對外連線能力。
* **進程死鎖起因：** 在網路異常且系統未完全喚醒的狀態下強制重啟，導致 VMware 嘗試寫入/讀取休眠狀態的虛擬記憶體暫存檔 (`.vmem`) 時發生資料損毀。隨後，Windows 實體機的核心層級 (Kernel) 產生了檔案鎖定，將崩潰的 VMX 進程死鎖，導致常規的重開 VMware 均無效。

### 🛠️ 解決方案 (Solution)
遇到此類嚴重死鎖，需執行「重置三連擊」：
1. **清理損毀檔：** 完全關閉 VMware，進入虛擬機實體資料夾，刪除所有 `.vmem` (記憶體暫存檔) 與 `.lck` (鎖定檔資料夾)。⚠️ *注意：絕對不可動到 `.vmdk` 與 `.vmx`。*
2. **解除核心死鎖 (最關鍵)：** 直接將 Windows 實體機「重新開機」，強制 Windows 釋放所有被卡住的記憶體與驅動程式鎖定。
3. **重新掛載環境：** 開機後，在 VMware 清單中將該虛擬機「移除 (Remove)」，接著透過「開啟 (Open)」重新載入 `final.vmx`，洗掉錯誤的 UI 快取，即可順利正常開機。