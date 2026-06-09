#ifndef BATTLE_H
#define BATTLE_H

#include <stdbool.h>
#include "auth.h"

#define MAX_MONSTERS 10 

// 怪物資料結構
typedef struct {
    int id;
    char name[50];
    int req_weapon;
    int hp_cost;
    int min_gold;   // 最小掉落金幣
    int max_gold;   // 最大掉落金幣
    bool is_dead;   // 紀錄這隻怪物是否已經在遭遇戰中被擊殺
} Monster;
void render_battle(UserProfile *user, bool use_big5);
void update_battle(UserProfile *user, const char *command, bool use_big5);

#endif