#include "memory.h"
#include <stddef.h>
#include <string.h>

// 简单的内存分配器实现（仅用于测试）
#define HEAP_SIZE (1024 * 1024) // 1MB堆空间
static char heap[HEAP_SIZE];
static size_t heap_used = 0;

void *kmalloc(size_t size)
{
    if (heap_used + size > HEAP_SIZE)
    {
        return NULL;
    }

    void *ptr = &heap[heap_used];
    heap_used += size;

    // 对齐到4字节边界
    heap_used = (heap_used + 3) & ~3;

    return ptr;
}

void kfree(void *ptr)
{
    // 简单实现，不实际释放内存
    (void)ptr;
}

// 创建内存管理结构
mm_struct_t *mm_create(void)
{
    mm_struct_t *mm = kmalloc(sizeof(mm_struct_t));
    if (!mm)
    {
        return NULL;
    }

    memset(mm, 0, sizeof(mm_struct_t));
    return mm;
}

// 销毁内存管理结构
void mm_destroy(mm_struct_t *mm)
{
    if (!mm)
    {
        return;
    }

    // 释放所有内存区域
    vm_area_struct_t *vma = mm->mmap;
    while (vma)
    {
        vm_area_struct_t *next = vma->next;
        kfree(vma);
        vma = next;
    }

    kfree(mm);
}

// 分配内核栈
void *allocate_kernel_stack(size_t size)
{
    return kmalloc(size);
}

// 释放内核栈
void free_kernel_stack(void *stack)
{
    kfree(stack);
}