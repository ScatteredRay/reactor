#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "file.h"

#include "gl.h"

#pragma pack(push, 1)
struct bmp_header
{
    unsigned char magic[2];
    unsigned int file_size;
    unsigned int reserved;
    unsigned int data_offset;

    unsigned int header_size;
    unsigned int width;
    unsigned int height;
    short num_planes;
    short bitpp;
    int compression_type;
    unsigned int image_size;
    int hppm;
    int vppm;
    unsigned int num_colors;
    unsigned int num_important_colors;

    unsigned int red_bitmask;
    unsigned int green_bitmask;
    unsigned int blue_bitmask;
    unsigned int alpha_bitmask;
};
#pragma pack(pop)

struct bitmap
{
    int width;
    int height;
    void* data;
    void* to_free; // Cause it's perhaps allocated as part as a file, we use this pointer to free the data.
};

bitmap* load_bmp(const char* filename)
{
    char* buffer = load_entire_file(filename, "rb");

    if(!buffer)
        return NULL;

    bmp_header* bmp = (bmp_header*)buffer;

    assert(bmp->magic[0] == 'B' && bmp->magic[1] == 'M');
    assert(bmp->header_size == 40);
    assert(bmp->num_planes == 1);
    assert(bmp->bitpp == 32);
    assert(bmp->compression_type == 0);

    printf("Image Dimensions: %dx%d Size: %d\n", bmp->width, bmp->height, bmp->image_size);

    assert(bmp->width*bmp->height*4 <= bmp->image_size);

    int* data = (int*)(buffer + bmp->data_offset);

    for(int i=0; i<bmp->width*bmp->height; i++)
    {
        int tmp = (data[i]  << 8) & 0xFFFFFF00;
        tmp |= (data[i] >> 24) & 0x000000FF;
        data[i] = tmp;
    }

    bitmap* img = new bitmap();

    img->width = bmp->width;
    img->height = bmp->height;
    img->data = data;
    img->to_free = buffer;

    return img;
}

void destroy_bitmap(bitmap* img)
{
    delete [] (char*)img->to_free;
    delete img;
}

GLuint CreateTexture(bitmap* img)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 img->width,
                 img->height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_INT_8_8_8_8,
                 img->data);

    return tex;
}

GLuint CreateTextureFromBMP(const char* filename)
{
    bitmap* img = load_bmp(filename);

    GLuint tex = CreateTexture(img);

    destroy_bitmap(img);

    return tex;
}

void DestroyTexture(GLuint tex)
{
    glDeleteTextures(1, &tex);
}
