#include <stdio.h>

#include "types.h"
#include "scheduler.h"
#include "process.h"
#include "context.h"

/**
 * @brief 就绪队列结构
 *
 * 使用简单的单向链表实现FIFO（先进先出）就绪队列
 */
static struct
{
    process_t *head; // 队列头
    process_t *tail; // 队列尾
} ready_queue = {NULL, NULL};

/**
 * @brief 初始化调度器
 *
 * 将就绪队列初始化为空
 */
void scheduler_init(void)
{
    ready_queue.head = NULL;
    ready_queue.tail = NULL;
    printf("Scheduler initialized (FIFO policy)\n");
}

/**
 * @brief 添加进程到调度队列
 *
 * 将进程添加到就绪队列尾部，实现FIFO（先进先出）调度策略
 *
 * @param proc 要添加的进程
 */
void scheduler_add(process_t *proc)
{
    if (!proc)
    {
        return;
    }

    proc->next = NULL;

    if (!ready_queue.head)
    {
        // 空队列
        ready_queue.head = proc;
        ready_queue.tail = proc;
    }
    else
    {
        // 添加到队列尾部
        ready_queue.tail->next = proc;
        ready_queue.tail = proc;
    }

    printf("Process %d (%s) added to scheduler queue\n",
           proc->pid, proc->name);
}

/**
 * @brief 从调度队列中移除进程
 *
 * 将指定进程从就绪队列中移除
 *
 * @param proc 要移除的进程
 */
void scheduler_remove(process_t *proc)
{
    if (!proc || !ready_queue.head)
    {
        return;
    }

    if (ready_queue.head == proc)
    {
        // 移除队列头
        ready_queue.head = proc->next;
        if (!ready_queue.head)
        {
            ready_queue.tail = NULL;
        }
        printf("Process %d (%s) removed from scheduler queue (head)\n",
               proc->pid, proc->name);
        return;
    }

    // 在队列中查找并移除
    process_t *current = ready_queue.head;
    while (current->next)
    {
        if (current->next == proc)
        {
            current->next = proc->next;
            if (ready_queue.tail == proc)
            {
                ready_queue.tail = current;
            }
            printf("Process %d (%s) removed from scheduler queue\n",
                   proc->pid, proc->name);
            return;
        }
        current = current->next;
    }
}

/**
 * @brief 获取下一个要运行的进程
 *
 * 从就绪队列中获取下一个要运行的进程（队列头）
 * 如果队列为空，返回当前运行的进程（空闲进程）
 *
 * @return process_t* 下一个要运行的进程
 */
process_t *scheduler_next(void)
{
    process_t *next = ready_queue.head ? ready_queue.head : process_get_current();

    if (next)
    {
        printf("Next process to run: %d (%s)\n", next->pid, next->name);
    }
    else
    {
        printf("No process to run, keeping idle\n");
    }

    return next;
}

/**
 * @brief 进行进程调度
 *
 * 选择下一个要运行的进程并进行上下文切换
 * 这是FIFO调度策略的实现
 */
void schedule(void)
{
    printf("Scheduling...\n");

    process_t *current = process_get_current();
    process_t *next = scheduler_next();

    if (current == next)
    {
        printf("No need to switch, keeping current process\n");
        return; // 无需切换
    }

    // 保存当前进程上下文
    if (current && current->state == PROCESS_RUNNING)
    {
        printf("Setting process %d (%s) to READY state\n",
               current->pid, current->name);
        current->state = PROCESS_READY;
        scheduler_add(current);
    }

    // 切换到新进程
    if (next)
    {
        printf("Setting process %d (%s) to RUNNING state\n",
               next->pid, next->name);
        next->state = PROCESS_RUNNING;
        process_set_current(next);
        scheduler_remove(next);

        // 切换上下文
        printf("Switching context from process %d to %d\n",
               current->pid, next->pid);
        if (current && current->context)
        {
            context_switch(current->context, next->context);
        }
        else
        {
            context_restore(next->context);
        }
    }
}

/**
 * @brief 时间片到期处理
 *
 * 在时钟中断处理中调用，更新进程计数器并触发调度
 * 注意：当前FIFO实现不使用时间片，此函数为将来支持其他调度算法预留
 */
void scheduler_tick(void)
{
    // FIFO调度器不需要处理时间片
    // 将来实现优先级或轮转调度时再完善此函数
}

/**
 * @brief 阻塞当前进程
 *
 * 将当前进程状态设置为阻塞，并触发调度器选择下一个进程运行
 */
void scheduler_block(void)
{
    process_t *current = process_get_current();
    if (current)
    {
        printf("Blocking process %d (%s)\n", current->pid, current->name);
        current->state = PROCESS_BLOCKED;
        schedule(); // 触发调度
    }
}

/**
 * @brief 唤醒进程
 *
 * 将指定进程的状态从阻塞改为就绪，并将其添加到就绪队列
 *
 * @param proc 要唤醒的进程
 */
void scheduler_wake(process_t *proc)
{
    if (proc && proc->state == PROCESS_BLOCKED)
    {
        printf("Waking up process %d (%s)\n", proc->pid, proc->name);
        proc->state = PROCESS_READY;
        scheduler_add(proc);
    }
}