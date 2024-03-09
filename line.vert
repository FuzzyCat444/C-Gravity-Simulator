#version 460 core

layout(location = 0) in vec3 vPos;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(vPos, 1.0);
    gl_Position.xyw *= 1.0001;
}