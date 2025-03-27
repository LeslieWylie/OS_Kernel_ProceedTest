# 内存管理模块文档

## 模块概述

内存管理模块负责操作系统的内存分配、释放和管理。该模块提供了内核内存分配器和内存管理数据结构，支持进程的内存空间管理和内核栈分配。

## 主要功能

- 内核堆内存分配与释放
- 进程内存空间管理
- 内核栈分配与管理
- 内存区域描述与管理

## 核心API

### 内存分配

- `kmalloc()`: 分配内核内存
- `kfree()`: 释放内核内存
- `allocate_kernel_stack()`: 分配内核栈
- `free_kernel_stack()`: 释放内核栈

### 内存管理结构

- `mm_create()`: 创建内存管理结构
- `mm_destroy()`: 销毁内存管理结构

## 数据结构

### 内存管理结构

内存管理结构通过`mm_struct_t`实现，管理进程的地址空间：

```c
struct mm_struct {
    vm_area_struct_t *mmap;  // 内存映射链表
    uint32_t start_code;     // 代码段起始
    uint32_t end_code;       // 代码段结束
    uint32_t start_data;     // 数据段起始
    uint32_t end_data;       // 数据段结束
    uint32_t start_heap;     // 堆起始
    uint32_t end_heap;       // 堆结束
    uint32_t start_stack;    // 栈起始
};
```

### 虚拟内存区域

虚拟内存区域通过`vm_area_struct_t`描述，表示进程地址空间中的连续区域：

```c
struct vm_area_struct {
    uint32_t start;              // 起始地址
    uint32_t end;                // 结束地址
    uint32_t flags;              // 权限标志
    struct vm_area_struct *next; // 链表下一个节点
};
```

## 源代码文件

- `kernel/mm/process_mm.c`: 进程内存管理实现
- `kernel/process/memory.c`: 基本内存分配器实现
- `include/memory.h`: 内存管理接口定义

## 当前实现限制

当前实现使用简化的静态堆分配器，具有以下限制：

1. 固定大小的堆空间（1MB）
2. 不支持内存回收（`kfree()`不实际释放内存）
3. 仅支持4字节对齐的内存分配
4. 不支持虚拟内存和页表管理

## 使用示例

```c
// 分配内存
void *ptr = kmalloc(size);
if(!ptr) {
    // 处理内存分配失败
}

// 使用内存...

// 释放内存
kfree(ptr);

// 创建内存管理结构
mm_struct_t *mm = mm_create();
if(!mm) {
    // 处理创建失败
}

// 使用内存管理结构...

// 销毁内存管理结构
mm_destroy(mm);
```

## 调试指南

- 内存分配和释放操作会打印详细的调试信息
- 常见问题包括内存溢出、未初始化的内存结构和内存泄漏

## 未来扩展

- 实现真正的内存释放机制
- 添加页表管理和虚拟内存支持
- 实现更高效的内存分配算法（如伙伴系统、slab分配器）
- 支持内存映射和共享内存 