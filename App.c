#include "App.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <glad/gl.h>
#include <linmath.h>

#include "Constants.h"
#include "SkyBox.h"
#include "Sphere.h"
#include "Vec.h"

App* App_create(int width, int height) {
    App* app = (App*) malloc(sizeof(App));
    
    AppKey* keys = app->keys;
    for (int i = 0; i < NUM_APP_KEYNAMES; i++) {
        AppKey key = { .down = 0, .downAmount = 0.0, .justPressed = 0 };
        keys[i] = key;
    }
    
    app->mouse = (AppMouse) {
        .x = 0.0, .y = 0.0, .dx = 0.0, .dy = 0.0,
        .rightDown = 0, .leftJustPressed = 0, .leftJustReleased = 0, .middleJustPressed = 0,
        .scroll = 0.0, .control = 0
    };
    
    app->width = width;
    app->height = height;
    
    app->looking = 0;
    
    srand(time(NULL));
    
    Framebuffer_create(&app->framebuffer1, width, height, 2);
    Framebuffer_create(&app->framebuffer2, width, height, 2);
    
    Shader_create(&app->finalRenderShader, "finalrender.vert", "finalrender.frag");
    
    SSBO_create(&app->blackHolesSSBO);
    Shader_create(&app->blackHoleShader, "blackhole.vert", "blackhole.frag");
    Mesh_create(&app->screenMesh, MESHFORMAT_32);
    const float screenVerts[] = {
        -1.0f, -1.0f, 0.0f,
        0.0f, 0.0f,
        
        1.0f, -1.0f, 0.0f,
        1.0f, 0.0f,
        
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f,
        
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f,
        
        -1.0f, 1.0f, 0.0f,
        0.0f, 1.0f,
        
        -1.0f, -1.0f, 0.0f,
        0.0f, 0.0f
    };
    Mesh_sendVertices(&app->screenMesh, screenVerts, 6);
    
    Shader_create(&app->skyBoxShader, "skybox.vert", "skybox.frag");
    Mesh_create(&app->skyBoxMesh, MESHFORMAT_32);
    generateSkyBox(&app->skyBoxMesh);
    Texture_create(&app->skyBoxTexture, "skybox1.png");
    
    Shader_create(&app->gridShader, "grid.vert", "grid.frag");
    SSBO_create(&app->gridBodiesSSBO);
    GridUniforms_get(&app->gridUniforms, &app->gridShader);
    Mesh_create(&app->gridMesh, MESHFORMAT_0);
    Mesh_sendVertices(&app->gridMesh, NULL, 1);
    GridInfo* gridInfo = &app->gridInfo;
    gridInfo->fadeStart = C_GRID_FADE_START;
    gridInfo->thickLineAlpha = C_GRID_THICK_LINE_ALPHA;
    gridInfo->thinLineAlpha = C_GRID_THIN_LINE_ALPHA;
    gridInfo->subdivisions = C_GRID_SUBDIVISIONS;
    gridInfo->resolution = C_GRID_RESOLUTION;
    app->showGrid = 1;
    
    Shader_create(&app->planetShader, "planet.vert", "planet.frag");
    Mesh_create(&app->planetMesh, MESHFORMAT_33);
    generateSphere(&app->planetMesh, 4);
    
    Mesh_create(&app->blackHoleMesh, MESHFORMAT_3);
    int blackHoleTrianglesCount = 100;
    float* blackHoleVerts = (float*) malloc(app->blackHoleMesh.floatsPerVertex * 3 * blackHoleTrianglesCount * sizeof(float));
    float angle1 = 0.0f;
    for (int i = 0, j = 0; i < blackHoleTrianglesCount; i++, j += 9) {
        float angle2 = 4.0f * C_PI2 * (i + 1.0f) / blackHoleTrianglesCount;
        float circleTriangle[] = { 
            cosf(angle1), sinf(angle1), 0.0f,
            cosf(angle2), sinf(angle2), 0.0f,
            0.0f, 0.0f, 0.0f
        };
        memcpy(&blackHoleVerts[j], circleTriangle, 9 * sizeof(float));
        angle1 = angle2;
    }
    Mesh_sendVertices(&app->blackHoleMesh, blackHoleVerts, 3 * blackHoleTrianglesCount);
    free(blackHoleVerts);
    
    Shader_create(&app->gaussianBlurShader, "gaussianblur.vert", "gaussianblur.frag");
    Shader_create(&app->starGlowShader, "starglow.vert", "starglow.frag");
    
    Shader_create(&app->lineShader, "line.vert", "line.frag");
    Mesh_create(&app->spawnTrajectoryMesh, MESHFORMAT_3);
    app->spawning = 0;
    Mesh_create(&app->spawnRadiusMesh, MESHFORMAT_3);
    int spawnRadiusVertsSize = 100;
    float* spawnRadiusVerts = (float*) malloc(app->spawnRadiusMesh.floatsPerVertex * spawnRadiusVertsSize * sizeof(float));
    for (int i = 0, j = 0; i < spawnRadiusVertsSize; i++, j += 3) {
        float angle = 4.0f * C_PI2 * i / (spawnRadiusVertsSize - 1);
        float circlePoint[] = { cosf(angle), sinf(angle), 0.0f };
        memcpy(&spawnRadiusVerts[j], circlePoint, 3 * sizeof(float));
    }
    Mesh_sendVertices(&app->spawnRadiusMesh, spawnRadiusVerts, spawnRadiusVertsSize);
    free(spawnRadiusVerts);
    app->spawnRadius = 0;
    app->spawnId = 0;
    app->selectedBodyType = BODYTYPE_PLANET;
    Mesh_create(&app->crossHairMesh, MESHFORMAT_3);
    float* crossHairVerts = (float*) malloc(app->crossHairMesh.floatsPerVertex * 5 * sizeof(float));
    memcpy(crossHairVerts, (float[15]) {
        -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    }, app->crossHairMesh.floatsPerVertex * 5 * sizeof(float));
    Mesh_sendVertices(&app->crossHairMesh, crossHairVerts, 5);
    free(crossHairVerts);
    app->showCrossHairs = 0;
    
    app->appUniforms = (AppUniforms) {
        .skyBoxMvp = Shader_getLocation(&app->skyBoxShader, "mvp"),
        
        .blackHoleNum = Shader_getLocation(&app->blackHoleShader, "numBlackHoles"),
        .blackHoleAspect = Shader_getLocation(&app->blackHoleShader, "aspect"),
        
        .planetMvp = Shader_getLocation(&app->planetShader, "mvp"),
        .planetMv = Shader_getLocation(&app->planetShader, "mv"),
        .planetPlanetColor = Shader_getLocation(&app->planetShader, "planetColor"),
        .planetType = Shader_getLocation(&app->planetShader, "type"),
        .planetCamDistRadius = Shader_getLocation(&app->planetShader, "camDistRadius"),
        
        .gridMvp = Shader_getLocation(&app->gridShader, "mvp"),
        .gridBodiesSize = Shader_getLocation(&app->gridShader, "bodiesSize"),
        
        .lineMvp = Shader_getLocation(&app->lineShader, "mvp"),
        .lineColor = Shader_getLocation(&app->lineShader, "color"),
        
        .gaussianBlurKernel = Shader_getLocation(&app->gaussianBlurShader, "kernel"),
        .gaussianBlurKernelSize = Shader_getLocation(&app->gaussianBlurShader, "kernelSize"),
        .gaussianBlurKernelOffset = Shader_getLocation(&app->gaussianBlurShader, "kernelOffset"),
        .gaussianBlurPass = Shader_getLocation(&app->gaussianBlurShader, "pass"),
        .gaussianBlurAspect = Shader_getLocation(&app->gaussianBlurShader, "aspect")
    };
    
    AppUniforms* appUniforms = &app->appUniforms;
    
    #define KERNEL_SIZE 121
    float kernel[KERNEL_SIZE];
    float kernelTotal = 0.0f;
    for (int i = 0; i < KERNEL_SIZE; i++) {
        float kernelValue = expf(-powf(4.0f * i / (KERNEL_SIZE - 1) - 2.0f, 2.0f));
        kernel[i] = kernelValue;
        kernelTotal += kernelValue;
    }
    for (int i = 0; i < KERNEL_SIZE; i++) {
        kernel[i] /= kernelTotal;
    }
    Shader* gaussianBlurShader = &app->gaussianBlurShader;
    Shader_sendFloats(gaussianBlurShader, appUniforms->gaussianBlurKernel, kernel, KERNEL_SIZE);
    Shader_sendInt(gaussianBlurShader, appUniforms->gaussianBlurKernelSize, KERNEL_SIZE);
    Shader_sendFloat(gaussianBlurShader, appUniforms->gaussianBlurKernelOffset, 0.001f);
    
    Universe_create(&app->universe, C_UNIVERSE_GRAVITY_CONSTANT, 1.0 / C_UNIVERSE_UPS,
        (double[3][3]) {
            { C_UNIVERSE_PLANET_GRAVITY, C_UNIVERSE_PLANET_GRAVITY, C_UNIVERSE_PLANET_GRAVITY },
            { C_UNIVERSE_STAR_GRAVITY, C_UNIVERSE_STAR_GRAVITY, C_UNIVERSE_STAR_GRAVITY },
            { C_UNIVERSE_BLACKHOLE_GRAVITY, C_UNIVERSE_BLACKHOLE_GRAVITY, C_UNIVERSE_BLACKHOLE_GRAVITY }
        }
    );
    app->doSimulate = 1;
    
    app->time = 0.0;
    
    return app;
}

void App_destroy(App* app) {
    if (app == NULL)
        return;
    
    Framebuffer_destroy(&app->framebuffer1);
    Framebuffer_destroy(&app->framebuffer2);
    
    Shader_destroy(&app->finalRenderShader);
    
    SSBO_destroy(&app->blackHolesSSBO);
    Shader_destroy(&app->blackHoleShader);
    Mesh_destroy(&app->screenMesh);
    
    Shader_destroy(&app->skyBoxShader);
    Mesh_destroy(&app->skyBoxMesh);
    Texture_destroy(&app->skyBoxTexture);
    
    Shader_destroy(&app->gridShader);
    SSBO_destroy(&app->gridBodiesSSBO);
    Mesh_destroy(&app->gridMesh);
    
    Shader_destroy(&app->planetShader);
    Mesh_destroy(&app->planetMesh);
    Mesh_destroy(&app->blackHoleMesh);
    
    Shader_destroy(&app->gaussianBlurShader);
    Shader_destroy(&app->starGlowShader);
    
    Shader_destroy(&app->lineShader);
    Mesh_destroy(&app->spawnTrajectoryMesh);
    Mesh_destroy(&app->spawnRadiusMesh);
    Mesh_destroy(&app->crossHairMesh);
    
    Universe_destroy(&app->universe);
    
    free(app);
}

void App_resize(App* app, int width, int height) {
    app->width = width;
    app->height = height;
    
    glViewport(0, 0, width, height);
    Framebuffer_destroy(&app->framebuffer1);
    Framebuffer_create(&app->framebuffer1, width, height, 2);
    Framebuffer_destroy(&app->framebuffer2);
    Framebuffer_create(&app->framebuffer2, width, height, 2);
}

static void App_updateKeys(App* app, double dt) {
    AppKey* keys = app->keys;
    const double downAmountDt = dt * C_KEY_RESPONSIVENESS;
    for (int i = 0; i < NUM_APP_KEYNAMES; i++) {
        AppKey key = keys[i];
        if (key.down) {
            key.downAmount += downAmountDt;
            if (key.downAmount > 1.0) key.downAmount = 1.0;
        } else {
            key.downAmount -= downAmountDt;
            if (key.downAmount < 0.0) key.downAmount = 0.0;
        }
        key.justPressed = 0;
        keys[i] = key;
    }
}

static void App_updateMouse(App* app, double dt) {
    AppMouse* mouse = &app->mouse;
    mouse->dx = 0.0;
    mouse->dy = 0.0;
    mouse->leftJustPressed = 0;
    mouse->leftJustReleased = 0;
    mouse->middleJustPressed = 0;
    mouse->scroll = 0.0;
    mouse->control = 0;
}

static Vec3 screenCoordsToWorldCoords(App* app, double x, double y) {
    Universe* universe = &app->universe;
    
    return Universe_rayTraceMousePlane(universe, x, y, app->width, app->height, C_CAMERA_FOV);
}

static int screenCoordsToBodyId(App* app, double x, double y) {
    Universe* universe = &app->universe;
    
    return Universe_rayTraceMouseBodies(universe, x, y, app->width, app->height, C_CAMERA_FOV);
}

static void focusOnBody(Universe* universe, int bodyId) {
    Camera* camera = Universe_getCamera(universe);
    int oldFocusedBodyId = camera->focusedBodyId;
    int newFocusedBodyId = bodyId;
    Vec3 camPos = Universe_getLocalPosition(universe, newFocusedBodyId, Universe_getGlobalPosition(universe, oldFocusedBodyId, camera->pos));
    if (newFocusedBodyId != -1) {
        double yaw = camera->rot.y;
        double pitch = camera->rot.x;
        Vec3 forward = { 0.0, 0.0, 1.0 };
        forward = vrotzx(forward, cos(yaw), sin(yaw));
        if (fabs(pitch) > C_PI4) {
            forward = vscl(forward, fabs(camPos.y) / tan(fabs(pitch)));
            camPos.x = forward.x;
            camPos.z = forward.z;
        } else {
            double dist = sqrt(vmag2((Vec3) { camPos.x, 0.0, camPos.z }));
            forward = vscl(forward, dist);
            camPos.x = forward.x;
            camPos.z = forward.z;
            camPos.y = dist * tan(-pitch);
        }
    }
    
    camera->focusedBodyId = newFocusedBodyId;
    camera->pos = camPos;
}

static double getSpawnRadius(App* app) {
    return pow(2.0, app->spawnRadius);
}

static double randF(double a, double b) {
    double t = (double) rand() / RAND_MAX;
    return a + t * (b - a);
}

static Vec3 randomColor(BodyType type) {
    if (type == BODYTYPE_PLANET) {
        return (Vec3) { randF(0.0, 1.0), randF(0.0, 1.0), randF(0.0, 1.0) };
    } else if (type == BODYTYPE_STAR) {
        int starColor = rand() % 4;
        switch (starColor) {
        case 0: // Red
            return (Vec3) { 1.0, randF(0.05, 0.15), randF(0.05, 0.15) };
            break;
        case 1: // Yellow
            return (Vec3) { randF(0.6, 1.0), randF(0.2, 0.4), randF(0.05, 0.1) };
            break;
        case 2: // Blue
            return (Vec3) { randF(0.05, 0.1), randF(0.15, 0.5), 1.0 };
            break;
        case 3: // White
            return (Vec3) { randF(0.5, 0.7), randF(0.5, 0.7), 1.0 };
            break;
        }
    }
    return (Vec3) { 0.0, 0.0, 0.0 };
}

void App_update(App* app, double dt) {
    app->time += dt;
    
    Universe* universe = &app->universe;
    Camera* camera = Universe_getCamera(universe);
    
    AppKey* keys = app->keys;
    AppMouse mouse = app->mouse;
    
    if (keys[APP_KEYNAME_HIDEGRID].justPressed)
        app->showGrid = !app->showGrid;
    
    if (keys[APP_KEYNAME_SHOWCROSSHAIRS].justPressed)
        app->showCrossHairs = !app->showCrossHairs;
    
    if (keys[APP_KEYNAME_PAUSESIMULATION].justPressed)
        app->doSimulate = !app->doSimulate;
    
    if (keys[APP_KEYNAME_UNDO].justPressed)
        Universe_popState(universe);
    
    if (keys[APP_KEYNAME_DELETE].justPressed) {
        int bodyToDelete = screenCoordsToBodyId(app, mouse.x, mouse.y);
        if (bodyToDelete != -1) {
            Universe_pushState(universe);
            Universe_removeBody(universe, bodyToDelete);
        }
    }
    
    int switchPlanet = 0;
    if (keys[APP_KEYNAME_PREVIOUSPLANET].justPressed)
        switchPlanet--;
    if (keys[APP_KEYNAME_NEXTPLANET].justPressed)
        switchPlanet++;
    if (switchPlanet != 0) {
        int numBodies;
        Body* bodies = Universe_getBodies(universe, &numBodies);
        int currentFocusedId = camera->focusedBodyId;
        int focusOnId = -1;
        if (numBodies > 0) {
            int idx = 0;
            if (currentFocusedId == -1) {
                idx = 0;
            } else {
                for (int i = 0; i < numBodies; i++) {
                    if (bodies[i].id == currentFocusedId) {
                        idx = i;
                        break;
                    }
                }
                idx += switchPlanet;
                if (idx == -1)
                    idx = numBodies - 1;
                else if (idx == numBodies)
                    idx = 0;
            }
            
            Body* body = &bodies[idx];
            focusOnId = body->id;
            double radius = 4 * body->radius;
            camera->focusedBodyId = -1;
            camera->rot.y = 0.0;
            camera->rot.x = -C_PI4;
            camera->pos = Universe_getGlobalPosition(universe, focusOnId, (Vec3) { 0.0, radius, -radius });
        }
        focusOnBody(universe, focusOnId);
    }
    
    if (keys[APP_KEYNAME_SELECTPLANET].justPressed)
        app->selectedBodyType = BODYTYPE_PLANET;
    if (keys[APP_KEYNAME_SELECTSTAR].justPressed)
        app->selectedBodyType = BODYTYPE_STAR;
    if (keys[APP_KEYNAME_SELECTBLACKHOLE].justPressed)
        app->selectedBodyType = BODYTYPE_BLACKHOLE;
    
    if (keys[APP_KEYNAME_CLEAR].justPressed) {
        Universe_pushState(universe);
        Universe_removeAllBodies(universe);
        Camera* removedBodiesCamera = Universe_getCamera(universe);
        Vec3 camPos = removedBodiesCamera->pos;
        camPos.x = 0.0;
        camPos.y = 10.0;
        camPos.z = 0.0;
        removedBodiesCamera->pos = camPos;
    }
    
    double cameraY = fabs(camera->pos.y);
    
    if (keys[APP_KEYNAME_CLEARDISTANT].justPressed) {
        Universe_pushState(universe);
        Universe_removeAllBodiesWithinRadius(universe, Universe_getGlobalPosition(universe, camera->focusedBodyId, camera->pos), C_GRID_RELATIVE_RADIUS * cameraY);
    }
    
    double camSpeed = C_CAMERA_SPEED * cameraY;
    if (camSpeed < C_CAMERA_SPEED)
        camSpeed = C_CAMERA_SPEED;
    camera->vel = (Vec3) { 0.0, 0.0, 0.0 };
    Camera_accelForward(camera, camSpeed * (keys[APP_KEYNAME_MOVEFORWARD].downAmount - keys[APP_KEYNAME_MOVEBACKWARD].downAmount));
    Camera_accelRight(camera, camSpeed * (keys[APP_KEYNAME_MOVERIGHT].downAmount - keys[APP_KEYNAME_MOVELEFT].downAmount));
    Camera_accelUp(camera, camSpeed * (keys[APP_KEYNAME_MOVEUP].downAmount - keys[APP_KEYNAME_MOVEDOWN].downAmount));
    Camera_move(camera, dt);
    
    if (mouse.rightDown) {
        const double sensitivity = 0.001 * C_MOUSE_SENSITIVITY;
        camera->rot.y -= mouse.dx * sensitivity;
        double pitch = camera->rot.x;
        // No delta time here because movement depends on the frequency of mouse events and mouse sensitivity
        pitch -= mouse.dy * sensitivity;
        if (pitch < -C_PI2)
            pitch = -C_PI2;
        else if (pitch > C_PI2)
            pitch = C_PI2;
        camera->rot.x = pitch;
        app->looking = 1;
    } else {
        app->looking = 0;
        if (app->mouse.control) {
            double simSpeed = universe->simSpeed;
            simSpeed *= exp(0.2 * C_MOUSE_SCROLL_SENSITIVITY * app->mouse.scroll);
            if (simSpeed < 0.001)
                simSpeed = 0.001;
            if (simSpeed > 1000.0)
                simSpeed = 1000.0;
            universe->simSpeed = simSpeed;
        } else {
            app->spawnRadius += 0.3 * C_MOUSE_SCROLL_SENSITIVITY * app->mouse.scroll;
        }
    }
    
    app->endSpawnTrajectory = Universe_getLocalPosition(universe, camera->focusedBodyId, screenCoordsToWorldCoords(app, mouse.x, mouse.y));
    if (mouse.leftJustReleased) {
        if (app->startSpawnTrajectory.x != INFINITY && app->endSpawnTrajectory.x != INFINITY && app->spawning && !app->looking) {
            double spawnRadius = getSpawnRadius(app);
            Vec3 spawnDiff = vdiff(app->startSpawnTrajectory, app->endSpawnTrajectory);
            double speed = sqrt(vmag2(spawnDiff));
            double cameraY = fabs(camera->pos.y);
            Vec3 velocity;
            if (speed > 0.0) {
                velocity = vnorm(spawnDiff);
                velocity = vscl(velocity, C_LAUNCH_SPEED * speed * pow(speed / cameraY, 1.0));
            } else {
                velocity = (Vec3) { 0.0, 0.0, 0.0 };
            }
            
            BodyType bodyType = app->selectedBodyType;
            Body body = {
                bodyType,
                app->spawnId++,
                randomColor(bodyType),
                Universe_getGlobalPosition(universe, camera->focusedBodyId, app->startSpawnTrajectory),
                Universe_getGlobalVelocity(universe, camera->focusedBodyId, velocity),
                spawnRadius,
                pow(spawnRadius, 3.0)
            };
            Universe_pushState(universe);
            Universe_addBody(universe, &body);
        }
        app->spawning = 0;
    }
    if (mouse.leftJustPressed && !app->looking) {
        app->startSpawnTrajectory = Universe_getLocalPosition(universe, camera->focusedBodyId, screenCoordsToWorldCoords(app, mouse.x, mouse.y));
        app->spawning = 1;
    }
    
    if (mouse.middleJustPressed) {
        if (mouse.control) {
            universe->simSpeed = 1.0;
        } else {
            Body* oldBody = Universe_getBody(universe, camera->focusedBodyId);
            int newFocusedBodyId = screenCoordsToBodyId(app, mouse.x, mouse.y);
            focusOnBody(universe, newFocusedBodyId);
            if (newFocusedBodyId == -1) {
                Universe_pushState(universe);
                Vec3 velOffset = vscl(camera->vel, -1.0 / universe->simSpeed);
                if (oldBody != NULL) {
                    velOffset = vdiff(velOffset, oldBody->vel);
                }
                Universe_offsetGlobalVelocity(universe, velOffset);
            }
        }
    }
    
    if (Universe_getBody(universe, camera->focusedBodyId) == NULL)
        camera->focusedBodyId = -1;
    
    if (app->doSimulate)
        Universe_update(universe, dt);
    
    App_updateKeys(app, dt);
    App_updateMouse(app, dt);
}

void App_draw(App* app) {
    AppUniforms* appUniforms = &app->appUniforms;
    Universe* universe = &app->universe;
    
    int numBodies;
    Body* bodies = Universe_getBodies(universe, &numBodies);
    
    Camera* camera = Universe_getCamera(universe);
    Vec3 camPos = camera->pos;
    Vec3 camRot = camera->rot;
    mat4x4 projection;
    float aspect = (float) app->width / app->height;
    mat4x4_perspective(projection, (float) C_CAMERA_FOV, aspect, 0.5f, 10000.0f);
    mat4x4 cameraRotate;
    mat4x4_identity(cameraRotate);
    mat4x4_rotate_X(cameraRotate, cameraRotate, (float) -camRot.x);
    mat4x4_rotate_Y(cameraRotate, cameraRotate, (float) -camRot.y);
    mat4x4 cameraTranslate;
    mat4x4_identity(cameraTranslate);
    mat4x4_translate(cameraTranslate, (float) -camPos.x, (float) -camPos.y, (float) -camPos.z);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Render sky box
    
    Framebuffer* framebuffer1 = &app->framebuffer1;
    Framebuffer_use(framebuffer1, (int[]) { 0 }, 1);
    
    glDisable(GL_DEPTH_TEST);
    
    Shader* skyBoxShader = &app->skyBoxShader;
    float skyBoxScaleFactor = 1000.0f;
    mat4x4 skyBoxScale;
    mat4x4_identity(skyBoxScale);
    mat4x4_scale_aniso(skyBoxScale, skyBoxScale, skyBoxScaleFactor, skyBoxScaleFactor, skyBoxScaleFactor);
    mat4x4 mvp;
    mat4x4_identity(mvp);
    mat4x4_mul(mvp, skyBoxScale, mvp);
    mat4x4_mul(mvp, cameraRotate, mvp);
    mat4x4_mul(mvp, projection, mvp);
    Shader_sendMat4(skyBoxShader, appUniforms->skyBoxMvp, mvp);
    
    Texture* skyBoxTexture = &app->skyBoxTexture;
    Texture_use(skyBoxTexture, 0);
    Mesh* skyBoxMesh = &app->skyBoxMesh;
    Shader_use(skyBoxShader);
    Mesh_draw(skyBoxMesh, DRAWMODE_TRIANGLES);
    
    glEnable(GL_DEPTH_TEST);
    
    // Render framebuffer1 to framebuffer2 with black hole distortion effect
    Framebuffer* framebuffer2 = &app->framebuffer2;
    Framebuffer_use(framebuffer2, (int[]) { 0 }, 1);
    glDisable(GL_DEPTH_TEST);
    
    int blackHoleStructSize = 16;
    int blackHolesCapacity = 1;
    int numBlackHoles = 0;
    char* blackHolesData = (char*) malloc(blackHolesCapacity * blackHoleStructSize);
    for (int i = 0; i < numBodies; i++) {
        Body* body = &bodies[i];
        Vec3 bodyPos = Universe_getLocalPosition(universe, camera->focusedBodyId, body->pos);
        if (body->type == BODYTYPE_BLACKHOLE) {
            int blackHoleIdx = numBlackHoles * blackHoleStructSize;
            
            vec4 tmpVec4;
            vec4 blackHolePosVec4 = {
                (float) bodyPos.x,
                (float) bodyPos.y,
                (float) bodyPos.z, 
                1.0f
            };
            mat4x4_mul_vec4(tmpVec4, cameraTranslate, blackHolePosVec4);
            vec4_dup(blackHolePosVec4, tmpVec4);
            mat4x4_mul_vec4(tmpVec4, cameraRotate, blackHolePosVec4);
            vec4_dup(blackHolePosVec4, tmpVec4);
            mat4x4_mul_vec4(tmpVec4, projection, blackHolePosVec4);
            vec4_dup(blackHolePosVec4, tmpVec4);
            float oneOverW = 1.0f / blackHolePosVec4[3];
            blackHolePosVec4[0] *= oneOverW * aspect;
            blackHolePosVec4[1] *= oneOverW;
            float blackHoleRadius = (float) body->radius * projection[1][1] * oneOverW;
            if (blackHoleRadius < 0.0f)
                blackHoleRadius = 0.0f;
            
            memcpy(&blackHolesData[blackHoleIdx], &blackHolePosVec4[0], 8);
            memcpy(&blackHolesData[blackHoleIdx + 8], &blackHoleRadius, 4);
            numBlackHoles++;
            if (numBlackHoles == blackHolesCapacity) {
                blackHolesCapacity *= 2;
                blackHolesData = (char*) realloc(blackHolesData, blackHolesCapacity * blackHoleStructSize);
            }
        }
    }
    SSBO* blackHolesSSBO = &app->blackHolesSSBO;
    SSBO_sendData(blackHolesSSBO, blackHolesData, numBlackHoles * blackHoleStructSize);
    free(blackHolesData);
    SSBO_bindBase(blackHolesSSBO, 0);
    Shader* blackHoleShader = &app->blackHoleShader;
    Shader_use(blackHoleShader);
    Shader_sendInt(blackHoleShader, appUniforms->blackHoleNum, numBlackHoles);
    Shader_sendFloat(blackHoleShader, appUniforms->blackHoleAspect, aspect);
    
    Texture_use(&framebuffer1->colorBuffers[0], 0);
    
    Mesh* screenMesh = &app->screenMesh;
    Mesh_draw(screenMesh, DRAWMODE_TRIANGLES);
    
    glEnable(GL_DEPTH_TEST);
    
    // Render planets
    
    Framebuffer_use(framebuffer2, (int[]) { 1 }, 1);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Framebuffer_use(framebuffer2, (int[]) { 0, 1 }, 2);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    Shader* planetShader = &app->planetShader;
    Shader_use(planetShader);
    Mesh* planetMesh = &app->planetMesh;
    Mesh* blackHoleMesh = &app->blackHoleMesh;
    mat4x4_identity(mvp);
    mat4x4_mul(mvp, cameraTranslate, mvp);
    mat4x4 mv;
    mat4x4_dup(mv, mvp);
    mat4x4_mul(mvp, cameraRotate, mvp);
    mat4x4_mul(mvp, projection, mvp);
    
    for (int i = 0; i < numBodies; i++) {
        Body* body = &bodies[i];
        BodyType type = body->type;
        Vec3 bodyColor = body->color;
        Vec3 bodyPos = body->pos;
        Vec3 bodyPosVec3 = Universe_getLocalPosition(universe, camera->focusedBodyId, bodyPos);
        double bodyRadius = body->radius;
        
        mat4x4 bodyTranslate;
        mat4x4_translate(bodyTranslate, (float) bodyPosVec3.x, (float) bodyPosVec3.y, (float) bodyPosVec3.z);
        mat4x4 bodyScale;
        mat4x4_identity(bodyScale);
        mat4x4_scale_aniso(bodyScale, bodyScale, (float) bodyRadius, (float) bodyRadius, (float) bodyRadius);
        
        mat4x4 bodyTransform;
        mat4x4_identity(bodyTransform);
        if (type == BODYTYPE_BLACKHOLE) {
            mat4x4 bodyRotate;
            mat4x4_identity(bodyRotate);
            mat4x4_rotate_Y(bodyRotate, bodyRotate, (float) camRot.y);
            mat4x4_rotate_X(bodyRotate, bodyRotate, (float) camRot.x);
            mat4x4_mul(bodyTransform, bodyRotate, bodyTransform);
        }
        mat4x4_mul(bodyTransform, bodyScale, bodyTransform);
        mat4x4_mul(bodyTransform, bodyTranslate, bodyTransform);
        mat4x4 bodyMvp;
        mat4x4_dup(bodyMvp, bodyTransform);
        mat4x4_mul(bodyMvp, mvp, bodyMvp);
        Shader_sendMat4(planetShader, appUniforms->planetMvp, bodyMvp);
        
        mat4x4 bodyMv;
        mat4x4_dup(bodyMv, bodyTransform);
        mat4x4_mul(bodyMv, mv, bodyMv);
        Shader_sendMat4(planetShader, appUniforms->planetMv, bodyMv);
        
        Shader_sendVec3(planetShader, appUniforms->planetPlanetColor, (float) bodyColor.x, (float) bodyColor.y, (float) bodyColor.z);
        Shader_sendInt(planetShader, appUniforms->planetType, (int) type);
        double camDist = sqrt(vmag2(vdiff(bodyPosVec3, camera->pos)));
        Shader_sendFloat(planetShader, appUniforms->planetCamDistRadius, (float) (camDist / bodyRadius));
        
        if (type == BODYTYPE_BLACKHOLE) {
            Mesh_draw(blackHoleMesh, DRAWMODE_TRIANGLES);
        } else {
            Mesh_draw(planetMesh, DRAWMODE_TRIANGLES);
        }
    }
    
    Framebuffer_use(framebuffer2, (int[]) { 0 }, 1);
    
    Shader* lineShader = &app->lineShader;
    if (!app->looking) {
        switch (app->selectedBodyType) {
        case BODYTYPE_PLANET:
            Shader_sendVec4(lineShader, appUniforms->lineColor, 1.0f, 1.0f, 0.0f, 1.0f);
            break;
        case BODYTYPE_STAR:
            Shader_sendVec4(lineShader, appUniforms->lineColor, 0.0f, 1.0f, 1.0f, 1.0f);
            break;
        case BODYTYPE_BLACKHOLE:
            Shader_sendVec4(lineShader, appUniforms->lineColor, 1.0f, 0.0f, 1.0f, 1.0f);
            break;
        }
        
        // Render spawn trajectory line
        
        if (app->spawning) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            mat4x4_identity(mvp);
            mat4x4_mul(mvp, cameraTranslate, mvp);
            mat4x4_mul(mvp, cameraRotate, mvp);
            mat4x4_mul(mvp, projection, mvp);
            Shader_sendMat4(lineShader, appUniforms->lineMvp, mvp);
            
            Mesh* spawnTrajectoryMesh = &app->spawnTrajectoryMesh;
            Vec3 sst = app->startSpawnTrajectory;
            Vec3 est = app->endSpawnTrajectory;
            float spawnTrajectoryVerts[] = {
                (float) sst.x, (float) sst.y, (float) sst.z,
                (float) est.x, (float) est.y, (float) est.z
            };
            Mesh_sendVertices(spawnTrajectoryMesh, spawnTrajectoryVerts, 2);
            Shader_use(lineShader);
            Mesh_draw(spawnTrajectoryMesh, DRAWMODE_LINESTRIP);
            
            glDisable(GL_BLEND);
        }
    }
    
    // Render grid
    
    if (app->showGrid) {
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        Mesh* gridMesh = &app->gridMesh;
        GridInfo* gridInfo = &app->gridInfo;
        GridUniforms* gridUniforms = &app->gridUniforms;
        
        gridInfo->view = camPos;
        
        double cameraY = fabs(camPos.y);
        if (cameraY < 1.0) cameraY = 1.0;
        gridInfo->radius = C_GRID_RELATIVE_RADIUS * cameraY;
        double ln2 = log(2.0);
        int exponent = log(cameraY) / ln2;
        gridInfo->spacing = C_GRID_SPACING * exp(ln2 * exponent);
        
        Shader* gridShader = &app->gridShader;
        mat4x4_identity(mvp);
        mat4x4_mul(mvp, cameraTranslate, mvp);
        mat4x4_mul(mvp, cameraRotate, mvp);
        mat4x4_mul(mvp, projection, mvp);
        Shader_sendMat4(gridShader, appUniforms->gridMvp, mvp);
        
        int bodiesDataStructSize = 32;
        int bodiesDataSize = numBodies * bodiesDataStructSize;
        char* bodiesData = (char*) malloc(bodiesDataSize);
        char* bodiesDataPtr = bodiesData;
        for (int i = 0; i < numBodies; i++) {
            Body* body = &bodies[i];
            Vec3 bodyPos = body->pos;
            
            int bodyTypeInt = (int) body->type;
            float bodyRadiusFloat = (float) body->radius;
            Vec3 bodyPosVec3 = Universe_getLocalPosition(universe, camera->focusedBodyId, bodyPos);
            float bodyPosFloat3[] = { (float) bodyPosVec3.x, (float) bodyPosVec3.y, (float) bodyPosVec3.z };
            memcpy(&bodiesDataPtr[0], &bodyTypeInt, 4);
            memcpy(&bodiesDataPtr[4], &bodyRadiusFloat, 4);
            memcpy(&bodiesDataPtr[16], &bodyPosFloat3[0], 12);
            bodiesDataPtr += bodiesDataStructSize;
        }
        Shader_sendInt(gridShader, appUniforms->gridBodiesSize, numBodies);
        
        SSBO* gridBodiesSSBO = &app->gridBodiesSSBO;
        SSBO_bindBase(gridBodiesSSBO, 0);
        SSBO_sendData(gridBodiesSSBO, bodiesData, bodiesDataSize);
        free(bodiesData);
        
        Shader_use(gridShader);
        drawGrid(gridShader, gridMesh, gridInfo, gridUniforms);
        
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }
    
    glDisable(GL_DEPTH_TEST);
    
    // Render vertical pass of gaussian blur from framebuffer2 to framebuffer1
    Framebuffer_use(framebuffer1, (int[]) { 1 }, 1);
    Shader* gaussianBlurShader = &app->gaussianBlurShader;
    Shader_use(gaussianBlurShader);
    Shader_sendInt(gaussianBlurShader, appUniforms->gaussianBlurPass, 0);
    Shader_sendFloat(gaussianBlurShader, appUniforms->gaussianBlurAspect, aspect);
    Texture_use(&framebuffer2->colorBuffers[1], 0);
    Mesh_draw(screenMesh, DRAWMODE_TRIANGLES);
    
    // Render horizontal pass of gaussian blur from framebuffer1 to framebuffer2
    Framebuffer_use(framebuffer2, (int[]) { 1 }, 1);
    Shader_use(gaussianBlurShader);
    Shader_sendInt(gaussianBlurShader, appUniforms->gaussianBlurPass, 1);
    Shader_sendFloat(gaussianBlurShader, appUniforms->gaussianBlurAspect, aspect);
    Texture_use(&framebuffer1->colorBuffers[1], 0);
    Mesh_draw(screenMesh, DRAWMODE_TRIANGLES);
    
    // Apply gaussian blur from framebuffer2 to framebuffer1
    Framebuffer_use(framebuffer1, (int[]) { 0 }, 1);
    Shader* starGlowShader = &app->starGlowShader;
    Shader_use(starGlowShader);
    Texture_use(&framebuffer2->colorBuffers[0], 0);
    Texture_use(&framebuffer2->colorBuffers[1], 1);
    Mesh_draw(screenMesh, DRAWMODE_TRIANGLES);
    
    // Render spawn radius circle
    if (!app->looking) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        float spawnRadiusF = (float) getSpawnRadius(app);
        Vec3 spawnRadiusPos = app->endSpawnTrajectory;
        float spawnRadiusX = (float) spawnRadiusPos.x;
        float spawnRadiusZ = (float) spawnRadiusPos.z;
        mat4x4_identity(mvp);
        mat4x4 spawnRadiusScale;
        mat4x4_identity(spawnRadiusScale);
        mat4x4_scale_aniso(spawnRadiusScale, spawnRadiusScale, spawnRadiusF, spawnRadiusF, spawnRadiusF);
        mat4x4 spawnRadiusRotate;
        mat4x4_identity(spawnRadiusRotate);
        mat4x4_rotate_Y(spawnRadiusRotate, spawnRadiusRotate, (float) camRot.y);
        mat4x4_rotate_X(spawnRadiusRotate, spawnRadiusRotate, (float) camRot.x);
        mat4x4 spawnRadiusTranslate;
        mat4x4_translate(spawnRadiusTranslate, spawnRadiusX, 0.0f, spawnRadiusZ);
        mat4x4_mul(mvp, spawnRadiusScale, mvp);
        mat4x4_mul(mvp, spawnRadiusRotate, mvp);
        mat4x4_mul(mvp, spawnRadiusTranslate, mvp);
        mat4x4_mul(mvp, cameraTranslate, mvp);
        mat4x4_mul(mvp, cameraRotate, mvp);
        mat4x4_mul(mvp, projection, mvp);
        Shader_sendMat4(lineShader, appUniforms->lineMvp, mvp);
        
        Shader_use(lineShader);
        Mesh* spawnRadiusMesh = &app->spawnRadiusMesh;
        Mesh_draw(spawnRadiusMesh, DRAWMODE_LINESTRIP);
        
        glDisable(GL_BLEND);
    }
    
    // Render crosshairs
    if (app->showCrossHairs) {
        for (int i = 0; i < numBodies; i++) {
            Body* body = &bodies[i];
            Vec3 bodyPos = Universe_getLocalPosition(universe, camera->focusedBodyId, body->pos);
            
            float camDist = 0.02f * (float) sqrt(vmag2(vdiff(bodyPos, camera->pos)));
            mat4x4_identity(mvp);
            mat4x4 crossHairScale;
            mat4x4_identity(crossHairScale);
            mat4x4_scale_aniso(crossHairScale, crossHairScale, camDist, camDist, camDist);
            mat4x4 crossHairRotate;
            mat4x4_identity(crossHairRotate);
            mat4x4_rotate_Y(crossHairRotate, crossHairRotate, (float) camRot.y);
            mat4x4_rotate_X(crossHairRotate, crossHairRotate, (float) camRot.x);
            mat4x4 crossHairTranslate;
            mat4x4_translate(crossHairTranslate, (float) bodyPos.x, 0.0f, (float) bodyPos.z);
            mat4x4_mul(mvp, crossHairScale, mvp);
            mat4x4_mul(mvp, crossHairRotate, mvp);
            mat4x4_mul(mvp, crossHairTranslate, mvp);
            mat4x4_mul(mvp, cameraTranslate, mvp);
            mat4x4_mul(mvp, cameraRotate, mvp);
            mat4x4_mul(mvp, projection, mvp);
            Shader_sendMat4(lineShader, appUniforms->lineMvp, mvp);
            
            Shader_use(lineShader);
            Shader_sendVec4(lineShader, appUniforms->lineColor, 1.0f, 1.0f, 0.0f, 1.0f);
            Mesh* crossHairMesh = &app->crossHairMesh;
            Mesh_draw(crossHairMesh, DRAWMODE_LINESTRIP);
        }
    }
    
    // Render final result from framebuffer2 to screen
    
    Framebuffer_unuse();
    
    Shader* finalRenderShader = &app->finalRenderShader;
    Shader_use(finalRenderShader);
    Texture_use(&framebuffer1->colorBuffers[0], 0);
    
    Mesh_draw(screenMesh, DRAWMODE_TRIANGLES);
    
    glEnable(GL_DEPTH_TEST);
}