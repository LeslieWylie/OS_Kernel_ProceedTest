[BITS 32]          ; 说明这是 32 位代码
section .multiboot
align 4
    dd 0x1BADB002  ; multiboot 魔数
    dd 0
    dd -(0x1BADB002)  ; 校验和

section .text
global _start
extern kernel_main

_start:
    call kernel_main
    hlt
