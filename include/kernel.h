// 核心头文件 - 系统核心功能和接口定义
#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"

// 内核入口点
void kernel_main(void);

// 内核状态和控制
void kernel_panic(const char *message);
void kernel_halt(void);
void kernel_reboot(void);

// 系统信息
const char *kernel_version(void);
uint64_t kernel_uptime(void);
void kernel_print_info(void);

// 子系统初始化函数
void memory_init(void);
void process_init(void);
void scheduler_init(void);
void interrupt_init(void);
void filesystem_init(void);
void device_init(void);

// 调试支持
#ifdef DEBUG
void kernel_debug_log(const char *format, ...);
void kernel_dump_state(void);
#endif

#endif /* KERNEL_H */
