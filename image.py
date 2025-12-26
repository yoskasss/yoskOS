# image_to_header.py
from PIL import Image
import os
from tkinter import filedialog, Tk

def image_to_header(image_path, output_path):
    img = Image.open(image_path).convert("RGB")
    width, height = img.size
    pixels = img.load()

    base_name = os.path.splitext(os.path.basename(output_path))[0]
    c_prefix = f"image_{base_name}"

    hex_values = []

    for y in range(height):
        for x in range(width):
            r, g, b = pixels[x, y]
            hex_values.extend([
                f"0x{r:02x}",
                f"0x{g:02x}",
                f"0x{b:02x}"
            ])

    array_content = ",\n  ".join(
        ", ".join(hex_values[i:i+12])
        for i in range(0, len(hex_values), 12)
    )

    header_content = f"""
// Resim verisi: {base_name}
static const unsigned char {c_prefix}_data[] = {{
  {array_content}
}};

static const unsigned int {c_prefix}_len = {len(hex_values)};

#define BITMAP_WIDTH_{base_name} {width}
#define BITMAP_HEIGHT_{base_name} {height}
"""

    with open(output_path, "w") as f:
        f.write(header_content)

    print(f"✔ {output_path} oluşturuldu")

def main():
    root = Tk()
    root.withdraw()

    image_path = filedialog.askopenfilename(
        title="Görsel Seç",
        filetypes=[("Images", "*.png *.jpg *.jpeg *.bmp")]
    )
    if not image_path:
        return

    output_path = filedialog.asksaveasfilename(
        title="Header Kaydet",
        defaultextension=".h",
        filetypes=[("C Header", "*.h")]
    )
    if not output_path:
        return

    image_to_header(image_path, output_path)

if __name__ == "__main__":
    main()
