#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>

#include <linmath.h>

typedef GLint ShaderLoc;

typedef struct Shader {
    GLuint programID;
} Shader;

int Shader_create(Shader* shader, const char* vertexFile, const char* fragmentFile);

void Shader_destroy(Shader* shader);

void Shader_use(const Shader* shader);

ShaderLoc Shader_getLocation(const Shader* shader, const char* name);

void Shader_sendFloat(Shader* shader, ShaderLoc location, float floatValue);

void Shader_sendFloats(Shader* shader, ShaderLoc location, float floatValues[], int numFloats);

void Shader_sendVec3(Shader* shader, ShaderLoc location, float x, float y, float z);

void Shader_sendVec4(Shader* shader, ShaderLoc location, float x, float y, float z, float w);

void Shader_sendInt(Shader* shader, ShaderLoc location, int intValue);

void Shader_sendMat4(Shader* shader, ShaderLoc location, mat4x4 matrix);

#endif