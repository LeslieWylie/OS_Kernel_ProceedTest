#ifndef _KERNEL_PROCESS_H
#define _KERNEL_PROCESS_H

/**
 * @file process.h
 * @brief 进程管理相关接口定义
 *
 * 提供进程管理系统的主要结构定义和函数声明
 */

#include <stdint.h>
#include <stddef.h>
#include "types.h"

/* 确保类型全部声明完全 */
struct context;
struct pt_regs;
struct mm_struct;
struct process;

/* 类型别名声明 */
typedef struct context context_t;
typedef struct pt_regs pt_regs_t;
typedef struct mm_struct mm_struct_t;
typedef struct process process_t;

/**
 * @brief 进程控制块结构
 */
struct process
{
    pid_t pid;             // 进程ID
    process_state_t state; // 进程状态
    uint32_t priority;     // 进程优先级
    uint32_t counter;      // 运行计数器/剩余时间片

    void *stack;        // 进程栈指针
    size_t stack_size;  // 栈大小
    context_t *context; // 内核态CPU上下文
    mm_struct_t *mm;    // 内存管理结构

    // 用户态相关
    void *user_stack;        // 用户态栈
    uint64_t user_stack_top; // 用户态栈顶
    uint64_t sscratch;       // 用户态/内核态切换临时存储
    pt_regs_t *pt_regs;      // 用户态寄存器保存

    // 进程关系
    struct process *parent; // 父进程
    struct process *next;   // 调度链表中的下一个进程

    // 统计和管理信息
    os_time_t creation_time; // 创建时间
    uint64_t cpu_time;       // CPU使用时间
    int exit_code;           // 退出码
    char name[32];           // 进程名称
    void *kernel_stack;      // 内核栈
    uint32_t flags;          // 进程标志
};

// 全局变量
extern process_t *current;  // 当前运行的进程
extern process_t *task[64]; // 进程数组，最多支持64个进程
extern int task_count;      // 当前进程数量

// 进程管理函数声明
void process_init(void);
pid_t process_create(void (*entry)(void *), void *arg, uint32_t priority);
void process_exit(int exit_code);
int process_wait(pid_t pid, int *exit_code);
process_t *process_get_current(void);
void process_set_current(process_t *proc);
void process_set_state(process_t *proc, process_state_t new_state);
const char *process_state_to_string(process_state_t state);
int process_is_alive(process_t *proc);

// 调度相关
void schedule(void);             // 进程调度实现
void schedule_sjf(void);         // 最短作业优先调度实现
void do_timer(void);             // 时间片处理
void switch_to(process_t *next); // 进程切换

#endif // _KERNEL_PROCESS_H