#include "Universe.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

void UniverseState_create(UniverseState* state) {
    state->bodiesSize = 0;
    state->bodiesCapacity = 1;
    state->bodies = (Body*) malloc(state->bodiesCapacity * sizeof(Body));
    Camera_create(&state->camera);
    state->dtSum = 0.0;
}

void UniverseState_destroy(UniverseState* state) {
    if (state == NULL)
        return;
    
    free(state->bodies);
}

void UniverseState_clone(UniverseState* state, UniverseState* clone) {
    int bodiesSize = state->bodiesSize;
    int bodiesCapacity = state->bodiesCapacity;
    
    *clone = *state;
    clone->bodies = (Body*) malloc(bodiesCapacity * sizeof(Body));
    memcpy(clone->bodies, state->bodies, bodiesSize * sizeof(Body));
}

void UniverseState_pushBody(UniverseState* state, Body* body) {
    int size = state->bodiesSize;
    int capacity = state->bodiesCapacity;
    Body* bodies = state->bodies;
    if (size == capacity) {
        capacity *= 2;
        bodies = (Body*) realloc(bodies, capacity * sizeof(Body));
    }
    bodies[size++] = *body;
    state->bodiesSize = size;
    state->bodiesCapacity = capacity;
    state->bodies = bodies;
}

void UniverseState_popBody(UniverseState* state, int id) {
    int size = state->bodiesSize;
    int capacity = state->bodiesCapacity;
    Body* bodies = state->bodies;
    
    int i = 0;
    while (i < size) {
        if (bodies[i].id == id) {
            for (int j = i + 1; j < size; j++) {
                bodies[j - 1] = bodies[j];
            }
            size--;
            break;
        } else {
            i++;
        }
    }
    
    if (2 * size < capacity && capacity > 1)
        capacity /= 2;
    bodies = (Body*) realloc(bodies, capacity * sizeof(Body));
    state->bodiesSize = size;
    state->bodiesCapacity = capacity;
    state->bodies = bodies;
}

void UniverseState_popAllBodies(UniverseState* state) {
    int bodiesSize = 0;
    int bodiesCapacity = 1;
    state->bodies = (Body*) realloc(state->bodies, bodiesCapacity * sizeof(Body));
    state->bodiesSize = bodiesSize;
    state->bodiesCapacity = bodiesCapacity;
}

Body* UniverseState_getBody(UniverseState* state, int id) {
    int size = state->bodiesSize;
    Body* bodies = state->bodies;
    if (id == -1)
        return NULL;
    for (int i = 0; i < size; i++) {
        if (bodies[i].id == id) {
            return &bodies[i];
        }
    }
    return NULL;
}

void Universe_create(Universe* universe, double g, double timestep, double attractionMatrix[3][3]) {
    universe->oldestIdx = 0;
    universe->newestIdx = 0;
    UniverseState_create(&universe->states[universe->newestIdx]);
    universe->timestep = timestep;
    universe->g = g;
    universe->simSpeed = 1.0;
    memcpy(&universe->attractionMatrix[0][0], &attractionMatrix[0][0], 9 * sizeof(double));
}

void Universe_destroy(Universe* universe) {
    if (universe == NULL)
        return;
    
    for (int i = universe->oldestIdx; i <= universe->newestIdx; i++) {
        UniverseState_destroy(&universe->states[i]);
    }
}

void Universe_update(Universe* universe, double dt) {
    const double simSpeed = universe->simSpeed;
    const double timestep = universe->timestep;
    const double g = universe->g;
    const double timestepGSimSpeed = timestep * g * simSpeed;
    const double timeStepSimSpeed = timestep * simSpeed;
    
    UniverseState* state = &universe->states[universe->newestIdx];
    Body* bodies = state->bodies;
    int bodiesSize = state->bodiesSize;
    double dtSum = state->dtSum;
    dtSum += dt;
    
    double (*attractionMatrix)[3] = &universe->attractionMatrix[0];
    
    while (dtSum >= timestep) {
        for (int i = 0; i < bodiesSize; i++) {
            Body* body1 = &bodies[i];
            int type1 = (int) body1->type;
            double radius1 = body1->radius;
            double mass1 = body1->mass;
            Vec3 pos1 = body1->pos;
            for (int j = i + 1; j < bodiesSize; j++) {
                Body* body2 = &bodies[j];
                int type2 = (int) body2->type;
                double radius2 = body2->radius;
                double mass2 = body2->mass;
                Vec3 pos2 = body2->pos;
                
                Vec3 displacement = vdiff(pos2, pos1);
                double r2 = vmag2(displacement);
                if (fabs(r2) == 0.0)
                    continue;
                double r = sqrt(r2);
                double radiusSum = radius1 + radius2;
                if (r < radiusSum) {
                    displacement = vscl(displacement, radiusSum / r);
                    r = radiusSum;
                    r2 = r * r;
                }
                
                double forceScale = timestepGSimSpeed / (r2 * r);
                Vec3 accelVec1 = vscl(displacement, forceScale * mass2 * attractionMatrix[type2][type1]);
                Vec3 accelVec2 = vscl(displacement, -1.0 * forceScale * mass1 * attractionMatrix[type1][type2]);
                body1->vel = vsum(body1->vel, accelVec1);
                body2->vel = vsum(body2->vel, accelVec2);
            }
            
            Vec3 newPos = vsum(body1->pos, vscl(body1->vel, timeStepSimSpeed));
            newPos.y = 0.0;
            body1->pos = newPos;
        }
        
        dtSum -= timestep;
    }
    
    state->dtSum = dtSum;
}

void Universe_pushState(Universe* universe) {
    int oldestIdx = universe->oldestIdx;
    int newestIdx = universe->newestIdx;
    int newestIdx2 = newestIdx + 1;
    
    if (newestIdx2 - oldestIdx == MAX_STATES) {
        Universe_popOldestState(universe);
        oldestIdx = universe->oldestIdx;
        newestIdx = universe->newestIdx;
        newestIdx2 = newestIdx + 1;
    }
    if (newestIdx2 == STATES_CAPACITY) {
        Universe_packStates(universe);
        oldestIdx = universe->oldestIdx;
        newestIdx = universe->newestIdx;
        newestIdx2 = newestIdx + 1;
    }
    
    UniverseState* state = &universe->states[newestIdx2];
    UniverseState_clone(&universe->states[newestIdx], state);
    universe->newestIdx = newestIdx2;
}

void Universe_popState(Universe* universe) {
    int newestIdx = universe->newestIdx;
    int newestIdx2 = newestIdx - 1;
    int oldestIdx = universe->oldestIdx;
    
    if (newestIdx == oldestIdx)
        return;
    
    UniverseState* state = &universe->states[newestIdx];
    UniverseState_destroy(state);
    universe->newestIdx = newestIdx2;
}

void Universe_popOldestState(Universe* universe) {
    int oldestIdx = universe->oldestIdx;
    int oldestIdx2 = oldestIdx + 1;
    int newestIdx = universe->newestIdx;
    
    if (newestIdx == oldestIdx)
        return;
    
    UniverseState_destroy(&universe->states[oldestIdx]);
    universe->oldestIdx = oldestIdx2;
}

void Universe_packStates(Universe* universe) {
    int oldestIdx = universe->oldestIdx;
    int newestIdx = universe->newestIdx;
    int newestIdx2 = newestIdx - oldestIdx;
    UniverseState* states = universe->states;
    
    for (int i = 0, j = oldestIdx; j <= newestIdx; i++, j++) {
        states[i] = states[j];
    }
    
    universe->oldestIdx = 0;
    universe->newestIdx = newestIdx2;
}

void Universe_offsetGlobalVelocity(Universe* universe, Vec3 vel) {
    vel.y = 0.0;
    int bodiesSize = 0;
    Body* bodies = Universe_getBodies(universe, &bodiesSize);
    for (int i = 0; i < bodiesSize; i++) {
        Body* body = &bodies[i];
        body->vel = vsum(body->vel, vel);
    }
}

void Universe_addBody(Universe* universe, Body* body) {
    UniverseState* state = &universe->states[universe->newestIdx];
    UniverseState_pushBody(state, body);
}

void Universe_removeBody(Universe* universe, int id) {
    UniverseState* state = &universe->states[universe->newestIdx];
    UniverseState_popBody(state, id);
}

void Universe_removeAllBodies(Universe* universe) {
    UniverseState* state = &universe->states[universe->newestIdx];
    UniverseState_popAllBodies(state);
}

void Universe_removeAllBodiesWithinRadius(Universe* universe, Vec3 center, double radius) {
    center.y = 0.0;
    double radius2 = radius * radius;
    int bodiesSize = 0;
    Body* bodies = Universe_getBodies(universe, &bodiesSize);
    int i = 0;
    while (i < bodiesSize) {
        Body* body = &bodies[i];
        double dist2 = vmag2(vdiff(center, body->pos));
        if (dist2 > radius2) {
            Universe_removeBody(universe, body->id);
            bodies = Universe_getBodies(universe, &bodiesSize);
        } else {
            i++;
        }
    }
}

Body* Universe_getBody(Universe* universe, int id) {
    UniverseState* state = &universe->states[universe->newestIdx];
    return UniverseState_getBody(state, id);
}

Body* Universe_getBodies(Universe* universe, int* bodiesSize) {
    UniverseState* state = &universe->states[universe->newestIdx];
    *bodiesSize = state->bodiesSize;
    return state->bodies;
}

Camera* Universe_getCamera(Universe* universe) {
    UniverseState* state = &universe->states[universe->newestIdx];
    return &state->camera;
}

Vec3 Universe_rayTraceMousePlane(Universe* universe, double x, double y, double width, double height, double fov) {
    UniverseState* state = &universe->states[universe->newestIdx];
    Camera* camera = &state->camera;
    
    Vec3 ray = Camera_pixelToRay(camera, x, y, width, height, fov);
    
    return rayPlaneY0(Universe_getGlobalPosition(universe, camera->focusedBodyId, camera->pos), ray, INFINITY).pos;
}

int Universe_rayTraceMouseBodies(Universe* universe, double x, double y, double width, double height, double fov) {
    UniverseState* state = &universe->states[universe->newestIdx];
    int bodiesSize = state->bodiesSize;
    Body* bodies = state->bodies;
    Camera* camera = &state->camera;
    Vec3 camPos = Universe_getGlobalPosition(universe, camera->focusedBodyId, camera->pos);
    Vec3 ray = Camera_pixelToRay(camera, x, y, width, height, fov);
    
    int id = -1;
    double closestD = INFINITY;
    for (int i = 0; i < bodiesSize; i++) {
        Body* body = &bodies[i];
        RayTrace trace = raySphere(camPos, ray, body->pos, body->radius);
        if (trace.d < closestD) {
            id = body->id;
            closestD = trace.d;
        }
    }
    
    return id;
}

Vec3 Universe_getLocalPosition(Universe* universe, int bodyId, Vec3 pos) {
    if (bodyId >= 0) {
        Body* body = Universe_getBody(universe, bodyId);
        if (body != NULL) {
            pos = vdiff(pos, body->pos);
        }
    }
    return pos;
}

Vec3 Universe_getGlobalPosition(Universe* universe, int bodyId, Vec3 pos) {
    if (bodyId >= 0) {
        Body* body = Universe_getBody(universe, bodyId);
        if (body != NULL) {
            pos = vsum(pos, body->pos);
        }
    }
    return pos;
}

Vec3 Universe_getGlobalVelocity(Universe* universe, int bodyId, Vec3 vel) {
    if (bodyId >= 0) {
        Body* body = Universe_getBody(universe, bodyId);
        if (body != NULL) {
            vel = vsum(vel, body->vel);
        }
    }
    return vel;
}