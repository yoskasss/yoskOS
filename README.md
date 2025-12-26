# yoskOS

Basit ve eğitici bir i686 işletim sistemi projesi.

## Özellikler
- Terminal kabuğu
- Basit dosya sistemi (fs.c)
- Notepad uygulaması
- Uptime uygulaması
- XOX ve Hangman oyunları
- Bitmap görüntü gösterimi

## Komutlar
- `help` - Komut listesi
- `clear` - Ekran temizleme
- `ls` - Dosya listesi
- `cd <dizin>` - Dizin değiştirme
- `pwd` - Mevcut dizin
- `cat <dosya>` - Dosya içeriği
- `write <dosya> <içerik>` - Dosya yazma
- `mkdir <isim>` - Klasör oluşturma
- `not <dosya>` - Notepad açma
- `ascii <metin>` - ASCII art
- `show` - Görüntü gösterimi
- `game` - Oyun menüsü
- `xox` - XOX oyunu
- `hangman` - Adam asmaca oyunu
## Görseller
<img width="719" height="410" alt="image" src="https://github.com/user-attachments/assets/77f2f6b2-ff1f-4f0d-bbff-f4c7d2dfaf99" />
<img width="685" height="448" alt="image" src="https://github.com/user-attachments/assets/731c2732-67ba-41b5-b67f-fa24a5cc43b1" />
<img width="685" height="448" alt="image" src="https://github.com/user-attachments/assets/35c0cae2-5088-485b-aa09-ee89d86f3193" />
<img width="685" height="448" alt="image" src="https://github.com/user-attachments/assets/89236692-f44b-488b-9d3e-40d10a257b28" />

## Gereksinimler
- NASM
- i686-elf-gcc (cross-compiler)
- Make
- GRUB
- QEMU (test için)

## Derleme
```bash
make
```

## Çalıştırma
```bash
qemu-system-i386 -cdrom yoskOS.iso -m 512
```

## Proje Yapısı
- `kernel/` - Çekirdek kodu
- `apps/` - Uygulamalar
- `include/` - Başlık dosyaları
- `libs/` - Kütüphaneler
- `build/` - Derleme çıktıları
- `iso/` - ISO dosyaları
## Görsel Ekleme
### image.py ile Header Oluşturma
`image.py` scripti, bir görsel dosyasını C header dosyasına dönüştürür.
```bash
python3 image.py
```
- Görsel seçin (PNG, JPG, BMP).
- Header dosyasını kaydedin (örn. `include/yeni.h`).

### Show Komutuna Görsel Ekleme
1. Oluşturulan header'ı `include/` dizinine koyun.
2. `kernel/images.c`'de `#include` ekleyin ve `available_images` dizisine yeni resmi ekleyin:
   ```c
   #include "../include/yeni.h"
   const Image available_images[] = {
       // ... mevcut resimler
       { yeni_image_data, YENI_IMAGE_WIDTH, YENI_IMAGE_HEIGHT },
   };
   ```
3. `num_available_images` otomatik güncellenir.
4. Derleyin: `make`
## Lisans
MIT License (LICENSE dosyasına bakın)

İyi çalışmalar!
