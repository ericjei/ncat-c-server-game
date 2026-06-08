#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "shop.h"
#include "i18n.h"

// ==========================================
// 讀取商店物品清單的核心副程式 (僅供 shop.c 內部使用)
// ==========================================
static int load_shop_items(ShopItem items[]) {
    FILE *fp = fopen("shop_items.txt", "r");
    if (fp == NULL) return 0; // 找不到檔案時，回傳商品數量為 0

    int count = 0;
    char line[100];
    
    // 一行一行讀取，直到檔案結束或達到數量上限
    while (fgets(line, sizeof(line), fp) && count < MAX_SHOP_ITEMS) {
        // 讀取 5 個欄位：編號, 名稱, 價格, 類型(0:裝備/1:消耗品), 效果值
        if (sscanf(line, "%d,%[^,],%d,%d,%d", 
            &items[count].id, 
            items[count].name, 
            &items[count].price, 
            &items[count].type, 
            &items[count].effect_value) == 5) {
            count++; 
        }
    }
    fclose(fp);
    return count; 
}

// ==========================================
// 渲染商店畫面
// ==========================================
void render_shop(UserProfile *user, bool use_big5) {
    safe_printf(use_big5, "【 村莊道具屋 】\n");
    safe_printf(use_big5, "老闆娘：『冒險者，看看有什麼需要的？』\n");
    safe_printf(use_big5, "目前裝備等級: %d | 傷藥數量: %d | 金幣: %d\n", user->weapon_level, user->potions, user->gold);
    safe_printf(use_big5, "----------------------------------------\n");

    ShopItem items[MAX_SHOP_ITEMS];
    int item_count = load_shop_items(items);

    if (item_count == 0) {
        safe_printf(use_big5, "老闆娘：『哎呀，今天物流沒來，還沒進貨呢！』\n");
    } else {
        for (int i = 0; i < item_count; i++) {
            // 根據類型顯示不同的說明文字
            char type_desc[20];
            if (items[i].type == 0) strcpy(type_desc, "裝備");
            else strcpy(type_desc, "消耗品");

            safe_printf(use_big5, "指令 -> [%d] 購買%s (%s) - %d 金幣\n", 
                        items[i].id, items[i].name, type_desc, items[i].price);
        }
    }
    safe_printf(use_big5, "----------------------------------------\n");
    safe_printf(use_big5, "指令 -> [leave] 離開道具屋\n");
}

// ==========================================
// 處理商店購買邏輯
// ==========================================
void update_shop(UserProfile *user, const char *command, bool use_big5) {
    if (strcmp(command, "leave") == 0) {
        user->map_id = 2; // 退回村莊
        safe_printf(use_big5, ">> 你離開了道具屋，回到村莊廣場。\n");
        return;
    } 

    ShopItem items[MAX_SHOP_ITEMS];
    int item_count = load_shop_items(items);
    int choice = atoi(command); // 將玩家輸入轉為數字 ID
    bool found = false;

    for (int i = 0; i < item_count; i++) {
        if (items[i].id == choice) { 
            found = true;
            
            // 檢查金幣是否足夠
            if (user->gold < items[i].price) {
                safe_printf(use_big5, ">> 老闆娘：『哎呀，你的金幣不夠喔！』\n");
                break;
            }

            // 【邏輯 1】：類型 0 (武器裝備 - 只能買一次/升級)
            if (items[i].type == 0) {
                if (user->weapon_level < items[i].effect_value) {
                    user->gold -= items[i].price;
                    user->weapon_level = items[i].effect_value;
                    safe_printf(use_big5, ">> 購買成功！你裝備了【%s】，戰鬥力提升了！\n", items[i].name);
                } else {
                    safe_printf(use_big5, ">> 老闆娘：『你手上的裝備已經比這個好了，別浪費錢！』\n");
                }
            } 
            // 【邏輯 2】：類型 1 (消耗品 - 可無限購買)
            else if (items[i].type == 1) {
                user->gold -= items[i].price;
                user->potions += 1; 
                safe_printf(use_big5, ">> 購買成功！你將【%s】放入了背包。(目前持有: %d 瓶)\n", items[i].name, user->potions);
            }
            
            break; // 處理完畢就跳出迴圈
        }
    }

    if (!found) {
        safe_printf(use_big5, ">> 老闆娘：『我聽不懂你在說什麼？或是這個商品賣完啦。』\n");
    }
}