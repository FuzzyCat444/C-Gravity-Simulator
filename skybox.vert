#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTex;

out vec2 tex;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(vPos, 1.0);
    tex = vTex;
}