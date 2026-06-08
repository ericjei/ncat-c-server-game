#ifndef SHOP_H
#define SHOP_H

#include <stdbool.h>
#include "auth.h" // 必須引入，因為需要用到 UserProfile 玩家資料結構

// 定義商店最多可以賣幾種商品
#define MAX_SHOP_ITEMS 10 

// ==========================================
// 定義商品資料結構
// ==========================================
typedef struct {
    int id;               // 購買指令 (例如 1, 2, 3...)
    char name[50];        // 物品名稱 (例如 鐵劍、傷藥)
    int price;            // 價格
    int type;             // 物品類型：0 代表武器裝備(升級制)，1 代表消耗品(堆疊制)
    int effect_value;     // 效果值：對於武器代表提供的等級，對於傷藥代表未來可補的血量
} ShopItem;

// ==========================================
// 宣告商店模組提供的對外副程式 (供 game.c 呼叫)
// ==========================================
void render_shop(UserProfile *user, bool use_big5);
void update_shop(UserProfile *user, const char *command, bool use_big5);

#endif