# 编译器和标志
CC = gcc
CFLAGS = -Wall -Wextra -I../../include/kernel

# 源文件
COMMON_SRCS = ../../kernel/process/process.c \
              ../../kernel/process/scheduler.c \
              ../../kernel/process/context.c \
              ../../kernel/process/memory.c

# 测试程序
TEST_PROGRAMS = test_process test_fifo

# 默认目标
all: $(TEST_PROGRAMS)

# 编译测试程序
test_process: test_process.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

test_fifo: test_fifo.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# 运行所有测试
test: $(TEST_PROGRAMS)
	@echo "Running process management tests..."
	./test_process
	@echo "\nRunning FIFO scheduler tests..."
	./test_fifo

# 清理
clean:
	rm -f $(TEST_PROGRAMS) *.o

.PHONY: all test clean 