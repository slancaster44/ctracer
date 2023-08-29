#include "ray.h"

Tuple3 RayPosition(Ray r, float pos) {
    return TupleAdd(r.origin, TupleScalarMultiply(r.direction, pos));
}

Ray RayTransform(Ray r, Matrix4x4 transformation) {
    Ray result = {
        origin: MatrixTupleMultiply(transformation, r.origin),
        direction: MatrixTupleMultiply(transformation, r.direction),
    };

    return result;
}