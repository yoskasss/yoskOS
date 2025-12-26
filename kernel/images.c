#include "../include/images.h" // Image struct'ının tanımı için

// Resim verilerini içeren .h dosyalarını SADECE BURADA include ediyoruz
#include "../include/1.h"
#include "../include/2.h"
#include "../include/3.h"
#include "../include/help.h"
// Yeni resimler için: #include "3.h"

// Resimlerimizi tutan ana diziyi SADECE BURADA tanımlıyoruz.
const Image available_images[] = {
    { image_1_data, BITMAP_WIDTH_1, BITMAP_HEIGHT_1 },
    { image_2_data, BITMAP_WIDTH_2, BITMAP_HEIGHT_2 },
    { image_3_data, BITMAP_WIDTH_3, BITMAP_HEIGHT_3 },
    { image_help_data, BITMAP_WIDTH_help, BITMAP_HEIGHT_help },
    // Yeni resimler için buraya ekleyin
};

// Resim sayısını SADECE BURADA, dizinin tam olarak görüldüğü yerde hesaplıyoruz.
const int num_available_images = sizeof(available_images) / sizeof(Image);