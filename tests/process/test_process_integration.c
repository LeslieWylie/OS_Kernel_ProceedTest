#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "process.h"
#include "scheduler.h"
#include "memory.h"
#include "context.h"
#include "../template/test_framework.h"

/* 测试数据 */
static int test_step = 0;
static pid_t test_pids[3] = {0};
static int process_run_counts[3] = {0};

/* 测试进程函数 */
void test_process_entry(void *arg) {
    int process_id = *(int*)arg;
    process_run_counts[process_id]++;
    
    // 记录执行
    printf("进程 %d 执行中，运行次数: %d\n", 
           process_id, process_run_counts[process_id]);
    
    // 短暂执行后主动放弃CPU
    if (process_run_counts[process_id] >= 3) {
        process_exit(0);  // 执行三次后退出
    }
}

/* 测试辅助函数 */
void wait_for_step(int step) {
    // 等待测试执行到特定步骤
    // 在实际测试中，这可能涉及等待进程调度或计时器
    test_step = step;
}

/* 测试用例 */

// 集成测试：进程创建与调度
void test_process_lifecycle() {
    // 初始化进程管理和调度系统
    process_init();
    
    // 创建测试数据
    int process_args[3] = {0, 1, 2};
    
    printf("=== 创建测试进程 ===\n");
    
    // 创建三个测试进程
    for (int i = 0; i < 3; i++) {
        test_pids[i] = process_create(test_process_entry, &process_args[i], i+1);
        ASSERT(test_pids[i] > 0);
        printf("进程 %d 创建，PID: %d\n", i, test_pids[i]);
    }
    
    printf("=== 模拟进程调度 ===\n");
    
    // 模拟调度循环
    for (int i = 0; i < 10; i++) {
        printf("调度循环 %d...\n", i);
        schedule();
        wait_for_step(i+1);
    }
    
    printf("=== 检查进程执行情况 ===\n");
    
    // 检查进程是否都执行过
    for (int i = 0; i < 3; i++) {
        printf("进程 %d 执行了 %d 次\n", i, process_run_counts[i]);
        
        // 在实际测试中，可能需要一些更复杂的断言
        // 这里简化为仅检查是否执行过
        ASSERT(process_run_counts[i] > 0);
    }
}

// 集成测试：进程间资源共享与同步
void test_process_coordination() {
    printf("=== 测试进程间协作 ===\n");
    
    // 创建共享资源
    int *shared_counter = (int*)kmalloc(sizeof(int));
    *shared_counter = 0;
    
    // 测试进程函数（使用闭包特性，在实际C代码中可能需要调整）
    void incrementer_process(void *arg) {
        for (int i = 0; i < 5; i++) {
            (*shared_counter)++;
            printf("进程增加计数器: %d\n", *shared_counter);
            
            // 模拟进程放弃CPU
            schedule();
        }
    }
    
    void reader_process(void *arg) {
        for (int i = 0; i < 3; i++) {
            printf("进程读取计数器: %d\n", *shared_counter);
            
            // 模拟进程放弃CPU
            schedule();
        }
    }
    
    // 创建测试进程
    pid_t incrementer_pid = process_create(incrementer_process, NULL, 2);
    pid_t reader_pid = process_create(reader_process, NULL, 1);
    
    ASSERT(incrementer_pid > 0);
    ASSERT(reader_pid > 0);
    
    // 模拟调度循环
    for (int i = 0; i < 8; i++) {
        printf("资源共享调度循环 %d...\n", i);
        schedule();
        wait_for_step(20 + i);
    }
    
    // 检查共享资源最终状态
    printf("共享计数器最终值: %d\n", *shared_counter);
    ASSERT(*shared_counter > 0);
    
    // 释放共享资源
    kfree(shared_counter);
}

// 主函数
int main() {
    TEST_SUITE_START();
    
    // 运行集成测试
    RUN_TEST(test_process_lifecycle);
    RUN_TEST(test_process_coordination);
    
    TEST_SUITE_END();
} 