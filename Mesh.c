#include "Mesh.h"

#include <stdlib.h>

int MeshFormat_floatsPerVertex(MeshFormat format) {
    switch (format) {
    case MESHFORMAT_0:
        return 0;
    case MESHFORMAT_3:
        return 3;
    case MESHFORMAT_32:
        return 5;
    case MESHFORMAT_33:
        return 6;
    }
}

static void Mesh_addAttributes3() {
    const int stride = MeshFormat_floatsPerVertex(MESHFORMAT_3) * sizeof(float);
    float* offset = 0;
    // Vec3
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(0);
}

static void Mesh_addAttributes32() {
    const int stride = MeshFormat_floatsPerVertex(MESHFORMAT_32) * sizeof(float);
    float* offset = 0;
    // Vec3
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(0);
    offset += 3;
    // Vec2
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(1);
}

static void Mesh_addAttributes33() {
    const int stride = MeshFormat_floatsPerVertex(MESHFORMAT_33) * sizeof(float);
    float* offset = 0;
    // Vec3
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(0);
    offset += 3;
    // Vec3
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(1);
}

void Mesh_create(Mesh* mesh, MeshFormat format) {
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    switch (format) {
    case MESHFORMAT_3:
        Mesh_addAttributes3();
        break;
    case MESHFORMAT_32:
        Mesh_addAttributes32();
        break;
    case MESHFORMAT_33:
        Mesh_addAttributes33();
        break;
    }
    
    mesh->vao = vao;
    mesh->vbo = vbo;
    mesh->verticesSize = 0;
    mesh->verticesCapacity = 0;
    mesh->format = format;
    mesh->floatsPerVertex = MeshFormat_floatsPerVertex(format);
}

void Mesh_destroy(Mesh* mesh) {
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
}

void Mesh_sendVertices(Mesh* mesh, const float vertices[], int numVertices) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    if (numVertices <= mesh->verticesCapacity) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->floatsPerVertex * numVertices * sizeof(float), vertices);
    } else {
        glBufferData(GL_ARRAY_BUFFER, mesh->floatsPerVertex * numVertices * sizeof(float), vertices, GL_DYNAMIC_DRAW);
        mesh->verticesCapacity = numVertices;
    }
    mesh->verticesSize = numVertices;
}

void Mesh_draw(Mesh* mesh, DrawMode mode) {
    GLenum glMode = 0;
    switch (mode) {
    case DRAWMODE_TRIANGLES:
        glMode = GL_TRIANGLES;
        break;
    case DRAWMODE_LINESTRIP:
        glMode = GL_LINE_STRIP;
        break;
    };
    glBindVertexArray(mesh->vao);
    glDrawArrays(glMode, 0, mesh->verticesSize);
}