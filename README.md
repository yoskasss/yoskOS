# yoskOS

<img width="320" height="320" alt="yoskOS" src="https://github.com/user-attachments/assets/201df4f8-89f5-4c2b-bda7-cf8b045e566b" />

A simple and educational i686 operating system project.

**Website:** [https://yoskasss.github.io/yoskOS/](https://yoskasss.github.io/yoskOS/)

**QEMU Installation Video:**
[https://shitmyweb.free.nf/view.php?id=694f0792a6e99](https://shitmyweb.free.nf/view.php?id=694f0792a6e99)

---

## Features

* Terminal shell
* Simple file system (`fs.c`)
* Notepad application
* Uptime application
* Tic-Tac-Toe (XOX) and Hangman games
* Bitmap image rendering

---

## Commands

### File System Commands

* `ls` – List directory contents
* `cd <directory>` – Change directory (use `cd ..` to go up)
* `pwd` – Show the current path
* `mkdir <name>` – Create a new directory
* `write <file> <content>` – Create a file and write content to it
* `cat <file>` – Display the contents of a file

### Applications

* `not` – Open the Notepad application
* `game` – Open the game menu
* `show` – Open the image viewer
* `ascii <text>` – Display text as ASCII art

### System Commands

* `info` – Display system information (supports flags like `info -a`)
* `screen` – Adjust screen resolution settings
* `colorbg <color>` – Change background color (e.g., black, blue, green, etc.)
* `log` – Save command history to a file
* `clear` – Clear the screen
* `help` – Show the help menu
* `exit` – Shut down the system

---


## Screenshots

<img width="731" height="431" alt="image" src="https://github.com/user-attachments/assets/28b105e6-f115-4ce4-aef6-1200bfbac575" />
<img width="719" height="451" alt="image" src="https://github.com/user-attachments/assets/6f89103e-8485-4e93-aaab-8e271907811c" />
<img width="685" height="448" alt="image" src="https://github.com/user-attachments/assets/35c0cae2-5088-485b-aa09-ee89d86f3193" />
<img width="715" height="399" alt="image" src="https://github.com/user-attachments/assets/4615ed98-9567-492b-a874-5c7cfdfab1f5" />
<img width="715" height="399" alt="image" src="https://github.com/user-attachments/assets/13bd99d4-3288-4149-83ba-f26e42b00700" />

---

## Requirements

* NASM
* i686-elf-gcc (cross-compiler)
* Make
* GRUB
* QEMU (for testing)

---

## Build

```bash
make
```

---

## Run

```bash
qemu-system-i386 -cdrom yoskOS.iso -m 512
```

---

## Project Structure

* `kernel/` – Kernel source code
* `apps/` – Applications
* `include/` – Header files
* `libs/` – Libraries
* `build/` – Build outputs
* `iso/` – ISO files

---

## Adding Images

### Creating a Header with image.py

The `image.py` script converts an image file into a C header file.

```bash
python3 image.py
```

* Select an image (PNG, JPG, BMP).
* Save the header file (e.g. `include/new_image.h`).

### Adding an Image to the `show` Command

1. Place the generated header file into the `include/` directory.
2. Add an `#include` in `kernel/images.c` and append the new image to the `available_images` array:

   ```c
   #include "../include/new_image.h"

   const Image available_images[] = {
       // ... existing images
       { new_image_data, NEW_IMAGE_WIDTH, NEW_IMAGE_HEIGHT },
   };
   ```
3. `num_available_images` is updated automatically.
4. Rebuild the project:

   ```bash
   make
   ```

---

## License

MIT License (see the LICENSE file)

---

Good luck! 🚀
