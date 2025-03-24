#ifndef _PRINTK_H
#define _PRINTK_H

#include <stdarg.h>

void printk(const char *fmt, ...);
void vga_putc(char c);

#endif
