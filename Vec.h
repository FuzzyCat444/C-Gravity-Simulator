#ifndef VEC_H
#define VEC_H

typedef struct Vec3 {
    double x, y, z;
} Vec3;

Vec3 vsum(Vec3 a, Vec3 b);

Vec3 vdiff(Vec3 a, Vec3 b);

Vec3 vscl(Vec3 a, double s);

double vdot(Vec3 a, Vec3 b);

double vmag2(Vec3 a);

Vec3 vnorm(Vec3 a);

Vec3 vrotxy(Vec3 v, double c, double s);

Vec3 vrotyz(Vec3 v, double c, double s);

Vec3 vrotzx(Vec3 v, double c, double s);

typedef struct RayTrace {
    double d;
    Vec3 pos;
} RayTrace;

RayTrace raySphere(Vec3 o, Vec3 u, Vec3 c, double r);

RayTrace rayPlaneY0(Vec3 o, Vec3 u, double maxDistance);

#endif