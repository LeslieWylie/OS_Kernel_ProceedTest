/**
 * cache.c - 文件系统缓存实现
 */

#include <stdint.h>
#include <stddef.h>
#include "../../include/types.h"

// 最大缓存块数量
#define MAX_CACHE_BLOCKS 256

// 缓存块状态标志
#define CACHE_CLEAN 0 // 缓存块与磁盘内容一致
#define CACHE_DIRTY 1 // 缓存块已修改，需要写回磁盘

// 缓存块结构
typedef struct cache_block
{
    uint64_t block_id;        // 块ID (设备ID+块号)
    uint8_t *data;            // 块数据
    uint32_t size;            // 块大小
    uint8_t status;           // 状态标志
    uint32_t reference_count; // 引用计数
    uint64_t last_access;     // 最后访问时间
    struct cache_block *next; // 链表下一个
    struct cache_block *prev; // 链表上一个
} cache_block_t;

// 缓存管理器
typedef struct
{
    cache_block_t *blocks[MAX_CACHE_BLOCKS]; // 缓存块数组
    uint32_t block_count;                    // 当前缓存块数量
    uint64_t hit_count;                      // 缓存命中计数
    uint64_t miss_count;                     // 缓存未命中计数
    uint64_t access_time;                    // 全局访问时间(用于LRU)
} cache_manager_t;

// 全局缓存管理器
static cache_manager_t cache_mgr;

/**
 * 初始化文件缓存系统
 */
void cache_init(void)
{
    // 初始化缓存管理器
    cache_mgr.block_count = 0;
    cache_mgr.hit_count = 0;
    cache_mgr.miss_count = 0;
    cache_mgr.access_time = 0;

    // 清空缓存块数组
    for (int i = 0; i < MAX_CACHE_BLOCKS; i++)
    {
        cache_mgr.blocks[i] = NULL;
    }
}

/**
 * 根据块ID查找缓存块
 *
 * @param block_id 块ID
 * @return 找到返回缓存块指针，否则返回NULL
 */
static cache_block_t *find_cache_block(uint64_t block_id)
{
    for (uint32_t i = 0; i < cache_mgr.block_count; i++)
    {
        if (cache_mgr.blocks[i] && cache_mgr.blocks[i]->block_id == block_id)
        {
            return cache_mgr.blocks[i];
        }
    }
    return NULL;
}

/**
 * 分配一个新的缓存块
 *
 * @param block_id 块ID
 * @param size 块大小
 * @return 分配的缓存块指针，失败返回NULL
 */
static cache_block_t *allocate_cache_block(uint64_t block_id, uint32_t size)
{
    cache_block_t *block;

    // 如果缓存已满，需要淘汰一个块
    if (cache_mgr.block_count >= MAX_CACHE_BLOCKS)
    {
        // 查找引用计数为0的最久未使用的块
        uint64_t oldest_time = 0xFFFFFFFFFFFFFFFF;
        int oldest_index = -1;

        for (uint32_t i = 0; i < cache_mgr.block_count; i++)
        {
            if (cache_mgr.blocks[i] &&
                cache_mgr.blocks[i]->reference_count == 0 &&
                cache_mgr.blocks[i]->last_access < oldest_time)
            {
                oldest_time = cache_mgr.blocks[i]->last_access;
                oldest_index = i;
            }
        }

        // 如果没有找到可淘汰的块，返回失败
        if (oldest_index == -1)
        {
            return NULL;
        }

        // 如果块是脏的，需要先写回磁盘
        if (cache_mgr.blocks[oldest_index]->status == CACHE_DIRTY)
        {
            // 写回磁盘操作(实际实现需要与设备层交互)
        }

        // 释放块内存
        block = cache_mgr.blocks[oldest_index];
        kfree(block->data);
        kfree(block);
        cache_mgr.blocks[oldest_index] = NULL;
    }

    // 分配新的缓存块
    block = kmalloc(sizeof(cache_block_t));
    if (!block)
    {
        return NULL;
    }

    // 分配数据缓冲区
    block->data = kmalloc(size);
    if (!block->data)
    {
        kfree(block);
        return NULL;
    }

    // 初始化缓存块
    block->block_id = block_id;
    block->size = size;
    block->status = CACHE_CLEAN;
    block->reference_count = 1;
    block->last_access = ++cache_mgr.access_time;
    block->next = NULL;
    block->prev = NULL;

    // 添加到缓存管理器
    for (uint32_t i = 0; i < MAX_CACHE_BLOCKS; i++)
    {
        if (cache_mgr.blocks[i] == NULL)
        {
            cache_mgr.blocks[i] = block;
            if (i >= cache_mgr.block_count)
            {
                cache_mgr.block_count = i + 1;
            }
            break;
        }
    }

    return block;
}

/**
 * 获取缓存块(如果不存在则从磁盘加载)
 *
 * @param block_id 块ID
 * @param size 块大小
 * @return 缓存块指针，失败返回NULL
 */
cache_block_t *cache_get_block(uint64_t block_id, uint32_t size)
{
    // 查找缓存
    cache_block_t *block = find_cache_block(block_id);

    if (block)
    {
        // 缓存命中
        cache_mgr.hit_count++;

        // 更新访问时间
        block->last_access = ++cache_mgr.access_time;

        // 增加引用计数
        block->reference_count++;

        return block;
    }

    // 缓存未命中
    cache_mgr.miss_count++;

    // 分配新的缓存块
    block = allocate_cache_block(block_id, size);
    if (!block)
    {
        return NULL;
    }

    // 从磁盘加载数据(实际实现需要与设备层交互)
    // ...

    return block;
}

/**
 * 释放缓存块(减少引用计数)
 *
 * @param block 缓存块指针
 */
void cache_release_block(cache_block_t *block)
{
    if (!block)
    {
        return;
    }

    // 减少引用计数
    if (block->reference_count > 0)
    {
        block->reference_count--;
    }
}

/**
 * 标记缓存块为脏(需要写回)
 *
 * @param block 缓存块指针
 */
void cache_mark_dirty(cache_block_t *block)
{
    if (block)
    {
        block->status = CACHE_DIRTY;
    }
}

/**
 * 将脏缓存块写回磁盘
 *
 * @param block 缓存块指针
 * @return 成功返回0，失败返回错误码
 */
int cache_sync_block(cache_block_t *block)
{
    if (!block || block->status != CACHE_DIRTY)
    {
        return 0;
    }

    // 写回磁盘(实际实现需要与设备层交互)
    // ...

    // 标记为干净
    block->status = CACHE_CLEAN;

    return 0;
}

/**
 * 同步所有脏缓存块到磁盘
 *
 * @return 成功返回0，失败返回错误码
 */
int cache_sync_all(void)
{
    for (uint32_t i = 0; i < cache_mgr.block_count; i++)
    {
        if (cache_mgr.blocks[i] && cache_mgr.blocks[i]->status == CACHE_DIRTY)
        {
            int ret = cache_sync_block(cache_mgr.blocks[i]);
            if (ret != 0)
            {
                return ret;
            }
        }
    }

    return 0;
}