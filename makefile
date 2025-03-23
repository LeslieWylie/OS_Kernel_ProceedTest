# OS内核主Makefile

# 编译器和标志
CC = gcc
AS = gcc
CFLAGS = -Wall -Wextra -I./include/kernel
ASFLAGS = -c

# 目标文件夹
BIN_DIR = bin

# 源代码目录
SRC_DIR = kernel
PROC_DIR = $(SRC_DIR)/process

# 源文件
PROC_SRCS = $(PROC_DIR)/process.c \
            $(PROC_DIR)/scheduler.c \
            $(PROC_DIR)/context.c \
            $(PROC_DIR)/memory.c

# 汇编源文件
ASM_SRCS = $(PROC_DIR)/switch.S

# 生成的目标文件
ASM_OBJS = $(ASM_SRCS:.S=.o)
C_OBJS = $(PROC_SRCS:.c=.o)
ALL_OBJS = $(C_OBJS) $(ASM_OBJS)

# 测试程序
TEST_PROGRAMS = test_process test_fifo test_context_switch
TEST_TARGETS = $(addprefix $(BIN_DIR)/, $(TEST_PROGRAMS))

# 默认目标
all: prepare $(TEST_TARGETS)

# 准备输出目录
prepare:
	@mkdir -p $(BIN_DIR)

# 编译.S汇编文件
%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@

# 编译.c源文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 编译测试程序
$(BIN_DIR)/test_process: tests/process/test_process.c $(ALL_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test_fifo: tests/process/test_fifo.c $(ALL_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test_context_switch: tests/process/test_context_switch.c $(ALL_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 运行所有测试
test: all
	@echo "\n=== 运行进程管理测试 ==="
	$(BIN_DIR)/test_process
	@echo "\n=== 运行FIFO调度器测试 ==="
	$(BIN_DIR)/test_fifo
	@echo "\n=== 运行上下文切换测试 ==="
	$(BIN_DIR)/test_context_switch

# 清理
clean:
	rm -f $(TEST_TARGETS) $(ALL_OBJS)

.PHONY: all prepare test clean
