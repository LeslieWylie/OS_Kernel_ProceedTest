#include "context.h"
#include <string.h>

// 初始化上下文
void context_init(context_t *context, void (*entry)(void *), void *arg, void *stack_top)
{
    memset(context, 0, sizeof(context_t));

    // 设置入口点
    context->eip = (uint32_t)entry;

    // 设置栈指针
    context->esp = (uint32_t)stack_top;

    // 设置参数
    context->eax = (uint32_t)arg;

    // 设置段寄存器
    context->cs = 0x08; // 内核代码段
    context->ds = 0x10; // 内核数据段
    context->es = 0x10;
    context->fs = 0x10;
    context->gs = 0x10;
    context->ss = 0x10;

    // 设置标志寄存器（启用中断）
    context->eflags = 0x202; // IF=1, IOPL=0
}

// 保存当前上下文
void context_save(context_t *context)
{
    __asm__ volatile(
        "movl %%eax, %0\n"
        "movl %%ebx, %1\n"
        "movl %%ecx, %2\n"
        "movl %%edx, %3\n"
        "movl %%esi, %4\n"
        "movl %%edi, %5\n"
        "movl %%ebp, %6\n"
        "movl %%esp, %7\n"
        : "=m"(context->eax), "=m"(context->ebx),
          "=m"(context->ecx), "=m"(context->edx),
          "=m"(context->esi), "=m"(context->edi),
          "=m"(context->ebp), "=m"(context->esp)
        :
        : "memory");

    // 保存段寄存器
    __asm__ volatile(
        "movw %%cs, %0\n"
        "movw %%ds, %1\n"
        "movw %%es, %2\n"
        "movw %%fs, %3\n"
        "movw %%gs, %4\n"
        "movw %%ss, %5\n"
        : "=m"(context->cs), "=m"(context->ds),
          "=m"(context->es), "=m"(context->fs),
          "=m"(context->gs), "=m"(context->ss)
        :
        : "memory");

    // 保存标志寄存器
    __asm__ volatile(
        "pushfl\n"
        "popl %0\n"
        : "=m"(context->eflags)
        :
        : "memory");
}

// 恢复上下文
void context_restore(context_t *context)
{
    // 恢复段寄存器
    __asm__ volatile(
        "movw %0, %%ds\n"
        "movw %0, %%es\n"
        "movw %0, %%fs\n"
        "movw %0, %%gs\n"
        :
        : "r"(context->ds)
        : "memory");

    // 恢复通用寄存器和栈
    __asm__ volatile(
        "movl %0, %%eax\n"
        "movl %1, %%ebx\n"
        "movl %2, %%ecx\n"
        "movl %3, %%edx\n"
        "movl %4, %%esi\n"
        "movl %5, %%edi\n"
        "movl %6, %%ebp\n"
        "movl %7, %%esp\n"
        :
        : "m"(context->eax), "m"(context->ebx),
          "m"(context->ecx), "m"(context->edx),
          "m"(context->esi), "m"(context->edi),
          "m"(context->ebp), "m"(context->esp)
        : "memory");

    // 恢复标志寄存器并跳转到eip
    __asm__ volatile(
        "pushl %0\n" // eflags
        "popfl\n"
        "pushl %1\n" // cs
        "pushl %2\n" // eip
        "iret\n"
        :
        : "r"(context->eflags),
          "r"(context->cs),
          "r"(context->eip));
}

// 切换上下文
void context_switch(context_t *from, context_t *to)
{
    context_save(from);
    context_restore(to);
}