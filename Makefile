# 設定編譯器與參數
CC = gcc
CFLAGS = -Wall -Wextra

# 最終要產生的執行檔名稱
TARGET = ctf_server

# 預設執行的指令
all: $(TARGET)

# 轉
$(TARGET): main.c auth.c i18n.c
	$(CC) $(CFLAGS) main.c auth.c i18n.c game.c village.c shop.c -o $(TARGET)
# ==========================================
# 【安全清理模式】(指令：make clean)
# 只刪除編譯出來的執行檔，絕對不會動到 users/ 資料庫！
# ==========================================
clean:
	rm -f $(TARGET)

# ==========================================
# 【危險重置模式】(指令：make reset)
# 如果你想把整個系統打掉重練，清空所有玩家資料，才使用這個指令
# ==========================================
reset:
	rm -f $(TARGET)
	rm -rf users/