# 模块测试Makefile模板
#
# 使用此模板为每个模块创建测试Makefile
# 需要设置的变量:
# - MODULE_NAME: 模块名称
# - UNIT_TESTS: 单元测试源文件列表
# - INTEGRATION_TESTS: 集成测试源文件列表
# - DEPENDENCIES: 依赖模块列表

# 模块名称 (由具体模块覆盖)
MODULE_NAME ?= unknown

# 源文件列表 (由具体模块覆盖)
UNIT_TESTS ?= 
INTEGRATION_TESTS ?=

# 依赖的模块 (由具体模块覆盖)
DEPENDENCIES ?=

# 编译器和标志
CC := gcc
CFLAGS := -Wall -Wextra -I../../include
LDFLAGS := 

# 目录定义
BUILD_DIR := ../../build/tests/$(MODULE_NAME)
BIN_DIR := ../../bin/tests/$(MODULE_NAME)
SOURCE_DIR := ../../kernel/$(MODULE_NAME)

# 获取依赖模块源文件
DEP_SOURCES := $(foreach dep,$(DEPENDENCIES),$(wildcard ../../kernel/$(dep)/*.c))
DEP_OBJS := $(patsubst ../../kernel/%.c,../../build/kernel/%.o,$(DEP_SOURCES))

# 测试可执行文件
UNIT_TEST_BINS := $(patsubst %.c,$(BIN_DIR)/%,$(UNIT_TESTS))
INTEGRATION_TEST_BINS := $(patsubst %.c,$(BIN_DIR)/%,$(INTEGRATION_TESTS))

# 目标集合
.PHONY: all test unit_test integration_test clean help

# 默认目标：构建所有测试
all: $(UNIT_TEST_BINS) $(INTEGRATION_TEST_BINS)
	@echo "$(MODULE_NAME) 模块所有测试构建完成"

# 运行单元测试
unit_test: $(UNIT_TEST_BINS)
	@echo "运行 $(MODULE_NAME) 模块单元测试..."
	@for test in $(UNIT_TEST_BINS); do \
		echo "----------------------------------------"; \
		echo "执行测试: `basename $$test`"; \
		$$test || echo "测试失败: `basename $$test`"; \
		echo "----------------------------------------"; \
	done

# 运行集成测试
integration_test: $(INTEGRATION_TEST_BINS)
	@echo "运行 $(MODULE_NAME) 模块集成测试..."
	@for test in $(INTEGRATION_TEST_BINS); do \
		echo "----------------------------------------"; \
		echo "执行测试: `basename $$test`"; \
		$$test || echo "测试失败: `basename $$test`"; \
		echo "----------------------------------------"; \
	done

# 运行所有测试
test: unit_test integration_test
	@echo "$(MODULE_NAME) 模块所有测试运行完成"

# 单元测试构建规则
$(BIN_DIR)/%: %.c
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)
	@echo "编译单元测试: $<"
	@$(CC) $(CFLAGS) -o $@ $< $(DEP_OBJS) $(LDFLAGS)

# 集成测试构建规则
$(BIN_DIR)/%: %.c
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)
	@echo "编译集成测试: $<"
	@$(CC) $(CFLAGS) -o $@ $< $(DEP_OBJS) $(LDFLAGS)

# 清理生成的文件
clean:
	@echo "清理 $(MODULE_NAME) 模块测试文件..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "清理完成"

# 显示帮助信息
help:
	@echo "$(MODULE_NAME) 模块测试"
	@echo ""
	@echo "可用目标:"
	@echo "  all:              构建所有测试"
	@echo "  clean:            清理生成的文件"
	@echo "  test:             运行所有测试"
	@echo "  unit_test:        运行单元测试"
	@echo "  integration_test: 运行集成测试"
	@echo ""
	@echo "单元测试: $(UNIT_TESTS)"
	@echo "集成测试: $(INTEGRATION_TESTS)"
	@echo "依赖模块: $(DEPENDENCIES)"
	@echo "" 