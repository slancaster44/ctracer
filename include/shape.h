#ifndef SHAPE_H
#define SHAPE_H

#include "tuple.h"
#include "ray.h"
#include "matrix.h"
#include "material.h"

#define MAX_NUMBER_INTERSECTIONS 2

typedef enum {
    SPHERE,
} SHAPE_TYPE;


typedef struct {
    Matrix4x4 transformation;
    Matrix4x4 inverse_transform;

    Material material;

    SHAPE_TYPE type;
} Shape;

typedef struct {
    Shape shape;
    Ray ray;
    float ray_times[MAX_NUMBER_INTERSECTIONS];
    int count;
} Intersections;

void ApplyTransformation(Shape* s, Matrix4x4 t);
void ConstructSphere(Shape* s, Tuple3 center_point, float radius);
Intersections Intersect(Shape s, Ray r);
Tuple3 NormalAt(Shape s, Tuple3 pnt);

#endif