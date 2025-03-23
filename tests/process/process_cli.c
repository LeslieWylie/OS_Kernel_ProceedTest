#include "process.h"
#include "scheduler.h"
#include "context.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_COMMAND_LEN 256
#define MAX_ARGS 10
#define MAX_TASKS 10

// 模拟printk函数
void printk(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

// 全局任务数组，用于保存正在运行的任务
typedef struct
{
    pid_t pid;
    int id;
    int priority;
    char name[32];
} task_info_t;

task_info_t running_tasks[MAX_TASKS];
int task_count = 0;

// 清空任务列表
void clear_tasks()
{
    memset(running_tasks, 0, sizeof(running_tasks));
    task_count = 0;
}

// 添加任务到列表
void add_task(pid_t pid, int id, int priority, const char *name)
{
    if (task_count < MAX_TASKS)
    {
        running_tasks[task_count].pid = pid;
        running_tasks[task_count].id = id;
        running_tasks[task_count].priority = priority;
        strncpy(running_tasks[task_count].name, name, 31);
        running_tasks[task_count].name[31] = '\0'; // 确保字符串结束
        task_count++;
    }
}

// 示例任务函数
void test_task(void *arg)
{
    int id = *(int *)arg;
    printf("\n[任务 %d] 开始执行\n", id);

    // 模拟工作
    for (int i = 0; i < 3; i++)
    {
        printf("[任务 %d] 步骤 %d 执行中...\n", id, i + 1);
    }

    printf("[任务 %d] 完成执行\n", id);

    // 结束进程
    process_exit(0);
}

// 长时间运行的任务
void long_task(void *arg)
{
    int id = *(int *)arg;
    printf("\n[长任务 %d] 开始执行\n", id);

    // 模拟长时间工作
    for (int i = 0; i < 10; i++)
    {
        printf("[长任务 %d] 步骤 %d 执行中...\n", id, i + 1);

        // 每执行2步，让出CPU
        if (i % 2 == 1)
        {
            printf("[长任务 %d] 主动让出CPU...\n", id);
            schedule();
        }
    }

    printf("[长任务 %d] 完成执行\n", id);

    // 结束进程
    process_exit(0);
}

// 无限循环任务
void infinite_task(void *arg)
{
    int id = *(int *)arg;
    printf("\n[无限任务 %d] 开始执行\n", id);

    int count = 0;
    while (1)
    {
        printf("[无限任务 %d] 循环 %d 次\n", id, ++count);

        // 每循环5次，让出CPU
        if (count % 5 == 0)
        {
            printf("[无限任务 %d] 主动让出CPU...\n", id);
            schedule();
        }
    }

    // 注意：这里永远不会执行到
    process_exit(0);
}

// 打印所有命令的帮助信息
void print_help()
{
    printf("\n支持的命令：\n");
    printf("  help                - 显示帮助信息\n");
    printf("  init                - 初始化进程系统\n");
    printf("  create <名称> <优先级> - 创建一个新进程\n");
    printf("  create-long <名称> <优先级> - 创建长时间运行的进程\n");
    printf("  create-infinite <名称> <优先级> - 创建无限循环的进程\n");
    printf("  list                - 列出所有进程\n");
    printf("  schedule            - 运行FIFO调度器\n");
    printf("  schedule-sjf        - 运行最短作业优先调度器\n");
    printf("  kill <PID>          - 终止指定PID的进程\n");
    printf("  block <PID>         - 阻塞指定PID的进程\n");
    printf("  wake <PID>          - 唤醒被阻塞的进程\n");
    printf("  exit                - 退出程序\n");
}

// 解析命令行参数
int parse_args(char *cmd, char *args[])
{
    int count = 0;
    char *token = strtok(cmd, " \t\n");

    while (token != NULL && count < MAX_ARGS)
    {
        args[count++] = token;
        token = strtok(NULL, " \t\n");
    }

    args[count] = NULL;
    return count;
}

// 查找PID对应的进程
process_t *find_process_by_pid(pid_t pid)
{
    for (int i = 0; i < task_count; i++)
    {
        if (task[i] && task[i]->pid == pid)
        {
            return task[i];
        }
    }
    return NULL;
}

// 处理用户命令
void process_command(char *cmd)
{
    char *args[MAX_ARGS];
    int argc = parse_args(cmd, args);

    if (argc == 0)
        return;

    // help命令 - 显示帮助
    if (strcmp(args[0], "help") == 0)
    {
        print_help();
    }
    // init命令 - 初始化进程系统
    else if (strcmp(args[0], "init") == 0)
    {
        printf("初始化进程系统...\n");
        process_init();
        scheduler_init();
        clear_tasks();
        printf("进程系统初始化完成\n");
    }
    // create命令 - 创建新进程
    else if (strcmp(args[0], "create") == 0)
    {
        if (argc < 3)
        {
            printf("用法: create <名称> <优先级>\n");
            return;
        }

        char *name = args[1];
        int priority = atoi(args[2]);

        if (priority < 0 || priority > 4)
        {
            printf("优先级必须在0-4之间\n");
            return;
        }

        int *task_id = malloc(sizeof(int));
        *task_id = task_count + 1;

        pid_t pid = process_create(test_task, task_id, priority);

        if (pid > 0)
        {
            printf("创建进程成功，名称: %s, PID: %d, 优先级: %d\n", name, pid, priority);
            add_task(pid, *task_id, priority, name);
        }
        else
        {
            printf("创建进程失败\n");
            free(task_id);
        }
    }
    // create-long命令 - 创建长时间运行的进程
    else if (strcmp(args[0], "create-long") == 0)
    {
        if (argc < 3)
        {
            printf("用法: create-long <名称> <优先级>\n");
            return;
        }

        char *name = args[1];
        int priority = atoi(args[2]);

        if (priority < 0 || priority > 4)
        {
            printf("优先级必须在0-4之间\n");
            return;
        }

        int *task_id = malloc(sizeof(int));
        *task_id = task_count + 1;

        pid_t pid = process_create(long_task, task_id, priority);

        if (pid > 0)
        {
            printf("创建长任务成功，名称: %s, PID: %d, 优先级: %d\n", name, pid, priority);
            add_task(pid, *task_id, priority, name);
        }
        else
        {
            printf("创建长任务失败\n");
            free(task_id);
        }
    }
    // create-infinite命令 - 创建无限循环的进程
    else if (strcmp(args[0], "create-infinite") == 0)
    {
        if (argc < 3)
        {
            printf("用法: create-infinite <名称> <优先级>\n");
            return;
        }

        char *name = args[1];
        int priority = atoi(args[2]);

        if (priority < 0 || priority > 4)
        {
            printf("优先级必须在0-4之间\n");
            return;
        }

        int *task_id = malloc(sizeof(int));
        *task_id = task_count + 1;

        pid_t pid = process_create(infinite_task, task_id, priority);

        if (pid > 0)
        {
            printf("创建无限任务成功，名称: %s, PID: %d, 优先级: %d\n", name, pid, priority);
            add_task(pid, *task_id, priority, name);
        }
        else
        {
            printf("创建无限任务失败\n");
            free(task_id);
        }
    }
    // list命令 - 列出所有进程
    else if (strcmp(args[0], "list") == 0)
    {
        printf("\n当前进程列表：\n");
        printf("-----------------------------------------------------\n");
        printf("| %-4s | %-10s | %-8s | %-10s | %-10s |\n", "ID", "PID", "优先级", "状态", "名称");
        printf("-----------------------------------------------------\n");

        // 遍历进程数组
        for (int i = 0; i < task_count; i++)
        {
            process_t *proc = NULL;

            // 查找进程
            for (int j = 0; j < 64; j++)
            {
                if (task[j] && task[j]->pid == running_tasks[i].pid)
                {
                    proc = task[j];
                    break;
                }
            }

            if (proc)
            {
                printf("| %-4d | %-10d | %-8d | %-10s | %-10s |\n",
                       running_tasks[i].id,
                       proc->pid,
                       proc->priority,
                       process_state_to_string(proc->state),
                       running_tasks[i].name);
            }
            else
            {
                printf("| %-4d | %-10d | %-8d | %-10s | %-10s |\n",
                       running_tasks[i].id,
                       running_tasks[i].pid,
                       running_tasks[i].priority,
                       "未知",
                       running_tasks[i].name);
            }
        }
        printf("-----------------------------------------------------\n");
        printf("当前运行中的进程: %s\n", current ? current->name : "无");
    }
    // schedule命令 - 运行FIFO调度器
    else if (strcmp(args[0], "schedule") == 0)
    {
        printf("运行FIFO调度器...\n");
        schedule();
        printf("调度完成\n");
    }
    // schedule-sjf命令 - 运行SJF调度器
    else if (strcmp(args[0], "schedule-sjf") == 0)
    {
        printf("运行最短作业优先(SJF)调度器...\n");
        schedule_sjf();
        printf("调度完成\n");
    }
    // kill命令 - 终止进程
    else if (strcmp(args[0], "kill") == 0)
    {
        if (argc < 2)
        {
            printf("用法: kill <PID>\n");
            return;
        }

        pid_t pid = atoi(args[1]);
        process_t *proc = find_process_by_pid(pid);

        if (proc)
        {
            printf("终止进程 PID: %d\n", pid);
            proc->state = PROCESS_TERMINATED;
            scheduler_remove(proc);
            schedule();
            printf("进程已终止\n");
        }
        else
        {
            printf("未找到PID为 %d 的进程\n", pid);
        }
    }
    // block命令 - 阻塞进程
    else if (strcmp(args[0], "block") == 0)
    {
        if (argc < 2)
        {
            printf("用法: block <PID>\n");
            return;
        }

        pid_t pid = atoi(args[1]);
        process_t *proc = find_process_by_pid(pid);

        if (proc)
        {
            printf("阻塞进程 PID: %d\n", pid);
            proc->state = PROCESS_BLOCKED;
            scheduler_remove(proc);
            schedule();
            printf("进程已阻塞\n");
        }
        else
        {
            printf("未找到PID为 %d 的进程\n", pid);
        }
    }
    // wake命令 - 唤醒进程
    else if (strcmp(args[0], "wake") == 0)
    {
        if (argc < 2)
        {
            printf("用法: wake <PID>\n");
            return;
        }

        pid_t pid = atoi(args[1]);
        process_t *proc = find_process_by_pid(pid);

        if (proc)
        {
            printf("唤醒进程 PID: %d\n", pid);
            scheduler_wake(proc);
            printf("进程已唤醒\n");
        }
        else
        {
            printf("未找到PID为 %d 的进程\n", pid);
        }
    }
    // exit命令 - 退出程序
    else if (strcmp(args[0], "exit") == 0)
    {
        printf("退出程序\n");
        exit(0);
    }
    else
    {
        printf("未知命令: %s, 输入 'help' 获取帮助\n", args[0]);
    }
}

// 主函数
int main()
{
    printf("进程管理系统命令行界面\n");
    printf("输入 'help' 获取可用命令列表\n");

    // 初始化系统
    printf("初始化进程系统...\n");
    process_init();
    scheduler_init();
    clear_tasks();
    printf("系统初始化完成\n\n");

    char command[MAX_COMMAND_LEN];

    while (1)
    {
        printf("> ");
        fflush(stdout);

        if (fgets(command, MAX_COMMAND_LEN, stdin) == NULL)
        {
            break;
        }

        // 去除换行符
        command[strcspn(command, "\n")] = 0;

        // 处理命令
        process_command(command);
    }

    return 0;
}