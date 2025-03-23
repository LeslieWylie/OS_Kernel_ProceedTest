#include "process.h"
#include "printk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// 测试进程函数
static void test_process(void *arg)
{
    const char *name = (const char *)arg;
    int count = 0;
    while (1)
    {
        printk("进程 %s 正在运行，计数: %d\n", name, count++);
        // 模拟一些工作
        for (volatile int i = 0; i < 1000000; i++)
            ;
    }
}

// 测试进程控制命令
void test_process_control(void)
{
    printk("\n=== 开始进程控制测试 ===\n");

    // 初始化进程系统
    process_init();

    // 创建测试进程
    pid_t pid1 = process_create(test_process, "测试进程1", 1);
    pid_t pid2 = process_create(test_process, "测试进程2", 2);
    pid_t pid3 = process_create(test_process, "测试进程3", 3);

    printk("创建了三个测试进程，PID: %d, %d, %d\n", pid1, pid2, pid3);

    // 测试ps命令
    printk("\n执行ps命令:\n");
    process_handle_command("ps");

    // 测试suspend命令
    printk("\n暂停进程 %d:\n", pid2);
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "suspend %d", pid2);
    process_handle_command(cmd);

    // 再次执行ps查看状态
    printk("\n执行ps命令查看状态:\n");
    process_handle_command("ps");

    // 测试priority命令
    printk("\n修改进程 %d 的优先级为4:\n", pid3);
    snprintf(cmd, sizeof(cmd), "priority %d 4", pid3);
    process_handle_command(cmd);

    // 测试resume命令
    printk("\n恢复进程 %d:\n", pid2);
    snprintf(cmd, sizeof(cmd), "resume %d", pid2);
    process_handle_command(cmd);

    // 测试kill命令
    printk("\n终止进程 %d:\n", pid1);
    snprintf(cmd, sizeof(cmd), "kill %d", pid1);
    process_handle_command(cmd);

    // 最后执行ps查看最终状态
    printk("\n执行ps命令查看最终状态:\n");
    process_handle_command("ps");

    printk("\n=== 进程控制测试完成 ===\n");
}

// 主函数
int main(void)
{
    // 运行测试
    test_process_control();
    return 0;
}