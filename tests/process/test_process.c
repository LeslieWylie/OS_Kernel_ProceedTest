#include "process.h"
#include "scheduler.h"
#include "context.h"
#include "memory.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// 模拟printk函数
void printk(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

// 测试进程函数
static void test_process_func(void *arg)
{
    int id = *(int *)arg;
    printf("Test process %d started\n", id);

    // 模拟一些工作
    for (int i = 0; i < 3; i++)
    {
        printf("Process %d: iteration %d\n", id, i);
    }

    printf("Test process %d completed\n", id);
}

// 测试进程创建
void test_process_creation()
{
    printf("\n=== Testing Process Creation ===\n");

    // 初始化进程系统
    process_init();

    // 创建测试进程
    int id1 = 1, id2 = 2;
    pid_t pid1 = process_create(test_process_func, &id1, 1);
    pid_t pid2 = process_create(test_process_func, &id2, 2);

    printf("Created processes with PIDs: %d, %d\n", pid1, pid2);

    if (pid1 > 0 && pid2 > 0)
    {
        printf("Process creation test: PASSED\n");
    }
    else
    {
        printf("Process creation test: FAILED\n");
    }
}

// 测试进程状态转换
void test_process_states()
{
    printf("\n=== Testing Process States ===\n");

    process_t *proc = process_get_current();
    if (!proc)
    {
        printf("No current process found\n");
        return;
    }

    printf("Initial state: %s\n", process_state_to_string(proc->state));

    process_set_state(proc, PROCESS_BLOCKED);
    printf("After blocking: %s\n", process_state_to_string(proc->state));

    process_set_state(proc, PROCESS_READY);
    printf("After setting ready: %s\n", process_state_to_string(proc->state));

    if (proc->state == PROCESS_READY)
    {
        printf("Process state transition test: PASSED\n");
    }
    else
    {
        printf("Process state transition test: FAILED\n");
    }
}

// 测试调度器
void test_scheduler()
{
    printf("\n=== Testing Scheduler ===\n");

    // 创建多个优先级不同的进程
    int ids[3] = {1, 2, 3};
    pid_t pids[3];

    for (int i = 0; i < 3; i++)
    {
        pids[i] = process_create(test_process_func, &ids[i], i + 1);
        printf("Created process with PID %d and priority %d\n", pids[i], i + 1);
    }

    // 运行几次调度
    printf("Running scheduler...\n");
    for (int i = 0; i < 5; i++)
    {
        schedule();
        printf("Schedule iteration %d completed\n", i + 1);
    }

    printf("Scheduler test completed\n");
}

// 主测试函数
int main()
{
    printf("Starting process management tests...\n");

    test_process_creation();
    test_process_states();
    test_scheduler();

    printf("\nAll tests completed.\n");
    return 0;
}