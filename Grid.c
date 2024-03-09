#include "Grid.h"

#include <math.h>
#include <stdlib.h>

typedef enum Axis {
    AXIS_X = 0, AXIS_Z
} Axis;

static int intersectCircle(double cx, double cz, double r, double pos, Axis axis, double result[2]) {
    double r2 = r * r;
    double a;
    double b;
    
    switch (axis) {
    case AXIS_X:
        a = cz;
        b = cx;
        break;
    case AXIS_Z:
        a = cx;
        b = cz;
        break;
    }
    
    double diffA = a - pos;
    double diffA2 = diffA * diffA;
    double discr = r2 - diffA2;
    if (discr < 0.0) return 0;
    double sqrtDiscr = sqrt(discr);
    result[0] = b - sqrtDiscr;
    result[1] = b + sqrtDiscr;
    
    return 1;
}

// Imagine we have a line L and a point P next to it. Let x be the location of a point S on the line.
// When x = 0, point S is as close as possible to point P. Let d be the distance between S and P when x = 0.
// The diagonal/euclidean distance between S and P is sqrt(x^2+d^2). We want gridline *segments* to have a length proportional
// to their distance sqrt(x^2+d^2) from the camera in order to put the detail where it counts. Longer lines will appear
// to be the same size when they are farther away from the camera, shorter lines will appear the same closer. This means 
// the density of the vertices ought to be 1/sqrt(x^2+d^2). We want to place vertices based on this exact density function.
// We first need the area under the curve 1/sqrt(x^2+d^2) at two x positions. Once we have the area corresponding to the two 
// x positions, we can linearly interpolate between them to get a new area under the curve that represents the integral at the 
// x position of interest. we can then use the INVERSE of the integral of this function to change BACK from area under the curve to 
// actual x position values; that inverse, d*sinh(x), is stored in the grid.vert shader. This is not the inverse of the 
// integral, but just the integral, asinh(x/d), to provide  the initial values that we linearly interpolate between. Once they 
// are linearly interpolated in the grid.vert shader, we use the inverse of this function to retrieve the x values to place the vertices.
// View this desmos graph for a visual explanation: https://www.desmos.com/calculator/6esvr0hjar
// An explanation of why the integral of the function and its inverse are needed is not easy for me to explain, but you can read
// more about this approach here https://en.wikipedia.org/wiki/Inverse_transform_sampling . The integral is essentially equivalent to
// the cumulative distribution function.
static double integral(double x, double distance) {
    return asinh(x / distance);
}

static void doLine(Shader* gridShader, Mesh* mesh, const GridInfo* gi, const GridUniforms* gu, double pos, Axis axis, double lineAlpha) {
    double viewX = gi->view.x;
    double viewY = gi->view.y;
    double viewZ = gi->view.z;
    double startEnd[2];
    int shouldDoLine = intersectCircle(viewX, viewZ, gi->radius, pos, axis, startEnd);
    if (!shouldDoLine) return;
    double lineLength = startEnd[1] - startEnd[0];
    
    double viewPos;
    double viewAxisPos;
    switch (axis) {
    case AXIS_X:
        viewAxisPos = viewX;
        viewPos = viewZ;
        break;
    case AXIS_Z:
        viewAxisPos = viewZ;
        viewPos = viewX;
        break;
    };
    double distToLine = fabs(viewPos - pos);
    double y = viewY;
    double distance = sqrt(distToLine * distToLine + y * y);
    // Decrease line resolution when farther away from camera
    int lineResolution = (int) (gi->resolution * 0.5 * lineLength / (distToLine + gi->radius));
    
    double integral1 = integral(startEnd[0] - viewAxisPos, distance);
    double integral2 = integral(startEnd[1] - viewAxisPos, distance);
    
    float lineInfo[6] = {
        (float) pos,
        (float) integral1,
        (float) (integral2 - integral1),
        (float) distance,
        (float) (1.0 / lineResolution),
        (float) lineAlpha
    };
    Shader_sendFloats(gridShader, gu->lineInfo, lineInfo, 6);
    
    // This mesh has no vertex data, we simply set the number of vertices to draw and let the vertex shader
    // generate the vertices. This saves a substantial amount of time because we don't have to send vertex 
    // data to the GPU. The vertex shader will generate vertices based on gl_VertexID which is the index
    // of the vertex being drawn. Using line strips instead of lines cuts the number of vertices in half.
    mesh->verticesSize = lineResolution + 1;
    Mesh_draw(mesh, DRAWMODE_LINESTRIP);
}

void drawGrid(Shader* gridShader, Mesh* mesh, const GridInfo* gi, const GridUniforms* gu) {
    double viewX = gi->view.x;
    double viewZ = gi->view.z;
    double radius = gi->radius;
    double thickLineAlpha = gi->thickLineAlpha;
    double thinLineAlpha = gi->thinLineAlpha;
    int subdivisions = gi->subdivisions;
    double spacing = gi->spacing;
    double subspacing = spacing / subdivisions;
    double radiusFadeStart = radius * gi->fadeStart;
    
    Shader_sendFloat(gridShader, gu->viewX, viewX);
    Shader_sendFloat(gridShader, gu->viewZ, viewZ);
    Shader_sendFloat(gridShader, gu->radius, radius);
    Shader_sendFloat(gridShader, gu->radiusFadeStart, radiusFadeStart);
    Shader_sendFloat(gridShader, gu->invR1, 1.0 / radiusFadeStart);
    Shader_sendFloat(gridShader, gu->invR2MinusInvR1, 1.0 / radius - 1.0 / radiusFadeStart);
    
    for (int i = 0; i < 2; i++) {
        Axis axis;
        double viewPos;
        if (i == 0) {
            axis = AXIS_X;
            viewPos = viewZ;
        } else {
            axis = AXIS_Z;
            viewPos = viewX;
        }
        Shader_sendInt(gridShader, gu->axis, axis);
        int lineIndex = (int) ceil((viewPos - radius) / subspacing);
        int endLineIndex = (int) ceil((viewPos + radius) / subspacing);
        while (lineIndex < endLineIndex) {
            double alpha = lineIndex % subdivisions == 0 ? thickLineAlpha : thinLineAlpha;
            doLine(gridShader, mesh, gi, gu, lineIndex * subspacing, axis, alpha);
            lineIndex++;
        }
    }
}

void GridUniforms_get(GridUniforms* gu, const Shader* shader) {
    gu->axis = Shader_getLocation(shader, "axis");
    gu->viewX = Shader_getLocation(shader, "viewX");
    gu->viewZ = Shader_getLocation(shader, "viewZ");
    gu->radius = Shader_getLocation(shader, "radius");
    gu->radiusFadeStart = Shader_getLocation(shader, "radiusFadeStart");
    gu->lineInfo = Shader_getLocation(shader, "lineInfo");
    gu->invR1 = Shader_getLocation(shader, "invR1");
    gu->invR2MinusInvR1 = Shader_getLocation(shader, "invR2MinusInvR1");
}