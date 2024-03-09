#ifndef GRID_H
#define GRID_H

#include "Mesh.h"
#include "Shader.h"
#include "Vec.h"

typedef struct GridInfo {
    Vec3 view;
    double radius;
    double fadeStart;
    double thickLineAlpha;
    double thinLineAlpha;
    double spacing;
    int subdivisions;
    int resolution;
} GridInfo;

typedef struct GridUniforms {
    ShaderLoc axis;
    ShaderLoc viewX;
    ShaderLoc viewZ;
    ShaderLoc radius;
    ShaderLoc radiusFadeStart;
    ShaderLoc lineInfo;
    ShaderLoc invR1;
    ShaderLoc invR2MinusInvR1;
} GridUniforms;

void drawGrid(Shader* gridShader, Mesh* mesh, const GridInfo* gi, const GridUniforms* gu);

void GridUniforms_get(GridUniforms* gu, const Shader* shader);

#endif