#include "Sphere.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

static void addTriangle(Mesh* mesh, float* vertices, int* triIndex, float p1[3], float p2[3], float p3[3]) {
    const int floatsPerVertex = mesh->floatsPerVertex;
    int offset = *triIndex * 3 * floatsPerVertex;
    float* triVerts[] = { p1, p2, p3 };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++)
            memcpy(&vertices[offset + j * 3], triVerts[i], 3 * sizeof(float));
        offset += floatsPerVertex;
    }
    (*triIndex)++;
}

static void normalize(float p[3]) {
    float len2 = 0.0f;
    for (int i = 0; i < 3; i++) {
        float v = p[i];
        len2 += v * v;
    }
    float oneOverLen = 1.0f / sqrtf(len2);
    for (int i = 0; i < 3; i++)
        p[i] *= oneOverLen;
}

static void subdivideEdge(float p1[3], float p2[3], float res[3]) {
    for (int i = 0; i < 3; i++)
        res[i] = 0.5f * (p1[i] + p2[i]);
    normalize(res);
}

static void subdivideTriangle(Mesh* mesh, float* vertices, int* triIndex, int subdivisions, float p1[3], float p2[3], float p3[3]) {
    if (subdivisions == 0) {
        addTriangle(mesh, vertices, triIndex, p1, p2, p3);
    } else {
        subdivisions--;
        float p4[3];
        float p5[3];
        float p6[3];
        subdivideEdge(p1, p2, p4);
        subdivideEdge(p2, p3, p5);
        subdivideEdge(p3, p1, p6);
        subdivideTriangle(mesh, vertices, triIndex, subdivisions, p1, p4, p6);
        subdivideTriangle(mesh, vertices, triIndex, subdivisions, p4, p2, p5);
        subdivideTriangle(mesh, vertices, triIndex, subdivisions, p6, p5, p3);
        subdivideTriangle(mesh, vertices, triIndex, subdivisions, p5, p6, p4);
    }
}

void generateSphere(Mesh* mesh, int subdivisions) {
    if (subdivisions < 0) subdivisions = 0;
    
    int sides = 20;
    for (int i = 0; i < subdivisions; i++) sides *= 4;
    int numVertices = 3 * sides;
    float* vertices = (float*) malloc(numVertices * mesh->floatsPerVertex * sizeof(float));
    
    const float phi = 1.618034f;
    float icosahedronVerts[] = {
        -phi, -1.0f, 0.0f,
        phi, -1.0f, 0.0f,
        phi, 1.0f, 0.0f,
        -phi, 1.0f, 0.0f,
        -1.0f, 0.0f, phi,
        1.0f, 0.0f, phi,
        1.0f, 0.0f, -phi,
        -1.0f, 0.0f, -phi,
        0.0f, -phi, 1.0f,
        0.0f, -phi, -1.0f,
        0.0f, phi, -1.0f,
        0.0f, phi, 1.0f
    };
    for (int i = 0; i < 12; i++) 
        normalize(&icosahedronVerts[i * 3]);
    const int icosahedronIdxs[] = {
        // Top cap
        11, 2, 10,
        2, 6, 10,
        6, 7, 10,
        7, 3, 10,
        3, 11, 10,
        // Middle strip
        4, 5, 11,
        2, 11, 5,
        5, 1, 2,
        6, 2, 1,
        1, 9, 6,
        7, 6, 9,
        9, 0, 7,
        3, 7, 0,
        0, 4, 3,
        11, 3, 4,
        // Bottom cap
        1, 5, 8,
        5, 4, 8,
        4, 0, 8,
        0, 9, 8,
        9, 1, 8
    };
    int triIndex = 0;
    for (int i = 0; i < 20; i++) {
        const int* idx = &icosahedronIdxs[3 * i];
        float* points[3];
        for (int j = 0; j < 3; j++) {
            points[j] = &icosahedronVerts[3 * idx[j]];
        }
        subdivideTriangle(mesh, vertices, &triIndex, subdivisions, points[0], points[1], points[2]);
    }
    
    Mesh_sendVertices(mesh, vertices, numVertices);
    free(vertices);
}