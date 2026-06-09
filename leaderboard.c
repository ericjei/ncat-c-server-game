#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "leaderboard.h"
#include "i18n.h"
#include <limits.h>
// 簡單的排序：氣泡排序法
void sort_leaderboard(LeaderboardEntry list[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (list[j].gold < list[j + 1].gold) {
                LeaderboardEntry temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}

void render_leaderboard(bool use_big5) {
    safe_printf(use_big5, "\n🏆 【 全服財富排行榜 】 🏆\n");
    safe_printf(use_big5, "----------------------------\n");

    DIR *d = opendir("users");
    struct dirent *dir;
    LeaderboardEntry list[100];
    int count = 0;

    if (d) {
        while ((dir = readdir(d)) != NULL && count < 100) {
            if (strstr(dir->d_name, ".txt")) {
                char path[300];
                if (strstr(dir->d_name, ".txt") == NULL) continue;
                snprintf(path, sizeof(path), "users/%s", dir->d_name);
                FILE *fp = fopen(path, "r");
                if (fp) {
                    char pass[50];
                    int r, h, g, m, l, w, p;
                    // 根據你的存檔順序讀取 (對應 auth.c 的 save_user_to_file)
                    if (fscanf(fp, "%s\n%d\n%d\n%d\n%d\n%d\n%d\n%d", 
                        pass, &r, &h, &g, &m, &l, &w, &p) == 8) {
                        strcpy(list[count].username, dir->d_name);
                        strtok(list[count].username, "."); // 去掉 .txt
                        list[count].gold = g;
                        count++;
                    }
                    fclose(fp);
                }
            }
        }
        closedir(d);
    }

    sort_leaderboard(list, count);

    for (int i = 0; i < count && i < 10; i++) {
        safe_printf(use_big5, "%d. %-10s -> %d 金幣\n", i + 1, list[i].username, list[i].gold);
    }
    safe_printf(use_big5, "----------------------------\n");
    safe_printf(use_big5, "指令 -> [b] 返回大廳\n");
    fflush(stdout);
}