#ifndef IMAGES_H
#define IMAGES_H

typedef struct {
    const unsigned char* data;
    int width;
    int height;
} Image;

extern const Image available_images[];
extern const int num_available_images;

#endif