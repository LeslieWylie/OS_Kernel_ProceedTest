#ifndef _PROCESS_H
#define _PROCESS_H

#include "types.h"
#include <stdint.h>

// 定义pid_t类型
typedef int32_t pid_t;

// 定义pt_regs_t类型
typedef struct pt_regs
{
    // 寄存器相关字段
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t rip, rflags;
    uint64_t cs, ss, ds, es, fs, gs;
} pt_regs_t;

// 进程状态定义
typedef enum
{
    PROCESS_CREATED,   // 新创建
    PROCESS_READY,     // 就绪态
    PROCESS_RUNNING,   // 运行态
    PROCESS_BLOCKED,   // 阻塞态
    PROCESS_TERMINATED // 终止态
} process_state_t;

// 进程控制块
typedef struct process
{
    pid_t pid;               // 进程ID
    process_state_t state;   // 进程状态
    uint32_t priority;       // 优先级
    uint32_t counter;        // 时间片计数器
    void *stack;             // 用户栈
    size_t stack_size;       // 栈大小
    struct context *context; // CPU上下文
    struct mm_struct *mm;    // 内存管理结构
    struct process *parent;  // 父进程
    struct process *next;    // 链表指针
    uint64_t creation_time;  // 创建时间
    uint64_t cpu_time;       // CPU使用时间
    int exit_code;           // 退出码
    void *kernel_stack;      // 内核栈
    uint32_t flags;          // 进程标志
    pt_regs_t *pt_regs;      // 用户态寄存器
} process_t;

// 进程管理函数
void process_init(void);
process_t *process_get_current(void);
void process_set_current(process_t *proc);
void process_set_state(process_t *proc, process_state_t new_state);
const char *process_state_to_string(process_state_t state);
int process_is_alive(process_t *proc);
pid_t process_create(void (*entry)(void *), void *arg, uint32_t priority);
void process_exit(int exit_code);
int process_wait(pid_t pid, int *exit_code);

// 进程控制命令
void process_handle_command(const char *cmdline);

// 外部变量声明
extern process_t *current;  // 当前运行的进程
extern process_t *task[64]; // 进程数组，最多支持64个进程
extern int task_count;      // 当前进程数量

<<<<<<< HEAD
// 调度相关
void schedule(void);     // 由scheduler.c实现的FIFO调度
void schedule_sjf(void); // 由process.c实现的SJF调度
void do_timer(void);
void switch_to(process_t *next);
=======
// 优先级初始值数组
extern uint32_t COUNTER_INIT_VALUES[5];
>>>>>>> 7998e53304db9a10bcd139d9f18755f696f65ff8

#endif // _PROCESS_H