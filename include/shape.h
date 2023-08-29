#ifndef SHAPE_H
#define SHAPE_H

#include "tuple.h"
#include "ray.h"
#include "matrix.h"

#define MAX_NUMBER_INTERSECTIONS 2

typedef enum {
    SPHERE,
} SHAPE_TYPE;

typedef struct {
    Tuple3 center_point;
} Sphere;

typedef struct {
    Matrix4x4 transformation;

    SHAPE_TYPE type;
    union {
        Sphere sphere;
    } as;
} Shape;

typedef struct {
    Shape shape;
    Ray ray;
    float ray_times[MAX_NUMBER_INTERSECTIONS];
    int count;
} Intersections;

void ConstructSphere(Shape* s, Tuple3 center_points);
void DeconstructShape(Shape* s);
Intersections Intersect(Shape s, Ray r);

#endif