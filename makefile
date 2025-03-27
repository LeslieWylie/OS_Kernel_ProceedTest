# OS内核主Makefile
# ==========================================

# 项目目录结构
ROOT_DIR    := $(shell pwd)
BUILD_DIR   := $(ROOT_DIR)/build
BIN_DIR     := $(ROOT_DIR)/bin
INCLUDE_DIR := $(ROOT_DIR)/include
KERNEL_DIR  := $(ROOT_DIR)/kernel
TESTS_DIR   := $(ROOT_DIR)/tests

# 编译器和编译选项
CC      := gcc
AS      := gcc
LD      := ld
AR      := ar
OBJCOPY := objcopy

# 编译标志
CFLAGS     := -Wall -Wextra -I$(INCLUDE_DIR) -fno-stack-protector -MMD -MP -g
CFLAGS_OPT := -O2
LDFLAGS    := 
ASFLAGS    := -c

# 是否启用调试
DEBUG ?= 1
ifeq ($(DEBUG), 1)
  CFLAGS += -DDEBUG
else
  CFLAGS += $(CFLAGS_OPT)
endif

# 子系统目录
SUBSYSTEMS := kernel/process kernel/mm kernel/fs kernel/utils

# 子系统目标 (每个目录编译为一个静态库)
SUBSYS_LIBS := $(foreach dir,$(SUBSYSTEMS),$(BUILD_DIR)/$(dir)/lib$(notdir $(dir)).a)

# 测试目录
TEST_DIRS := tests/process

# 主要目标
.PHONY: all clean prepare subsystems tests

# 默认目标 - 编译所有内容
all: prepare subsystems tests

# 创建必要的目录
prepare:
	@echo "Creating build directories..."
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@for dir in $(SUBSYSTEMS); do \
		mkdir -p $(BUILD_DIR)/$$dir; \
	done
	@for dir in $(TEST_DIRS); do \
		mkdir -p $(BUILD_DIR)/$$dir; \
	done

# 编译所有子系统
subsystems: $(SUBSYS_LIBS)

# 编译静态库的通用规则
$(BUILD_DIR)/%.a:
	@echo "Building $@..."
	@$(MAKE) -C $(dir $(subst $(BUILD_DIR)/,,$@)) BUILD_DIR=$(BUILD_DIR)/$(dir $(subst $(BUILD_DIR)/,,$@))

# 编译测试程序
.PHONY: tests
tests:
	@for dir in $(TEST_DIRS); do \
		echo "Building tests in $$dir..."; \
		$(MAKE) -C $$dir BUILD_DIR=$(BUILD_DIR)/$$dir BIN_DIR=$(BIN_DIR); \
	done

# 运行所有测试
.PHONY: run-tests
run-tests: tests
	@echo "\n=== 运行所有测试 ==="
	@for dir in $(TEST_DIRS); do \
		echo "\n--- 运行 $$dir 测试 ---"; \
		$(MAKE) -C $$dir run BUILD_DIR=$(BUILD_DIR)/$$dir BIN_DIR=$(BIN_DIR) || exit 1; \
	done

# 清理编译产物
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)/*
	@rm -rf $(BIN_DIR)/*
	@echo "All build artifacts removed."

# 包含自动生成的依赖
-include $(shell find $(BUILD_DIR) -name "*.d" 2>/dev/null)
