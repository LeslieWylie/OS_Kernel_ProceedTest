#include <string.h>

#include "types.h"
#include "context.h"
#include "process.h"

/**
 * @brief 初始化上下文
 *
 * 为一个新的执行上下文设置初始状态，包括入口点、参数和栈指针。
 * 这通常在创建新进程或线程时使用。
 *
 * @param context 上下文结构指针
 * @param entry 入口函数
 * @param arg 传递给入口函数的参数
 * @param stack_top 栈顶指针
 */
void context_init(context_t *context, void (*entry)(void *), void *arg, void *stack_top)
{
    // 清零上下文结构
    memset(context, 0, sizeof(context_t));

    // 设置程序计数器指向入口函数
    context->rip = (uint64_t)entry;

    // 设置栈顶指针
    context->rsp = (uint64_t)stack_top;

    // x86_64调用约定：第一个参数存放在rdi寄存器
    context->rdi = (uint64_t)arg;

    // 设置段寄存器为内核代码段和数据段
    context->cs = 0x08; // 内核代码段选择子
    context->ds = 0x10; // 内核数据段选择子
    context->es = 0x10;
    context->fs = 0x10;
    context->gs = 0x10;
    context->ss = 0x10;

    // 设置标志寄存器，打开中断
    context->rflags = 0x202; // IF=1，开中断
}

/**
 * @brief 保存当前CPU上下文
 *
 * 将当前CPU的寄存器状态保存到指定的上下文结构中
 * 使用内联汇编直接访问并保存各个寄存器
 *
 * @param context 保存目标上下文结构的指针
 */
void context_save(context_t *context)
{
    // 保存通用寄存器
    __asm__ __volatile__(
        "movq %%rax, 0(%0)\n\t"   // 保存RAX到context偏移0
        "movq %%rbx, 8(%0)\n\t"   // 保存RBX到context偏移8
        "movq %%rcx, 16(%0)\n\t"  // 保存RCX到context偏移16
        "movq %%rdx, 24(%0)\n\t"  // 保存RDX到context偏移24
        "movq %%rsi, 32(%0)\n\t"  // 保存RSI到context偏移32
        "movq %%rdi, 40(%0)\n\t"  // 保存RDI到context偏移40
        "movq %%rbp, 48(%0)\n\t"  // 保存RBP到context偏移48
        "movq %%rsp, 56(%0)\n\t"  // 保存RSP到context偏移56
        "movq %%r8,  64(%0)\n\t"  // 保存R8到context偏移64
        "movq %%r9,  72(%0)\n\t"  // 保存R9到context偏移72
        "movq %%r10, 80(%0)\n\t"  // 保存R10到context偏移80
        "movq %%r11, 88(%0)\n\t"  // 保存R11到context偏移88
        "movq %%r12, 96(%0)\n\t"  // 保存R12到context偏移96
        "movq %%r13, 104(%0)\n\t" // 保存R13到context偏移104
        "movq %%r14, 112(%0)\n\t" // 保存R14到context偏移112
        "movq %%r15, 120(%0)\n\t" // 保存R15到context偏移120
        :
        : "r"(context) // 输入操作数：context指针
        : "memory");   // 通知编译器汇编代码修改了内存

    // 保存段寄存器
    __asm__ __volatile__(
        "movw %%cs, %0\n\t" // 保存CS到context->cs
        "movw %%ds, %1\n\t" // 保存DS到context->ds
        "movw %%es, %2\n\t" // 保存ES到context->es
        "movw %%fs, %3\n\t" // 保存FS到context->fs
        "movw %%gs, %4\n\t" // 保存GS到context->gs
        "movw %%ss, %5\n\t" // 保存SS到context->ss
        : "=m"(context->cs),
          "=m"(context->ds),
          "=m"(context->es),
          "=m"(context->fs),
          "=m"(context->gs),
          "=m"(context->ss));

    // 保存标志寄存器
    __asm__ __volatile__(
        "pushfq\n\t"  // 将RFLAGS压入栈
        "popq %0\n\t" // 将栈顶值弹出到context->rflags
        : "=m"(context->rflags)
        :
        : "memory");
}

/**
 * @brief 恢复CPU上下文
 *
 * 从指定的上下文结构恢复CPU寄存器状态
 * 注意：此函数不会返回，而是跳转到恢复后的上下文中继续执行
 *
 * @param context 要恢复的上下文结构指针
 */
void context_restore(context_t *context)
{
    // 恢复段寄存器（不能直接设置CS和SS，需要特权级转换）
    __asm__ __volatile__(
        "movw %0, %%ds\n\t" // 恢复DS
        "movw %0, %%es\n\t" // 恢复ES
        "movw %0, %%fs\n\t" // 恢复FS
        "movw %0, %%gs\n\t" // 恢复GS
        :
        : "rm"((uint16_t)context->ds) // 输入操作数：从context->ds读取值
        : "memory");                  // 通知编译器汇编代码修改了内存

    // 先保存栈指针到临时变量，因为后面恢复通用寄存器时会覆盖context指针
    uint64_t stack_ptr = context->rsp;

    // 将context的地址存入临时变量，避免在通用寄存器恢复时同时用于地址计算
    void *ctx_ptr = context;

    // 恢复除rbp和rsp外的所有通用寄存器
    __asm__ __volatile__(
        "movq 0(%0), %%rax\n\t"   // 恢复RAX
        "movq 8(%0), %%rbx\n\t"   // 恢复RBX
        "movq 16(%0), %%rcx\n\t"  // 恢复RCX
        "movq 24(%0), %%rdx\n\t"  // 恢复RDX
        "movq 32(%0), %%rsi\n\t"  // 恢复RSI
        "movq 40(%0), %%rdi\n\t"  // 恢复RDI
        "movq 64(%0), %%r8\n\t"   // 恢复R8
        "movq 72(%0), %%r9\n\t"   // 恢复R9
        "movq 80(%0), %%r10\n\t"  // 恢复R10
        "movq 88(%0), %%r11\n\t"  // 恢复R11
        "movq 96(%0), %%r12\n\t"  // 恢复R12
        "movq 104(%0), %%r13\n\t" // 恢复R13
        "movq 112(%0), %%r14\n\t" // 恢复R14
        "movq 120(%0), %%r15\n\t" // 恢复R15
        :
        : "r"(ctx_ptr) // 输入操作数：context指针临时副本
        : "memory");   // 通知编译器汇编代码修改了内存

    // 恢复rbp（使用临时寄存器避免直接修改rbp导致栈帧混乱）
    __asm__ __volatile__(
        "movq 48(%0), %%rax\n\t" // 将RBP值先加载到RAX
        "movq %%rax, %%rbp\n\t"  // 再从RAX加载到RBP
        :
        : "r"(ctx_ptr)      // 输入操作数：context指针临时副本
        : "memory", "rax"); // 通知编译器汇编代码修改了内存和RAX

    // 恢复rsp
    __asm__ __volatile__(
        "movq %0, %%rsp\n\t" // 恢复RSP
        :
        : "r"(stack_ptr) // 输入操作数：保存的栈指针
        : "memory");     // 通知编译器汇编代码修改了内存

    // 恢复标志寄存器
    __asm__ __volatile__(
        "pushq %0\n\t" // 将RFLAGS值压入栈
        "popfq\n\t"    // 从栈中弹出值到RFLAGS寄存器
        :
        : "m"(context->rflags) // 输入操作数：context->rflags
        : "memory");           // 通知编译器汇编代码修改了内存

    // 跳转到rip地址继续执行
    __asm__ __volatile__(
        "jmp *%0\n\t" // 跳转到指针指向的地址
        :
        : "r"(context->rip) // 输入操作数：跳转目标地址
        : "memory");        // 通知编译器汇编代码修改了内存

    // 注意：此处代码永远不会被执行，因为已经跳转到新上下文
}

/**
 * @brief 上下文切换
 *
 * 保存当前上下文到from，然后恢复to指向的上下文
 * 此函数会在返回时恢复到不同的执行流
 *
 * @param from 保存当前上下文的目标结构
 * @param to 要恢复的上下文结构
 */
void context_switch(context_t *from, context_t *to)
{
    // 先保存当前上下文
    context_save(from);

    // 然后恢复目标上下文（此函数不会返回）
    context_restore(to);

    // 此代码永远不会执行，因为context_restore会跳转到新上下文
}

// 汇编实现的上下文切换函数 - 声明为外部函数
// 实际实现在switch.S文件中
extern void __switch_to(process_t *prev, process_t *next);

/**
 * @brief 进程上下文切换
 *
 * 这是一个包装函数，调用汇编实现的进程切换函数
 * 切换过程包括保存所有寄存器状态、更新TSS等操作
 *
 * @param prev 前一个进程
 * @param next 下一个进程
 */
void process_context_switch(process_t *prev, process_t *next)
{
    // 调用汇编实现的切换函数
    __switch_to(prev, next);
}