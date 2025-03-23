# x86_64上下文切换汇编实现

# 为了在C和汇编之间正确共享结构体布局，我们定义以下常量
# 根据context_t结构体的布局确定各寄存器偏移
# 这些值需要与context.h中的结构定义保持一致

# context_t成员偏移量
.set RAX_OFFSET,     0x00    # rax在context_t中的偏移
.set RBX_OFFSET,     0x08    # rbx在context_t中的偏移
.set RCX_OFFSET,     0x10    # rcx在context_t中的偏移
.set RDX_OFFSET,     0x18    # rdx在context_t中的偏移
.set RSI_OFFSET,     0x20    # rsi在context_t中的偏移
.set RDI_OFFSET,     0x28    # rdi在context_t中的偏移
.set RBP_OFFSET,     0x30    # rbp在context_t中的偏移
.set RSP_OFFSET,     0x38    # rsp在context_t中的偏移
.set R8_OFFSET,      0x40    # r8在context_t中的偏移
.set R9_OFFSET,      0x48    # r9在context_t中的偏移
.set R10_OFFSET,     0x50    # r10在context_t中的偏移
.set R11_OFFSET,     0x58    # r11在context_t中的偏移
.set R12_OFFSET,     0x60    # r12在context_t中的偏移
.set R13_OFFSET,     0x68    # r13在context_t中的偏移
.set R14_OFFSET,     0x70    # r14在context_t中的偏移
.set R15_OFFSET,     0x78    # r15在context_t中的偏移
.set CS_OFFSET,      0x80    # cs在context_t中的偏移
.set DS_OFFSET,      0x82    # ds在context_t中的偏移
.set ES_OFFSET,      0x84    # es在context_t中的偏移
.set FS_OFFSET,      0x86    # fs在context_t中的偏移
.set GS_OFFSET,      0x88    # gs在context_t中的偏移
.set SS_OFFSET,      0x8A    # ss在context_t中的偏移
.set RIP_OFFSET,     0x90    # rip在context_t中的偏移
.set RFLAGS_OFFSET,  0x98    # rflags在context_t中的偏移

# struct process中context字段的偏移（根据实际结构体布局调整）
.set CONTEXT_OFFSET, 0x20    # 假设context在struct process中的偏移是32字节

.section .text
.global __switch_to

# void __switch_to(struct process *prev, struct process *next)
# 寄存器约定：
# %rdi - prev (第一个参数)
# %rsi - next (第二个参数)
__switch_to:
    # 计算prev->context和next->context的地址
    addq $CONTEXT_OFFSET, %rdi
    addq $CONTEXT_OFFSET, %rsi
    
    # 保存当前上下文到prev->context
    movq %rax, RAX_OFFSET(%rdi)
    movq %rbx, RBX_OFFSET(%rdi)
    movq %rcx, RCX_OFFSET(%rdi)
    movq %rdx, RDX_OFFSET(%rdi)
    # %rsi和%rdi在这里已经被修改过了，不能直接保存
    # 我们需要借助于栈来保存它们的原始值
    movq %rsi, RSI_OFFSET(%rdi)
    movq %rdi, RDI_OFFSET(%rdi)
    movq %rbp, RBP_OFFSET(%rdi)
    
    # 获取并保存原来的rsp（进入函数前的）
    leaq 8(%rsp), %rax          # 返回地址后面的第一个栈位置，即调用前的rsp
    movq %rax, RSP_OFFSET(%rdi)
    
    movq %r8,  R8_OFFSET(%rdi)
    movq %r9,  R9_OFFSET(%rdi)
    movq %r10, R10_OFFSET(%rdi)
    movq %r11, R11_OFFSET(%rdi)
    movq %r12, R12_OFFSET(%rdi)
    movq %r13, R13_OFFSET(%rdi)
    movq %r14, R14_OFFSET(%rdi)
    movq %r15, R15_OFFSET(%rdi)
    
    # 保存返回地址到rip
    movq (%rsp), %rax           # 获取返回地址
    movq %rax, RIP_OFFSET(%rdi)

    # 获取并保存标志寄存器
    pushfq
    popq %rax
    movq %rax, RFLAGS_OFFSET(%rdi)
    
    # 从next->context恢复寄存器
    # 先用r11保存next->context的地址，避免丢失
    movq %rsi, %r11

    # 恢复通用寄存器
    movq RAX_OFFSET(%r11), %rax
    movq RBX_OFFSET(%r11), %rbx
    movq RCX_OFFSET(%r11), %rcx
    movq RDX_OFFSET(%r11), %rdx
    movq RBP_OFFSET(%r11), %rbp
    movq R8_OFFSET(%r11),  %r8
    movq R9_OFFSET(%r11),  %r9
    movq R10_OFFSET(%r11), %r10
    # r11保留到最后恢复
    movq R12_OFFSET(%r11), %r12
    movq R13_OFFSET(%r11), %r13
    movq R14_OFFSET(%r11), %r14
    movq R15_OFFSET(%r11), %r15
    
    # 恢复标志寄存器
    movq RFLAGS_OFFSET(%r11), %rax
    pushq %rax
    popfq
    
    # 恢复栈指针
    movq RSP_OFFSET(%r11), %rsp
    
    # 恢复rsi和rdi
    movq RSI_OFFSET(%r11), %rsi
    movq RDI_OFFSET(%r11), %rdi
    
    # 恢复r11
    movq R11_OFFSET(%r11), %r11
    
    # 直接ret，让函数返回到保存的返回地址
    ret 