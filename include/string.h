#ifndef _STRING_H
#define _STRING_H

#include "types.h"

// 字符串操作函数
void *memset(void *s, int c, uint32_t n);
void *memcpy(void *dest, const void *src, uint32_t n);
int memcmp(const void *s1, const void *s2, uint32_t n);
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, uint32_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, uint32_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, uint32_t n);

// 字符串转换函数
int atoi(const char *nptr);
char *itoa(int value, char *str, int base);

// 字符串分割函数
char *strtok(char *str, const char *delim);

#endif // _STRING_H
