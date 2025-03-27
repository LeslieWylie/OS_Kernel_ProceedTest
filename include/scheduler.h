#ifndef _KERNEL_SCHEDULER_H
#define _KERNEL_SCHEDULER_H

/**
 * @file scheduler.h
 * @brief 进程调度器相关接口
 *
 * 提供进程调度、队列管理等功能
 */

#include <stdint.h>
#include "types.h"
#include "process.h"

/**
 * @brief 初始化调度器
 */
void scheduler_init(void);

/**
 * @brief 添加进程到调度队列
 *
 * @param proc 要添加的进程
 */
void scheduler_add(process_t *proc);

/**
 * @brief 从调度队列中移除进程
 *
 * @param proc 要移除的进程
 */
void scheduler_remove(process_t *proc);

/**
 * @brief 进行进程调度
 *
 * 选择下一个要运行的进程并切换
 */
void schedule(void);

/**
 * @brief 获取下一个要运行的进程
 *
 * @return process_t* 下一个要运行的进程
 */
process_t *scheduler_next(void);

/**
 * @brief 时间片到期处理
 *
 * 更新进程计数器并在必要时触发调度
 */
void scheduler_tick(void);

/**
 * @brief 阻塞当前进程
 *
 * 将当前进程状态设为阻塞并触发调度
 */
void scheduler_block(void);

/**
 * @brief 唤醒进程
 *
 * @param proc 要唤醒的进程
 */
void scheduler_wake(process_t *proc);

#endif // _KERNEL_SCHEDULER_H