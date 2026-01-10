# yoskOS 26.01.11
<img width="320" height="320" alt="yoskOS" src="https://github.com/user-attachments/assets/201df4f8-89f5-4c2b-bda7-cf8b045e566b" />

A simple and educational i686 operating system project.

**Website:** https://yoskasss.github.io/yoskOS/


## Features in V 26.01.11
- **Change background colour:** Changing the background colour with the `colorbg` command.
- **Updated info:** You can now obtain more information using the `info` command.
- **A more appealing help screen**
- **More organised and modular source code**

## Commands
- `help` - Command list
- `clear` - Clear screen
- `ls` - File list
- `cd <directory>` - Change directory
- `pwd` - Current directory
- `cat <file>` - File content
- `write <file> <content>` - Write file
- `mkdir <name>` - Create folder
- `not <file>` - Open Notepad
- `ascii <text>` - ASCII art
- `show` - Image display
- `game` - Game menu
- `xox` - Tic-tac-toe game
- `hangman` - Hangman game
- `info` - System information
- `colorbg <color>` - Change background color
- `exit` - Safe exit
- `log` - Get system logs
- `screen` - Screen resizing
## Screenshots
<img width="719" height="410" alt="image" src="https://github.com/user-attachments/assets/77f2f6b2-ff1f-4f0d-bbff-f4c7d2dfaf99" />
<img width="685" height="448" alt="image" src="https://github.com/user-attachments/assets/731c2732-67ba-41b5-b67f-fa24a5cc43b1" />
<img width="685" height="448" alt="image" src="https://github.com/user-attachments/assets/35c0cae2-5088-485b-aa09-ee89d86f3193" />
<img width="685" height="448" alt="image" src="https://github.com/user-attachments/assets/89236692-f44b-488b-9d3e-40d10a257b28" />

## Requirements
- NASM
- i686-elf-gcc (cross-compiler)
- Make
- GRUB
- QEMU (for testing)

## Building
```bash
make
```

## Running
```bash
qemu-system-i386 -cdrom yoskOS.iso -m 512
```

## Project Structure
```
yoskOS/
├── boot.asm                 # Bootloader assembly code
├── fs.c                     # File system source code
├── fs.h                     # File system header file
├── image.py                 # Image conversion script
├── index.html               # Website
├── LICENSE                  # License file
├── linker.ld                # Linker script
├── Makefile                 # Build file
├── readme.md                # This file
├── rm                       # Removal script
├── apps/                    # Applications directory
│   ├── notepad.c            # Notepad application
│   ├── uptime.c             # Uptime application
│   └── games/               # Games
│       └── games.c          # Game source code
├── Görüntüler/              # Images directory
├── grub/                    # GRUB configuration
│   └── grub.cfg             # GRUB configuration file
├── include/                 # Header files
│   ├── 1.h                  # Image headers
│   ├── 2.h
│   ├── 3.h
│   ├── console.h            # Console header file
│   ├── games.h              # Game headers
│   ├── help.h               # Help headers
│   ├── images.h             # Image headers
│   ├── info.h               # System info headers
│   ├── kernel.h             # Kernel headers
│   ├── notepad.h            # Notepad headers
│   ├── string.h             # String headers
│   └── uptime.h             # Uptime headers
├── kernel/                  # Kernel source codes
│   ├── images.c             # Image processing
│   ├── kernel_entry.asm     # Kernel entry assembly
│   └── kernel.c             # Main kernel code
└── libs/                    # Library source codes
    ├── console.c            # Console library
    ├── div64.c              # 64-bit division
    ├── info.c               # System information
    └── string.c             # String operations
```
## Adding Images
### Creating Header with image.py
The `image.py` script converts an image file to a C header file.
```bash
python3 image.py
```
- Select an image (PNG, JPG, BMP).
- Save the header file (e.g. `include/new.h`).

### Adding Image to Show Command
1. Place the created header in the `include/` directory.
2. In `kernel/images.c`, add `#include` and add the new image to the `available_images` array:
   ```c
   #include "../include/new.h"
   const Image available_images[] = {
       // ... existing images
       { new_image_data, NEW_IMAGE_WIDTH, NEW_IMAGE_HEIGHT },
   };
   ```
3. `num_available_images` updates automatically.
4. Build: `make`
## License
MIT License (see LICENSE file)

Good luck!
