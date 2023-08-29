#ifndef SHAPE_H
#define SHAPE_H

#include "tuple.h"
#include "ray.h"

typedef enum {
    SPHERE,
} SHAPE_TYPE;

typedef struct {
    Tuple3 center_point;
    float radius;
} Sphere;

typedef struct {
    SHAPE_TYPE type;
    union {
        Sphere* sphere;
    } as;
} Shape;

void ConstructSphere(Shape* s, Tuple3 center_point, float radius);
void DeconstructShape(Shape* s);
Tuple3 Intersect(Shape s, Ray r);

#endif