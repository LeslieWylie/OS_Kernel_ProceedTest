# 进程模块文档

## 模块概述

进程模块是操作系统内核的核心组件之一，负责进程的创建、调度、销毁等生命周期管理。该模块提供了必要的数据结构和函数，使操作系统能够支持多任务处理。

## 主要功能

- 进程创建与初始化
- 进程调度（支持FIFO和SJF调度算法）
- 进程上下文切换
- 进程状态管理
- 内核栈分配与管理

## 核心API

### 进程管理

- `process_init()`: 初始化进程子系统
- `process_create()`: 创建新进程
- `process_exit()`: 退出当前进程
- `process_wait()`: 等待子进程结束
- `process_set_state()`: 设置进程状态

### 进程调度

- `schedule()`: 触发进程调度
- `schedule_sjf()`: 最短作业优先调度
- `do_timer()`: 时间片处理函数

### 上下文切换

- `context_init()`: 初始化上下文结构
- `context_switch()`: 进程上下文切换
- `switch_to()`: 底层切换函数

## 数据结构

### 进程控制块 (PCB)

进程控制块通过`process_t`结构体实现，包含进程的所有关键信息：

```c
struct process {
    pid_t pid;                 // 进程ID
    process_state_t state;     // 进程状态
    uint32_t priority;         // 优先级
    uint32_t counter;          // 时间片计数
    void *stack;               // 进程栈
    context_t *context;        // CPU上下文
    mm_struct_t *mm;           // 内存管理结构
    // ...其他字段
};
```

### 进程状态

进程可以处于以下几种状态：

- `PROCESS_CREATED`: 新创建
- `PROCESS_READY`: 就绪态
- `PROCESS_RUNNING`: 运行态
- `PROCESS_BLOCKED`: 阻塞态
- `PROCESS_TERMINATED`: 终止态

## 源代码文件

- `kernel/process/process.c`: 进程管理实现
- `kernel/process/scheduler.c`: 调度器实现
- `kernel/process/context.c`: 上下文管理实现
- `kernel/process/memory.c`: 进程内存管理
- `kernel/process/switch.S`: 底层上下文切换汇编实现

## 使用示例

```c
// 创建一个新进程
pid_t pid = process_create(task_entry, arg, priority);
if(pid < 0) {
    // 创建失败处理
}

// 等待进程结束
int exit_code;
process_wait(pid, &exit_code);
```

## 调试指南

- 使用`printf()`打印进程状态变化和调度信息
- 检查进程控制块字段确保正确初始化
- 常见问题包括内存分配失败和上下文切换错误

## 未来扩展

- 实现更多高级调度算法
- 添加进程优先级动态调整
- 支持进程间通信机制 