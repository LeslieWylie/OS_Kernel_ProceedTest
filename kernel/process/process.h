#ifndef _PROCESS_H
#define _PROCESS_H

#include "types.h"
#include "memory.h"
#include "context.h"

// 进程状态定义
typedef enum
{
    PROCESS_CREATED,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

// 进程控制块
typedef struct process
{
    pid_t pid;              // 进程ID
    process_state_t state;  // 进程状态
    uint32_t priority;      // 优先级
    uint32_t counter;       // 时间片计数器
    void *stack;            // 用户栈
    size_t stack_size;      // 栈大小
    context_t *context;     // CPU上下文
    mm_struct_t *mm;        // 内存管理结构
    struct process *parent; // 父进程
    struct process *next;   // 链表指针
    uint64_t creation_time; // 创建时间
    uint64_t cpu_time;      // CPU使用时间
    int exit_code;          // 退出码
    void *kernel_stack;     // 内核栈
    uint32_t flags;         // 进程标志
    pt_regs_t *pt_regs;     // 用户态寄存器
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
extern process_t *current;
extern process_t *task[64];
extern int task_count;

#endif // _PROCESS_H