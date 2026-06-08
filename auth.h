// auth.h
#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    char username[50];
    char password[50];
    int role_level;
    // --- 遊戲專用欄位 ---
    int hp;
    int gold;
    int map_id;
    int luck;
    int weapon_level; 
    int potions;
} UserProfile;

void strip_newline(char *str);
void register_user(UserProfile *user, const char *filename, bool use_big5);
void login_user(UserProfile *user, FILE *fp, bool use_big5);
void change_username(UserProfile *user, bool use_big5);

#endif