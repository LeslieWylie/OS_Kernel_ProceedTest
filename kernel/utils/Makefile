# 工具子系统Makefile
# ==========================================

# 相对路径
TOP_DIR     := ../..
INCLUDE_DIR := $(TOP_DIR)/include
BUILD_DIR   ?= $(TOP_DIR)/build/kernel/utils
BIN_DIR     := $(TOP_DIR)/bin

# 编译器和标志
CC      := gcc
AR      := ar
CFLAGS  := -Wall -Wextra -I$(INCLUDE_DIR) -I. -fno-stack-protector -MMD -MP -g

# 源文件
C_SRCS  := string.c printk.c debug.c
C_OBJS  := $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.o))

# 生成的库
LIB_TARGET := $(BUILD_DIR)/libutils.a

# 默认目标
.PHONY: all clean

all: prepare $(LIB_TARGET)

# 准备目录
prepare:
	@mkdir -p $(BUILD_DIR)

# 创建静态库
$(LIB_TARGET): $(C_OBJS)
	@echo "Creating utilities library: $@"
	@$(AR) rcs $@ $^

# 编译C文件
$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	@echo "Cleaning utilities subsystem"
	@rm -f $(C_OBJS) $(LIB_TARGET)
	@rm -f $(C_OBJS:.o=.d)

# 包含自动生成的依赖
-include $(C_OBJS:.o=.d) 