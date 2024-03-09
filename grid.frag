#version 460 core

in float alpha;

out vec4 fragColor;

void main() {
    fragColor = vec4(1.0, 1.0, 1.0, alpha);
}