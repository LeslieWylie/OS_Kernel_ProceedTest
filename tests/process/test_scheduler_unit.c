#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "process.h"
#include "scheduler.h"
#include "../template/test_framework.h"

/* 测试数据 */
static process_t *test_processes[3] = {NULL, NULL, NULL};

/* 模拟函数 */
void *test_kmalloc(size_t size) {
    return malloc(size);
}

void test_kfree(void *ptr) {
    free(ptr);
}

/* 测试准备与清理 */
void setup() {
    // 创建测试进程
    for (int i = 0; i < 3; i++) {
        test_processes[i] = (process_t *)test_kmalloc(sizeof(process_t));
        memset(test_processes[i], 0, sizeof(process_t));
        test_processes[i]->pid = i + 1;
        test_processes[i]->state = PROCESS_READY;
        test_processes[i]->priority = 3 - i;  // 优先级递减
        test_processes[i]->counter = i + 1;   // 时间片递增
        sprintf(test_processes[i]->name, "process_%d", i + 1);
    }
    
    // 初始化调度器
    scheduler_init();
}

void teardown() {
    // 释放测试进程
    for (int i = 0; i < 3; i++) {
        if (test_processes[i]) {
            test_kfree(test_processes[i]);
            test_processes[i] = NULL;
        }
    }
}

/* 测试用例 */

// 测试调度器队列操作
void test_scheduler_queue() {
    // 添加进程到调度队列
    scheduler_add(test_processes[0]);
    scheduler_add(test_processes[1]);
    
    // 获取下一个要运行的进程（应该是队列头）
    process_t *next = scheduler_next();
    ASSERT_NOT_NULL(next);
    ASSERT_EQ(next->pid, test_processes[0]->pid);
    
    // 添加第三个进程
    scheduler_add(test_processes[2]);
    
    // 从队列移除第一个进程
    scheduler_remove(test_processes[0]);
    
    // 再次获取下一个进程（应该是现在的队列头）
    next = scheduler_next();
    ASSERT_NOT_NULL(next);
    ASSERT_EQ(next->pid, test_processes[1]->pid);
}

// 测试SJF调度算法
void test_sjf_scheduling() {
    // 确保进程准备就绪
    for (int i = 0; i < 3; i++) {
        test_processes[i]->state = PROCESS_READY;
        scheduler_add(test_processes[i]);
    }
    
    // 设置当前进程
    process_set_current(NULL);
    
    // 调用SJF调度（应该选择最短作业）
    // 注意：在实际测试中我们需要模拟scheduler_next的行为
    // 这里仅作示例，如果函数内部直接修改了外部状态，可能需要适当调整测试
    
    // 检查时间片最短的进程是否被选为下一个运行的进程
    process_t *shortest_job = test_processes[0]; // process_1，counter=1
    process_t *next = scheduler_next();
    
    ASSERT_NOT_NULL(next);
    ASSERT_EQ(next->pid, shortest_job->pid);
}

// 测试时间片处理
void test_timer_tick() {
    // 设置当前进程
    process_t *current = test_processes[1]; // process_2, counter=2
    process_set_current(current);
    current->state = PROCESS_RUNNING;
    
    // 初始计数器
    int initial_counter = current->counter;
    
    // 模拟时钟中断
    do_timer();
    
    // 检查计数器是否减少
    ASSERT_EQ(current->counter, initial_counter - 1);
    
    // 多次触发时钟中断，直到时间片用完
    while (current->counter > 0) {
        do_timer();
    }
    
    // 时间片用完后，应该触发调度
    ASSERT_EQ(current->counter, 0);
    // 注意：通常调度会由do_timer()触发，这里仅检查计数器状态
}

// 主函数
int main() {
    TEST_SUITE_START();
    
    // 设置初始测试环境
    setup();
    
    // 运行测试
    RUN_TEST(test_scheduler_queue);
    RUN_TEST(test_sjf_scheduling);
    RUN_TEST(test_timer_tick);
    
    // 清理测试环境
    teardown();
    
    TEST_SUITE_END();
} 