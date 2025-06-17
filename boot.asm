[BITS 16]
[ORG 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; Kernel'i 0x1000 adresine yükle (GRUB bu işi yapacak)
    jmp 0x0000:0x7E00

times 510 - ($ - $$) db 0
dw 0xAA55
