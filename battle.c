#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "battle.h"
#include "i18n.h"

// --- 遭遇戰狀態管理 ---
static Monster current_enemies[3];
static int current_enemy_count = 0;
static bool in_encounter = false;

// 動態讀取怪物清單 (新格式：ID,名稱,最大HP,攻擊力,最小金幣,最大金幣)
static int load_monsters(Monster db[]) {
    FILE *fp = fopen("monsters.txt", "r");
    if (fp == NULL) return 0;

    int count = 0;
    char line[100];
    
    while (fgets(line, sizeof(line), fp) && count < MAX_MONSTERS) {
        if (sscanf(line, "%d,%[^,],%d,%d,%d,%d", 
            &db[count].id, 
            db[count].name, 
            &db[count].max_hp, 
            &db[count].attack, 
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

    current_enemy_count = (rand() % 3) + 1; 
    for (int i = 0; i < current_enemy_count; i++) {
        int idx = rand() % db_count;
        current_enemies[i] = db[idx];
        current_enemies[i].id = i + 1; // 重新編號為 1, 2, 3，方便玩家輸入指令
        current_enemies[i].current_hp = current_enemies[i].max_hp; // 滿血生成
    }
    in_encounter = true;
}

void render_battle(UserProfile *user, bool use_big5) {
    if (!in_encounter) {
        generate_encounter();
    }

    safe_printf(use_big5, "\n【 危機四伏的荒野 】\n");
    safe_printf(use_big5, "冷風吹過，草叢中傳來陣陣低吼聲...\n");
    safe_printf(use_big5, "你的狀態 -> HP: %d | 金幣: %d | 武器 Lv.%d | 傷藥: %d\n", 
                user->hp, user->gold, user->weapon_level, user->potions);
    safe_printf(use_big5, "----------------------------------------\n");

    int alive_count = 0;
    safe_printf(use_big5, "⚠️ 你遭遇了 %d 隻怪物！\n", current_enemy_count);
    
    for (int i = 0; i < current_enemy_count; i++) {
        if (!current_enemies[i].is_dead) {
            safe_printf(use_big5, "指令 -> [%d] 攻擊 %s (HP: %d/%d | 攻擊力: %d)\n", 
                        current_enemies[i].id, 
                        current_enemies[i].name, 
                        current_enemies[i].current_hp, 
                        current_enemies[i].max_hp,
                        current_enemies[i].attack);
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
    safe_printf(use_big5, "指令 -> [leave] 逃回大廳\n");
}

void update_battle(UserProfile *user, const char *command, bool use_big5) {
    if (strcmp(command, "leave") == 0) {
        user->map_id = 0; // 退回大廳
        in_encounter = false; 
        safe_printf(use_big5, "\n>> 🏃 你頭也不回地跑回了安全的大廳。\n");
        return;
    } 
    
    if (strcmp(command, "search") == 0) {
        in_encounter = false; 
        safe_printf(use_big5, "\n>> 👣 你往荒野深處走去，尋找新的獵物...\n");
        return;
    }

    // 【喝藥水邏輯】(喝藥水視為消耗一回合，怪物會趁機偷襲)
    bool action_taken = false; 
    if (strcmp(command, "h") == 0) {
        if (user->potions > 0) {
            user->potions -= 1;
            user->hp += 50;
            if (user->hp > 100) user->hp = 100; 
            safe_printf(use_big5, "\n>> 🧪 咕嚕咕嚕... 你喝下了傷藥，HP 恢復了！(目前 HP: %d)\n", user->hp);
            action_taken = true;
        } else {
            safe_printf(use_big5, "\n>> ⚠️ 傷藥已經用光了！你白白浪費了防守的空檔！\n");
            action_taken = true; // 找藥水浪費時間，依然會被怪物打
        }
    }

    // 【玩家攻擊邏輯】
    int choice = atoi(command); 
    for (int i = 0; i < current_enemy_count; i++) {
        if (current_enemies[i].id == choice && !current_enemies[i].is_dead) { 
            action_taken = true;
            
            // 根據武器等級計算玩家攻擊力 (基礎 15 + 每級 8 + 隨機浮動)
            int player_atk = 15 + (user->weapon_level * 8) + (rand() % 5);
            current_enemies[i].current_hp -= player_atk;
            
            safe_printf(use_big5, "\n>> 🗡️ 你揮舞武器，對【%s】造成 %d 點傷害！\n", current_enemies[i].name, player_atk);

            // 檢查怪物是否被打死
            if (current_enemies[i].current_hp <= 0) {
                int reward = current_enemies[i].min_gold + (rand() % (current_enemies[i].max_gold - current_enemies[i].min_gold + 1));
                user->gold += reward;
                current_enemies[i].is_dead = true; 
                safe_printf(use_big5, ">> 💀 【%s】哀嚎一聲倒下了！(獲得 %d 金幣)\n", current_enemies[i].name, reward);
            }
            break; 
        }
    }

    if (!action_taken && choice != 0) {
        safe_printf(use_big5, "\n>> ⚠️ 你對著空氣揮舞武器，什麼也沒打中。\n");
        action_taken = true; // 揮空也算一回合
    }

    // 【怪物反擊回合】(只要玩家有動作，且沒逃跑，存活的怪物就會反擊)
    if (action_taken) {
        for (int i = 0; i < current_enemy_count; i++) {
            if (!current_enemies[i].is_dead) {
                int e_atk = current_enemies[i].attack + (rand() % 4); // 怪物攻擊微小浮動
                user->hp -= e_atk;
                safe_printf(use_big5, ">> 👾 【%s】朝你猛撲，造成 %d 點傷害！\n", current_enemies[i].name, e_atk);
            }
        }

        // 【玩家死亡判定】
        if (user->hp <= 0) {
            safe_printf(use_big5, "\n>> 💀 慘烈！你承受不住怪物的猛攻，倒在了血泊中...\n");
            safe_printf(use_big5, ">> 醒來時你已經被抬回廣場，為此付出了 10 金幣醫療費。\n");
            
            user->hp = 20;
            user->gold -= 10;
            if (user->gold < 0) user->gold = 0;
            
            user->map_id = 0; // 強制送回大廳
            in_encounter = false; 
        }
    }
}