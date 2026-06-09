#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "village.h"
#include "i18n.h" // 需要用到 safe_printf

// 1. 村落畫面渲染副程式
void render_village(UserProfile *user, bool use_big5) {
    safe_printf(use_big5, "【 寧靜村落 】\n");
    safe_printf(use_big5, "充滿生活氣息的地方，路邊有老人在曬太陽。東邊 (e) 可以回到大廳。\n");
    safe_printf(use_big5, "目前狀態 -> HP: %d | 金幣: %d | 裝備等級: %d | 傷藥: %d\n", user->hp, user->gold, user->weapon_level, user->potions);
    safe_printf(use_big5, "----------------------------------------\n");
    safe_printf(use_big5, "指令 -> [e] 回到大廳 | [shop] 進入道具屋 | [l] 進入排行 | [pray] 祈禱(花費10金幣提升幸運) | [exit] 離開遊戲\n");
}

// 2. 村落邏輯更新副程式
void update_village(UserProfile *user, const char *command, bool use_big5) {
    if (strcmp(command, "e") == 0) {
        user->map_id = 0; // 狀態變更：切換回大廳
        safe_printf(use_big5, ">> 你走回了繁華的冒險者大廳。\n");
    } 
    else if (strcmp(command, "shop") == 0) { // 【新增】：進入商店
        user->map_id = 3; // 切換狀態至商店
        safe_printf(use_big5, ">> 你推開木門，走進了充滿魔法氣息的道具屋。\n");
        
    }
    else if (strcmp(command, "pray") == 0) {
        if (user->gold >= 10) {
            user->gold -= 10;
            user->luck += 1;
            safe_printf(use_big5, ">> 你誠心地祈禱，感覺身體輕盈了些！(幸運 +1, 金幣 -10)\n");
        } else {
            safe_printf(use_big5, ">> 你的金幣不足，老神父慈祥地對你搖了搖頭。\n");
        }
    } 
    else if (strcmp(command, "l") == 0) {
        user->map_id = 5;
    }else {
        safe_printf(use_big5, ">> 這裡沒有這個動作。\n");
    }
}