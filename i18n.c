#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <iconv.h>
#include "i18n.h"

void safe_printf(bool to_big5, const char *format, ...) {
    // 1. 處理變數參數，把原本 printf 要印的東西先塞進 buffer 裡
    char buffer[1024]; 
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 2. 如果使用者不需要轉碼 (UTF-8 終端機)，直接印出即可
    if (!to_big5) {
        printf("%s", buffer);
        return;
    }

    // 3. 需要轉碼為 Big5：開啟 iconv 轉換引擎 (UTF-8 轉 BIG5)
    iconv_t cd = iconv_open("BIG5", "UTF-8");
    if (cd == (iconv_t)-1) {
        // 如果系統不支援轉碼，降級直接輸出
        printf("%s", buffer);
        return;
    }

    // 準備轉碼用的緩衝區與指標
    char out_buffer[2048];
    char *pin = buffer;
    char *pout = out_buffer;
    size_t inbytesleft = strlen(buffer);
    size_t outbytesleft = sizeof(out_buffer) - 1;

    // 執行轉碼
    if (iconv(cd, &pin, &inbytesleft, &pout, &outbytesleft) == (size_t)-1) {
        // 如果遇到無法轉換的特殊字元而失敗，降級輸出原字串
        printf("%s", buffer);
    } else {
        *pout = '\0'; // 確保字串有結尾符號
        printf("%s", out_buffer);
    }
    
    // 關閉引擎
    iconv_close(cd);
}