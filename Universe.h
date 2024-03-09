#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "Vec.h"
#include "Camera.h"
#include "Constants.h"

typedef enum BodyType {
    BODYTYPE_PLANET = 0,
    BODYTYPE_STAR,
    BODYTYPE_BLACKHOLE
} BodyType;

typedef struct Body {
    BodyType type;
    int id;
    Vec3 color;
    Vec3 pos;
    Vec3 vel;
    double radius;
    double mass;
} Body;

typedef struct UniverseState {
    int bodiesSize;
    int bodiesCapacity;
    Body* bodies;
    Camera camera;
    double dtSum;
} UniverseState;

void UniverseState_create(UniverseState* state);

void UniverseState_destroy(UniverseState* state);

void UniverseState_clone(UniverseState* state, UniverseState* clone);

void UniverseState_pushBody(UniverseState* state, Body* body);

void UniverseState_popBody(UniverseState* state, int id);

void UniverseState_popAllBodies(UniverseState* state);

Body* UniverseState_getBody(UniverseState* state, int id);

#define STATES_CAPACITY 100
#define MAX_STATES 50

typedef struct Universe {
    UniverseState states[STATES_CAPACITY];
    int oldestIdx;
    int newestIdx;
    double timestep;
    double g;
    double simSpeed;
    double attractionMatrix[3][3];
} Universe;

void Universe_create(Universe* universe, double g, double timestep, double attractionMatrix[3][3]);

void Universe_destroy(Universe* universe);

void Universe_update(Universe* universe, double dt);

void Universe_pushState(Universe* universe);

void Universe_popState(Universe* universe);

void Universe_popOldestState(Universe* universe);

void Universe_packStates(Universe* universe);

void Universe_offsetGlobalVelocity(Universe* universe, Vec3 vel);

void Universe_addBody(Universe* universe, Body* body);

void Universe_removeBody(Universe* universe, int id);

void Universe_removeAllBodies(Universe* universe);

void Universe_removeAllBodiesWithinRadius(Universe* universe, Vec3 center, double radius);

Body* Universe_getBody(Universe* universe, int id);

Body* Universe_getBodies(Universe* universe, int* bodiesSize);

Camera* Universe_getCamera(Universe* universe);

Vec3 Universe_rayTraceMousePlane(Universe* universe, double x, double y, double width, double height, double fov);

int Universe_rayTraceMouseBodies(Universe* universe, double x, double y, double width, double height, double fov);

Vec3 Universe_getLocalPosition(Universe* universe, int bodyId, Vec3 pos);

Vec3 Universe_getGlobalPosition(Universe* universe, int bodyId, Vec3 pos);

Vec3 Universe_getGlobalVelocity(Universe* universe, int bodyId, Vec3 vel);

#endif