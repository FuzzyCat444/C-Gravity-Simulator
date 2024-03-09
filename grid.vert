#version 460 core

out float alpha;

uniform mat4 mvp;

uniform int axis;

uniform float viewX;
uniform float viewZ;
uniform float radius;
uniform float radiusFadeStart;
uniform float invR1;
uniform float invR2MinusInvR1;

uniform float lineInfo[6];

uniform int bodiesSize;

struct Body {
    int type;
    float radius;
    vec4 pos;
};

layout(std430, binding = 0) buffer Bodies
{
    Body bodies[];
};

// The inverse of the integral(x, d) function in Grid.c. This is NOT the derivative.
// After we linearly interpolate between the integral1 and integral2 initial values,
// we use this function to transform the integral value to a positional value. This
// will cause the positions to be arranged with a density of 1/sqrt(x^2+d^2).
float inverseIntegral(float x) {
    const float distance = lineInfo[3];
    return distance * sinh(x);
}

float planetWarp(float x2, float r) {
    r *= 3.0;
    float r2 = r * r;
    x2 /= r2;
    float x4 = x2 * x2;
    float sr = 1.2 * r;
    if (abs(x2) < 1.0) {
        return sr * (0.5 * (1.0 / (x2 + 1.0) + 1.0 / (x4 + 1.0)) + 0.25) * 0.8;
    } else {
        return sr * 6.0 / (5.0 * (x2 + 1.0));
    }
}

float starWarp(float x2, float r) {
    r *= 3.0;
    float r2 = r * r;
    x2 /= r2;
    float x4 = x2 * x2;
    float sr = 1.2 * r;
    if (abs(x2) < 1.0) {
        return sr * (0.5 * (1.0 / (x2 + 1.0) + 1.0 / (x4 + 1.0)) + 0.25) * 0.8;
    } else {
        return sr * 6.0 / (5.0 * (x2 + 1.0));
    }
}

float blackHoleWarp(float x2, float r) {
    r *= 15.0;
    float r2 = r * r;
    x2 /= r2;
    return r / x2;
}

float warp(vec2 pos) {
    float sum = 0.0;
    for (int i = 0; i < bodiesSize; i++) {
        Body body = bodies[i];
        vec2 diff = pos - body.pos.xz;
        float x2 = dot(diff, diff);
        
        switch (body.type) {
        case 0:
            sum -= planetWarp(x2, body.radius);
            break;
        case 1:
            sum -= starWarp(x2, body.radius);
            break;
        case 2:
            sum -= blackHoleWarp(x2, body.radius);
            break;
        }
    }
    return sum;
}

void main() {
    const float pos = lineInfo[0];
    const float integral1 = lineInfo[1];
    const float integral21 = lineInfo[2];
    const float distance = lineInfo[3];
    const float oneOverN = lineInfo[4];
    const float lineAlpha = lineInfo[5];

    // Interpolate linearly between integral1 and integral2, where integral21 is (integral2 - integral1)
    // Then, take the inverse of the integral to get vertex positions. This results in the vertices being
    // denser when they are closer to the camera.
    float value = inverseIntegral(integral1 + integral21 * gl_VertexID * oneOverN);
    vec3 vPos = vec3(0.0, 0.0, 0.0);
    switch (axis) { 
    case 0: // X axis
        value += viewX;
    
        vPos.x = value;
        vPos.z = pos;
        break;
    case 1: // Z axis
        value += viewZ;
    
        vPos.x = pos;
        vPos.z = value;
        break;
    }
    vPos.y = warp(vec2(vPos.x, vPos.z));
    
    float vAlpha = 1.0;
    float distToView = length(vec2(viewX - vPos.x, viewZ - vPos.z));
    if (distToView > radiusFadeStart) {
        float frac = (1.0 / distToView - invR1) / invR2MinusInvR1;
        vAlpha = smoothstep(1.0, 0.0, frac);
    }
    vAlpha *= lineAlpha;
    
    gl_Position = mvp * vec4(vPos, 1.0);
    alpha = vAlpha;
}