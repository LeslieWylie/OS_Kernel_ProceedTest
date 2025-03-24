CC = gcc
CFLAGS = -Wall -O2 -I../../include -I. -g
AS = as
ASFLAGS = --64

SRCS = process.c scheduler.c context.c memory.c process_control.c ../../include/printk.c test_process_control.c
OBJS = $(SRCS:.c=.o)
ASM_SRCS = switch.S
ASM_OBJS = $(ASM_SRCS:.S=.o)

TARGET = process_test

all: $(TARGET)

$(TARGET): $(OBJS) $(ASM_OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(ASM_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(ASM_OBJS) $(TARGET)

.PHONY: test
test: $(TARGET)
	./$(TARGET) 