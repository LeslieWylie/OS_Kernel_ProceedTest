[BITS 32]
[GLOBAL idt_load]

idt_load:
    mov eax, [esp + 4]  ; 读取传递的 IDT 指针
    lidt [eax]          ; 加载 IDT
    ret                 ; 返回
