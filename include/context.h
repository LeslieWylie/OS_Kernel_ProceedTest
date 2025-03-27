#ifndef _KERNEL_CONTEXT_H
#define _KERNEL_CONTEXT_H

/**
 * @file context.h
 * @brief 上下文操作相关接口
 *
 * 提供CPU上下文保存、恢复和切换的相关结构和函数
 */

#include <stdint.h>
#include "types.h"

/**
 * @brief CPU上下文结构 (64位)
 *
 * 保存所有需要在上下文切换时保存的寄存器
 */
struct context
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
};

/**
 * @brief 初始化上下文
 *
 * @param context 上下文结构指针
 * @param entry 入口函数
 * @param arg 传递给入口函数的参数
 * @param stack_top 栈顶指针
 */
void context_init(context_t *context, void (*entry)(void *), void *arg, void *stack_top);

/**
 * @brief 上下文切换
 *
 * @param from 源上下文
 * @param to 目标上下文
 */
void context_switch(context_t *from, context_t *to);

/**
 * @brief 保存当前上下文
 *
 * @param context 保存的目标上下文结构
 */
void context_save(context_t *context);

/**
 * @brief 恢复上下文
 *
 * @param context 要恢复的上下文
 */
void context_restore(context_t *context);

/**
 * @brief 进程上下文切换
 *
 * @param prev 前一个进程
 * @param next 下一个进程
 */
void process_context_switch(process_t *prev, process_t *next);

#endif // _KERNEL_CONTEXT_H