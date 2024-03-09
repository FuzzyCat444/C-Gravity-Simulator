#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTex;

out vec2 tex;

void main() {
    gl_Position = vec4(vPos, 1.0);
    tex = vTex;
}