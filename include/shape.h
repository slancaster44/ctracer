#ifndef SHAPE_H
#define SHAPE_H

#include "tuple.h"
#include "ray.h"
#include "matrix.h"
#include "material.h"

typedef enum {
    SPHERE,
    PLANE,
} SHAPE_TYPE;

typedef struct {
    Matrix4x4 transformation;
    Matrix4x4 inverse_transform;

    Material material;

    SHAPE_TYPE type;
} Shape;

void ApplyTransformation(Shape* s, Matrix4x4 t);
Shape NewSphere(Tuple3 center_point, float radius);
Shape NewPlane(Tuple3 pnt, Tuple3 normal_vector);
Tuple3 NormalAt(Shape* s, Tuple3 pnt);

#endif