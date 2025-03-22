#ifndef PRINTK_H
#define PRINTK_H

#include <stddef.h>  

void printk(const char *format, ...);
void vga_putc(char c);

#endif
