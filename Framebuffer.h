#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/gl.h>

#include "Texture.h"

typedef struct Framebuffer {
    GLuint fbo;
    int colorBuffersSize;
    Texture colorBuffers[8];
    Texture depthBuffer;
} Framebuffer;

void Framebuffer_create(Framebuffer* framebuffer, unsigned int width, unsigned int height, int colorBufferCount);

void Framebuffer_destroy(Framebuffer* framebuffer);

void Framebuffer_use(Framebuffer* framebuffer, int colorBuffers[], int colorBuffersCount);

void Framebuffer_unuse();

#endif