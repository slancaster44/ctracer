#include "ray.h"

Tuple3 RayPosition(Ray r, double pos) {
    return TupleAdd(r.origin, TupleScalarMultiply(r.direction, pos));
}

Ray RayTransform(Ray r, Matrix4x4 transformation) {
    r.origin = MatrixTupleMultiply(transformation, r.origin);
    r.direction = MatrixTupleMultiply(transformation, r.direction);
    return r;
}

Ray NewRay(Tuple3 origin, Tuple3 direction) {
    Ray r;
    r.origin = origin;
    r.direction = direction;
    return r;
}