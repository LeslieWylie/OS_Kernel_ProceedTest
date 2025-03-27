/**
 * init.c - 内核初始化
 */

#include "../../include/kernel.h"
#include "../../include/printk.h"
#include "../../include/keyboard.h"
#include "../../include/shell.h"
#include "../../include/interrupt.h"
#include "../../include/io.h"

/**
 * 初始化PIC中断控制器
 */
void init_pic(void)
{
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

/**
 * 内核主函数 - 系统入口点
 */
void kernel_main(void)
{
    printk("Initializing system...\n");

    printk("GOOD MANY TIMES +==============================================\n");
    printk("PhyOS is the BEST OS!!!\n");

    // TODO: 初始化进程管理
    // process_init();

    // 初始化中断系统
    init_interrupts();

    // 初始化键盘
    keyboard_init();

    // TODO: 初始化异常处理
    // init_exceptions();

    // TODO: 初始化文件系统
    // vfs_init();

    // 启用中断
    __asm__ volatile("sti");

    printk("GOOD MANY TIMES +==============================================\n");
    printk("System ready. Type a command:\n");

    // 启动shell
    shell();
}