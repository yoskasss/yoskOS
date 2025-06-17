[bits 32]
section .multiboot
    align 4
    dd 0x1BADB002       ; magic number
    dd 0x0              ; flags
    dd -(0x1BADB002)    ; checksum

section .text
    extern kernel_main      ; <-- eksik olan buydu
    global _start

_start:
    call kernel_main
    cli
.hang:
    hlt
    jmp .hang
