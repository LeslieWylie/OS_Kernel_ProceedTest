#include "keyboard.h"
#include "printk.h"
#include "io.h"
#include "interrupt.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IRQ1 33  // 键盘中断号（0x21）

#define INPUT_BUFFER_SIZE 256

static char input_buffer[INPUT_BUFFER_SIZE];
static int buffer_index = 0;

// ASCII 对照表（简化版）
static char scancode_to_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0
};

// 键盘中断处理程序
void keyboard_handler() {
    printk("first we get it.\n");
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);
    
    if (scancode & 0x80) {
        // 忽略按键释放事件
    } else {
        char c = scancode_to_ascii[scancode];
        if (c) {
            printk("%c", c);  // 打印到屏幕
            if (c == '\n') {
                input_buffer[buffer_index] = '\0';
                buffer_index = 0;
            } else if (buffer_index < INPUT_BUFFER_SIZE - 1) {
                input_buffer[buffer_index++] = c;
            }
        }
    }
}

char get_key() {
    while (buffer_index == 0);  // 等待用户输入
    char c = input_buffer[0];

    // 移动缓冲区（删除第一个字符）
    for (int i = 0; i < buffer_index - 1; i++) {
        input_buffer[i] = input_buffer[i + 1];
    }
    buffer_index--;

    return c;
}


// 读取输入
void read_input(char *buffer, int size) {
    int i = 0;
    char c;

    while (1) {
        c = get_key();  // 获取键盘输入（应该是阻塞的）

        if (c == '\n' || i >= size - 1) {
            buffer[i] = '\0';  // 结束字符串
            break;
        } else if (c == '\b' && i > 0) {  // 处理退格键
            i--;
            printk("\b \b");  // 在屏幕上删除字符
        } else {
            buffer[i++] = c;
            printk("%c", c);  // 显示输入的字符
        }
    }
}

int try_enable_keyboard() {
    // 等待输出缓冲区非空
    while (!(inb(0x64) & 0x01)) {
        // ...
    }

    uint8_t ack = inb(0x60);

    if (ack == 0xFA) {
        // 收到 ACK，表示键盘成功启用扫描
        return 0; // 0 表示成功
    } 
    else if (ack == 0xFE) {
        // 键盘请求重发命令
        // 根据需要，可以设置一个循环或计数，最多重发几次
        for (int i = 0; i < 3; i++) { 
            // 先等待输入缓冲区为空
            while (inb(0x64) & 0x02);

            // 再次发送 0xF4 命令
            outb(0x60, 0xF4);

            // 等待键盘返回
            while (!(inb(0x64) & 0x01)) {
                // ...
            }
            ack = inb(0x60);
            if (ack == 0xFA) {
                return 0; // 成功启用
            }
            // 如果仍是 0xFE 则继续重发，直到超过次数就放弃
        }

        // 多次重发仍失败，视为无法启用
        return -1;
    } 
    else {
        // 其它返回值，可能是错误或意外情况
        // 这里可以打印出来调试，看是具体什么值
        // printk("Keyboard enable got unexpected ack=0x%x\n", ack);

        return -1; 
    }
}

// 注册键盘中断
void keyboard_init() {
    register_interrupt_handler(33, keyboard_handler);

    // 先对 0x64 写 0xD4（告知下一条命令是发给键盘）
    while (inb(0x64) & 0x02);  // 等待输入缓冲区为空
    outb(0x64, 0xD4);

    // 再等待输入缓冲区为空
    while (inb(0x64) & 0x02);
    outb(0x60, 0xF4);  // 发启用扫描命令

    // 检查返回值
    if (try_enable_keyboard() == 0) {
        printk("Keyboard initialized.\n");
    } else {
        printk("Failed to enable keyboard scanning.\n");
    }
}

