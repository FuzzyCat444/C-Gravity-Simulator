#include "Shader.h"

#include <stdio.h>
#include <stdlib.h>

int Shader_create(Shader* shader, const char* vertexPath, const char* fragmentPath) {
    char* vertexStr = 0;
    long vertexStrLen = 0;
    FILE* vertexFile = fopen(vertexPath, "rb");
    if (vertexFile == NULL) {
        printf("Could not open vertex shader %s.\n", vertexPath);
    }
    
    fseek(vertexFile, 0, SEEK_END);
    vertexStrLen = ftell(vertexFile);
    fseek(vertexFile, 0, SEEK_SET);
    vertexStr = (char*) malloc(vertexStrLen + 1);
    if (vertexStr == NULL) {
        fclose(vertexFile);
        return 1;
    }
    fread(vertexStr, 1, vertexStrLen, vertexFile);
    vertexStr[vertexStrLen] = 0;
    
    fclose(vertexFile);
    
    char* fragmentStr = 0;
    long fragmentStrLen = 0;
    FILE* fragmentFile = fopen(fragmentPath, "rb");
    if (fragmentFile == NULL) {
        free(vertexStr);
        printf("Could not open fragment shader %s.\n", fragmentPath);
        return 1;
    }
    
    fseek(fragmentFile, 0, SEEK_END);
    fragmentStrLen = ftell(fragmentFile);
    fseek(fragmentFile, 0, SEEK_SET);
    fragmentStr = (char*) malloc(fragmentStrLen + 1);
    if (fragmentStr == NULL) {
        free(vertexStr);
        fclose(fragmentFile);
        return 1;
    }
    fread(fragmentStr, 1, fragmentStrLen, fragmentFile);
    fragmentStr[fragmentStrLen] = 0;
    
    fclose(fragmentFile);
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSource = vertexStr;
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    GLint vertexShaderCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompiled);
    if (vertexShaderCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        
        char* errorLog = (char*) malloc(maxLength * sizeof(char));
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, errorLog);
        
        printf("Vertex shader (%s) compile error:\n%s", vertexPath, errorLog);
        free(errorLog);
        
        free(vertexStr);
        free(fragmentStr);
        glDeleteShader(vertexShader);
        return 1;
    }
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSource = fragmentStr;
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    GLint fragmentShaderCompiled = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompiled);
    if (fragmentShaderCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        
        char* errorLog = (char*) malloc(maxLength * sizeof(char));
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, errorLog);
        
        printf("Fragment shader (%s) compile error:\n%s", fragmentPath, errorLog);
        free(errorLog);
        
        free(vertexStr);
        free(fragmentStr);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 1;
    }
    
    free(vertexStr);
    free(fragmentStr);
    
    GLuint program = glCreateProgram();
    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    
    glLinkProgram(program);
    
    GLint programLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &programLinked);
    if (programLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        
        char* errorLog = (char*) malloc(maxLength * sizeof(char));
        glGetProgramInfoLog(program, maxLength, &maxLength, errorLog);
        
        printf("Program (%s, %s) link error:\n%s", vertexPath, fragmentPath, errorLog);
        free(errorLog);
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        return 1;
    }
    
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
 
    shader->programID = program;
 
    return 0;
}

void Shader_destroy(Shader* shader) {
    glDeleteProgram(shader->programID);
}

static GLuint currentShader;

void Shader_use(const Shader* shader) {
    GLuint programID = shader->programID;
    if (currentShader != programID) {
        glUseProgram(programID);
        currentShader = programID;
    }
}

ShaderLoc Shader_getLocation(const Shader* shader, const char* name) {
    return glGetUniformLocation(shader->programID, name);
}

void Shader_sendFloat(Shader* shader, ShaderLoc location, float floatValue) {
    Shader_use(shader);
    glUniform1f(location, floatValue);
}

void Shader_sendFloats(Shader* shader, ShaderLoc location, float floatValues[], int numFloats) {
    Shader_use(shader);
    glUniform1fv(location, numFloats, floatValues);
}

void Shader_sendVec3(Shader* shader, ShaderLoc location, float x, float y, float z) {
    Shader_use(shader);
    glUniform3f(location, x, y, z);
}

void Shader_sendVec4(Shader* shader, ShaderLoc location, float x, float y, float z, float w) {
    Shader_use(shader);
    glUniform4f(location, x, y, z, w);
}

void Shader_sendInt(Shader* shader, ShaderLoc location, int intValue) {
    Shader_use(shader);
    glUniform1i(location, intValue);
}

void Shader_sendMat4(Shader* shader, ShaderLoc location, mat4x4 matrix) {
    Shader_use(shader);
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}