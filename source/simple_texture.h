#ifndef _SIMPLE_TEXTURE_H_
#define _SIMPLE_TEXTURE_H_

typedef unsigned int GLuint;
typedef unsigned int GLenum;
struct bitmap;

bitmap* load_bmp(const char* filename);
void destroy_bitmap(bitmap* img);
unsigned int bitmap_width(bitmap* bmp);
unsigned int bitmap_height(bitmap* bmp);

GLuint CreateTexture(unsigned int width, unsigned int height, void* data, GLenum format, GLenum type);
GLuint CreateTexture(bitmap* img);
GLuint CreateTextureFromBMP(const char* filename);
void DestroyTexture(GLuint tex);

#endif //_SIMPLE_TEXTURE_H_
