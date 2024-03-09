#version 460 core

in vec2 tex;

out vec4 fragColor;

layout(binding = 0) uniform sampler2D uTexture;

void main() {
    fragColor = texture(uTexture, tex);
}