#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"     // 基本类型定义
#include "process.h"   // 进程相关
#include "memory.h"    // 内存管理
#include "context.h"   // 上下文操作
#include "scheduler.h" // 调度器

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

/**
 * @brief 初始化进程管理系统
 *
 * 创建idle进程（PID 0）并初始化调度器
 */
void process_init(void)
{
    printf("Initializing process management system...\n");

    // 创建idle进程（PID 0）
    process_t *idle = (process_t *)kmalloc(sizeof(process_t));
    if (!idle)
    {
        printf("ERROR: Failed to create idle process\n");
        return;
    }

    // 初始化idle进程
    memset(idle, 0, sizeof(process_t));
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
        printf("ERROR: Failed to allocate kernel stack for idle process\n");
        kfree(idle);
        return;
    }
    idle->flags = 0;
    strncpy(idle->name, "idle", sizeof(idle->name) - 1);
    idle->name[sizeof(idle->name) - 1] = '\0';

    // 初始化上下文
    idle->context = (context_t *)kmalloc(sizeof(context_t));
    if (!idle->context)
    {
        printf("ERROR: Failed to allocate context for idle process\n");
        kfree(idle->kernel_stack);
        kfree(idle);
        return;
    }
    memset(idle->context, 0, sizeof(context_t));

    // 设置全局变量
    current = idle;
    task[0] = idle;
    task_count = 1;

    // 初始化调度器
    scheduler_init();

    printf("Process subsystem initialized successfully.\n");
    printf("Idle process (PID 0) created and running.\n");
}

/**
 * @brief 获取当前运行的进程
 *
 * @return process_t* 当前进程控制块指针
 */
process_t *process_get_current(void)
{
    return current;
}

/**
 * @brief 设置当前运行的进程
 *
 * @param proc 要设置为当前进程的进程控制块
 */
void process_set_current(process_t *proc)
{
    current = proc;
}

/**
 * @brief 设置进程状态
 *
 * @param proc 要修改的进程
 * @param new_state 新的进程状态
 */
void process_set_state(process_t *proc, process_state_t new_state)
{
    if (proc)
    {
        process_state_t old_state = proc->state;
        proc->state = new_state;

        if (old_state != new_state)
        {
            printf("Process %d (%s) state changed: %s -> %s\n",
                   proc->pid, proc->name,
                   process_state_to_string(old_state),
                   process_state_to_string(new_state));
        }
    }
}

/**
 * @brief 将进程状态转换为字符串
 *
 * @param state 进程状态枚举值
 * @return const char* 状态的字符串表示
 */
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

/**
 * @brief 检查进程是否活着
 *
 * @param proc 要检查的进程
 * @return int 如果进程存在且未终止返回1，否则返回0
 */
int process_is_alive(process_t *proc)
{
    return proc && proc->state != PROCESS_TERMINATED;
}

/**
 * @brief 创建一个新进程
 *
 * @param entry 进程入口函数
 * @param arg 传递给入口函数的参数
 * @param priority 进程优先级
 * @return pid_t 新进程的PID，失败返回-1
 */
pid_t process_create(void (*entry)(void *), void *arg, uint32_t priority)
{
    // 检查参数
    if (!entry)
    {
        printf("ERROR: Cannot create process with NULL entry point\n");
        return -1;
    }

    // 检查进程数量上限
    if (task_count >= 64)
    {
        printf("ERROR: Cannot create process, maximum process count reached\n");
        return -1; // 进程数量已达上限
    }

    // 分配进程控制块
    process_t *proc = (process_t *)kmalloc(sizeof(process_t));
    if (!proc)
    {
        printf("ERROR: Failed to allocate process control block\n");
        return -1;
    }

    // 初始化进程控制块
    memset(proc, 0, sizeof(process_t));
    proc->pid = next_pid++;
    proc->state = PROCESS_CREATED;
    proc->priority = priority < 5 ? priority : 4; // 确保优先级在有效范围内
    proc->counter = COUNTER_INIT_VALUES[proc->priority];

    // 分配内核栈
    proc->kernel_stack = kmalloc(4096); // 4KB内核栈
    if (!proc->kernel_stack)
    {
        printf("ERROR: Failed to allocate kernel stack for process %d\n", proc->pid);
        kfree(proc);
        return -1;
    }

    // 分配上下文
    proc->context = (context_t *)kmalloc(sizeof(context_t));
    if (!proc->context)
    {
        printf("ERROR: Failed to allocate context for process %d\n", proc->pid);
        kfree(proc->kernel_stack);
        kfree(proc);
        return -1;
    }

    // 初始化上下文
    context_init(proc->context, entry, arg, (char *)proc->kernel_stack + 4096);

    // 为用户态进程准备
    proc->pt_regs = (pt_regs_t *)kmalloc(sizeof(pt_regs_t));
    if (!proc->pt_regs)
    {
        printf("ERROR: Failed to allocate register state for process %d\n", proc->pid);
        kfree(proc->context);
        kfree(proc->kernel_stack);
        kfree(proc);
        return -1;
    }
    memset(proc->pt_regs, 0, sizeof(pt_regs_t));

    // 设置进程关系
    proc->parent = current;

    // 设置进程名称（默认使用PID）
    snprintf(proc->name, sizeof(proc->name), "proc-%d", proc->pid);

    // 记录进程创建时间（简单实现）
    proc->creation_time = 0; // 在真实系统中应该设置为当前时间

    // 设置为就绪状态
    process_set_state(proc, PROCESS_READY);

    // 添加到进程数组
    for (int i = 0; i < 64; i++)
    {
        if (task[i] == NULL)
        {
            task[i] = proc;
            if (i >= task_count)
            {
                task_count = i + 1;
            }
            break;
        }
    }

    // 添加到调度队列
    scheduler_add(proc);

    printf("Created process %d (%s) with priority %d, entry at %p\n",
           proc->pid, proc->name, proc->priority, (void *)entry);
    return proc->pid;
}

/**
 * @brief 结束当前进程
 *
 * @param exit_code 进程退出码
 */
void process_exit(int exit_code)
{
    // 检查当前进程
    if (!current)
    {
        printf("ERROR: No current process to exit\n");
        return;
    }

    // 防止终止idle进程
    if (current->pid == 0)
    {
        printf("ERROR: Cannot terminate idle process\n");
        return;
    }

    printf("Process %d (%s) exiting with code %d\n",
           current->pid, current->name, exit_code);

    // 设置退出码和状态
    current->exit_code = exit_code;
    process_set_state(current, PROCESS_TERMINATED);

    // 释放资源
    if (current->mm)
    {
        mm_destroy(current->mm);
        current->mm = NULL;
    }

    if (current->pt_regs)
    {
        kfree(current->pt_regs);
        current->pt_regs = NULL;
    }

    // 从调度器中移除
    scheduler_remove(current);

    // 唤醒等待此进程的其他进程
    for (int i = 0; i < task_count; i++)
    {
        if (task[i] && task[i]->state == PROCESS_BLOCKED &&
            task[i]->pid > 0) // 确保不是空进程且不是idle进程
        {
            // 简单实现，实际应该检查等待原因
            scheduler_wake(task[i]);
        }
    }

    printf("Process %d resources released, scheduling next process\n", current->pid);

    // 触发调度
    schedule();

    // 注意：此处代码永远不会被执行，因为调度后已经切换到其他进程
    printf("ERROR: process_exit() returned! This should never happen.\n");
    while (1)
    {
    } // 防止返回
}

/**
 * @brief 等待指定进程结束
 *
 * @param pid 要等待的进程ID
 * @param exit_code 用于存储退出码的指针，如果不需要可以为NULL
 * @return int 成功返回0，失败返回-1
 */
int process_wait(pid_t pid, int *exit_code)
{
    // 检查当前进程
    if (!current)
    {
        printf("ERROR: No current process to wait\n");
        return -1;
    }

    // 防止idle进程等待
    if (current->pid == 0)
    {
        printf("ERROR: Idle process cannot wait\n");
        return -1;
    }

    // 防止自等待
    if (current->pid == pid)
    {
        printf("ERROR: Process cannot wait for itself\n");
        return -1;
    }

    // 查找目标进程
    process_t *target = NULL;
    for (int i = 0; i < task_count; i++)
    {
        if (task[i] && task[i]->pid == pid)
        {
            target = task[i];
            break;
        }
    }

    // 检查目标进程是否存在
    if (!target)
    {
        printf("ERROR: Cannot wait for non-existent process %d\n", pid);
        return -1; // 进程不存在
    }

    printf("Process %d (%s) waiting for process %d (%s) to terminate\n",
           current->pid, current->name, target->pid, target->name);

    // 如果进程未终止，阻塞当前进程
    if (target->state != PROCESS_TERMINATED)
    {
        // 标记等待原因（真实实现中应该记录具体等待的进程）
        current->state = PROCESS_BLOCKED;
        schedule();

        // 从调度中返回后，再次检查目标进程
        // 重新查找，因为可能已经被释放
        target = NULL;
        for (int i = 0; i < task_count; i++)
        {
            if (task[i] && task[i]->pid == pid)
            {
                target = task[i];
                break;
            }
        }

        // 如果进程不存在或未终止，返回错误
        if (!target)
        {
            printf("ERROR: Target process %d no longer exists after waiting\n", pid);
            return -1;
        }

        if (target->state != PROCESS_TERMINATED)
        {
            printf("ERROR: Target process %d not terminated after waiting\n", pid);
            return -1;
        }
    }

    // 进程已终止，获取退出码
    if (exit_code)
    {
        *exit_code = target->exit_code;
    }

    printf("Process %d terminated with exit code %d\n",
           target->pid, target->exit_code);

    // 释放已终止进程的资源
    if (target->context)
    {
        kfree(target->context);
    }
    if (target->kernel_stack)
    {
        free_kernel_stack(target->kernel_stack);
    }

    // 从任务数组中移除
    for (int i = 0; i < task_count; i++)
    {
        if (task[i] == target)
        {
            task[i] = NULL;
            break;
        }
    }

    // 释放进程控制块
    kfree(target);

    // 更新进程计数
    int new_count = 0;
    for (int i = 0; i < task_count; i++)
    {
        if (task[i] != NULL)
        {
            new_count = i + 1;
        }
    }
    task_count = new_count > 0 ? new_count : 1; // 至少保留idle进程

    return 0;
}

/**
 * @brief 时间片到期处理
 *
 * 更新当前进程的时间计数，并在必要时触发调度
 */
void do_timer(void)
{
    if (!current)
    {
        return;
    }

    // 更新CPU时间统计
    current->cpu_time++;

    // 打印调试信息
    if (current->pid != 0) // 不是idle进程
    {
        printf("Timer tick: process %d (%s), counter=%d, cpu_time=%lu\n",
               current->pid, current->name, current->counter, (unsigned long)current->cpu_time);
    }

    // 更新调度计数器
    if (current->counter > 0)
    {
        current->counter--;
    }

    // 如果计数器为0，触发调度
    if (current->counter == 0)
    {
        printf("Process %d (%s) time slice exhausted, scheduling...\n", current->pid, current->name);
        schedule();
    }
}

/**
 * @brief 简单的调度算法 (SJF - Shortest Job First)
 *
 * 优先选择计数器值最小的非零进程运行
 */
void schedule_sjf(void)
{
    if (!current)
    {
        printf("ERROR: No current process in schedule_sjf\n");
        return;
    }

    printf("Performing SJF scheduling...\n");

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
        printf("No runnable process with non-zero counter, refreshing time slices\n");

        int ready_count = 0;
        for (int i = 1; i < task_count; i++)
        {
            if (task[i] && task[i]->state == PROCESS_READY)
            {
                task[i]->counter = COUNTER_INIT_VALUES[task[i]->priority % 5];
                printf("Refreshed process %d (%s) counter to %d\n",
                       task[i]->pid, task[i]->name, task[i]->counter);
                ready_count++;
            }
        }

        if (ready_count > 0)
        {
            // 重新调度
            schedule_sjf();
            return;
        }
        else
        {
            printf("No ready processes, returning to idle\n");
            // 切换到空闲进程
            if (current->pid != 0)
            {
                switch_to(task[0]);
            }
            return;
        }
    }

    // 切换到选中的进程
    printf("SJF selected process %d (%s) with counter %d\n",
           task[next]->pid, task[next]->name, task[next]->counter);
    switch_to(task[next]);
}

/**
 * @brief 切换到指定进程
 *
 * @param next 要切换到的目标进程
 */
void switch_to(process_t *next)
{
    // 参数检查
    if (!next)
    {
        printf("ERROR: Cannot switch to NULL process\n");
        return;
    }

    if (!current)
    {
        printf("ERROR: No current process in switch_to\n");
        return;
    }

    // 如果目标进程已经是当前进程，则无需切换
    if (current == next)
    {
        printf("Process %d (%s) is already running, no switch needed\n",
               current->pid, current->name);
        return;
    }

    // 保存当前进程和目标进程的指针
    process_t *prev = current;

    // 切换当前进程指针
    current = next;

    // 更新进程状态
    if (prev->state == PROCESS_RUNNING)
    {
        prev->state = PROCESS_READY;
    }
    next->state = PROCESS_RUNNING;

    printf("Switching from process %d (%s) to %d (%s)\n",
           prev->pid, prev->name, next->pid, next->name);

    // 检查上下文是否有效
    if (!prev->context || !next->context)
    {
        printf("ERROR: Invalid context in switch_to (prev=%p, next=%p)\n",
               prev->context, next->context);
        return;
    }

    // 切换上下文
    context_switch(prev->context, next->context);

    // 注意：此处代码只会在再次切换回来时执行
    printf("Returned to process %d (%s) after context switch\n",
           prev->pid, prev->name);
}