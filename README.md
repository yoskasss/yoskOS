# yoskOS

yoskOS, basit ve eğitici bir işletim sistemi uygulaması sağlamayı amaçlayan özel bir işletim sistemi projesidir.

## Proje Yapısı

- `apps/` - Kullanıcı uygulamaları
- `build/` - Derleme çıktı dizini
- `grub/` - GRUB önyükleyici yapılandırması
- `include/` - Başlık dosyaları
- `iso/` - ISO görüntüsü ile ilgili dosyalar
- `kernel/` - Çekirdek kaynak kodu
- `libs/` - Kütüphane dosyaları

## Derleme

yoskOS'u derlemek için ihtiyacınız olanlar:
- NASM (Netwide Assembler)
- GCC çapraz derleyici
- Make
- GRUB

Derlemek için aşağıdaki komutu çalıştırın:
```bash
make
```

## Çalıştırma

Derleme tamamlandıktan sonra, yoskOS'u QEMU kullanarak çalıştırabilirsiniz:
```bash
qemu-system-i386 -cdrom yoskOS.iso
```

## Lisans

Bu proje açık kaynaklıdır ve MIT Lisansı altında kullanılabilir. 