#include "context.h"
#include <string.h>

// 初始化上下文
void context_init(context_t *context, void (*entry)(void *), void *arg, void *stack_top)
{
    memset(context, 0, sizeof(context_t));
    context->rip = (uint64_t)entry;
    context->rsp = (uint64_t)stack_top;
    context->rdi = (uint64_t)arg;
    context->cs = 0x08;
    context->ds = 0x10;
    context->es = 0x10;
    context->fs = 0x10;
    context->gs = 0x10;
    context->ss = 0x10;
    context->rflags = 0x202;
}

// 保存当前上下文
void context_save(context_t *context)
{
    __asm__ __volatile__(
        "movq %%rax, 0(%0)\n\t"
        "movq %%rbx, 8(%0)\n\t"
        "movq %%rcx, 16(%0)\n\t"
        "movq %%rdx, 24(%0)\n\t"
        "movq %%rsi, 32(%0)\n\t"
        "movq %%rdi, 40(%0)\n\t"
        "movq %%rbp, 48(%0)\n\t"
        "movq %%rsp, 56(%0)\n\t"
        "movq %%r8,  64(%0)\n\t"
        "movq %%r9,  72(%0)\n\t"
        "movq %%r10, 80(%0)\n\t"
        "movq %%r11, 88(%0)\n\t"
        "movq %%r12, 96(%0)\n\t"
        "movq %%r13, 104(%0)\n\t"
        "movq %%r14, 112(%0)\n\t"
        "movq %%r15, 120(%0)\n\t"
        :
        : "r"(context)
        : "memory");

    // 保存段寄存器
    __asm__ __volatile__(
        "movw %%cs, %0\n\t"
        "movw %%ds, %1\n\t"
        "movw %%es, %2\n\t"
        "movw %%fs, %3\n\t"
        "movw %%gs, %4\n\t"
        "movw %%ss, %5\n\t"
        : "=m"(context->cs),
          "=m"(context->ds),
          "=m"(context->es),
          "=m"(context->fs),
          "=m"(context->gs),
          "=m"(context->ss));

    // 保存标志寄存器
    __asm__ __volatile__(
        "pushfq\n\t"
        "popq %0\n\t"
        : "=m"(context->rflags)
        :
        : "memory");
}

// 恢复上下文
void context_restore(context_t *context)
{
    // 恢复段寄存器
    __asm__ __volatile__(
        "movw %0, %%ds\n\t"
        "movw %0, %%es\n\t"
        "movw %0, %%fs\n\t"
        "movw %0, %%gs\n\t"
        :
        : "r"((uint16_t)context->ds)
        : "memory");

    // 恢复通用寄存器
    __asm__ __volatile__(
        "movq 0(%0), %%rax\n\t"
        "movq 8(%0), %%rbx\n\t"
        "movq 16(%0), %%rcx\n\t"
        "movq 24(%0), %%rdx\n\t"
        "movq 32(%0), %%rsi\n\t"
        "movq 40(%0), %%rdi\n\t"
        "movq 48(%0), %%rbp\n\t"
        "movq 56(%0), %%rsp\n\t"
        "movq 64(%0), %%r8\n\t"
        "movq 72(%0), %%r9\n\t"
        "movq 80(%0), %%r10\n\t"
        "movq 88(%0), %%r11\n\t"
        "movq 96(%0), %%r12\n\t"
        "movq 104(%0), %%r13\n\t"
        "movq 112(%0), %%r14\n\t"
        "movq 120(%0), %%r15\n\t"
        :
        : "r"(context)
        : "memory", "rax", "rbx", "rcx", "rdx",
          "rsi", "rdi", "rbp", "rsp",
          "r8", "r9", "r10", "r11",
          "r12", "r13", "r14", "r15");

    // 恢复标志寄存器并跳转到rip
    __asm__ __volatile__(
        "pushq %0\n\t" // ss
        "pushq %1\n\t" // rsp
        "pushq %2\n\t" // rflags
        "pushq %3\n\t" // cs
        "pushq %4\n\t" // rip
        "iretq\n\t"
        :
        : "r"((uint64_t)context->ss),
          "r"(context->rsp),
          "r"(context->rflags),
          "r"((uint64_t)context->cs),
          "r"(context->rip)
        : "memory");
}

// 切换上下文
void context_switch(context_t *from, context_t *to)
{
    if (from && to)
    {
        context_save(from);
        context_restore(to);
    }
}