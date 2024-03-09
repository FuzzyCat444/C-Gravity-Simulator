#ifndef SSBO_H
#define SSBO_H

#include <glad/gl.h>

typedef struct SSBO {
    GLuint ssbo;
} SSBO;

void SSBO_create(SSBO* ssbo);

void SSBO_destroy(SSBO* ssbo);

void SSBO_sendData(SSBO* ssbo, void* data, int dataSize);

void SSBO_bindBase(SSBO* ssbo, int binding);

#endif