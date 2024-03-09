#include "SSBO.h"

void SSBO_create(SSBO* ssbo) {
    glGenBuffers(1, &ssbo->ssbo);
}

void SSBO_destroy(SSBO* ssbo) {
    glDeleteBuffers(1, &ssbo->ssbo);
}

void SSBO_sendData(SSBO* ssbo, void* data, int dataSize) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize, data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO_bindBase(SSBO* ssbo, int binding) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo->ssbo);
}