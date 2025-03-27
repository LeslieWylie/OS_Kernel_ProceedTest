#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

/**
 * @file memory.h
 * @brief 内存管理相关接口
 *
 * 提供内存分配、释放和管理相关的结构和函数
 */

#include <stddef.h>
#include <stdint.h>
#include "types.h"

/**
 * @brief 内存区域描述符
 */
struct vm_area_struct
{
    uint32_t start;              // 起始地址
    uint32_t end;                // 结束地址
    uint32_t flags;              // 权限标志
    struct vm_area_struct *next; // 链表下一个节点
};

/* 定义类型别名 */
typedef struct vm_area_struct vm_area_struct_t;

/**
 * @brief 内存管理结构
 */
struct mm_struct
{
    vm_area_struct_t *mmap; // 内存映射链表
    uint32_t start_code;    // 代码段起始
    uint32_t end_code;      // 代码段结束
    uint32_t start_data;    // 数据段起始
    uint32_t end_data;      // 数据段结束
    uint32_t start_heap;    // 堆起始
    uint32_t end_heap;      // 堆结束
    uint32_t start_stack;   // 栈起始
};

/* 定义类型别名 */
typedef struct mm_struct mm_struct_t;

/**
 * @brief 分配内存
 *
 * @param size 需要分配的字节数
 * @return void* 分配的内存指针，失败返回NULL
 */
void *kmalloc(size_t size);

/**
 * @brief 释放内存
 *
 * @param ptr 要释放的内存指针
 */
void kfree(void *ptr);

/**
 * @brief 创建内存管理结构
 *
 * @return mm_struct_t* 创建的内存管理结构指针
 */
mm_struct_t *mm_create(void);

/**
 * @brief 销毁内存管理结构
 *
 * @param mm 要销毁的内存管理结构
 */
void mm_destroy(mm_struct_t *mm);

/**
 * @brief 分配内核栈
 *
 * @param size 栈大小
 * @return void* 栈指针
 */
void *allocate_kernel_stack(size_t size);

/**
 * @brief 释放内核栈
 *
 * @param stack 栈指针
 */
void free_kernel_stack(void *stack);

#endif // _KERNEL_MEMORY_H