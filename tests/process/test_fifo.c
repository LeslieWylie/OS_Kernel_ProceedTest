#include "process.h"
#include "scheduler.h"
#include "printk.h"
#include <stdio.h>
#include <string.h>

// 模拟printk函数
void printk(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

// 测试进程函数
static void test_process(void *arg)
{
    int id = *(int *)arg;
    printf("Process %d started\n", id);

    // 模拟工作
    for (int i = 0; i < 3; i++)
    {
        printf("Process %d: Step %d\n", id, i + 1);
    }

    printf("Process %d finished\n", id);
}

// 测试FIFO调度
void test_fifo_scheduling()
{
    printf("\n=== Testing FIFO Scheduler ===\n");

    // 初始化进程系统
    process_init();

    // 创建测试进程
    int process_ids[5] = {1, 2, 3, 4, 5};
    pid_t pids[5];

    // 按顺序创建进程
    printf("Creating processes in order: 1, 2, 3, 4, 5\n");
    for (int i = 0; i < 5; i++)
    {
        pids[i] = process_create(test_process, &process_ids[i], 1);
        printf("Created process %d with PID %d\n", i + 1, pids[i]);
    }

    // 运行调度器，验证FIFO顺序
    printf("\nRunning scheduler - processes should execute in FIFO order\n");
    for (int i = 0; i < 10; i++)
    {
        printf("\nSchedule iteration %d:\n", i + 1);
        schedule();

        // 获取当前运行的进程
        process_t *current = process_get_current();
        if (current)
        {
            printf("Currently running: Process with PID %d\n", current->pid);
        }
    }
}

// 测试阻塞和唤醒
void test_block_and_wake()
{
    printf("\n=== Testing Process Block and Wake ===\n");

    // 创建两个测试进程
    int id1 = 1, id2 = 2;
    pid_t pid1 = process_create(test_process, &id1, 1);
    pid_t pid2 = process_create(test_process, &id2, 1);

    printf("Created two processes: PID %d and %d\n", pid1, pid2);

    // 运行第一个进程
    schedule();
    process_t *proc1 = process_get_current();

    // 阻塞当前进程
    printf("Blocking process PID %d\n", proc1->pid);
    scheduler_block();

    // 检查是否切换到第二个进程
    process_t *current = process_get_current();
    printf("After blocking - Current process: PID %d\n", current->pid);

    // 唤醒第一个进程
    printf("Waking up process PID %d\n", proc1->pid);
    scheduler_wake(proc1);

    // 再次调度
    schedule();
    current = process_get_current();
    printf("After wake - Current process: PID %d\n", current->pid);
}

int main()
{
    printf("Starting FIFO scheduler tests...\n");

    test_fifo_scheduling();
    test_block_and_wake();

    printf("\nAll FIFO scheduler tests completed.\n");
    return 0;
}