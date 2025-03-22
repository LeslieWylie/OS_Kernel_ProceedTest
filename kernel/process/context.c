#include "context.h"
#include <string.h>

// 初始化上下文
void context_init(context_t *context, void (*entry)(void *), void *arg, void *stack_top)
{
    memset(context, 0, sizeof(context_t));

    // 设置入口点
    context->rip = (uint64_t)entry;

    // 设置栈指针
    context->rsp = (uint64_t)stack_top;

    // 设置参数 (x86_64 调用约定: 第一个参数在 rdi)
    context->rdi = (uint64_t)arg;

    // 设置段寄存器
    context->cs = 0x08; // 内核代码段
    context->ds = 0x10; // 内核数据段
    context->es = 0x10;
    context->fs = 0x10;
    context->gs = 0x10;
    context->ss = 0x10;

    // 设置标志寄存器（启用中断）
    context->rflags = 0x202; // IF=1, IOPL=0
}

// 保存当前上下文
void context_save(context_t *context)
{
    // 保存通用寄存器
    __asm__ volatile(
        "movq %%rax, %0\n"
        "movq %%rbx, %1\n"
        "movq %%rcx, %2\n"
        "movq %%rdx, %3\n"
        "movq %%rsi, %4\n"
        "movq %%rdi, %5\n"
        "movq %%rbp, %6\n"
        "movq %%rsp, %7\n"
        "movq %%r8,  %8\n"
        "movq %%r9,  %9\n"
        "movq %%r10, %10\n"
        "movq %%r11, %11\n"
        "movq %%r12, %12\n"
        "movq %%r13, %13\n"
        "movq %%r14, %14\n"
        "movq %%r15, %15\n"
        : "=m"(context->rax), "=m"(context->rbx),
          "=m"(context->rcx), "=m"(context->rdx),
          "=m"(context->rsi), "=m"(context->rdi),
          "=m"(context->rbp), "=m"(context->rsp),
          "=m"(context->r8), "=m"(context->r9),
          "=m"(context->r10), "=m"(context->r11),
          "=m"(context->r12), "=m"(context->r13),
          "=m"(context->r14), "=m"(context->r15)
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
        "pushfq\n"
        "popq %0\n"
        : "=m"(context->rflags)
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

    // 恢复通用寄存器
    __asm__ volatile(
        "movq %0, %%rax\n"
        "movq %1, %%rbx\n"
        "movq %2, %%rcx\n"
        "movq %3, %%rdx\n"
        "movq %4, %%rsi\n"
        "movq %5, %%rdi\n"
        "movq %6, %%rbp\n"
        "movq %7, %%rsp\n"
        "movq %8, %%r8\n"
        "movq %9, %%r9\n"
        "movq %10, %%r10\n"
        "movq %11, %%r11\n"
        "movq %12, %%r12\n"
        "movq %13, %%r13\n"
        "movq %14, %%r14\n"
        "movq %15, %%r15\n"
        :
        : "m"(context->rax), "m"(context->rbx),
          "m"(context->rcx), "m"(context->rdx),
          "m"(context->rsi), "m"(context->rdi),
          "m"(context->rbp), "m"(context->rsp),
          "m"(context->r8), "m"(context->r9),
          "m"(context->r10), "m"(context->r11),
          "m"(context->r12), "m"(context->r13),
          "m"(context->r14), "m"(context->r15)
        : "memory");

    // 恢复标志寄存器并跳转到rip
    __asm__ volatile(
        "pushq %0\n" // rflags
        "popfq\n"
        "pushq %1\n" // cs
        "pushq %2\n" // rip
        "iretq\n"
        :
        : "r"(context->rflags),
          "r"((uint64_t)context->cs),
          "r"(context->rip));
}

// 切换上下文
void context_switch(context_t *from, context_t *to)
{
    context_save(from);
    context_restore(to);
}