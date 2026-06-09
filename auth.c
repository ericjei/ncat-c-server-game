#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"
#include "i18n.h"
#include "game.h" // 引入遊戲模組

// 工具函式：過濾換行符號
void strip_newline(char *str) {
    str[strcspn(str, "\r\n")] = 0;
}

// ==========================================
// 註冊功能實作
// ==========================================
void register_user(UserProfile *user, const char *filename, bool use_big5) {
    safe_printf(use_big5, ">> 系統查無專屬資料檔 '%s'，將為您建立新身分。\n", filename);
    safe_printf(use_big5, ">> 請設定您的密碼：");
    
    if (!fgets(user->password, sizeof(user->password), stdin)) return;
    strip_newline(user->password);
    
    // 設定遊戲初始數值
    user->role_level = 1; 
    user->hp = 100;       // 新手初始滿血
    user->gold = 0;       // 新手 0 元
    user->map_id = 0;     // 出生在大廳 (地圖 0)
    user->luck = (rand() % 10) + 1; //1-10運
    user->weapon_level = 0; 
    user->potions = 0;

    FILE *fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, "%s\n", user->password);
        fprintf(fp, "%d\n", user->role_level);
        fprintf(fp, "%d\n", user->hp);      // 寫入 HP
        fprintf(fp, "%d\n", user->gold);    // 寫入金幣
        fprintf(fp, "%d\n", user->map_id);  // 寫入地圖位置
        fprintf(fp, "%d\n", user->luck);//寫入運氣
        fprintf(fp, "%d\n", user->weapon_level); 
        fprintf(fp, "%d\n", user->potions);
        fclose(fp);
        safe_printf(use_big5, "\n>> 註冊成功！您的專屬資料檔已建立。\n");
        safe_printf(use_big5, ">> 系統正在為您自動跳轉至登入畫面...\n\n");
    } else {
        safe_printf(use_big5, "\n>> 系統錯誤：無法建立檔案。\n");
    }
}

// ==========================================
// 更改使用者名稱模組
// ==========================================
void change_username(UserProfile *user, bool use_big5) {
    char new_username[50];
    char old_filename[150];
    char new_filename[150];

    safe_printf(use_big5, "\n>> 請輸入新的登入名稱：");
    if (!fgets(new_username, sizeof(new_username), stdin)) return;
    strip_newline(new_username);

    // 防呆：如果輸入空白，或跟原本的名字一模一樣，就取消操作
    if (strlen(new_username) == 0 || strcmp(new_username, user->username) == 0) {
        safe_printf(use_big5, ">> 名稱無效或未更改，已取消操作。\n");
        return;
    }

    sprintf(old_filename, "users/%s.txt", user->username);
    sprintf(new_filename, "users/%s.txt", new_username);

    // 【防禦機制】檢查新名稱是否已經被別人註冊了
    FILE *check = fopen(new_filename, "r");
    if (check != NULL) {
        fclose(check); 
        safe_printf(use_big5, ">> 更改失敗：名稱 '%s' 已被他人使用！\n", new_username);
        return;
    }

    // 使用 C 語言內建的 rename 函式更改實體檔案名稱
    if (rename(old_filename, new_filename) == 0) {
        strcpy(user->username, new_username);
        safe_printf(use_big5, ">> 更改成功！您的新名稱是：%s\n", user->username);
    } else {
        safe_printf(use_big5, ">> 系統錯誤：檔案重新命名失敗。\n");
    }
}

// ==========================================
// 登入功能實作
// ==========================================
void login_user(UserProfile *user, FILE *fp, bool use_big5) {
    char input_password[50];
    char temp_str[20]; // 用來暫存讀取到的數值字串

    // 1. 讀取舊有身分資料
    fgets(user->password, sizeof(user->password), fp);
    strip_newline(user->password);
    
    fgets(temp_str, sizeof(temp_str), fp);
    user->role_level = atoi(temp_str);
    
    // 2. 讀取遊戲狀態數值
    fgets(temp_str, sizeof(temp_str), fp);
    user->hp = atoi(temp_str);
    
    fgets(temp_str, sizeof(temp_str), fp);
    user->gold = atoi(temp_str);
    
    fgets(temp_str, sizeof(temp_str), fp);
    user->map_id = atoi(temp_str);

    fgets(temp_str, sizeof(temp_str), fp);
    user->luck = atoi(temp_str);

    fgets(temp_str, sizeof(temp_str), fp);
    user->weapon_level = atoi(temp_str);

    fgets(temp_str, sizeof(temp_str), fp);
    user->potions = atoi(temp_str);
    fclose(fp); // 讀取完畢即可關閉檔案

    safe_printf(use_big5, ">> 歡迎回來，玩家 %s！ (權限等級: %d)\n", user->username, user->role_level);
    safe_printf(use_big5, ">> 請輸入密碼進行身分驗證：");
    
    if (!fgets(input_password, sizeof(input_password), stdin)) return;
    strip_newline(input_password);

    // 密碼錯誤：直接 return，退回 main() 裡面的首頁大迴圈
    if (strcmp(input_password, user->password) != 0) {
        safe_printf(use_big5, "\n>> 密碼錯誤！已將您退回首頁。\n\n");
        return; 
    }

    // 密碼正確
    safe_printf(use_big5, "\n>> 登入成功！\n");
    safe_printf(use_big5, "FLAG{NPTU_Ultimate_System_Integration_2026}\n");

    // ==========================================
    // 登入成功後：進入系統控制面板 (無窮迴圈)
    // ==========================================
    char choice[10];
    while (1) {
        safe_printf(use_big5, "\n========================================\n");
        safe_printf(use_big5, "   【 玩家控制面板 】 - 當前身分：%s\n", user->username);
        safe_printf(use_big5, "========================================\n");
        safe_printf(use_big5, "1. 進入遊戲世界 (冒險開始)\n"); 
        safe_printf(use_big5, "2. 更改登入名稱\n");
        safe_printf(use_big5, "3. 登出並返回首頁\n");
        safe_printf(use_big5, "請選擇操作 (1~3): ");

        if (!fgets(choice, sizeof(choice), stdin)) return;
        strip_newline(choice);

        if (strcmp(choice, "1") == 0) {
            run_game_loop(user, use_big5); // 呼叫 game.c 的遊戲主循環
        } else if (strcmp(choice, "2") == 0) {
            change_username(user, use_big5);
        } else if (strcmp(choice, "3") == 0) {
            safe_printf(use_big5, ">> 正在安全登出... 再見，%s！\n\n", user->username);
            return; // 結束 login_user，退回 main()
        } else {
            safe_printf(use_big5, ">> 無效的選項，請重新輸入。\n");
        }
    }
}
// ==========================================
// 儲存玩家進度實作
// ==========================================
void save_user_to_file(UserProfile *user) {
    char filename[150];
    // 組合出該玩家的存檔路徑，例如 "users/lin.txt"
    sprintf(filename, "users/%s.txt", user->username);
    
    // 使用 "w" 模式開啟檔案，這會直接覆蓋掉舊的檔案內容
    FILE *fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, "%s\n", user->password);
        fprintf(fp, "%d\n", user->role_level);
        fprintf(fp, "%d\n", user->hp);
        fprintf(fp, "%d\n", user->gold);
        fprintf(fp, "%d\n", user->map_id);
        fprintf(fp, "%d\n", user->luck);
        fprintf(fp, "%d\n", user->weapon_level); 
        fprintf(fp, "%d\n", user->potions);
        fclose(fp);
    }
}