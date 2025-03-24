#include "process.h"
#include "printk.h"
#include "scheduler.h"
#include <string.h>

// 进程控制命令
#define MAX_CMD_LEN 256
#define MAX_ARGS 8

// 命令结构体
typedef struct
{
    const char *name;
    void (*handler)(int argc, char *argv[]);
    const char *description;
} cmd_t;

// 命令处理函数声明
static void cmd_ps(int argc, char *argv[]);
static void cmd_kill(int argc, char *argv[]);
static void cmd_suspend(int argc, char *argv[]);
static void cmd_resume(int argc, char *argv[]);
static void cmd_priority(int argc, char *argv[]);
static void cmd_help(int argc, char *argv[]);

// 命令表
static const cmd_t commands[] = {
    {"ps", cmd_ps, "显示所有进程信息"},
    {"kill", cmd_kill, "终止指定进程"},
    {"suspend", cmd_suspend, "暂停指定进程"},
    {"resume", cmd_resume, "恢复指定进程"},
    {"priority", cmd_priority, "设置进程优先级"},
    {"help", cmd_help, "显示帮助信息"},
    {NULL, NULL, NULL}};

// 显示所有进程信息
static void cmd_ps(int argc, char *argv[])
{
    printk("\n进程列表:\n");
    printk("PID\t状态\t\t优先级\t计数器\tCPU时间\n");
    printk("----------------------------------------\n");

    for (int i = 0; i < task_count; i++)
    {
        if (task[i])
        {
            printk("%d\t%s\t%d\t%d\t%d\n",
                   task[i]->pid,
                   process_state_to_string(task[i]->state),
                   task[i]->priority,
                   task[i]->counter,
                   task[i]->cpu_time);
        }
    }
    printk("\n");
}

// 终止指定进程
static void cmd_kill(int argc, char *argv[])
{
    if (argc != 2)
    {
        printk("用法: kill <pid>\n");
        return;
    }

    pid_t pid = atoi(argv[1]);
    process_t *target = NULL;

    for (int i = 0; i < task_count; i++)
    {
        if (task[i] && task[i]->pid == pid)
        {
            target = task[i];
            break;
        }
    }

    if (!target)
    {
        printk("错误: 进程 %d 不存在\n", pid);
        return;
    }

    if (target->pid == 0)
    {
        printk("错误: 不能终止idle进程\n");
        return;
    }

    process_exit(0);
    printk("进程 %d 已终止\n", pid);
}

// 暂停指定进程
static void cmd_suspend(int argc, char *argv[])
{
    if (argc != 2)
    {
        printk("用法: suspend <pid>\n");
        return;
    }

    pid_t pid = atoi(argv[1]);
    process_t *target = NULL;

    for (int i = 0; i < task_count; i++)
    {
        if (task[i] && task[i]->pid == pid)
        {
            target = task[i];
            break;
        }
    }

    if (!target)
    {
        printk("错误: 进程 %d 不存在\n", pid);
        return;
    }

    if (target->state == PROCESS_BLOCKED)
    {
        printk("进程 %d 已经处于暂停状态\n", pid);
        return;
    }

    target->state = PROCESS_BLOCKED;
    scheduler_remove(target);
    printk("进程 %d 已暂停\n", pid);
}

// 恢复指定进程
static void cmd_resume(int argc, char *argv[])
{
    if (argc != 2)
    {
        printk("用法: resume <pid>\n");
        return;
    }

    pid_t pid = atoi(argv[1]);
    process_t *target = NULL;

    for (int i = 0; i < task_count; i++)
    {
        if (task[i] && task[i]->pid == pid)
        {
            target = task[i];
            break;
        }
    }

    if (!target)
    {
        printk("错误: 进程 %d 不存在\n", pid);
        return;
    }

    if (target->state != PROCESS_BLOCKED)
    {
        printk("进程 %d 未处于暂停状态\n", pid);
        return;
    }

    target->state = PROCESS_READY;
    scheduler_add(target);
    printk("进程 %d 已恢复\n", pid);
}

// 设置进程优先级
static void cmd_priority(int argc, char *argv[])
{
    if (argc != 3)
    {
        printk("用法: priority <pid> <new_priority>\n");
        return;
    }

    pid_t pid = atoi(argv[1]);
    uint32_t new_priority = atoi(argv[2]);

    if (new_priority < 1 || new_priority > 5)
    {
        printk("错误: 优先级必须在1-5之间\n");
        return;
    }

    process_t *target = NULL;
    for (int i = 0; i < task_count; i++)
    {
        if (task[i] && task[i]->pid == pid)
        {
            target = task[i];
            break;
        }
    }

    if (!target)
    {
        printk("错误: 进程 %d 不存在\n", pid);
        return;
    }

    target->priority = new_priority;
    target->counter = COUNTER_INIT_VALUES[new_priority % 5];
    printk("进程 %d 的优先级已设置为 %d\n", pid, new_priority);
}

// 显示帮助信息
static void cmd_help(int argc, char *argv[])
{
    printk("\n可用命令:\n");
    for (const cmd_t *cmd = commands; cmd->name != NULL; cmd++)
    {
        printk("%-10s - %s\n", cmd->name, cmd->description);
    }
    printk("\n");
}

// 解析命令行
static void parse_command(char *cmdline)
{
    char *argv[MAX_ARGS];
    int argc = 0;
    char *token = strtok(cmdline, " ");

    while (token && argc < MAX_ARGS)
    {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    if (argc == 0)
    {
        return;
    }

    // 查找并执行命令
    for (const cmd_t *cmd = commands; cmd->name != NULL; cmd++)
    {
        if (strcmp(argv[0], cmd->name) == 0)
        {
            cmd->handler(argc, argv);
            return;
        }
    }

    printk("未知命令: %s\n", argv[0]);
    printk("输入 'help' 查看可用命令\n");
}

// 处理命令行输入
void process_handle_command(const char *cmdline)
{
    char cmd[MAX_CMD_LEN];
    strncpy(cmd, cmdline, MAX_CMD_LEN - 1);
    cmd[MAX_CMD_LEN - 1] = '\0';

    // 移除末尾的换行符
    char *newline = strchr(cmd, '\n');
    if (newline)
    {
        *newline = '\0';
    }

    parse_command(cmd);
}