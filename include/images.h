#ifndef IMAGES_H
#define IMAGES_H

// Sadece veri tiplerini ve bildirimleri içerir.
// HİÇBİR .h dosyası burada include EDİLMEZ.

typedef struct {
    const unsigned char* data;
    int width;
    int height;
} Image;

// Diğer dosyalara bu değişkenlerin var olduğunu bildiriyoruz (extern).
// Değişkenlerin kendisi images.c içinde tanımlı.
extern const Image available_images[];
extern const int num_available_images;

#endif // IMAGES_H