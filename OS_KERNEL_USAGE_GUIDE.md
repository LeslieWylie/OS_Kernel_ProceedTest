# OS 内核进程管理模块使用教程

这份教程将指导您如何使用 OS 内核的进程管理模块。本模块提供了进程创建、调度和上下文切换等基本功能。

## 前置要求

确保您的系统满足以下要求：

- GCC 编译器
- Make 工具
- 64 位 Linux/Windows 系统(支持 x86_64 架构)

## 编译步骤

1. 克隆代码仓库到本地：

   ```bash
   git clone <仓库地址>
   cd os_kernel-main
   ```

2. 编译项目：

   ```bash
   make clean   # 清理之前的构建
   make         # 编译所有代码
   ```

3. 查看生成的二进制文件：
   ```bash
   ls bin/
   # 应该看到: test_process test_fifo test_context_switch
   ```

## 运行测试

我们提供了三个测试程序来验证进程管理模块的功能：

### 运行所有测试

```bash
make test
```

### 单独运行测试

```bash
# 测试进程创建和状态管理
./bin/test_process

# 测试FIFO调度器
./bin/test_fifo

# 测试上下文切换
./bin/test_context_switch
```

## 在自己的项目中使用

要在您自己的项目中使用这个进程管理模块，请按照以下步骤操作：

1. 复制以下目录到您的项目中：

   - `include/kernel/` - 所有头文件
   - `kernel/process/` - 所有实现文件

2. 在您的构建系统中设置正确的包含路径：

   ```
   -I<您的项目路径>/include/kernel
   ```

3. 编译所有源文件：

   - `process.c`
   - `scheduler.c`
   - `context.c`
   - `memory.c`
   - `switch.S`

4. 在您的代码中引入所需的头文件：
   ```c
   #include "process.h"    // 进程管理
   #include "scheduler.h"  // 调度器
   #include "context.h"    // 上下文切换
   #include "memory.h"     // 内存管理
   ```

## 进程管理 API 使用示例

### 初始化进程系统

```c
#include "process.h"

int main() {
    // 初始化进程管理系统
    process_init();

    // 其他代码...
    return 0;
}
```

### 创建新进程

```c
#include "process.h"
#include <stdio.h>

// 进程任务函数
void my_process(void *arg) {
    int id = *(int*)arg;
    printf("进程 %d 正在运行\n", id);

    // 进程的工作...

    // 进程结束
    process_exit(0);
}

int main() {
    process_init();

    // 创建进程参数
    int process_id = 1;

    // 创建新进程，优先级为2
    pid_t pid = process_create(my_process, &process_id, 2);
    if (pid > 0) {
        printf("创建了新进程，PID: %d\n", pid);
    } else {
        printf("进程创建失败\n");
    }

    // 运行调度器
    schedule();

    return 0;
}
```

### 等待进程结束

```c
#include "process.h"
#include <stdio.h>

int main() {
    process_init();

    // 创建进程...
    pid_t pid = process_create(/* ... */);

    // 等待进程结束
    int exit_code;
    if (process_wait(pid, &exit_code) == 0) {
        printf("进程 %d 已结束，退出码: %d\n", pid, exit_code);
    } else {
        printf("等待进程失败\n");
    }

    return 0;
}
```

### 使用 FIFO 调度器

```c
#include "process.h"
#include "scheduler.h"
#include <stdio.h>

int main() {
    process_init();

    // 创建多个进程...

    // 运行FIFO调度器
    schedule();

    return 0;
}
```

### 阻塞和唤醒进程

```c
#include "process.h"
#include "scheduler.h"
#include <stdio.h>

int main() {
    process_init();

    // 创建进程...
    pid_t pid = process_create(/* ... */);

    // 获取进程控制块
    process_t *proc = NULL;
    for (int i = 0; i < task_count; i++) {
        if (task[i] && task[i]->pid == pid) {
            proc = task[i];
            break;
        }
    }

    if (proc) {
        // 阻塞进程
        proc->state = PROCESS_BLOCKED;
        schedule(); // 调度到其他进程

        // ...

        // 唤醒进程
        scheduler_wake(proc);
        schedule(); // 重新调度
    }

    return 0;
}
```

## 调试提示

1. 使用 `printf` 打印进程状态和转换：

   ```c
   printf("进程 %d 状态: %s\n", proc->pid, process_state_to_string(proc->state));
   ```

2. 检查进程计数器和优先级：

   ```c
   printf("进程 %d: 优先级=%d, 计数器=%d\n",
          proc->pid, proc->priority, proc->counter);
   ```

3. 跟踪调度决策：
   ```c
   process_t *next = scheduler_next();
   printf("下一个运行的进程: PID=%d\n", next ? next->pid : -1);
   ```

## 已知问题和限制

1. 仅支持内核态进程，不支持用户态进程
2. 不支持进程间通信
3. 内存管理使用简化实现，不支持内存释放和回收
4. 最多支持 64 个并发进程
5. 没有实现信号机制

## 扩展和定制

您可以通过以下方式扩展和定制进程管理模块：

1. 添加新的调度算法：

   - 在 `scheduler.c` 中实现新的调度函数
   - 在 `scheduler.h` 中声明该函数
   - 在 `process.h` 中添加对该函数的引用

2. 增加进程优先级级别：

   - 修改 `PRIORITY_INIT_VALUES` 和 `COUNTER_INIT_VALUES` 数组
   - 更新优先级相关的调度逻辑

3. 实现进程间通信机制：

   - 在 `process.h` 中定义 IPC 相关的数据结构
   - 在 `process.c` 中实现 IPC 功能

4. 添加更多的进程状态：
   - 在 `process_state_t` 枚举中添加新状态
   - 更新 `process_state_to_string` 函数
