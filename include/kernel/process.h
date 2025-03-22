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

// 前向声明
struct mm_struct;
struct context;

// 进程控制块结构
typedef struct process
{
    pid_t pid;               // 进程ID
    process_state_t state;   // 进程状态
    uint32_t priority;       // 进程优先级
    void *stack;             // 进程栈指针
    size_t stack_size;       // 栈大小
    struct context *context; // CPU上下文
    struct mm_struct *mm;    // 内存管理结构
    struct process *parent;  // 父进程
    struct process *next;    // 调度链表中的下一个进程
    uint64_t creation_time;  // 创建时间
    uint64_t cpu_time;       // CPU使用时间
    int exit_code;           // 退出码
    char name[32];           // 进程名称
    void *kernel_stack;      // 内核栈
    uint32_t flags;          // 进程标志
} process_t;

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

#endif // _KERNEL_PROCESS_H