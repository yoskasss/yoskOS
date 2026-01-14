# yoskOS 26.01.15
<img width="320" height="320" alt="yoskOS" src="https://github.com/user-attachments/assets/201df4f8-89f5-4c2b-bda7-cf8b045e566b" />

A simple and educational i686 operating system project.

**Website:** https://yoskasss.github.io/yoskOS/

### File System Commands
- `ls` - Lists all files and folders in the current directory
- `cd <directory>` - Changes to the specified directory (use `cd ..` to go back)
- `pwd` - Displays the current directory path
- `cat <file>` - Shows the content of a file
- `write <file> <content>` - Creates or overwrites a file with specified content
- `mkdir <name>` - Creates a new directory

### Applications
- `gui` - Opens the graphical user interface menu
- `not <file>` - Opens the Notepad application (supports saving/loading files)
- `ascii <text>` - Displays text as ASCII art
- `show` - Image viewer for displaying system images
- `game` - Opens the game menu
### Process Management
- `ps` - Lists all running processes with their information
- `pinfo <pid>` - Shows detailed information about a specific process
- `pskill <pid>` - Terminates a process by its process ID

### Games
- `xox` - Tic-tac-toe game (play against the computer)
- `hangman` - Hangman word guessing game

### System Commands
- `help` - Displays the help menu with all available commands
- `clear` - Clears the screen
- `info` - Shows system information (supports options like `info -a`)
- `colorbg <color>` - Changes the background color
  - Available colors: black, blue, green, cyan, red, magenta, brown, lightgray, darkgray, lightblue, lightgreen, lightcyan, lightred, lightmagenta, yellow, white
- `screen` - Adjusts screen resolution
- `log` - Saves command history to a file
- `exit` - Safely shuts down the system
## Screenshots

<img width="758" height="467" alt="ss9" src="https://github.com/user-attachments/assets/e4214eec-81b0-4003-b81d-1f557eddcab6" />
<img width="758" height="467" alt="ss8" src="https://github.com/user-attachments/assets/f086c22c-d6bc-4fa4-833e-ef395a46f158" />
<img width="758" height="467" alt="ss7" src="https://github.com/user-attachments/assets/1e11c7a0-68d5-455a-a53f-71884e7847dc" />
<img width="758" height="467" alt="ss6" src="https://github.com/user-attachments/assets/35bd626f-772e-4274-a1e6-c7be537e29fd" />




V26.01.11

<img width="698" height="384" alt="image" src="https://github.com/user-attachments/assets/04870436-3c0c-4e6c-942f-e1579cde549b" />
<img width="698" height="384" alt="image" src="https://github.com/user-attachments/assets/b386dcc2-f796-4608-8360-a68619317d54" />
<img width="700" height="452" alt="image" src="https://github.com/user-attachments/assets/0a4c56e4-290d-439f-b23a-db0db4b3d122" />
<img width="685" height="448" alt="image" src="https://github.com/user-attachments/assets/731c2732-67ba-41b5-b67f-fa24a5cc43b1" />
<img width="700" height="452" alt="image" src="https://github.com/user-attachments/assets/a52dad6d-c2f3-481e-8b00-84391c9de0c2" />



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
├── apps/                    # Applications directory
│   ├── notepad.c            # Notepad application
│   ├── uptime.c             # Uptime application
│   └── games/               # Games
│       └── games.c          # Game source code
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
│   ├── process.h            # Process management headers
│   ├── string.h             # String headers
│   └── uptime.h             # Uptime headers
├── kernel/                  # Kernel source codes
│   ├── images.c             # Image processing
│   ├── kernel_entry.asm     # Kernel entry assembly
│   ├── kernel.c             # Main kernel code
│   └── process.c            # Process management implementation
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
