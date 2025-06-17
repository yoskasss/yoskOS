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

## Görüntüler
![resim](https://github.com/user-attachments/assets/6b20f29e-29a1-4c92-9193-384e1bb3881b)


https://github.com/user-attachments/assets/c1e9e650-f2da-4081-999e-a3d26864a9c4



## ISO Dosyası
[Doğrudan İndirme linki](https://github.com/yoskasss/yoskOS/releases/download/ISO/yoskOS.iso)



[İndirme Sayfası](https://github.com/yoskasss/yoskOS/releases/download/ISO/yoskOS.iso)

## Lisans

Bu proje açık kaynaklıdır ve MIT Lisansı altında kullanılabilir. 
