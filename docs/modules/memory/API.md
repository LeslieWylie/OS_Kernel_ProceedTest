# 内存管理模块 API 文档

本文档详细描述了内存管理模块提供的所有API函数、结构体和常量。

## 数据类型

### 结构体定义

#### vm_area_struct_t

```c
struct vm_area_struct {
    uint32_t start;              // 起始地址
    uint32_t end;                // 结束地址
    uint32_t flags;              // 权限标志
    struct vm_area_struct *next; // 链表下一个节点
};
typedef struct vm_area_struct vm_area_struct_t;
```

内存区域描述符，表示进程地址空间中的一段连续内存区域。

**成员说明：**
- `start`: 区域起始虚拟地址
- `end`: 区域结束虚拟地址（不包含）
- `flags`: 权限和属性标志
- `next`: 指向下一个内存区域的指针，形成链表

#### mm_struct_t

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
typedef struct mm_struct mm_struct_t;
```

内存管理结构，管理一个进程的完整地址空间。

**成员说明：**
- `mmap`: 内存区域链表头指针
- `start_code`/`end_code`: 代码段地址范围
- `start_data`/`end_data`: 数据段地址范围
- `start_heap`/`end_heap`: 堆地址范围
- `start_stack`: 栈起始地址

### 常量定义

内存区域权限标志：

```c
#define VM_READ     0x00000001  // 可读
#define VM_WRITE    0x00000002  // 可写
#define VM_EXEC     0x00000004  // 可执行
#define VM_SHARED   0x00000008  // 共享内存
#define VM_GROWSUP  0x00000010  // 可向上增长（堆）
#define VM_GROWSDOWN 0x00000020 // 可向下增长（栈）
```

## 函数 API

### 内存分配函数

#### kmalloc

```c
void *kmalloc(size_t size);
```

从内核堆分配指定大小的内存块。

**参数：**
- `size`: 需要分配的内存大小（字节）

**返回值：**
- 成功返回分配的内存指针
- 失败返回NULL

**说明：**
当前实现使用简单的线性分配器，分配的内存默认4字节对齐。分配失败通常是由于内存不足导致。

#### kfree

```c
void kfree(void *ptr);
```

释放通过kmalloc分配的内存。

**参数：**
- `ptr`: 要释放的内存指针

**返回值：**
- 无

**说明：**
当前实现不实际释放内存，仅作为API占位。在真实系统中应实现完整的内存回收。

### 内核栈函数

#### allocate_kernel_stack

```c
void *allocate_kernel_stack(size_t size);
```

为进程分配内核栈。

**参数：**
- `size`: 栈大小（字节）

**返回值：**
- 成功返回分配的栈指针
- 失败返回NULL

**说明：**
分配的栈空间通常为4KB或8KB，具体取决于操作系统配置。栈增长方向为从高地址向低地址。

#### free_kernel_stack

```c
void free_kernel_stack(void *stack);
```

释放内核栈。

**参数：**
- `stack`: 栈指针

**返回值：**
- 无

### 内存管理结构函数

#### mm_create

```c
mm_struct_t *mm_create(void);
```

创建新的内存管理结构。

**参数：**
- 无

**返回值：**
- 成功返回新创建的内存管理结构指针
- 失败返回NULL

**说明：**
创建一个新的、初始化为零的内存管理结构。通常在创建新进程时调用。

#### mm_destroy

```c
void mm_destroy(mm_struct_t *mm);
```

销毁内存管理结构及其管理的所有资源。

**参数：**
- `mm`: 要销毁的内存管理结构

**返回值：**
- 无

**说明：**
释放`mm`所指向的内存管理结构及其管理的所有内存区域。通常在进程退出时调用。

## 使用示例

### 基本内存分配与释放

```c
// 分配12字节内存
void *buffer = kmalloc(12);
if (!buffer) {
    printf("Memory allocation failed\n");
    return ERROR;
}

// 使用内存
memset(buffer, 0, 12);

// 释放内存
kfree(buffer);
```

### 创建和管理进程地址空间

```c
// 创建新进程的内存管理结构
mm_struct_t *mm = mm_create();
if (!mm) {
    printf("Failed to create memory space\n");
    return ERROR;
}

// 为进程分配内核栈
void *kernel_stack = allocate_kernel_stack(4096);
if (!kernel_stack) {
    printf("Failed to allocate kernel stack\n");
    mm_destroy(mm);
    return ERROR;
}

// 进程结束时释放资源
free_kernel_stack(kernel_stack);
mm_destroy(mm);
```

## 调试提示

调试内存问题时，可以关注以下输出：

- "Memory allocated: X bytes at Y" - 显示内存分配情况
- "Memory allocation failed" - 表示内存分配失败
- "Memory free requested at X" - 显示内存释放请求

## 注意事项

1. 当前实现不支持实际的内存释放，可能导致长时间运行时内存耗尽
2. 所有分配的内存默认对齐到4字节边界
3. 没有边界检查机制，使用时需小心避免缓冲区溢出 