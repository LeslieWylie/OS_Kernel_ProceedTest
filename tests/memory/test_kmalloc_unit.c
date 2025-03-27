#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "memory.h"
#include "../template/test_framework.h"

/**
 * @brief 内存分配器kmalloc函数的单元测试
 */

/* 测试用例 */

// 测试基本内存分配
void test_kmalloc_basic() {
    // 分配内存
    void *ptr = kmalloc(10);
    ASSERT_NOT_NULL(ptr);
    
    // 填充内存并检查
    memset(ptr, 'A', 10);
    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(((char*)ptr)[i], 'A');
    }
    
    // 释放内存
    kfree(ptr);
}

// 测试分配不同大小的内存
void test_kmalloc_sizes() {
    // 小块内存
    void *ptr1 = kmalloc(1);
    ASSERT_NOT_NULL(ptr1);
    
    // 中等大小
    void *ptr2 = kmalloc(100);
    ASSERT_NOT_NULL(ptr2);
    
    // 较大块
    void *ptr3 = kmalloc(1000);
    ASSERT_NOT_NULL(ptr3);
    
    // 检查地址不重叠
    ptrdiff_t diff1 = (char*)ptr2 - (char*)ptr1;
    ptrdiff_t diff2 = (char*)ptr3 - (char*)ptr2;
    
    // 地址应该是向上增长的，差值应该大于等于分配的大小
    ASSERT(diff1 > 0);
    ASSERT(diff2 > 0);
    ASSERT(diff1 >= 1);
    ASSERT(diff2 >= 100);
    
    // 释放内存
    kfree(ptr1);
    kfree(ptr2);
    kfree(ptr3);
}

// 测试对齐问题
void test_kmalloc_alignment() {
    // 分配一些不同大小的内存块
    void *ptr1 = kmalloc(1);
    void *ptr2 = kmalloc(3);
    void *ptr3 = kmalloc(7);
    void *ptr4 = kmalloc(4);
    
    // 检查是否按4字节对齐
    ASSERT_EQ((uintptr_t)ptr1 % 4, 0);
    ASSERT_EQ((uintptr_t)ptr2 % 4, 0);
    ASSERT_EQ((uintptr_t)ptr3 % 4, 0);
    ASSERT_EQ((uintptr_t)ptr4 % 4, 0);
    
    // 释放内存
    kfree(ptr1);
    kfree(ptr2);
    kfree(ptr3);
    kfree(ptr4);
}

// 测试极限情况
void test_kmalloc_edge_cases() {
    // 分配0字节
    void *ptr1 = kmalloc(0);
    ASSERT_NOT_NULL(ptr1);  // 应该返回非空指针
    
    // 分配非常大的内存块（应该失败）
    void *ptr2 = kmalloc(10 * 1024 * 1024);  // 10MB, 超过1MB堆限制
    ASSERT_NULL(ptr2);
    
    // 释放内存
    kfree(ptr1);
}

// 测试多次分配和释放
void test_kmalloc_multiple() {
    #define TEST_COUNT 10
    void *ptrs[TEST_COUNT];
    
    // 多次分配
    for (int i = 0; i < TEST_COUNT; i++) {
        ptrs[i] = kmalloc(10 + i);
        ASSERT_NOT_NULL(ptrs[i]);
    }
    
    // 检查所有指针不同
    for (int i = 0; i < TEST_COUNT; i++) {
        for (int j = i + 1; j < TEST_COUNT; j++) {
            ASSERT(ptrs[i] != ptrs[j]);
        }
    }
    
    // 释放所有内存
    for (int i = 0; i < TEST_COUNT; i++) {
        kfree(ptrs[i]);
    }
}

// 主函数
int main() {
    TEST_SUITE_START();
    
    // 运行测试
    RUN_TEST(test_kmalloc_basic);
    RUN_TEST(test_kmalloc_sizes);
    RUN_TEST(test_kmalloc_alignment);
    RUN_TEST(test_kmalloc_edge_cases);
    RUN_TEST(test_kmalloc_multiple);
    
    TEST_SUITE_END();
}
