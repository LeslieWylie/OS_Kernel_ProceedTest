# 指定编译器和工具
AS = nasm
CC = gcc
LD = ld
GRUB = grub-mkrescue
QEMU = qemu-system-x86_64

# 目录结构
BUILD_DIR = build
KERNEL_DIR = kernel
DRIVERS_DIR = drivers
ARCH_DIR = arch/x86
ISO_DIR = iso

# 生成的文件
BOOT_OBJ = $(BUILD_DIR)/boot.o
KERNEL_OBJS = $(BUILD_DIR)/kernel.o $(BUILD_DIR)/printk.o $(BUILD_DIR)/keyboard.o \
              $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/shell.o $(BUILD_DIR)/string.o $(BUILD_DIR)/idt.o \
			#   $(BUILD_DIR)/

KERNEL_BIN = $(BUILD_DIR)/kernel.bin
ISO_FILE = PhyOS.iso

# 编译选项
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -c -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld

# 默认目标
all: $(ISO_FILE)

# 确保 build 目录存在
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 编译 boot.S
$(BOOT_OBJ): boot/boot.S | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# 编译arch/x86的文件
$(BUILD_DIR)/idt.o: $(ARCH_DIR)/idt.S | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# 编译 kernel/ 里的 C 文件
$(BUILD_DIR)/kernel.o: $(KERNEL_DIR)/kernel.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/printk.o: $(KERNEL_DIR)/printk.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/shell.o: $(KERNEL_DIR)/shell.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/interrupt.o: $(KERNEL_DIR)/interrupt.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/string.o: kernel/string.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# 编译 drivers/ 里的 C 文件
$(BUILD_DIR)/keyboard.o: $(DRIVERS_DIR)/keyboard.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# 链接所有目标文件
$(KERNEL_BIN): $(BOOT_OBJ) $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# 复制内核到 ISO 目录
copy: $(KERNEL_BIN)
	mkdir -p $(ISO_DIR)/boot/
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/

# 生成 ISO
$(ISO_FILE): copy
	$(GRUB) -o $@ $(ISO_DIR)

# 运行 QEMU
run: $(ISO_FILE)
	$(QEMU) -cdrom $<

# 清理编译生成的文件
clean:
	rm -rf $(BUILD_DIR) $(ISO_FILE)
