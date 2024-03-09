#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/gl.h>

typedef struct Texture {
    unsigned int width;
    unsigned int height;
    GLuint textureID;
} Texture;

int Texture_create(Texture* texture, const char* texturePath);

void Texture_createColor(Texture* texture, unsigned int width, unsigned int height);

void Texture_createDepth(Texture* texture, unsigned int width, unsigned int height);

void Texture_destroy(Texture* texture);

void Texture_use(Texture* texture, unsigned int unit);

#endif