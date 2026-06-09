#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "auth.h" // 為了使用 UserProfile

typedef struct {
    char username[50];
    int gold;
} LeaderboardEntry;

void update_leaderboard(UserProfile *user);
void render_leaderboard(bool use_big5);

#endif