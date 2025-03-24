#include "types.h"
#include "../../include/kernel/process.h"
#include "../../include/kernel/memory.h"
#include "../../include/kernel/scheduler.h"
#include "context.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 全局变量
static pid_t next_pid = 1;
process_t *current = NULL;    // 当前运行的进程
process_t *task[64] = {NULL}; // 进程数组
int task_count = 0;           // 当前进程数量

// 进程初始计数器和优先级
static uint32_t PRIORITY_INIT_VALUES[5] = {0, 1, 2, 3, 4};
static uint32_t COUNTER_INIT_VALUES[5] = {0, 4, 3, 2, 1};

// 声明context_switch汇编函数
extern void __switch_to(process_t *prev, process_t *next);

// 初始化进程管理系统
void process_init(void)
{
    // 创建idle进程（PID 0）
    process_t *idle = kmalloc(sizeof(process_t));
    if (!idle)
    {
        printf("Failed to create idle process\n");
        return;
    }

    // 初始化idle进程
    idle->pid = 0;
    idle->state = PROCESS_RUNNING;
    idle->priority = 0;
    idle->counter = 0;
    idle->stack = NULL;
    idle->stack_size = 0;
    idle->context = NULL;
    idle->mm = NULL;
    idle->parent = NULL;
    idle->next = NULL;
    idle->creation_time = 0;
    idle->cpu_time = 0;
    idle->exit_code = 0;
    idle->kernel_stack = kmalloc(4096); // 4KB内核栈
    if (!idle->kernel_stack)
    {
        kfree(idle);
        return;
    }
    idle->flags = 0;

    current = idle;
    task[0] = idle;
    task_count = 1;

    // 初始化上下文
    idle->context = kmalloc(sizeof(context_t));
    if (!idle->context)
    {
        kfree(idle->kernel_stack);
        kfree(idle);
        return;
    }
    memset(idle->context, 0, sizeof(context_t));

    scheduler_init();
}

// 获取当前运行的进程
process_t *process_get_current(void)
{
    return current;
}

// 设置当前运行的进程
void process_set_current(process_t *proc)
{
    current = proc;
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
    if (task_count >= 64)
    {
        return -1; // 进程数量已达上限
    }

    // 分配进程控制块
    process_t *proc = kmalloc(sizeof(process_t));
    if (!proc)
    {
        return -1;
    }

    // 初始化进程控制块
    memset(proc, 0, sizeof(process_t));
    proc->pid = next_pid++;
    proc->state = PROCESS_CREATED;
    proc->priority = priority;
    proc->counter = COUNTER_INIT_VALUES[priority % 5];

    // 分配内核栈
    proc->kernel_stack = kmalloc(4096); // 4KB内核栈
    if (!proc->kernel_stack)
    {
        kfree(proc);
        return -1;
    }

    // 分配上下文
    proc->context = kmalloc(sizeof(context_t));
    if (!proc->context)
    {
        kfree(proc->kernel_stack);
        kfree(proc);
        return -1;
    }

    // 初始化上下文
    context_init(proc->context, entry, arg, (char *)proc->kernel_stack + 4096);

    // 为用户态进程准备
    proc->pt_regs = kmalloc(sizeof(pt_regs_t));
    if (!proc->pt_regs)
    {
        kfree(proc->context);
        kfree(proc->kernel_stack);
        kfree(proc);
        return -1;
    }
    memset(proc->pt_regs, 0, sizeof(pt_regs_t));

    // 设置进程关系
    proc->parent = current;

    // 设置为就绪状态
    process_set_state(proc, PROCESS_READY);

    // 添加到进程数组
    task[task_count++] = proc;

    return proc->pid;
}

// 结束当前进程
void process_exit(int exit_code)
{
    if (!current)
    {
        return;
    }

    current->exit_code = exit_code;
    process_set_state(current, PROCESS_TERMINATED);

    // 释放资源
    if (current->mm)
    {
        mm_destroy(current->mm);
    }
    if (current->context)
    {
        kfree(current->context);
    }
    if (current->kernel_stack)
    {
        free_kernel_stack(current->kernel_stack);
    }

    // 从调度器中移除
    scheduler_remove(current);

    // 触发调度
    schedule(); // 使用scheduler.c中的FIFO调度实现
}

// 等待指定进程结束
int process_wait(pid_t pid, int *exit_code)
{
    // 查找进程
    process_t *target = NULL;
    for (int i = 1; i < task_count; i++)
    {
        if (task[i] && task[i]->pid == pid)
        {
            target = task[i];
            break;
        }
    }

    if (!target)
    {
        return -1; // 进程不存在
    }

    // 简单实现：如果进程未终止，阻塞当前进程
    if (target->state != PROCESS_TERMINATED)
    {
        current->state = PROCESS_BLOCKED;
        schedule(); // 使用scheduler.c中的FIFO调度实现
    }

    // 进程已终止，获取退出码
    if (exit_code)
    {
        *exit_code = target->exit_code;
    }

    return 0;
}

// 时间片中断处理
void do_timer(void)
{
    // 当前进程的计数器减1
    if (current && current->pid > 0)
    {
        current->counter--;
        // 如果计数器为0，触发调度
        if (current->counter == 0)
        {
            schedule(); // 使用scheduler.c中的FIFO调度实现
        }
    }
}

// 简单的调度算法 (SJF - Shortest Job First)
void schedule_sjf(void)
{
    int next = 0;
    uint32_t min_counter = 0xFFFFFFFF;

    // 查找计数器值最小的非零进程
    for (int i = 1; i < task_count; i++)
    {
        if (task[i] && task[i]->state == PROCESS_READY &&
            task[i]->counter > 0 && task[i]->counter < min_counter)
        {
            min_counter = task[i]->counter;
            next = i;
        }
    }

    // 如果没有可运行的进程，重新分配时间片
    if (next == 0)
    {
        for (int i = 1; i < task_count; i++)
        {
            if (task[i] && task[i]->state == PROCESS_READY)
            {
                task[i]->counter = COUNTER_INIT_VALUES[task[i]->priority % 5];
            }
        }
        // 重新调度
        schedule_sjf();
        return;
    }

    // 切换到选中的进程
    switch_to(task[next]);
}

// 切换到下一个进程
void switch_to(process_t *next)
{
    if (!next || current == next)
    {
        return;
    }

    process_t *prev = current;
    current = next;

    // 切换状态
    if (prev->state == PROCESS_RUNNING)
    {
        prev->state = PROCESS_READY;
    }
    next->state = PROCESS_RUNNING;

    // 切换上下文
    context_switch(prev->context, next->context);
}