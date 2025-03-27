# 内核总体Makefile
# ==========================================

# 相对路径
TOP_DIR     := ..
INCLUDE_DIR := $(TOP_DIR)/include
BUILD_DIR   ?= $(TOP_DIR)/build/kernel

# 编译器和标志
CC      := gcc
LD      := ld
AR      := ar
CFLAGS  := -Wall -Wextra -I$(INCLUDE_DIR) -fno-stack-protector -MMD -MP -g
LDFLAGS := -T $(TOP_DIR)/linker.ld

# 内核子系统
SUBSYSTEMS := process mm fs utils

# 子系统库
SUBSYS_LIBS := $(foreach dir,$(SUBSYSTEMS),$(BUILD_DIR)/$(dir)/lib$(dir).a)

# 内核源文件
KERNEL_SRCS := core.c interrupt.c syscall.c shell.c
KERNEL_OBJS := $(addprefix $(BUILD_DIR)/, $(KERNEL_SRCS:.c=.o))

# 内核镜像
KERNEL_TARGET := $(BUILD_DIR)/kernel.elf

# 默认目标
.PHONY: all clean subdirs $(SUBSYSTEMS)

all: subdirs $(KERNEL_TARGET)

# 编译所有子系统
subdirs: $(SUBSYSTEMS)

$(SUBSYSTEMS):
	@echo "Building $@ subsystem..."
	@$(MAKE) -C $@ BUILD_DIR=$(BUILD_DIR)/$@

# 链接内核
$(KERNEL_TARGET): $(KERNEL_OBJS) $(SUBSYS_LIBS)
	@echo "Linking kernel..."
	@$(LD) $(LDFLAGS) -o $@ $^

# 编译C文件
$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	@echo "Cleaning kernel..."
	@for dir in $(SUBSYSTEMS); do \
		$(MAKE) -C $$dir clean; \
	done
	@rm -f $(KERNEL_OBJS) $(KERNEL_TARGET)
	@rm -f $(KERNEL_OBJS:.o=.d)

# 包含自动生成的依赖
-include $(KERNEL_OBJS:.o=.d) 