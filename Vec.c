#include "Vec.h"

#include <math.h>

Vec3 vsum(Vec3 a, Vec3 b) {
    return (Vec3) { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vec3 vdiff(Vec3 a, Vec3 b) {
    return (Vec3) { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vec3 vscl(Vec3 a, double s) {
    return (Vec3) { s * a.x, s * a.y, s * a.z };
}

double vdot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double vmag2(Vec3 a) {
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

Vec3 vnorm(Vec3 a) {
    double oneOverLen = 1.0 / sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    return (Vec3) { oneOverLen * a.x, oneOverLen * a.y, oneOverLen * a.z };
}

Vec3 vrotxy(Vec3 v, double c, double s) {
    return (Vec3) { v.x * c - v.y * s, v.x * s + v.y * c, v.z };
}

Vec3 vrotyz(Vec3 v, double c, double s) {
    return (Vec3) { v.x, v.y * c - v.z * s, v.y * s + v.z * c };
}

Vec3 vrotzx(Vec3 v, double c, double s) {
    return (Vec3) { v.z * s + v.x * c, v.y, v.z * c - v.x * s };
}

static RayTrace nullRayTrace() {
    return (RayTrace) { INFINITY, (Vec3) { INFINITY, INFINITY, INFINITY } };
}

RayTrace raySphere(Vec3 o, Vec3 u, Vec3 c, double r) {
    Vec3 oc = vdiff(o, c);
    double oc2 = vmag2(oc);
    double udotoc = vdot(u, oc);
    double udotoc2 = udotoc * udotoc;
    double r2 = r * r;
    
    double del = udotoc2 - oc2 + r2;
    if (del < 0.0)
        return nullRayTrace();
    
    double sqrtDel = sqrt(del);
    double d = -udotoc - sqrtDel;
    if (d < 0.0) {
        d = -udotoc + sqrtDel;
        if (d < 0.0)
            return nullRayTrace();
    }
    
    return (RayTrace) { d, vsum(o, vscl(u, d)) };
}

RayTrace rayPlaneY0(Vec3 o, Vec3 u, double maxDistance) {
    double d = -o.y / u.y;
    if (d < 0.0 || d > maxDistance)
        return nullRayTrace();
    RayTrace result = (RayTrace) { d, vsum(o, vscl(u, d)) };
    result.pos.y = 0.0;
    return result;
}