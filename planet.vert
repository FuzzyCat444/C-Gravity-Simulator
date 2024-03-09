#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNorm;

out vec3 pos;
out vec3 norm;

uniform mat4 mvp;
uniform mat4 mv;

void main() {
    gl_Position = mvp * vec4(vPos, 1.0);
    pos = (mv * vec4(vPos, 1.0)).xyz;
    norm = vNorm;
}