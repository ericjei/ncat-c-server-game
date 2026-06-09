#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "auth.h"
#include "i18n.h"
#include "village.h"
#include "shop.h"
#include "battle.h"

// 定義場景描述函式 (Render 階段的一部分)
void render_scene(UserProfile *user, bool use_big5) {
    safe_printf(use_big5, "\n========================================\n");
    if (user->map_id == 0) {
        safe_printf(use_big5, "【 冒險者大廳 】\n");
        safe_printf(use_big5, "這裡燈火通明，氣氛熱絡。北邊 (w) 是通往未知區域的大門。\n");
        safe_printf(use_big5, "狀態 -> HP: %d | 金幣: %d\n", user->hp, user->gold);
        safe_printf(use_big5, "指令 -> [w] 向北移動 | [rest] 休息 | [v] 前往村落 | [exit] 離開遊戲\n");
    } else if (user->map_id == 1) {
        safe_printf(use_big5, "【 黑暗荒野 】\n");
        safe_printf(use_big5, "四周陰暗寒冷，不時傳來野獸的低吼。南邊 (s) 可以回到大廳。\n");
        safe_printf(use_big5, "狀態 -> HP: %d | 金幣: %d\n", user->hp, user->gold);
        safe_printf(use_big5, "指令 -> [s] 向南移動 | [search] 探索四周  | [battle] 戰鬥爽| [bag] 開啟背包| [exit] 離開遊戲\n");
    }else if (user->map_id == 2) {
        // 呼叫外部 village.c 提供的副程式
        render_village(user, use_big5);
    safe_printf(use_big5, "========================================\n");
    safe_printf(use_big5, "請輸入你的行動：");
    }
    else if (user->map_id == 3) {
        render_shop(user, use_big5);
    }
    else if (user->map_id == 4) {
        render_battle(user, use_big5);
    }
}

// 遊戲主循環
void run_game_loop(UserProfile *user, bool use_big5) {
    char command[50];
    bool is_playing = true;

    safe_printf(use_big5, "\n>> 正在載入遊戲世界，準備進行連線...\n");

    // 進入無窮迴圈：這就是 Game Loop！
    while (is_playing) {
        
        // 階段 1：Render (畫面渲染)
        render_scene(user, use_big5);

        // 階段 2：Input (讀取輸入)
        if (!fgets(command, sizeof(command), stdin)) return;
        command[strcspn(command, "\r\n")] = 0; // 去除換行符號

        safe_printf(use_big5, "\n"); // 排版用空行

        // 階段 3：Update (邏輯更新)
        // 處理全域系統指令
        if (strcmp(command, "exit") == 0) {
            safe_printf(use_big5, ">> 儲存進度並退出遊戲世界...\n");
            is_playing = false; // 改變狀態，準備跳出迴圈
            continue;
        }
        else if (strcmp(command, "bag") == 0) {  // <--- 新增這裡
            safe_printf(use_big5, "\n--- 背包 ---");
            safe_printf(use_big5, "金幣: %d 枚", user->gold);
            safe_printf(use_big5, "-----------\n");
            continue;
        }

        // 處理大地圖邏輯
        if (user->map_id == 0) { // 在大廳
            if (strcmp(command, "w") == 0) {
                user->map_id = 1; // 改變位置狀態
                safe_printf(use_big5, ">> 你推開沉重的大門，朝北方的荒野走去...\n");
            }else if (strcmp(command, "v") == 0) {
                user->map_id = 2; 
                safe_printf(use_big5, ">> 你沿著小徑漫步，來到了寧靜的村落。\n");
            } else if (strcmp(command, "rest") == 0) {
                user->hp = 100;
                safe_printf(use_big5, ">> 你在溫暖的火爐旁休息，生命值完全恢復了！\n");
            } else {
                safe_printf(use_big5, ">> 無效的指令，或者你不能在這裡這樣做。\n");
            }
        } 
        else if (user->map_id == 1) { // 在荒野
            if (strcmp(command, "s") == 0) {
                user->map_id = 0; // 改變位置狀態
                safe_printf(use_big5, ">> 你匆忙往南走，回到了安全的大廳。\n");
            } else if (strcmp(command, "search") == 0) {
                int base_gold = (rand() % 11+(user->luck * 2)) ;
                user->gold += base_gold; // 更新資源狀態
                safe_printf(use_big5, ">> 你在樹叢中找到了一個破舊的錢袋，獲得了 %d 枚金幣！\n",base_gold);
                safe_printf(use_big5, ">> 目前總金幣: %d\n",user->gold);
            } 
            else if(strcmp(command, "battle") == 0){
                user->map_id = 4;
                safe_printf(use_big5, ">> 前往戰鬥\n");
            }
            else {
                safe_printf(use_big5, ">> 無效的指令，或者你不能在這裡這樣做。\n");
            }
        }
        else if (user->map_id == 2) { // 村落邏輯
            update_village(user, command, use_big5);
        }
        else if (user->map_id == 3) {
            update_shop(user, command, use_big5);
        }
        else if (user->map_id == 4) {
            update_battle(user, command, use_big5);
        }
        // --- 每次邏輯更新完畢後，你可以在這裡呼叫存檔函式 ---
        // save_user_to_file(user); 
    }
}