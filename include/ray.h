#ifndef RAY_H
#define RAY_H

#include "tuple.h"
#include "matrix.h"

typedef struct {
    Tuple3 origin;
    Tuple3 direction;
} Ray;

Ray NewRay(Tuple3 origin, Tuple3 direction);
Tuple3 RayPosition(Ray r, double pos);
Ray RayTransform(Ray r, Matrix4x4 transformation);

#endif