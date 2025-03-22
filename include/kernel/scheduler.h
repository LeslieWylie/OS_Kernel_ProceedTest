#ifndef _KERNEL_SCHEDULER_H
#define _KERNEL_SCHEDULER_H

#include "process.h"

// 调度器初始化
void scheduler_init(void);

// 添加进程到调度队列
void scheduler_add(process_t *proc);

// 从调度队列中移除进程
void scheduler_remove(process_t *proc);

// 进行进程调度
void schedule(void);

// 获取下一个要运行的进程
process_t *scheduler_next(void);

// 时间片到期处理
void scheduler_tick(void);

// 阻塞当前进程
void scheduler_block(void);

// 唤醒进程
void scheduler_wake(process_t *proc);

#endif // _KERNEL_SCHEDULER_H