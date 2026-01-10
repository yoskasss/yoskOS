ISO_NAME = yoskOS.iso
CFLAGS = -m32 -ffreestanding -nostdlib -Wall -Wextra -Iinclude

KERNEL_SRCS = kernel/kernel.c kernel/images.c apps/notepad.c apps/uptime.c apps/games/games.c libs/string.c libs/div64.c libs/console.c libs/info.c
KERNEL_OBJS = $(patsubst %.c,build/%.o,$(KERNEL_SRCS))

all: iso

build/kernel_entry.o: kernel/kernel_entry.asm
	mkdir -p build/kernel
	nasm -f elf kernel/kernel_entry.asm -o build/kernel_entry.o

build/%.o: %.c
	mkdir -p $(dir $@)
	i686-elf-gcc $(CFLAGS) -c $< -o $@

build/kernel.bin: build/kernel_entry.o $(KERNEL_OBJS) linker.ld
	i686-elf-ld -m elf_i386 -T linker.ld -o build/kernel.bin build/kernel_entry.o $(KERNEL_OBJS)

iso: build/kernel.bin grub/grub.cfg
	mkdir -p iso/boot/grub
	cp build/kernel.bin iso/boot/kernel.bin
	cp grub/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_NAME) iso

run: iso
	qemu-system-i386 -cdrom $(ISO_NAME)

clean:
	rm -rf build iso $(ISO_NAME)