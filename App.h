#ifndef APP_H
#define APP_H

#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "SSBO.h"
#include "Grid.h"
#include "Universe.h"
#include "Framebuffer.h"

typedef enum AppKeyName {
    APP_KEYNAME_MOVELEFT = 0,
    APP_KEYNAME_MOVERIGHT,
    APP_KEYNAME_MOVEBACKWARD,
    APP_KEYNAME_MOVEFORWARD,
    APP_KEYNAME_MOVEDOWN,
    APP_KEYNAME_MOVEUP,
    APP_KEYNAME_HIDEGRID,
    APP_KEYNAME_SHOWCROSSHAIRS,
    APP_KEYNAME_PAUSESIMULATION,
    APP_KEYNAME_UNDO,
    APP_KEYNAME_DELETE,
    APP_KEYNAME_CLEAR,
    APP_KEYNAME_CLEARDISTANT,
    APP_KEYNAME_PREVIOUSPLANET,
    APP_KEYNAME_NEXTPLANET,
    APP_KEYNAME_SELECTPLANET,
    APP_KEYNAME_SELECTSTAR,
    APP_KEYNAME_SELECTBLACKHOLE,
    NUM_APP_KEYNAMES
} AppKeyName;

typedef struct AppKey {
    int down;
    double downAmount;
    int justPressed;
} AppKey;

typedef struct AppMouse {
    double x, y, dx, dy;
    int rightDown;
    int leftJustPressed;
    int leftJustReleased;
    int middleJustPressed;
    double scroll;
    int control;
} AppMouse;

typedef struct AppUniforms {
    ShaderLoc skyBoxMvp;
    
    ShaderLoc blackHoleNum;
    ShaderLoc blackHoleAspect;
    
    ShaderLoc planetMvp;
    ShaderLoc planetMv;
    ShaderLoc planetPlanetColor;
    ShaderLoc planetType;
    ShaderLoc planetCamDistRadius;
    
    ShaderLoc gridMvp;
    ShaderLoc gridBodiesSize;
    
    ShaderLoc lineMvp;
    ShaderLoc lineColor;
    
    ShaderLoc gaussianBlurKernel;
    ShaderLoc gaussianBlurKernelSize;
    ShaderLoc gaussianBlurKernelOffset;
    ShaderLoc gaussianBlurPass;
    ShaderLoc gaussianBlurAspect;
} AppUniforms;

typedef struct App {
    AppKey keys[NUM_APP_KEYNAMES];
    AppMouse mouse;
    
    int width, height;
    int looking;
    
    Framebuffer framebuffer1;
    Framebuffer framebuffer2;
    
    Shader finalRenderShader;
    Mesh screenMesh;
    
    Shader blackHoleShader;
    SSBO blackHolesSSBO;
    
    Shader skyBoxShader;
    Mesh skyBoxMesh;
    Texture skyBoxTexture;
    
    Shader gridShader;
    SSBO gridBodiesSSBO;
    Mesh gridMesh;
    GridInfo gridInfo;
    GridUniforms gridUniforms;
    int showGrid;
    
    Shader planetShader;
    Mesh planetMesh;
    Mesh blackHoleMesh;
    
    Shader gaussianBlurShader;
    Shader starGlowShader;
    
    Shader lineShader;
    Mesh spawnTrajectoryMesh;
    Vec3 startSpawnTrajectory;
    Vec3 endSpawnTrajectory;
    int spawning;
    Mesh spawnRadiusMesh;
    double spawnRadius;
    int spawnId;
    BodyType selectedBodyType;
    Mesh crossHairMesh;
    int showCrossHairs;
    
    AppUniforms appUniforms;
    
    Universe universe;
    int doSimulate;
    
    double time;
} App;

App* App_create();

void App_destroy(App* app);

void App_resize(App* app, int width, int height);

void App_update(App* app, double dt);

void App_draw(App* app);

#endif