#include "process.h"
#include "scheduler.h"
#include "printk.h"
#include <string.h>

// 测试进程函数
static void test_process(void *arg)
{
    const char *name = (const char *)arg;
    for (int i = 0; i < 3; i++)
    {
        printk("Process %s: iteration %d\n", name, i);
        // 模拟一些工作
        for (volatile int j = 0; j < 1000000; j++)
            ;
    }
}

// 测试进程创建和调度
void test_process_creation(void)
{
    printk("Starting process creation test...\n");

    // 初始化进程系统
    process_init();

    // 创建测试进程
    pid_t pid1 = process_create(test_process, "Process1", 1);
    pid_t pid2 = process_create(test_process, "Process2", 2);
    pid_t pid3 = process_create(test_process, "Process3", 1);

    printk("Created processes with PIDs: %d, %d, %d\n", pid1, pid2, pid3);

    // 运行调度器
    for (int i = 0; i < 10; i++)
    {
        schedule();
        // 模拟时间片
        for (volatile int j = 0; j < 1000000; j++)
            ;
    }

    printk("Process creation test completed.\n");
}

// 测试进程状态转换
void test_process_state_transitions(void)
{
    printk("Starting process state transition test...\n");

    process_t *current = process_get_current();
    if (current)
    {
        printk("Current process (PID %d) state: %s\n",
               current->pid,
               process_state_to_string(current->state));

        // 测试状态转换
        process_set_state(current, PROCESS_BLOCKED);
        printk("After blocking - State: %s\n",
               process_state_to_string(current->state));

        scheduler_wake(current);
        printk("After waking - State: %s\n",
               process_state_to_string(current->state));
    }

    printk("Process state transition test completed.\n");
}

// 测试进程优先级调度
void test_priority_scheduling(void)
{
    printk("Starting priority scheduling test...\n");

    // 创建不同优先级的进程
    pid_t high_prio = process_create(test_process, "HighPriority", 3);
    pid_t med_prio = process_create(test_process, "MedPriority", 2);
    pid_t low_prio = process_create(test_process, "LowPriority", 1);

    printk("Created processes - High: %d, Med: %d, Low: %d\n",
           high_prio, med_prio, low_prio);

    // 运行调度器
    for (int i = 0; i < 10; i++)
    {
        schedule();
        // 模拟时间片
        for (volatile int j = 0; j < 1000000; j++)
            ;
    }

    printk("Priority scheduling test completed.\n");
}

// 运行所有测试
void run_process_tests(void)
{
    printk("=== Starting Process Management Tests ===\n");

    test_process_creation();
    test_process_state_transitions();
    test_priority_scheduling();

    printk("=== All Process Management Tests Completed ===\n");
}