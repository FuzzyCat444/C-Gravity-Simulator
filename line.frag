#version 460 core

out vec4 fragColor;

in float alpha;

uniform vec4 color;

void main() {
    fragColor = color;
}