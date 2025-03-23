CC = gcc
CFLAGS = -Wall -O2 -fno-stack-protector -fomit-frame-pointer -nostdinc -I../../include
AS = as
ASFLAGS = --64

SRCS = process.c scheduler.c context.c memory.c process_control.c
OBJS = $(SRCS:.c=.o)
ASM_SRCS = switch.S
ASM_OBJS = $(ASM_SRCS:.S=.o)

# 测试程序
TEST_SRCS = test_process_control.c
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_BIN = process_test

all: $(OBJS) $(ASM_OBJS) $(TEST_BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@

$(TEST_BIN): $(OBJS) $(ASM_OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(ASM_OBJS) $(TEST_OBJS)

clean:
	rm -f $(OBJS) $(ASM_OBJS) $(TEST_OBJS) $(TEST_BIN)

.PHONY: test
test: $(TEST_BIN)
	./$(TEST_BIN) 