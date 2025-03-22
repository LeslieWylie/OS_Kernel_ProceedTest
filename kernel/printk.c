#include "printk.h"
#include <stdarg.h>
#include <stdint.h>
#include "io.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY (volatile char *)0xB8000
#define VGA_COLOR 0x07

size_t cursor_x = 0;
size_t cursor_y = 0;

void move_cursor() {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_putc(char c) {
    volatile char *video_memory = VGA_MEMORY;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') { // 处理退格键
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = VGA_WIDTH - 1;
        }
        size_t index = (cursor_y * VGA_WIDTH + cursor_x) * 2;
        video_memory[index] = ' ';  // 清除字符
        video_memory[index + 1] = VGA_COLOR;
    } else {
        size_t index = (cursor_y * VGA_WIDTH + cursor_x) * 2;
        video_memory[index] = c;
        video_memory[index + 1] = VGA_COLOR;
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= VGA_HEIGHT) {
        for (size_t row = 1; row < VGA_HEIGHT; row++) {
            for (size_t col = 0; col < VGA_WIDTH; col++) {
                size_t src = (row * VGA_WIDTH + col) * 2;
                size_t dst = ((row - 1) * VGA_WIDTH + col) * 2;
                video_memory[dst] = video_memory[src];
                video_memory[dst + 1] = video_memory[src + 1];
            }
        }

        for (size_t col = 0; col < VGA_WIDTH; col++) {
            size_t index = ((VGA_HEIGHT - 1) * VGA_WIDTH + col) * 2;
            video_memory[index] = ' ';
            video_memory[index + 1] = VGA_COLOR;
        }

        cursor_y = VGA_HEIGHT - 1;
    }

    move_cursor(); // 更新硬件光标
}


void printk(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 's': { // 处理字符串
                    const char *str = va_arg(args, const char *);
                    while (*str) vga_putc(*str++);
                    break;
                }
                case 'd': { // 处理整数
                    int num = va_arg(args, int);
                    char buffer[32];
                    itoa(num, buffer, 10);
                    for (char *p = buffer; *p; p++) vga_putc(*p);
                    break;
                }
                case 'x': { // 处理十六进制
                    int num = va_arg(args, int);
                    char buffer[32];
                    itoa(num, buffer, 16);
                    for (char *p = buffer; *p; p++) vga_putc(*p);
                    break;
                }
                case 'c': { // 处理单个字符
                    char c = (char) va_arg(args, int);
                    vga_putc(c);
                    break;
                }
                default:
                    vga_putc('%');
                    vga_putc(*format);
            }
        } else {
            vga_putc(*format);
        }
        format++;
    }

    va_end(args);
    move_cursor(); // 更新光标
}


void itoa(int num, char *str, int base) {
    int i = 0, isNegative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }

    while (num) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num /= base;
    }

    if (isNegative) str[i++] = '-';

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}
