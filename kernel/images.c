#include "../include/images.h" // For the Image struct definition

// We include the .h files containing image data ONLY HERE
#include "../include/1.h"
#include "../include/2.h"
#include "../include/3.h"
#include "../include/help.h"
// For new images: #include "new_image.h"

// We define the main array holding all images ONLY HERE
const Image available_images[] = {
    { image_1_data, BITMAP_WIDTH_1, BITMAP_HEIGHT_1 },
    { image_2_data, BITMAP_WIDTH_2, BITMAP_HEIGHT_2 },
    { image_3_data, BITMAP_WIDTH_3, BITMAP_HEIGHT_3 },
    { image_help_data, BITMAP_WIDTH_help, BITMAP_HEIGHT_help },
    // Add new images here
};

// We calculate the number of images ONLY HERE, where the full array is visible
const int num_available_images = sizeof(available_images) / sizeof(Image);