# 操作系统内核测试框架主Makefile
#
# 作用: 提供统一的测试构建和运行接口
# 使用: make [target]
#   可用目标:
#     all:        构建所有测试
#     clean:      清理生成的文件
#     test:       运行所有测试
#     process:    运行进程模块测试
#     memory:     运行内存模块测试
#     kernel:     运行内核模块测试
#     fs:         运行文件系统模块测试
#     drivers:    运行设备驱动模块测试
#     integration: 运行集成测试

# 编译器和标志
CC := gcc
CFLAGS := -Wall -Wextra -I../include
LDFLAGS := 

# 目录定义
TEST_DIRS := process memory kernel fs drivers integration
BUILD_DIR := ../build/tests
BIN_DIR := ../bin/tests

# 目标模块
.PHONY: all clean test $(TEST_DIRS) 

# 默认目标: 构建所有测试
all: $(TEST_DIRS)

# 测试目标: 运行所有测试
test: $(TEST_DIRS)
	@echo "========================================="
	@echo "运行所有测试"
	@echo "========================================="
	@for dir in $(TEST_DIRS); do \
		if [ -d "$$dir" ]; then \
			echo "----------------------------------------"; \
			echo "运行 $$dir 模块测试"; \
			echo "----------------------------------------"; \
			$(MAKE) -C $$dir test || exit 1; \
		fi; \
	done
	@echo "========================================="
	@echo "所有测试运行完成"
	@echo "========================================="

# 清理所有生成的文件
clean:
	@echo "清理所有测试生成的文件..."
	@for dir in $(TEST_DIRS); do \
		if [ -d "$$dir" ]; then \
			$(MAKE) -C $$dir clean; \
		fi; \
	done
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "清理完成"

# 为每个模块构建测试
$(TEST_DIRS):
	@if [ -d "$@" ]; then \
		echo "构建 $@ 模块测试..."; \
		mkdir -p $(BUILD_DIR)/$@ $(BIN_DIR)/$@; \
		$(MAKE) -C $@ all || exit 1; \
		echo "$@ 模块测试构建完成"; \
	else \
		echo "警告: $@ 目录不存在，跳过"; \
	fi

# 显示帮助信息
help:
	@echo "操作系统内核测试框架"
	@echo ""
	@echo "可用目标:"
	@echo "  all:        构建所有测试"
	@echo "  clean:      清理生成的文件"
	@echo "  test:       运行所有测试"
	@echo "  process:    运行进程模块测试"
	@echo "  memory:     运行内存模块测试"
	@echo "  kernel:     运行内核模块测试"
	@echo "  fs:         运行文件系统模块测试"
	@echo "  drivers:    运行设备驱动模块测试"
	@echo "  integration: 运行集成测试"
	@echo "" 