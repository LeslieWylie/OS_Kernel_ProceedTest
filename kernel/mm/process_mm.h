#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <stddef.h>

// 内存区域结构
typedef struct vm_area_struct {
    unsigned long start;
    unsigned long end;
    unsigned long flags;
    struct vm_area_struct *next;
} vm_area_struct_t;

// 内存管理结构
typedef struct mm_struct {
    vm_area_struct_t *mmap;
    unsigned long start_code, end_code;
    unsigned long start_data, end_data;
    unsigned long start_brk, brk;
    unsigned long start_stack;
    int count;
} mm_struct_t;

// 内存管理函数
void *kmalloc(size_t size);
void kfree(void *ptr);
mm_struct_t *mm_create(void);
void mm_destroy(mm_struct_t *mm);
void *allocate_kernel_stack(size_t size);
void free_kernel_stack(void *stack);

#endif /* _KERNEL_MEMORY_H */ 