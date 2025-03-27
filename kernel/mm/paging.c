/**
 * paging.c - 分页机制管理模块
 */

#include <stdint.h>
#include <stddef.h>
#include "../../include/memory.h"
#include "physical.h"
#include "virtual.h"

// 页表标志
#define PAGE_PRESENT   (1UL << 0)    // 页面存在
#define PAGE_WRITE     (1UL << 1)    // 可写
#define PAGE_USER      (1UL << 2)    // 用户级
#define PAGE_PWT       (1UL << 3)    // 写透
#define PAGE_PCD       (1UL << 4)    // 禁用缓存
#define PAGE_ACCESSED  (1UL << 5)    // 已访问
#define PAGE_DIRTY     (1UL << 6)    // 已修改
#define PAGE_GLOBAL    (1UL << 8)    // 全局页

// 页大小
#define PAGE_SIZE      4096
#define PAGE_SHIFT     12
#define PAGE_MASK      (~(PAGE_SIZE-1))

// 获取CR3寄存器(包含页表根目录物理地址)
static inline uint64_t get_cr3(void)
{
    uint64_t cr3;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

// 设置CR3寄存器
static inline void set_cr3(uint64_t cr3)
{
    __asm__ __volatile__("mov %0, %%cr3" :: "r"(cr3) : "memory");
}

// 刷新TLB(快表)
static inline void flush_tlb(void)
{
    uint64_t cr3 = get_cr3();
    set_cr3(cr3);
}

// 刷新单个虚拟地址的TLB
static inline void flush_tlb_single(uint64_t addr)
{
    __asm__ __volatile__("invlpg (%0)" :: "r"(addr) : "memory");
}

/**
 * 初始化分页机制
 */
void paging_init(void)
{
    // 已经由bootloader设置了初始页表
    // 这里可以进行页表重新设置或补充
    
    // 初始化虚拟内存管理
    vmm_init();
}

/**
 * 创建一个新的页表结构
 * 
 * @return 新页表的CR3值，失败返回0
 */
uint64_t paging_create_address_space(void)
{
    // 分配一个新的PGD
    uint64_t pgd_phys = phys_alloc_page();
    if (pgd_phys == 0) {
        return 0;
    }
    
    // 清零PGD
    pgd_t *pgd = (pgd_t *)phys_to_virt(pgd_phys);
    for (int i = 0; i < 512; i++) {
        pgd->entries[i] = 0;
    }
    
    // 复制内核空间映射
    // (通常从当前页表复制高半部分)
    
    return pgd_phys;
}

/**
 * 销毁页表结构
 * 
 * @param cr3 要销毁的页表的CR3值
 */
void paging_destroy_address_space(uint64_t cr3)
{
    // 遍历页表结构并释放所有分配的页表
    
    // 最后释放PGD
    phys_free_page(cr3);
}

/**
 * 切换到指定地址空间
 * 
 * @param cr3 要切换到的地址空间的CR3值
 */
void paging_switch_address_space(uint64_t cr3)
{
    set_cr3(cr3);
} 