# OS Kernel 简介

这是一个简单的操作系统内核实现，包含基本的进程管理和内存管理功能。本项目旨在提供对操作系统内核的基本理解和实践经验。

## 功能特性

- 基本进程管理
  - 进程创建与终止
  - 进程状态管理
  - 先进先出(FIFO)和最短作业优先(SJF)调度算法
- 内存管理
  - 基本内存分配与释放
  - 内核堆管理

## 构建和测试

### 环境需求

- GCC 编译器
- Make 工具
- Windows 或 Linux 系统

### 构建内核

在项目根目录下执行：

```bash
make
```

### 测试进程管理功能

本项目提供了一个命令行界面程序，用于测试进程管理功能。

在 Windows 系统上：

```bash
cd tests/process
run_tests.bat
```

在 Linux 系统上：

```bash
cd tests/process
chmod +x run_tests.sh
./run_tests.sh
```

### 进程管理测试命令

命令行测试程序支持以下命令：

- `help` - 显示帮助信息
- `init` - 初始化进程系统
- `create <名称> <优先级>` - 创建一个新进程（短任务）
- `create-long <名称> <优先级>` - 创建长时间运行的进程
- `create-infinite <名称> <优先级>` - 创建无限循环的进程
- `list` - 列出所有进程
- `schedule` - 运行 FIFO 调度器
- `schedule-sjf` - 运行最短作业优先调度器
- `kill <PID>` - 终止指定 PID 的进程
- `block <PID>` - 阻塞指定 PID 的进程
- `wake <PID>` - 唤醒被阻塞的进程
- `exit` - 退出程序

## 项目结构

```
.
├── include/kernel/        # 头文件
│   ├── context.h          # 上下文切换相关定义
│   ├── memory.h           # 内存管理相关定义
│   ├── process.h          # 进程管理的主要头文件
│   └── scheduler.h        # 调度器相关定义
├── kernel/process/        # 核心代码实现
│   ├── context.c          # 上下文切换实现
│   ├── memory.c           # 内存管理实现
│   ├── process.c          # 进程管理主要实现
│   ├── scheduler.c        # 调度器实现
│   └── switch.S           # 汇编实现的上下文切换
└── tests/process/         # 测试程序
    ├── test_context_switch.c # 上下文切换测试
    ├── test_fifo.c        # FIFO调度器测试
    └── test_process.c     # 进程管理测试
```

## 进程创建和管理

### 创建进程

使用`process_create`函数创建新进程：

```c
pid_t process_create(void (*entry)(void *), void *arg, uint32_t priority);
```

参数说明：

- `entry`: 进程入口函数
- `arg`: 传递给入口函数的参数
- `priority`: 进程优先级（0-4，0 最高）

返回值：

- 成功：返回大于 0 的进程 ID
- 失败：返回小于等于 0 的错误码

### 进程调度

系统支持两种调度算法：

1. FIFO (先进先出) - 通过`schedule()`函数调用
2. SJF (最短作业优先) - 通过`schedule_sjf()`函数调用

### 进程状态

进程可以处于以下状态：

- PROCESS_CREATED - 已创建
- PROCESS_READY - 就绪态
- PROCESS_RUNNING - 运行态
- PROCESS_BLOCKED - 阻塞态
- PROCESS_TERMINATED - 终止态

## 注意事项

- 本系统仅供学习和研究使用，不适合生产环境
- 当前版本功能仍在开发中，可能存在稳定性问题
- 未实现完整的进程间通信和同步机制

## 编译步骤

1. 确保系统已安装 GCC 编译器
2. 运行以下命令编译项目：

```bash
make clean    # 清理之前的构建
make          # 编译所有代码
```

编译后的可执行文件将位于 `bin/` 目录下。

## 运行测试

编译完成后，可以运行以下命令来执行测试：

```bash
# 运行所有测试
make test

# 或单独运行某个测试
./bin/test_process
./bin/test_fifo
./bin/test_context_switch
```

## 主要组件

### 进程管理 (process.c)

进程管理模块提供进程的创建、终止和状态转换功能：

- `process_init()`: 初始化进程系统
- `process_create()`: 创建新进程
- `process_exit()`: 终止当前进程
- `process_wait()`: 等待指定进程结束

### 调度器 (scheduler.c)

调度器模块实现了进程的调度策略：

- FIFO 调度: 先进先出策略，先创建的进程先执行
- SJF 调度: 优先执行所需时间最短的进程

主要函数:

- `schedule()`: FIFO 调度实现
- `schedule_sjf()`: 最短作业优先调度实现
- `scheduler_block()`: 阻塞当前进程
- `scheduler_wake()`: 唤醒被阻塞的进程

### 上下文切换 (context.c, switch.S)

上下文切换模块实现了进程间的 CPU 上下文切换：

- `context_init()`: 初始化上下文
- `context_switch()`: C 语言实现的上下文切换
- `__switch_to()`: 汇编实现的上下文切换

### 内存管理 (memory.c)

提供简化的内存管理功能：

- `kmalloc()`: 分配内核内存
- `kfree()`: 释放内核内存
- `allocate_kernel_stack()`: 分配内核栈
- `free_kernel_stack()`: 释放内核栈

## 开发计划

- 添加更多的调度算法 (如轮转调度、多级反馈队列等)
- 实现进程间通信机制
- 实现完整的内存管理系统
- 添加用户态进程支持

## 环境安装

首先我们需要输入安装包：

```bash
sudo apt update
sudo apt install build-essential
sudo apt install nasm
sudo apt install grub-common
sudo apt install xorriso
sudo apt install mtools
sudo apt install qemu-system-x86
sudo rm -rf /snap/core20/
sudo apt install --reinstall libc6

```

大家可以去 fork 仓库然后新建文件夹，写下自己的那一部分。

直接构建并且运行 make 文件，也就是：

```bash
make clean && make && make run
```

come on.

## 文件结构（并不可以及时更新）：

# 最重要！！！内核

我们的 kernel/目录下，存放着有关整个系统的运行模块。比如我们的 kernel.c，就是我们的主模块。大家写完某个模块之后需要耦合，就需要和 kernel/文件夹下的某些模块耦合，比如 shell。

所以大家要做的，就是去写自己的那一块~
比如我们设备管理，就是上面的 drivers/的文件夹，下面存放我们的各种设备，比如内核等等。

## 中断处理

我们在 arch/x86 的目录下，设置了如此的文件结构：

## 文件管理

## 内存管理
