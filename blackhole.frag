#version 460 core

in vec2 tex;

out vec4 fragColor;

layout(binding = 0) uniform sampler2D uColor;

struct BlackHole {
    vec2 screenPos;
    float screenRadius;
};

layout(std430, binding = 0) buffer BlackHoles
{
    BlackHole blackHoles[];
};

uniform int numBlackHoles;
uniform float aspect;

void main() {
    vec2 fragPos = 2.0 * tex - vec2(1.0, 1.0);
    fragPos.x *= aspect;
    
    vec2 offset = vec2(0.0);
    for (int i = 0; i < numBlackHoles; i++) {
        BlackHole bh = blackHoles[i];
        
        vec2 diff = bh.screenPos - fragPos;
        float diffLen = length(diff);
        
        if (diffLen < bh.screenRadius * 0.99) {
            fragColor = vec4(0.0, 0.0, 0.0, 1.0);
            return;
        } 
        
        float margin = bh.screenRadius * 1.4;
        float xBoundary = abs(bh.screenPos.x) - margin;
        float yBoundary = abs(bh.screenPos.y) - margin;
        float dropoff = xBoundary > aspect ? aspect / xBoundary :
                        yBoundary > 1.0 ? 1.0 / yBoundary : 1.0;
        
        float x = abs(diffLen / bh.screenRadius) - 1.0;
        x *= 0.13;
        float x2 = x * x;
        float x4 = x2 * x2;
        float x8 = x4 * x4;
        float lensStrength = 0.3 / (2.2 * x8 + 1.2 * x4 + 0.6 * x2 + 0.1);
        offset += dropoff * lensStrength * diff / diffLen * bh.screenRadius;
    }
    vec2 newTex = fragPos + offset;
    newTex.x /= aspect;
    newTex = 0.5 * (newTex + vec2(1.0, 1.0));
    fragColor = texture(uColor, newTex);
}