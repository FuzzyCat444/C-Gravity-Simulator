#include "Framebuffer.h"

void Framebuffer_create(Framebuffer* framebuffer, unsigned int width, unsigned int height, int colorBufferCount) {
    glGenFramebuffers(1, &framebuffer->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);
    
    if (colorBufferCount < 1)
        colorBufferCount = 1;
    if (colorBufferCount > 8)
        colorBufferCount = 8;
    framebuffer->colorBuffersSize = colorBufferCount;
    for (int i = 0; i < colorBufferCount; i++) {
        Texture_createColor(&framebuffer->colorBuffers[i], width, height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, framebuffer->colorBuffers[i].textureID, 0);
    }
    Texture_createDepth(&framebuffer->depthBuffer, width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer->depthBuffer.textureID, 0);
}

void Framebuffer_destroy(Framebuffer* framebuffer) {
    if (framebuffer == NULL)
        return;
    
    glDeleteFramebuffers(1, &framebuffer->fbo);
    for (int i = 0; i < framebuffer->colorBuffersSize; i++) {
        Texture_destroy(&framebuffer->colorBuffers[i]);
    }
    Texture_destroy(&framebuffer->depthBuffer);
}

void Framebuffer_use(Framebuffer* framebuffer, int colorBuffers[], int colorBuffersCount) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);
    
    GLenum colorAttachments[8];
    for (int i = 0; i < colorBuffersCount; i++) {
        colorAttachments[i] = GL_COLOR_ATTACHMENT0 + colorBuffers[i];
    }
    glDrawBuffers(colorBuffersCount, &colorAttachments[0]);
}

void Framebuffer_unuse() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}