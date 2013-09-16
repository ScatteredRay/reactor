#ifndef _SIMPLE_TEXTURE_H_
#define _SIMPLE_TEXTURE_H_

struct bitmap;

bitmap* load_bmp(const char* filename);
void destroy_bitmap(bitmap* img);
unsigned int bitmap_width(bitmap* bmp);
unsigned int bitmap_height(bitmap* bmp);

GLuint CreateTexture(bitmap* img);
GLuint CreateTextureFromBMP(const char* filename);
void DestroyTexture(GLuint tex);

#endif //_SIMPLE_TEXTURE_H_
