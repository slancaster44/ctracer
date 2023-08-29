#include "ray.h"

Tuple3 RayPosition(Ray r, float pos) {
    return TupleAdd(r.origin, TupleScalarMultiply(r.direction, pos));
}