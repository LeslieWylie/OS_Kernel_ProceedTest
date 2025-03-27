#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "process.h"
#include "../template/test_framework.h"

/* 测试数据 */
static process_t *test_process = NULL;

/* 模拟函数 */
void *test_kmalloc(size_t size) {
    return malloc(size);
}

void test_kfree(void *ptr) {
    free(ptr);
}

/* 测试准备与清理 */
void setup() {
    test_process = (process_t *)test_kmalloc(sizeof(process_t));
    memset(test_process, 0, sizeof(process_t));
    test_process->pid = 1;
    test_process->state = PROCESS_CREATED;
    test_process->priority = 2;
    test_process->counter = 5;
    strncpy(test_process->name, "test_process", sizeof(test_process->name) - 1);
}

void teardown() {
    if (test_process) {
        test_kfree(test_process);
        test_process = NULL;
    }
}

/* 测试用例 */

// 测试进程状态函数
void test_process_state() {
    // 测试状态字符串转换
    ASSERT_STR_EQ(process_state_to_string(PROCESS_CREATED), "CREATED");
    ASSERT_STR_EQ(process_state_to_string(PROCESS_READY), "READY");
    ASSERT_STR_EQ(process_state_to_string(PROCESS_RUNNING), "RUNNING");
    ASSERT_STR_EQ(process_state_to_string(PROCESS_BLOCKED), "BLOCKED");
    ASSERT_STR_EQ(process_state_to_string(PROCESS_TERMINATED), "TERMINATED");
    
    // 测试状态切换
    process_set_state(test_process, PROCESS_READY);
    ASSERT_EQ(test_process->state, PROCESS_READY);
    
    process_set_state(test_process, PROCESS_RUNNING);
    ASSERT_EQ(test_process->state, PROCESS_RUNNING);
    
    process_set_state(test_process, PROCESS_BLOCKED);
    ASSERT_EQ(test_process->state, PROCESS_BLOCKED);
}

// 测试进程存活状态检查
void test_process_is_alive() {
    // 活跃状态测试
    process_set_state(test_process, PROCESS_CREATED);
    ASSERT(process_is_alive(test_process));
    
    process_set_state(test_process, PROCESS_READY);
    ASSERT(process_is_alive(test_process));
    
    process_set_state(test_process, PROCESS_RUNNING);
    ASSERT(process_is_alive(test_process));
    
    process_set_state(test_process, PROCESS_BLOCKED);
    ASSERT(process_is_alive(test_process));
    
    // 终止状态测试
    process_set_state(test_process, PROCESS_TERMINATED);
    ASSERT(!process_is_alive(test_process));
    
    // NULL测试
    ASSERT(!process_is_alive(NULL));
}

// 主函数
int main() {
    TEST_SUITE_START();
    
    // 设置初始测试环境
    setup();
    
    // 运行测试
    RUN_TEST(test_process_state);
    RUN_TEST(test_process_is_alive);
    
    // 清理测试环境
    teardown();
    
    TEST_SUITE_END();
} 