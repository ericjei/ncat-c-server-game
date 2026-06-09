#ifndef BATTLE_H
#define BATTLE_H

#include <stdbool.h>
#include "auth.h" 

#define MAX_MONSTERS 50

typedef struct {
    int id;
    char name[50];
    int max_hp;       // 怪物的總血量
    int current_hp;   // 回合制戰鬥中的當前血量
    int attack;       // 怪物的攻擊力
    int min_gold;
    int max_gold;
    bool is_dead;
} Monster;
void render_battle(UserProfile *user, bool use_big5);
void update_battle(UserProfile *user, const char *command, bool use_big5);

#endif