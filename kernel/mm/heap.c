/**
 * heap.c - 内核堆内存管理模块
 */

#include <stdint.h>
#include <stddef.h>
#include "../../include/memory.h"

// 内存块头部结构
typedef struct block_header {
    uint32_t size;                   // 块大小(不包括头部)
    uint32_t magic;                  // 魔数，用于检测内存破坏
    uint8_t is_free;                 // 是否是空闲块
    struct block_header *next;       // 下一个块
    struct block_header *prev;       // 上一个块
} block_header_t;

// 内存块魔数
#define HEAP_MAGIC 0x12345678

// 堆管理器
static struct {
    block_header_t *first_block;     // 第一个内存块
    block_header_t *last_block;      // 最后一个内存块
    uint64_t heap_start;             // 堆起始地址
    uint64_t heap_end;               // 堆结束地址
    uint64_t heap_size;              // 堆大小
    uint64_t free_size;              // 空闲内存大小
} heap_manager;

/**
 * 初始化内核堆
 * 
 * @param start 堆起始地址
 * @param size 堆大小
 */
void heap_init(uint64_t start, uint64_t size)
{
    // 确保起始地址对齐
    if (start & 0xF) {
        uint64_t adjust = 16 - (start & 0xF);
        start += adjust;
        size -= adjust;
    }
    
    // 确保大小对齐
    size &= ~0xF;
    
    // 初始化堆管理器
    heap_manager.heap_start = start;
    heap_manager.heap_end = start + size;
    heap_manager.heap_size = size;
    heap_manager.free_size = size;
    
    // 创建初始空闲块
    block_header_t *first = (block_header_t *)start;
    first->size = size - sizeof(block_header_t);
    first->magic = HEAP_MAGIC;
    first->is_free = 1;
    first->next = NULL;
    first->prev = NULL;
    
    heap_manager.first_block = first;
    heap_manager.last_block = first;
}

/**
 * 在堆中分配内存
 * 
 * @param size 需要分配的字节数
 * @return 分配的内存指针，失败返回NULL
 */
void *kmalloc(size_t size)
{
    // 对齐到16字节
    size = (size + 15) & ~15;
    
    // 遍历空闲块链表，查找足够大的块
    block_header_t *current = heap_manager.first_block;
    
    while (current) {
        // 找到足够大的空闲块
        if (current->is_free && current->size >= size) {
            // 如果块明显过大，进行分割
            if (current->size > size + sizeof(block_header_t) + 16) {
                // 计算新块的地址
                uint64_t new_block_addr = (uint64_t)current + sizeof(block_header_t) + size;
                block_header_t *new_block = (block_header_t *)new_block_addr;
                
                // 初始化新块
                new_block->size = current->size - size - sizeof(block_header_t);
                new_block->magic = HEAP_MAGIC;
                new_block->is_free = 1;
                new_block->next = current->next;
                new_block->prev = current;
                
                // 更新当前块
                current->size = size;
                current->next = new_block;
                
                // 更新链表
                if (new_block->next) {
                    new_block->next->prev = new_block;
                } else {
                    heap_manager.last_block = new_block;
                }
            }
            
            // 标记为已分配
            current->is_free = 0;
            
            // 更新空闲内存大小
            heap_manager.free_size -= current->size + sizeof(block_header_t);
            
            // 返回数据区地址
            return (void *)((uint64_t)current + sizeof(block_header_t));
        }
        
        current = current->next;
    }
    
    // 没有找到合适的块
    return NULL;
}

/**
 * 释放先前分配的内存
 * 
 * @param ptr 先前由kmalloc返回的指针
 */
void kfree(void *ptr)
{
    if (!ptr) {
        return;
    }
    
    // 获取块头部
    block_header_t *block = (block_header_t *)((uint64_t)ptr - sizeof(block_header_t));
    
    // 校验魔数
    if (block->magic != HEAP_MAGIC) {
        // 内存可能已被破坏
        return;
    }
    
    // 标记为空闲
    block->is_free = 1;
    
    // 更新空闲内存大小
    heap_manager.free_size += block->size + sizeof(block_header_t);
    
    // 尝试合并相邻的空闲块
    
    // 与后一个块合并
    if (block->next && block->next->is_free) {
        block_header_t *next = block->next;
        
        // 合并大小
        block->size += next->size + sizeof(block_header_t);
        
        // 更新指针
        block->next = next->next;
        if (next->next) {
            next->next->prev = block;
        } else {
            heap_manager.last_block = block;
        }
    }
    
    // 与前一个块合并
    if (block->prev && block->prev->is_free) {
        block_header_t *prev = block->prev;
        
        // 合并大小
        prev->size += block->size + sizeof(block_header_t);
        
        // 更新指针
        prev->next = block->next;
        if (block->next) {
            block->next->prev = prev;
        } else {
            heap_manager.last_block = prev;
        }
    }
}

/**
 * 获取堆统计信息
 * 
 * @param total_size 返回堆总大小
 * @param free_size 返回空闲内存大小
 */
void heap_stats(uint64_t *total_size, uint64_t *free_size)
{
    if (total_size) {
        *total_size = heap_manager.heap_size;
    }
    
    if (free_size) {
        *free_size = heap_manager.free_size;
    }
} 