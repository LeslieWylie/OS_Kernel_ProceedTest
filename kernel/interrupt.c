#include "interrupt.h"
#include "io.h"
#include "printk.h"
#include "string.h"  

#define IDT_SIZE 256

typedef struct {
    unsigned short base_low;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) idt_ptr_t;

idt_entry_t idt[IDT_SIZE];
idt_ptr_t idtp;

// 载入 IDT
extern void idt_load(unsigned int);

void pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void exception_handler(int interrupt_number) {
    printk("\nException occurred! Interrupt: %d\n", interrupt_number);
    while(1);
}

void init_exceptions() {
    for (int i = 0; i < 32; i++) {
        register_interrupt_handler(i, exception_handler);
    }
}


// 注册中断
void register_interrupt_handler(int num, void (*handler)()) {
    unsigned int base = (unsigned int)handler;
    idt[num].base_low = base & 0xFFFF;
    idt[num].sel = 0x08;  // 代码段选择子
    idt[num].always0 = 0;
    idt[num].flags = 0x8E; // 中断门
    idt[num].base_high = (base >> 16) & 0xFFFF;

    // printk("IDT[%d] -> base: %x flags: %x\n", num, base, idt[num].flags);
}


void debug_print_idt() {
    for (int i = 0; i < 256; i++) {
        if (idt[i].base_low || idt[i].base_high) {
            printk("IDT[%d] -> base: %x%x flags: %x\n",
                   i, idt[i].base_high, idt[i].base_low, idt[i].flags);
        }
    }
}

// 初始化 IDT
void init_interrupts() {
    

    idtp.limit = (sizeof(idt_entry_t) * IDT_SIZE) - 1;
    idtp.base = (unsigned int)&idt;

    memset(&idt, 0, sizeof(idt_entry_t) * IDT_SIZE);

    init_exceptions(); // 处理 CPU 异常
    pic_remap();
    outb(0x21, 0xFD); // 仅启用键盘中断 (IRQ1)
    outb(0xA1, 0xFF); // 屏蔽从 PIC 所有中断


    idt_load((unsigned int)&idtp);

    // debug_print_idt();

    printk("IDT initialized.\n");
}
