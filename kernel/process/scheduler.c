#include "scheduler.h"
#include "process.h"
#include "context.h"
#include <stdio.h>

// FIFO就绪队列
static struct
{
    process_t *head; // 队列头
    process_t *tail; // 队列尾
} ready_queue = {NULL, NULL};

// 初始化调度器
void scheduler_init(void)
{
    ready_queue.head = NULL;
    ready_queue.tail = NULL;
}

// 添加进程到调度队列（FIFO尾部）
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
}

// 从调度队列中移除进程
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
            return;
        }
        current = current->next;
    }
}

// 获取下一个要运行的进程（FIFO队列头）
process_t *scheduler_next(void)
{
    return ready_queue.head ? ready_queue.head : process_get_current();
}

// 进行进程调度
void schedule(void)
{
    process_t *current = process_get_current();
    process_t *next = scheduler_next();

    if (current == next)
    {
        return; // 无需切换
    }

    // 保存当前进程上下文
    if (current && current->state == PROCESS_RUNNING)
    {
        current->state = PROCESS_READY;
        scheduler_add(current);
    }

    // 切换到新进程
    if (next)
    {
        next->state = PROCESS_RUNNING;
        process_set_current(next);
        scheduler_remove(next);

        // 切换上下文
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

// 时间片到期处理（FIFO不需要处理时间片）
void scheduler_tick(void)
{
    // FIFO调度器不需要处理时间片
}

// 阻塞当前进程
void scheduler_block(void)
{
    process_t *current = process_get_current();
    if (current)
    {
        current->state = PROCESS_BLOCKED;
        schedule(); // 触发调度
    }
}

// 唤醒进程
void scheduler_wake(process_t *proc)
{
    if (proc && proc->state == PROCESS_BLOCKED)
    {
        proc->state = PROCESS_READY;
        scheduler_add(proc);
    }
}