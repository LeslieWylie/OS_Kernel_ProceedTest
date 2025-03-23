# 编译器和标志
CC = gcc
AS = gcc # 使用gcc来处理汇编文件
CFLAGS = -Wall -Werror -I../../include -I../../include/kernel
ASFLAGS = -c

# 源文件
COMMON_SRCS = ../../kernel/process/process.c \
              ../../kernel/process/scheduler.c \
              ../../kernel/process/context.c \
              ../../kernel/process/memory.c

# 汇编源文件
ASM_SRCS = ../../kernel/process/switch.S

# 生成的目标文件
ASM_OBJS = $(ASM_SRCS:.S=.o)
C_OBJS = $(COMMON_SRCS:.c=.o)

# 测试程序
TEST_PROGRAMS = test_process test_fifo test_context_switch

# 默认目标
all: $(TEST_PROGRAMS)

# 编译.S汇编文件
%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@

# 编译.c源文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 编译测试程序
test_process: test_process.c $(C_OBJS) $(ASM_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test_fifo: test_fifo.c $(C_OBJS) $(ASM_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test_context_switch: test_context_switch.c $(C_OBJS) $(ASM_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 运行所有测试
test: $(TEST_PROGRAMS)
	@echo "Running process management tests..."
	./test_process
	@echo "\nRunning FIFO scheduler tests..."
	./test_fifo
	@echo "\nRunning context switch tests..."
	./test_context_switch

# 清理
clean:
	rm -f $(TEST_PROGRAMS) *.o $(C_OBJS) $(ASM_OBJS)

.PHONY: all test clean 