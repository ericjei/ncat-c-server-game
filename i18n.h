#ifndef I18N_H
#define I18N_H

#include <stdbool.h>

// 宣告一個加強版的輸出函式：safe_printf
// 第一個參數決定是否要轉成 Big5，後面的參數就跟原本的 printf 完全一樣
void safe_printf(bool to_big5, const char *format, ...);

#endif
