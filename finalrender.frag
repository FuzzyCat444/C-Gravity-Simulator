#version 460 core

in vec2 tex;

out vec4 fragColor;

layout(binding = 0) uniform sampler2D uColor;

void main() {
    fragColor = texture(uColor, tex);
}