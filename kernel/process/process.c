#include "process.h"
#include "memory.h"
#include "scheduler.h"
#include "context.h"
#include "printk.h"

// 全局变量
static pid_t next_pid = 1;
static process_t *current_process = NULL;

// 初始化进程管理系统
void process_init(void)
{
    // 创建idle进程（PID 0）
    process_t *idle = kmalloc(sizeof(process_t));
    if (!idle)
    {
        printk("Failed to create idle process\n");
        return;
    }

    // 初始化idle进程
    idle->pid = 0;
    idle->state = PROCESS_RUNNING;
    idle->priority = 0;
    idle->stack = NULL;
    idle->stack_size = 0;
    idle->context = NULL;
    idle->mm = NULL;
    idle->parent = NULL;
    idle->next = NULL;
    idle->creation_time = 0;
    idle->cpu_time = 0;
    idle->exit_code = 0;
    idle->kernel_stack = NULL;
    idle->flags = 0;

    current_process = idle;
    scheduler_init();
}

// 获取当前运行的进程
process_t *process_get_current(void)
{
    return current_process;
}

// 设置当前运行的进程
void process_set_current(process_t *proc)
{
    current_process = proc;
}

// 设置进程状态
void process_set_state(process_t *proc, process_state_t new_state)
{
    if (proc)
    {
        proc->state = new_state;
    }
}

// 将进程状态转换为字符串，用于调试
const char *process_state_to_string(process_state_t state)
{
    switch (state)
    {
    case PROCESS_CREATED:
        return "CREATED";
    case PROCESS_READY:
        return "READY";
    case PROCESS_RUNNING:
        return "RUNNING";
    case PROCESS_BLOCKED:
        return "BLOCKED";
    case PROCESS_TERMINATED:
        return "TERMINATED";
    default:
        return "UNKNOWN";
    }
}

// 检查进程是否活着
int process_is_alive(process_t *proc)
{
    return proc && proc->state != PROCESS_TERMINATED;
}

// 创建一个新进程
// 注意: 这是一个简化版本，暂不处理内存分配和上下文创建的复杂逻辑
pid_t process_create(void (*entry)(void *), void *arg, uint32_t priority)
{
    // 分配进程控制块
    process_t *proc = kmalloc(sizeof(process_t));
    if (!proc)
    {
        return -1;
    }

    // 分配内核栈
    proc->kernel_stack = allocate_kernel_stack(4096); // 4KB内核栈
    if (!proc->kernel_stack)
    {
        kfree(proc);
        return -1;
    }

    // 初始化进程控制块
    proc->pid = next_pid++;
    proc->state = PROCESS_CREATED;
    proc->priority = priority;
    proc->stack = NULL;
    proc->stack_size = 0;
    proc->context = kmalloc(sizeof(context_t));
    if (!proc->context)
    {
        free_kernel_stack(proc->kernel_stack);
        kfree(proc);
        return -1;
    }

    // 初始化上下文
    context_init(proc->context, entry, arg, (char *)proc->kernel_stack + 4096);

    proc->mm = mm_create();
    proc->parent = current_process;
    proc->next = NULL;
    proc->creation_time = 0; // TODO: 获取系统时间
    proc->cpu_time = 0;
    proc->exit_code = 0;
    proc->flags = 0;

    // 设置为就绪状态
    process_set_state(proc, PROCESS_READY);

    // 添加到调度器
    scheduler_add(proc);

    return proc->pid;
}

// 结束当前进程
void process_exit(int exit_code)
{
    if (!current_process)
    {
        return;
    }

    current_process->exit_code = exit_code;
    process_set_state(current_process, PROCESS_TERMINATED);

    // 释放资源
    if (current_process->mm)
    {
        mm_destroy(current_process->mm);
    }
    if (current_process->context)
    {
        kfree(current_process->context);
    }
    if (current_process->kernel_stack)
    {
        free_kernel_stack(current_process->kernel_stack);
    }

    // 从调度器中移除
    scheduler_remove(current_process);

    // 触发调度
    schedule();
}

// 等待指定进程结束
int process_wait(pid_t pid, int *exit_code)
{
    // TODO: 实现进程等待功能
    return -1;
}
EOF;