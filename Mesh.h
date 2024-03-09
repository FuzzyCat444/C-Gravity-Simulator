#ifndef MESH_H
#define MESH_H

#include <glad/gl.h>

typedef enum DrawMode {
    DRAWMODE_TRIANGLES,
    DRAWMODE_LINESTRIP
} DrawMode;

typedef enum MeshFormat {
    MESHFORMAT_0,
    MESHFORMAT_3,
    MESHFORMAT_32,
    MESHFORMAT_33
} MeshFormat;

typedef struct Mesh {
    GLuint vao;
    GLuint vbo;
    int verticesSize;
    int verticesCapacity;
    MeshFormat format;
    int floatsPerVertex;
} Mesh;

void Mesh_create(Mesh* mesh, MeshFormat format);

void Mesh_destroy(Mesh* mesh);

void Mesh_sendVertices(Mesh* mesh, const float vertices[], int numVertices);

void Mesh_draw(Mesh* mesh, DrawMode mode);

#endif