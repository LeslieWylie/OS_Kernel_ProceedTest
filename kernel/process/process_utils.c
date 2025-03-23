#include "process.h"
#include <stdio.h>

/**
 * 将进程状态转换为可读字符串
 * 
 * @param state 进程状态枚举值
 * @return 对应的状态字符串
 */
const char *process_state_to_string(process_state_t state)
{
    switch (state)
    {
    case PROCESS_CREATED:
        return "已创建";
    case PROCESS_READY:
        return "就绪";
    case PROCESS_RUNNING:
        return "运行中";
    case PROCESS_BLOCKED:
        return "阻塞";
    case PROCESS_TERMINATED:
        return "已终止";
    default:
        return "未知状态";
    }
}

/**
 * 检查进程是否存活（非终止状态）
 * 
 * @param proc 进程控制块指针
 * @return 1表示存活，0表示不存活
 */
int process_is_alive(process_t *proc)
{
    if (proc == NULL)
        return 0;
    
    return (proc->state != PROCESS_TERMINATED);
}

/**
 * 设置进程状态
 * 
 * @param proc 进程控制块指针
 * @param new_state 新的进程状态
 */
void process_set_state(process_t *proc, process_state_t new_state)
{
    if (proc != NULL)
    {
        proc->state = new_state;
    }
}

/**
 * 获取当前正在运行的进程
 * 
 * @return 当前进程的控制块指针
 */
process_t *process_get_current(void)
{
    return current;
}

/**
 * 设置当前运行的进程
 * 
 * @param proc 要设置为当前运行的进程
 */
void process_set_current(process_t *proc)
{
    current = proc;
} 