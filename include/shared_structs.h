#ifndef SHARED_STRUCTS_H
#define SHARED_STRUCTS_H

#define MAX_IMAGE_SIZE 1024 * 1024

struct ImageMetadata {
    int height;
    int width;
    int channels;
};

struct ProcessedImage {
    struct ImageMetadata metadata;
    unsigned char image_data[MAX_IMAGE_SIZE];
};

#endif // SHARED_STRUCTS_H