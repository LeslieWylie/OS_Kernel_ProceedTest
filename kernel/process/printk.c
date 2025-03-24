#include <stdio.h>
#include <stdarg.h>
#include "printk.h"

void printk(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    fflush(stdout); // 确保输出立即显示
}