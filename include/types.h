#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>
#include <stddef.h>

/* === 基本类型定义 === */
typedef int32_t pid_t;      // 进程ID类型
typedef uint32_t os_time_t; // 时间类型 (避免与系统time_t冲突)
typedef uint32_t dev_t;     // 设备ID
typedef int32_t error_t;    // 错误码
typedef uint32_t mode_t;    // 文件模式
typedef uint64_t ino_t;     // inode编号
typedef int64_t os_off_t;   // 文件偏移量 (避免与系统off_t冲突)

/* === 前向声明 === */
typedef struct process process_t;
typedef struct context context_t;
typedef struct vm_area_struct vm_area_struct_t;
typedef struct mm_struct mm_struct_t;

/* === 进程管理相关类型 === */
// 进程状态
typedef enum
{
    PROCESS_CREATED,   // 新创建
    PROCESS_READY,     // 就绪态
    PROCESS_RUNNING,   // 运行态
    PROCESS_BLOCKED,   // 阻塞态
    PROCESS_TERMINATED // 终止态
} process_state_t;

// 处理器寄存器结构体
typedef struct pt_regs
{
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t rip, rflags;
    uint16_t cs, ds, es, fs, gs, ss;
} pt_regs_t;

/* === 内存管理相关类型 === */
// 页表条目类型
typedef uint64_t pte_t;
// 物理地址
typedef uint64_t phys_addr_t;
// 虚拟地址
typedef uint64_t virt_addr_t;

/* === 文件系统相关类型 === */
// 文件描述符
typedef int32_t fd_t;
// 文件类型
typedef enum
{
    FILE_TYPE_REGULAR,   // 普通文件
    FILE_TYPE_DIRECTORY, // 目录
    FILE_TYPE_CHAR_DEV,  // 字符设备
    FILE_TYPE_BLOCK_DEV, // 块设备
    FILE_TYPE_FIFO,      // 管道
    FILE_TYPE_SOCKET,    // 套接字
    FILE_TYPE_SYMLINK    // 符号链接
} file_type_t;

/* === 系统调用相关类型 === */
typedef int32_t syscall_t;

/* === 错误码定义 === */
#define E_SUCCESS 0   // 成功
#define E_INVAL -1    // 无效参数
#define E_NOMEM -2    // 内存不足
#define E_IO -3       // I/O错误
#define E_NOTFOUND -4 // 未找到
#define E_EXIST -5    // 已存在
#define E_NOTSUP -6   // 不支持
#define E_PERM -7     // 权限不足
#define E_BUSY -8     // 资源忙

#endif /* _TYPES_H */
