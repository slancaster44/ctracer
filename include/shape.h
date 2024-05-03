#ifndef SHAPE_H
#define SHAPE_H

#include "tuple.h"
#include "ray.h"
#include "matrix.h"
#include "material.h"

typedef enum {
    SPHERE,
    PLANE,
    CUBE,
} SHAPE_TYPE;

typedef struct Shape {
    Matrix4x4 transformation;
    Matrix4x4 inverse_transform;
    Material material;
    SHAPE_TYPE type;
} Shape;

void ApplyTransformation(Shape* s, Matrix4x4 t);

Shape NewSphere(Tuple3 center_point, double radius);
Shape NewPlane(Tuple3 pnt, Tuple3 normal_vector);
Shape NewCube(Tuple3 pnt, double size);

Tuple3 NormalAt(Shape* s, Tuple3 pnt);

void AddShape(Scene* s, Shape sp);

#endif