#version 460 core

in vec2 tex;

layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D colorIn;
layout(binding = 1) uniform sampler2D blurIn;

void main() {
    vec4 blurInColor = texture(blurIn, tex);
    float a = blurInColor.a;
    if (a > 0.0)
        blurInColor.rgb /= a;
    a *= 2.0;
    if (a > 1.0)
        a = 1.0;
    vec4 colorInColor = texture(colorIn, tex);
    if (colorInColor.a > 0.0) {
        fragColor = (1.0 - a) * texture(colorIn, tex) + a * blurInColor;
    } else {
        fragColor = colorInColor;
    }
    fragColor.a = 1.0;
}