#include "SkyBox.h"

#include <stdlib.h>
#include <string.h>

static void copyPointsTexCoords(float vertices[], int offset, int vi, float points[], float texCoords[]) {
    memcpy(&vertices[offset], &points[3 * vi], 3 * sizeof(float));
    memcpy(&vertices[offset + 3], &texCoords[2 * vi], 2 * sizeof(float));
}

static void addQuad(Mesh* mesh, float vertices[], int quadIndex, float points[], float texCoords[]) {
    const int floatsPerVertex = mesh->floatsPerVertex;
    int offset = quadIndex * 6 * floatsPerVertex;
    for (int i = 0; i < 6; i++) {
        int vi = i % 3 + (i > 3);
        copyPointsTexCoords(vertices, offset, vi, points, texCoords);
        offset += floatsPerVertex;
    }
}

void generateSkyBox(Mesh* mesh) {
    float* vertices = (float*) malloc(36 * mesh->floatsPerVertex * sizeof(float));
    
    // Back
    addQuad(mesh, vertices, 0, 
        (float[]) {
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
        }, 
        (float[]) {
            0.25f, 0.6666f,
            0.5f,  0.6666f,
            0.5f,  0.3333f,
            0.25f, 0.3333f
        }
    );
    // Top
    addQuad(mesh, vertices, 1, 
        (float[]) {
            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
        }, 
        (float[]) {
            0.25f, 0.3333f,
            0.5f,  0.3333f,
            0.5f,  0.0f,
            0.25f, 0.0f
        }
    );
    // Bottom
    addQuad(mesh, vertices, 2, 
        (float[]) {
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
        }, 
        (float[]) {
            0.25f, 1.0f,
            0.5f,  1.0f,
            0.5f,  0.6666f,
            0.25f, 0.6666f
        }
    );
    // Left
    addQuad(mesh, vertices, 3, 
        (float[]) {
            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
        }, 
        (float[]) {
            0.0f,  0.6666f,
            0.25f, 0.6666f,
            0.25f, 0.3333f,
            0.0f,  0.3333f
        }
    );
    // Right
    addQuad(mesh, vertices, 4, 
        (float[]) {
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
        }, 
        (float[]) {
            0.5f,  0.6666f,
            0.75f, 0.6666f,
            0.75f, 0.3333f,
            0.5f,  0.3333f
        }
    );
    // Back
    addQuad(mesh, vertices, 5, 
        (float[]) {
             1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f,  1.0f, 1.0f,
             1.0f,  1.0f, 1.0f,
        }, 
        (float[]) {
            0.75f, 0.6666f,
            1.0f,  0.6666f,
            1.0f,  0.3333f,
            0.75f, 0.3333f
        }
    );
    
    Mesh_sendVertices(mesh, vertices, 36);
    free(vertices);
}