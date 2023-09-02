#include "ray.h"

Tuple3 RayPosition(Ray r, float pos) {
    return TupleAdd(r.origin, TupleScalarMultiply(r.direction, pos));
}

Ray RayTransform(Ray r, Matrix4x4 transformation) {
    r.origin = MatrixTupleMultiply(transformation, r.origin);
    r.direction = MatrixTupleMultiply(transformation, r.direction);
    return r;
}

void ConstructRay(Ray* r, Tuple3 origin, Tuple3 direction) {
    r->origin = origin;
    r->direction = direction;
}