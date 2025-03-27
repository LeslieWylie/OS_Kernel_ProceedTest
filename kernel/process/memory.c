#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "memory.h"

/**
 * @brief 简单的堆内存分配器
 *
 * 仅用于核心内存分配，测试用途
 * 真实操作系统应该使用更高效的内存分配器和页表管理
 */

// 堆区域定义
#define HEAP_SIZE (1024 * 1024) // 1MB堆空间
static char heap[HEAP_SIZE];    // 静态分配的堆内存
static size_t heap_used = 0;    // 已使用的堆字节数

/**
 * @brief 分配内核内存
 *
 * 从静态堆中分配指定大小的内存块
 *
 * @param size 需要分配的字节数
 * @return void* 分配的内存指针，失败返回NULL
 */
void *kmalloc(size_t size)
{
    // 检查是否有足够的空间
    if (heap_used + size > HEAP_SIZE)
    {
        printf("Memory allocation failed: requested %zu bytes, only %zu available\n",
               size, HEAP_SIZE - heap_used);
        return NULL;
    }

    // 分配内存块
    void *ptr = &heap[heap_used];
    heap_used += size;

    // 对齐到4字节边界，提高访问效率
    heap_used = (heap_used + 3) & ~3;

    printf("Memory allocated: %zu bytes at %p (total used: %zu/%d)\n",
           size, ptr, heap_used, HEAP_SIZE);
    return ptr;
}

/**
 * @brief 释放内核内存
 *
 * 注意：当前实现不实际释放内存，仅为API兼容
 * 真实操作系统应该实现完整的内存管理和回收
 *
 * @param ptr 要释放的内存指针
 */
void kfree(void *ptr)
{
    // 简单实现，不实际释放内存
    // 真实操作系统应该实现完整的内存回收
    printf("Memory free requested at %p (not implemented)\n", ptr);
    (void)ptr;
}

/**
 * @brief 创建内存管理结构
 *
 * 为进程分配并初始化内存管理结构
 *
 * @return mm_struct_t* 创建的内存管理结构指针，失败返回NULL
 */
mm_struct_t *mm_create(void)
{
    mm_struct_t *mm = kmalloc(sizeof(mm_struct_t));
    if (!mm)
    {
        printf("Failed to create memory management structure\n");
        return NULL;
    }

    // 初始化内存管理结构
    memset(mm, 0, sizeof(mm_struct_t));
    printf("Memory management structure created at %p\n", mm);
    return mm;
}

/**
 * @brief 销毁内存管理结构
 *
 * 释放与内存管理结构关联的所有资源
 *
 * @param mm 要销毁的内存管理结构
 */
void mm_destroy(mm_struct_t *mm)
{
    if (!mm)
    {
        return;
    }

    printf("Destroying memory management structure at %p\n", mm);

    // 释放所有内存区域
    vm_area_struct_t *vma = mm->mmap;
    while (vma)
    {
        vm_area_struct_t *next = vma->next;
        printf("Freeing memory area: %p [0x%08x-0x%08x]\n",
               vma, vma->start, vma->end);
        kfree(vma);
        vma = next;
    }

    kfree(mm);
}

/**
 * @brief 分配内核栈
 *
 * 为进程分配内核栈内存
 *
 * @param size 栈大小（字节）
 * @return void* 栈指针，失败返回NULL
 */
void *allocate_kernel_stack(size_t size)
{
    void *stack = kmalloc(size);
    if (stack)
    {
        printf("Kernel stack allocated: %zu bytes at %p\n", size, stack);
    }
    return stack;
}

/**
 * @brief 释放内核栈
 *
 * 释放之前分配的内核栈内存
 *
 * @param stack 栈指针
 */
void free_kernel_stack(void *stack)
{
    printf("Freeing kernel stack at %p\n", stack);
    kfree(stack);
}