#ifndef IMAGES_H
#define IMAGES_H

// This header contains only type definitions and declarations.
// NO image .h files are included here.

typedef struct {
    const unsigned char* data;
    int width;
    int height;
} Image;

// External declarations to inform other files that these variables exist.
// The actual definitions are in images.c.
extern const Image available_images[];
extern const int num_available_images;

#endif // IMAGES_H