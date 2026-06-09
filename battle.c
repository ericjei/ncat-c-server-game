#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "battle.h"
#include "i18n.h"

// --- 遭遇戰狀態管理 (確保重新 render 時怪物不會洗牌) ---
static Monster current_enemies[3];
static int current_enemy_count = 0;
static bool in_encounter = false;

// 動態讀取怪物清單
static int load_monsters(Monster db[]) {
    FILE *fp = fopen("monsters.txt", "r");
    if (fp == NULL) return 0;

    int count = 0;
    char line[100];
    
    while (fgets(line, sizeof(line), fp) && count < MAX_MONSTERS) {
        if (sscanf(line, "%d,%[^,],%d,%d,%d,%d", 
            &db[count].id, 
            db[count].name, 
            &db[count].req_weapon, 
            &db[count].hp_cost, 
            &db[count].min_gold, 
            &db[count].max_gold) == 6) {
            
            db[count].is_dead = false; 
            count++; 
        }
    }
    fclose(fp);
    return count; 
}

// 隨機生成 1~3 隻怪物
static void generate_encounter() {
    Monster db[MAX_MONSTERS];
    int db_count = load_monsters(db);
    
    if (db_count == 0) return;

    current_enemy_count = (rand() % 3) + 1; // 隨機 1~3 隻
    for (int i = 0; i < current_enemy_count; i++) {
        int idx = rand() % db_count;
        current_enemies[i] = db[idx];
        current_enemies[i].id = i + 1; // 覆寫 ID 為 1, 2, 3，方便玩家輸入
    }
    in_encounter = true;
}

void render_battle(UserProfile *user, bool use_big5) {
    if (!in_encounter) {
        generate_encounter();
    }

    safe_printf(use_big5, "【 危機四伏的荒野 】\n");
    safe_printf(use_big5, "冷風吹過，草叢中傳來陣陣低吼聲...\n");
    safe_printf(use_big5, "目前狀態 -> HP: %d | 金幣: %d | 武器等級: %d | 傷藥: %d\n", 
                user->hp, user->gold, user->weapon_level, user->potions);
    safe_printf(use_big5, "----------------------------------------\n");

    int alive_count = 0;
    safe_printf(use_big5, "⚠️ 你遭遇了 %d 隻怪物！\n", current_enemy_count);
    
    for (int i = 0; i < current_enemy_count; i++) {
        if (!current_enemies[i].is_dead) {
            // 畫面上依然顯示需求，但這只是參考，程式不會鎖死玩家
            safe_printf(use_big5, "指令 -> [%d] 討伐 %s (建議武器 Lv.%d | 消耗 %d HP | 獎勵 %d~%d 金幣)\n", 
                        current_enemies[i].id, 
                        current_enemies[i].name, 
                        current_enemies[i].req_weapon, 
                        current_enemies[i].hp_cost, 
                        current_enemies[i].min_gold, 
                        current_enemies[i].max_gold);
            alive_count++;
        } else {
            safe_printf(use_big5, "        [%d] %s (💀 已擊倒)\n", current_enemies[i].id, current_enemies[i].name);
        }
    }

    if (alive_count == 0) {
        safe_printf(use_big5, "\n🎉 附近的怪物都被你清理乾淨了！\n");
        safe_printf(use_big5, "指令 -> [search] 繼續尋找下一批怪物\n");
    }

    safe_printf(use_big5, "----------------------------------------\n");
    safe_printf(use_big5, "指令 -> [h] 喝下傷藥 (消耗 1 瓶，恢復 50 HP)\n");
    safe_printf(use_big5, "指令 -> [leave] 逃回村莊\n");
}

void update_battle(UserProfile *user, const char *command, bool use_big5) {
    if (strcmp(command, "leave") == 0) {
        user->map_id = 2; // 退回村莊
        in_encounter = false; 
        safe_printf(use_big5, ">> 你頭也不回地跑回了安全的村莊。\n");
        return;
    } 
    
    if (strcmp(command, "search") == 0) {
        in_encounter = false; 
        safe_printf(use_big5, ">> 你往荒野深處走去，尋找新的獵物...\n");
        return;
    }

    // 【喝藥水邏輯】
    if (strcmp(command, "h") == 0) {
        if (user->potions > 0) {
            user->potions -= 1;
            user->hp += 50;
            if (user->hp > 100) user->hp = 100; 
            safe_printf(use_big5, ">> 咕嚕咕嚕... 你喝下了傷藥，HP 恢復了！(目前 HP: %d)\n", user->hp);
        } else {
            safe_printf(use_big5, ">> 你伸手往背包一摸... 糟糕，傷藥已經用光了！\n");
        }
        return;
    }

    // 【戰鬥邏輯】
    int choice = atoi(command); 
    bool found = false;

    for (int i = 0; i < current_enemy_count; i++) {
        if (current_enemies[i].id == choice && !current_enemies[i].is_dead) { 
            found = true;
            
            // ⚠️ 這裡已經完全刪除了武器等級檢查與 HP 不足的限制檢查！

            // 1. 計算隨機金幣獎勵
            int reward = current_enemies[i].min_gold + (rand() % (current_enemies[i].max_gold - current_enemies[i].min_gold + 1));
            
            // 2. 進行戰鬥扣血與金幣結算
            user->hp -= current_enemies[i].hp_cost;
            user->gold += reward;
            current_enemies[i].is_dead = true; 

            safe_printf(use_big5, ">> ⚔️ 激戰過後，你成功擊敗了【%s】！(失去 %d HP，獲得 %d 金幣)\n", 
                        current_enemies[i].name, current_enemies[i].hp_cost, reward);

            // 3. 硬派死亡檢測：如果玩家血量扣到 0 或以下，原地復活並強制送回村莊
            if (user->hp <= 0) {
                safe_printf(use_big5, ">> 💀 慘烈！你在擊倒怪物的同時也耗盡了最後一滴血...\n");
                safe_printf(use_big5, ">> 醒來時你已經被熱心村民抬回廣場，為了療傷付出了 10 金幣醫療費。\n");
                
                user->hp = 20;       // 復活給予基礎血量
                user->gold -= 10;    // 扣除醫藥費
                if (user->gold < 0) user->gold = 0;
                
                user->map_id = 2;     // 強制送回村莊
                in_encounter = false; // 結束這場遭遇戰
            }
            break; 
        }
    }

    if (!found) {
        safe_printf(use_big5, ">> 你對著空氣揮舞武器，什麼也沒打中。或者目標已經死了？\n");
    }
}