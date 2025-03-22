#ifndef _KERNEL_CONTEXT_H
#define _KERNEL_CONTEXT_H

#include <stdint.h>

// CPU上下文结构
typedef struct context
{
    // 通用寄存器
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t esp;

    // 段寄存器
    uint32_t cs;
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t gs;
    uint32_t ss;

    // 程序计数器和标志寄存器
    uint32_t eip;
    uint32_t eflags;
} context_t;

// 上下文操作函数
void context_init(context_t *context, void (*entry)(void *), void *arg, void *stack_top);
void context_switch(context_t *from, context_t *to);
void context_save(context_t *context);
void context_restore(context_t *context);

#endif // _KERNEL_CONTEXT_H