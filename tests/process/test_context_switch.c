#include "process.h"
#include "context.h"
#include <stdio.h>
#include <string.h>

// 测试进程1和2的任务函数
void test_task1(void *arg)
{
    printf("Task 1 started with arg: %p\n", arg);
    for (int i = 0; i < 3; i++)
    {
        printf("Task 1: iteration %d\n", i);
    }
    printf("Task 1 completed\n");
}

void test_task2(void *arg)
{
    printf("Task 2 started with arg: %p\n", arg);
    for (int i = 0; i < 3; i++)
    {
        printf("Task 2: iteration %d\n", i);
    }
    printf("Task 2 completed\n");
}

// 全局测试变量
process_t *test_task[2];
int current_task = 0;

// 模拟时钟中断处理函数
void test_timer_handler(void)
{
    printf("Timer interrupt, switching tasks...\n");

    // 切换到下一个任务
    int next_task = (current_task + 1) % 2;
    process_t *prev = test_task[current_task];
    process_t *next = test_task[next_task];

    printf("Switching from task %d to task %d\n", current_task, next_task);

    current_task = next_task;

    // 执行上下文切换
    context_switch(prev->context, next->context);
}

int main(void)
{
    printf("=== Context Switching Test ===\n");

    // 创建测试进程1
    test_task[0] = (process_t *)malloc(sizeof(process_t));
    if (!test_task[0])
    {
        printf("Failed to allocate memory for task 1\n");
        return -1;
    }
    memset(test_task[0], 0, sizeof(process_t));

    // 分配进程1的内核栈和上下文
    test_task[0]->kernel_stack = malloc(4096);
    if (!test_task[0]->kernel_stack)
    {
        printf("Failed to allocate kernel stack for task 1\n");
        free(test_task[0]);
        return -1;
    }

    test_task[0]->context = (context_t *)malloc(sizeof(context_t));
    if (!test_task[0]->context)
    {
        printf("Failed to allocate context for task 1\n");
        free(test_task[0]->kernel_stack);
        free(test_task[0]);
        return -1;
    }

    // 初始化进程1
    test_task[0]->pid = 1;
    test_task[0]->state = PROCESS_READY;
    test_task[0]->priority = 1;
    test_task[0]->counter = 5;
    context_init(test_task[0]->context, test_task1, (void *)0x12345,
                 (char *)test_task[0]->kernel_stack + 4096);

    // 创建测试进程2
    test_task[1] = (process_t *)malloc(sizeof(process_t));
    if (!test_task[1])
    {
        printf("Failed to allocate memory for task 2\n");
        free(test_task[0]->context);
        free(test_task[0]->kernel_stack);
        free(test_task[0]);
        return -1;
    }
    memset(test_task[1], 0, sizeof(process_t));

    // 分配进程2的内核栈和上下文
    test_task[1]->kernel_stack = malloc(4096);
    if (!test_task[1]->kernel_stack)
    {
        printf("Failed to allocate kernel stack for task 2\n");
        free(test_task[1]);
        free(test_task[0]->context);
        free(test_task[0]->kernel_stack);
        free(test_task[0]);
        return -1;
    }

    test_task[1]->context = (context_t *)malloc(sizeof(context_t));
    if (!test_task[1]->context)
    {
        printf("Failed to allocate context for task 2\n");
        free(test_task[1]->kernel_stack);
        free(test_task[1]);
        free(test_task[0]->context);
        free(test_task[0]->kernel_stack);
        free(test_task[0]);
        return -1;
    }

    // 初始化进程2
    test_task[1]->pid = 2;
    test_task[1]->state = PROCESS_READY;
    test_task[1]->priority = 2;
    test_task[1]->counter = 5;
    context_init(test_task[1]->context, test_task2, (void *)0x54321,
                 (char *)test_task[1]->kernel_stack + 4096);

    // 设置当前进程为进程1，并开始执行
    current_task = 0;
    printf("Starting task 1...\n");
    context_restore(test_task[0]->context);

    // 这行代码不应该被执行，因为context_restore会直接跳转到任务函数
    printf("ERROR: Should not reach here!\n");

    return 0;
}