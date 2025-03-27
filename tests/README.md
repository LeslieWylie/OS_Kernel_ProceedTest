# 操作系统内核测试框架

## 概述

本测试框架为操作系统内核提供了完整的单元测试和集成测试解决方案。所有测试代码都位于 `tests/` 目录中，并按照模块进行组织。测试框架支持独立运行单元测试和集成测试，以及整体测试。

## 目录结构

```
tests/
│
├── Makefile                # 主Makefile，用于构建和运行所有测试
│
├── template/               # 测试模板目录
│   ├── Makefile           # 模块测试Makefile模板
│   └── test_framework.h   # 测试框架头文件
│
├── process/               # 进程模块测试
│   ├── Makefile          # 进程测试Makefile
│   ├── test_process_unit.c      # 进程单元测试
│   ├── test_scheduler_unit.c    # 调度器单元测试
│   └── test_process_integration.c # 进程集成测试
│
├── memory/                # 内存模块测试
├── kernel/                # 内核模块测试
├── fs/                    # 文件系统测试
├── drivers/               # 设备驱动测试
└── integration/           # 整体集成测试
```

## 测试类型

测试框架支持两种主要的测试类型：

### 单元测试

单元测试针对系统的各个独立组件进行测试，通常只关注一个特定的函数或一组紧密相关的函数。单元测试使用 `test_*_unit.c` 命名，并包含在相应模块的测试目录中。

单元测试的特点：

- 独立性：不依赖于系统其他部分
- 快速：执行时间短
- 精确：针对具体功能点

### 集成测试

集成测试验证系统不同组件之间的交互，确保它们作为一个整体正常工作。集成测试使用 `test_*_integration.c` 命名。

集成测试的特点：

- 验证模块间交互
- 测试真实场景
- 检测接口兼容性问题

## 运行测试

### 运行所有测试

```bash
cd tests
make test
```

### 运行特定模块的测试

```bash
cd tests
make process  # 运行进程模块测试
make memory   # 运行内存模块测试
```

### 只运行单元测试或集成测试

```bash
cd tests/process
make unit_test          # 只运行单元测试
make integration_test   # 只运行集成测试
```

## 开发新的测试

1. 在对应模块的目录中创建测试文件：

   - 单元测试：`test_功能_unit.c`
   - 集成测试：`test_功能_integration.c`

2. 更新对应模块的 `Makefile`，添加新的测试文件到 `UNIT_TESTS` 或 `INTEGRATION_TESTS` 变量。

3. 使用测试框架提供的断言宏编写测试：
   - `ASSERT(condition)`：检查条件是否为真
   - `ASSERT_EQ(actual, expected)`：检查两个值是否相等
   - `ASSERT_STR_EQ(actual, expected)`：检查两个字符串是否相等
   - `ASSERT_NULL(value)`：检查值是否为 NULL
   - `ASSERT_NOT_NULL(value)`：检查值是否不为 NULL

## 测试框架结构

测试框架使用 C 语言实现，提供了一套简单的断言宏和测试运行器。框架的核心组件是 `test_framework.h`，它定义了以下主要功能：

- 测试断言宏（ASSERT\_\*）
- 测试套件控制（TEST_SUITE_START/END）
- 测试用例管理（TEST_CASE, RUN_TEST）
- 测试结果报告

## 最佳实践

1. **测试独立性**：每个测试应该能够独立运行，不依赖于其他测试的状态
2. **使用 setup/teardown**：为测试准备环境和清理资源
3. **分层测试**：先进行单元测试，再进行集成测试
4. **测试边界条件**：包括无效输入、极限值和异常情况
5. **保持测试简洁**：每个测试函数专注于一个功能点
