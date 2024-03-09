#version 460 core

in vec2 tex;

layout(location = 0) out vec4 blurOut;

layout(binding = 0) uniform sampler2D blurIn;

uniform float kernel[512];
uniform int kernelSize;
uniform float kernelOffset;
uniform int pass;
uniform float aspect;

void main() {
    vec2 offsetVec = vec2(0.0, kernelOffset);
    if (pass == 1) {
        offsetVec = vec2(kernelOffset / aspect, 0.0);
    }
    
    vec4 avg = vec4(0.0, 0.0, 0.0, 0.0);
    int kernelSize2 = kernelSize / 2;
    for (int i = 0; i < kernelSize; i++) {
        vec4 blurColor = texture(blurIn, tex + offsetVec * (i - kernelSize2));
        avg += blurColor * kernel[i];
    }
    blurOut = avg;
}