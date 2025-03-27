# 内存管理子系统Makefile
# ==========================================

# 相对路径
TOP_DIR     := ../..
INCLUDE_DIR := $(TOP_DIR)/include
BUILD_DIR   ?= $(TOP_DIR)/build/kernel/mm
BIN_DIR     := $(TOP_DIR)/bin

# 编译器和标志
CC      := gcc
AR      := ar
CFLAGS  := -Wall -Wextra -I$(INCLUDE_DIR) -I. -fno-stack-protector -MMD -MP -g

# 源文件 - 按照功能模块组织
C_SRCS  := physical.c virtual.c heap.c paging.c process_mm.c
C_OBJS  := $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.o))

# 生成的库
LIB_TARGET := $(BUILD_DIR)/libmm.a

# 默认目标
.PHONY: all clean

all: prepare $(LIB_TARGET)

# 准备目录
prepare:
	@mkdir -p $(BUILD_DIR)

# 创建静态库
$(LIB_TARGET): $(C_OBJS)
	@echo "Creating memory management library: $@"
	@$(AR) rcs $@ $^

# 编译C文件
$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	@echo "Cleaning memory management subsystem"
	@rm -f $(C_OBJS) $(LIB_TARGET)
	@rm -f $(C_OBJS:.o=.d)

# 包含自动生成的依赖
-include $(C_OBJS:.o=.d) 