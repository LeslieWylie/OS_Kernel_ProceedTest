#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <stdint.h>
#include <stddef.h>

// 内存区域描述符
typedef struct vm_area_struct
{
    uint32_t start;              // 起始地址
    uint32_t end;                // 结束地址
    uint32_t flags;              // 权限标志
    struct vm_area_struct *next; // 链表下一个节点
} vm_area_struct_t;

// 内存管理结构
typedef struct mm_struct
{
    vm_area_struct_t *mmap; // 内存映射链表
    uint32_t start_code;    // 代码段起始
    uint32_t end_code;      // 代码段结束
    uint32_t start_data;    // 数据段起始
    uint32_t end_data;      // 数据段结束
    uint32_t start_heap;    // 堆起始
    uint32_t end_heap;      // 堆结束
    uint32_t start_stack;   // 栈起始
} mm_struct_t;

// 内存管理函数
void *kmalloc(size_t size);
void kfree(void *ptr);
mm_struct_t *mm_create(void);
void mm_destroy(mm_struct_t *mm);
void *allocate_kernel_stack(size_t size);
void free_kernel_stack(void *stack);

#endif // _KERNEL_MEMORY_H