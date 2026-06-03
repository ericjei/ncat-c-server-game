#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include "auth.h"
#include "i18n.h"

int main() {
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);

    // ==========================================
    // 1. 終端機編碼設定 (必須用純英文)
    // ==========================================
    char choice[10];
    bool use_big5 = false;
    while (1)
    {
        printf("Please select your terminal encoding / 請選擇終端機編碼:\n");
        printf("1. Windows CMD (Big5)\n");
        printf("2. Mac / Linux / Modern Terminal (UTF-8)\n");
        printf("Select (1/2): ");
    
        if (fgets(choice, sizeof(choice), stdin)) {
            if (choice[0] == '1') {
                use_big5 = true;
                break;
            }
            else if (choice[0] == '2') {
                use_big5 = false;
                break;
            }
            else{
                continue;
            }
        }
    }

    // ==========================================
    // 2. 系統初始化與資料夾檢查
    // ==========================================
    struct stat st = {0};
    if (stat("users", &st) == -1) {
        safe_printf(use_big5, "[系統日誌] 偵測到初次啟動，正在建立 users 資料庫資料夾...\n");
        mkdir("users", 0777); 
    }

    // ==========================================
    // 3. 進入登入/註冊流程
    // ==========================================
    while (1) {
        UserProfile currentUser;
        char filename[150];

        safe_printf(use_big5, "\n========================================\n");
        safe_printf(use_big5, "   【 智機系綜合挑戰賽：伺服器入口 】\n");
        safe_printf(use_big5, "========================================\n");
        safe_printf(use_big5, "請輸入帳號 (或輸入 exit 關閉連線)：");

        if (!fgets(currentUser.username, sizeof(currentUser.username), stdin)) break;
        strip_newline(currentUser.username); 

        // 【新增防呆】：提供一個真正的離開機制
        if (strcmp(currentUser.username, "exit") == 0) {
            safe_printf(use_big5, ">> 連線已中斷。期待您再次挑戰！\n");
            break; // 徹底打破大迴圈，結束程式
        }

        sprintf(filename, "users/%s.txt", currentUser.username);
        
        FILE *fp = fopen(filename, "r");
        if (fp == NULL) {
            register_user(&currentUser, filename, use_big5); 
            fp = fopen(filename, "r");
        }

        if (fp != NULL) {
            // 呼叫登入模組。
            // 只要密碼錯誤，或者玩家選擇登出，login_user 就會 return。
            // 程式就會順著迴圈，跑回第 3 步驟的開頭，重新印出「請輸入帳號」！
            login_user(&currentUser, fp, use_big5);    
        } else {
            safe_printf(use_big5, "\n>> 系統嚴重錯誤：無法讀取資料檔。\n");
        }
    }

    return 0;
}