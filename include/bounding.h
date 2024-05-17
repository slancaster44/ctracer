#ifndef BOUNDING_H
#define BOUNDING_H

#include "tuple.h"
#include "shape.h"

typedef struct {
    Tuple3 minimum_bound;
    Tuple3 maximum_bound;
    Shape as_cube;
} Bounds;

Bounds ShapeBounds(Shape* s);
Bounds TransformBounds(Bounds b, Matrix4x4 m);
bool IsInBounds(Bounds b, Ray r);
Tuple3 Centroid(Bounds b);


#endif