[bits 32]

section .multiboot
    align 4
    ; Multiboot header
    magic equ 0x1BADB002
    flags equ 0
    checksum equ -(magic + flags)
    
    dd magic
    dd flags
    dd checksum

section .text
    extern kernel_main
    global _start

_start:
    ; Set up kernel stack
    mov esp, stack_top
    
    ; Push Multiboot info as argument
    push ebx
    push eax
    
    ; Call kernel_main
    call kernel_main
    
    ; Halt if kernel returns
    cli
.hang:
    hlt
    jmp .hang

section .bss
    align 16
    stack_bottom:
    resb 16384
    stack_top:
