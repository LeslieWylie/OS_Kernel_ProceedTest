#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

/**
 * @file test_framework.h
 * @brief 简单的单元测试框架
 *
 * 提供基本的测试断言和测试运行器
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* 颜色定义，用于彩色输出 */
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/* 测试统计 */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* 测试断言宏 */
#define ASSERT(test) do { \
    if (!(test)) { \
        printf("%s[FAIL]%s %s:%d: %s\n", KRED, KNRM, __FILE__, __LINE__, #test); \
        tests_failed++; \
        return; \
    } \
} while (0)

#define ASSERT_EQ(actual, expected) do { \
    if ((actual) != (expected)) { \
        printf("%s[FAIL]%s %s:%d: %s != %s\n", KRED, KNRM, __FILE__, __LINE__, #actual, #expected); \
        tests_failed++; \
        return; \
    } \
} while (0)

#define ASSERT_NOT_NULL(value) do { \
    if ((value) == NULL) { \
        printf("%s[FAIL]%s %s:%d: %s is NULL\n", KRED, KNRM, __FILE__, __LINE__, #value); \
        tests_failed++; \
        return; \
    } \
} while (0)

#define ASSERT_NULL(value) do { \
    if ((value) != NULL) { \
        printf("%s[FAIL]%s %s:%d: %s is not NULL\n", KRED, KNRM, __FILE__, __LINE__, #value); \
        tests_failed++; \
        return; \
    } \
} while (0)

/* 运行测试用例 */
#define RUN_TEST(test) do { \
    printf("%s[TEST]%s Running %s...\n", KCYN, KNRM, #test); \
    tests_run++; \
    int prev_failed = tests_failed; \
    test(); \
    if (prev_failed == tests_failed) { \
        printf("%s[PASS]%s %s\n", KGRN, KNRM, #test); \
        tests_passed++; \
    } \
} while (0)

/* 开始测试套件 */
#define TEST_SUITE_START() do { \
    printf("%s=======================================%s\n", KYEL, KNRM); \
    printf("%s       开始运行测试套件              %s\n", KYEL, KNRM); \
    printf("%s=======================================%s\n\n", KYEL, KNRM); \
    tests_run = 0; \
    tests_passed = 0; \
    tests_failed = 0; \
} while (0)

/* 结束测试套件并报告结果 */
#define TEST_SUITE_END() do { \
    printf("\n%s=======================================%s\n", KYEL, KNRM); \
    printf("%s       测试套件运行完成              %s\n", KYEL, KNRM); \
    printf("%s=======================================%s\n", KYEL, KNRM); \
    printf("总计运行测试: %d\n", tests_run); \
    printf("%s通过: %d%s\n", KGRN, tests_passed, KNRM); \
    printf("%s失败: %d%s\n", KRED, tests_failed, KNRM); \
    return tests_failed > 0 ? 1 : 0; \
} while (0)

#endif /* TEST_FRAMEWORK_H */