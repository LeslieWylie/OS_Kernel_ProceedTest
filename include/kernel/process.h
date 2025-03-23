#ifndef _KERNEL_PROCESS_H
#define _KERNEL_PROCESS_H

#include <stdint.h>
#include <stddef.h>

// 进程状态枚举
typedef enum
{
    PROCESS_CREATED,   // 新创建
    PROCESS_READY,     // 就绪态
    PROCESS_RUNNING,   // 运行态
    PROCESS_BLOCKED,   // 阻塞态
    PROCESS_TERMINATED // 终止态
} process_state_t;

// 进程ID类型
typedef int32_t pid_t;

// 进程对用户态寄存器的保存结构
typedef struct pt_regs
{
    // 通用寄存器
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    // 段寄存器
    uint16_t gs;
    uint16_t fs;
    uint16_t es;
    uint16_t ds;

    // 中断相关
    uint64_t error_code; // 错误码
    uint64_t rip;        // 指令指针
    uint64_t cs;         // 代码段
    uint64_t rflags;     // 标志寄存器
    uint64_t rsp;        // 栈指针
    uint64_t ss;         // 栈段
} pt_regs_t;

// 前向声明
struct mm_struct;
struct context;

// 进程控制块结构
typedef struct process
{
    pid_t pid;             // 进程ID
    process_state_t state; // 进程状态
    uint32_t priority;     // 进程优先级
    uint32_t counter;      // 运行计数器/剩余时间片

    void *stack;             // 进程栈指针
    size_t stack_size;       // 栈大小
    struct context *context; // 内核态CPU上下文
    struct mm_struct *mm;    // 内存管理结构

    // 用户态相关
    void *user_stack;        // 用户态栈
    uint64_t user_stack_top; // 用户态栈顶
    uint64_t sscratch;       // 用户态/内核态切换临时存储
    pt_regs_t *pt_regs;      // 用户态寄存器保存

    // 进程关系
    struct process *parent; // 父进程
    struct process *next;   // 调度链表中的下一个进程

    // 统计和管理信息
    uint64_t creation_time; // 创建时间
    uint64_t cpu_time;      // CPU使用时间
    int exit_code;          // 退出码
    char name[32];          // 进程名称
    void *kernel_stack;     // 内核栈
    uint32_t flags;         // 进程标志
} process_t;

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
void schedule(void);
void do_timer(void);
void switch_to(process_t *next);

#endif // _KERNEL_PROCESS_H