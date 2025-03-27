# 进程子系统Makefile
# ==========================================

# 相对路径
TOP_DIR     := ../..
INCLUDE_DIR := $(TOP_DIR)/include
BUILD_DIR   := $(TOP_DIR)/build/kernel/process
BIN_DIR     := $(TOP_DIR)/bin

# 编译器和标志
CC       := gcc
AS       := gcc
AR       := ar
CFLAGS   := -Wall -Wextra -I$(INCLUDE_DIR) -I. -ffreestanding -nostdlib \
            -fno-stack-protector -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
            -Werror -MMD -MP -g
ASFLAGS  := -I$(INCLUDE_DIR) -c

# 源文件 - 只保留核心进程管理文件
C_SRCS   := process.c scheduler.c context.c memory.c
ASM_SRCS := switch.S

# 目标文件
C_OBJS   := $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.o))
ASM_OBJS := $(addprefix $(BUILD_DIR)/, $(ASM_SRCS:.S=.o))
ALL_OBJS := $(C_OBJS) $(ASM_OBJS)

# 生成的库
LIB_TARGET := $(BUILD_DIR)/libprocess.a

# 默认目标
.PHONY: all clean

all: prepare $(LIB_TARGET)

# 准备目录
prepare:
	@mkdir -p $(BUILD_DIR)

# 创建静态库
$(LIB_TARGET): $(ALL_OBJS)
	@echo "Creating process library: $@"
	@$(AR) rcs $@ $^

# 编译C文件
$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# 编译汇编文件
$(BUILD_DIR)/%.o: %.S
	@echo "Assembling $<"
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) -c $< -o $@

# 清理
clean:
	@echo "Cleaning process subsystem"
	@rm -f $(ALL_OBJS) $(LIB_TARGET)
	@rm -f $(ALL_OBJS:.o=.d)

# 包含自动生成的依赖
-include $(C_OBJS:.o=.d) 