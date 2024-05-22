#include "shape.h"
#include "intersection.h"
#include "equality.h"

#include <math.h>

Shape NewSphere(Tuple3 cp, double radius)
{
    Shape s;
    s.material = NewMaterial(NewTuple3(0.8, 1.0, 0.6, 1.0));
    s.type = SPHERE;

    Matrix4x4 center_point_translation = TranslationMatrix(cp[0], cp[1], cp[2]);
    Tuple3 radius_vector = TupleScalarMultiply(NewVec3(1, 1, 1), radius);
    Matrix4x4 radius_scaling = ScalingMatrix(radius_vector[0], radius_vector[1], radius_vector[2]);

    s.transformation = MatrixMultiply(center_point_translation, radius_scaling);
    s.inverse_transform = MatrixInvert(s.transformation);

    return s;
}

Shape NewPlane(Tuple3 pnt, Tuple3 normal)
{
    Shape s;
    s.material = NewMaterial(NewTuple3(1.0, 0.8, 0.6, 1.0));
    s.type = PLANE;

    Matrix4x4 translation = TranslationMatrix(pnt[0], pnt[1], pnt[2]);

    // Find transform such that inv(transpose(M)) * vec(0, 1, 0) = normal
    normal = TupleNormalize(normal);
    Matrix4x4 rotation = IdentityMatrix();

    rotation.contents[0][1] = normal[0];
    rotation.contents[1][1] = normal[1] == 0.0 ? EQUALITY_EPSILON : normal[1];
    rotation.contents[2][1] = normal[2];
    rotation.contents[3][1] = normal[3];

    rotation = MatrixTranspose(rotation);
    rotation = MatrixInvert(rotation);

    s.transformation = MatrixMultiply(IdentityMatrix(), translation);
    s.transformation = MatrixMultiply(s.transformation, rotation);

    s.inverse_transform = MatrixInvert(s.transformation);

    return s;
}

Shape NewCube(Tuple3 location, double size)
{
    Shape s;
    s.material = NewMaterial(NewTuple3(1.0, 0.8, 0.6, 1.0));
    s.type = CUBE;

    Matrix4x4 center_point_translation = TranslationMatrix(location[0], location[1], location[2]);
    Tuple3 size_vector = TupleScalarMultiply(NewVec3(1, 1, 1), size);
    Matrix4x4 size_matrix = ScalingMatrix(size_vector[0], size_vector[1], size_vector[2]);

    s.transformation = MatrixMultiply(center_point_translation, size_matrix);
    s.inverse_transform = MatrixInvert(s.transformation);

    return s;
}

void ApplyTransformation(Shape *s, Matrix4x4 t)
{
    s->transformation = MatrixMultiply(s->transformation, t);
    s->inverse_transform = MatrixInvert(s->transformation);
}
