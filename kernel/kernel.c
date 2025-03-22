#include "kernel.h"
#include "printk.h"
#include "keyboard.h"
#include "shell.h"
#include "interrupt.h"
#include "io.h"

void init_pic() {
    outb(0x20, 0x11); // 初始化主PIC
    outb(0xA0, 0x11); // 初始化从PIC
    outb(0x21, 0x20); // 主PIC IRQ 基地址 0x20
    outb(0xA1, 0x28); // 从PIC IRQ 基地址 0x28
    outb(0x21, 0x04); // 设置主从 PIC 连接
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void kernel_main() {
   printk("Initializing system...\n");

    printk("GOOD MANY TIMES +==============================================\n"); printk("PhyOS is the BEST OS!!!\n");
    
    // // 初始化进程管理
    // init_process();


    // // 初始化中断系统
    init_interrupts();

    // // 初始化键盘
    keyboard_init();

    // // 初始化异常
    init_exceptions();

    // // 初始化文件系统


    // __asm__ volatile ("cli"); // 禁用中断
    // outb(0x21, 0xFF);  // 屏蔽所有中断
    // outb(0xA1, 0xFF);  // 屏蔽所有中断

    // init_interrupts();  // 确保 IDT 已经初始化
    // init_pic();  // 初始化 PIC
    __asm__ volatile ("sti"); // 启用中断

    

    // while(1) {
    //     printk("...wahha\n");
    //     for (volatile int i = 0; i < 1000000000000; i++);
    // }


    printk("GOOD MANY TIMES +==============================================\n");

    printk("System ready. Type a command:\n");

    // while (1) {
    //     __asm__ volatile ("hlt");
    // }
    shell();

    
}
