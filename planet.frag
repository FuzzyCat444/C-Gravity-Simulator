#version 460 core

in vec3 pos;
in vec3 norm;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 starGlow;

uniform vec3 planetColor;
uniform int type;
uniform float camDistRadius;

#define NUM_LIGHTS 4
vec3 lights[NUM_LIGHTS][2] = {
    { 0.6 * vec3(1.0, 1.0, 0.9), normalize(vec3(-1.0, 0.5, -0.7)) },
    { 0.4 * vec3(0.7, 0.0, 1.0), normalize(vec3(0.3, 0.0, -1.0)) },
    { 0.4 * vec3(1.0, 0.0, 0.5), normalize(vec3(1.0, 1.0, 1.0)) },
    { 0.6 * vec3(1.0, 0.4, 0.4), normalize(vec3(0.0, -1.0, 0.0)) }
};

vec4 starLight(vec4 color, float intensity, float t) {
    vec4 brightened = color;
    brightened.r = 1.0 - exp(-intensity * color.r / t);
    brightened.g = 1.0 - exp(-intensity * color.g / t);
    brightened.b = 1.0 - exp(-intensity * color.b / t);
    return brightened;
}

void main() {
    vec3 normal = normalize(norm);
    
    switch (type) {
    case 0:
        vec3 lightSum = vec3(0.0, 0.0, 0.0);
        for (int i = 0; i < NUM_LIGHTS; i++) {
            float light = max(dot(normal, -lights[i][1]), 0.0);
            lightSum += light * lights[i][0];
        }
        
        float lightRatio = 0.7;
        float ambient = 0.7;
        fragColor = vec4(((1.0 - lightRatio) * ambient * vec3(1.0, 1.0, 1.0) + lightRatio * lightSum) * planetColor, 1.0);
        starGlow = vec4(0.0, 0.0, 0.0, 0.0);
        break;
    case 1:
        float brightnessInside = max(dot(normalize(-pos), normal), 0.0);
        vec4 planetColor4 = vec4(planetColor, 1.0);
        
        fragColor = starLight(planetColor4, 1.0 + 0.1 * camDistRadius, brightnessInside);
        fragColor.a = 0.0;
        starGlow = starLight(planetColor4, 2.0, 0.5);
        break;
    case 2:
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
        starGlow = vec4(0.0, 0.0, 0.0, 0.0);
        break;
    }
}