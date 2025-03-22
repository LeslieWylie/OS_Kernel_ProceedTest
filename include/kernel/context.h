#ifndef _KERNEL_CONTEXT_H
#define _KERNEL_CONTEXT_H

#include <stdint.h>

// CPU上下文结构 (64位)
typedef struct context
{
    // 通用寄存器
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    // 段寄存器
    uint16_t cs;
    uint16_t ds;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;
    uint16_t ss;

    // 程序计数器和标志寄存器
    uint64_t rip;
    uint64_t rflags;
} context_t;

// 上下文操作函数
void context_init(context_t *context, void (*entry)(void *), void *arg, void *stack_top);
void context_switch(context_t *from, context_t *to);
void context_save(context_t *context);
void context_restore(context_t *context);

#endif // _KERNEL_CONTEXT_H