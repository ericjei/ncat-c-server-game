#ifndef VILLAGE_H
#define VILLAGE_H

#include <stdbool.h>
#include "auth.h" // 需要用到 UserProfile 結構體

// 宣告村莊提供的兩個副程式
void render_village(UserProfile *user, bool use_big5);
void update_village(UserProfile *user, const char *command, bool use_big5);

#endif