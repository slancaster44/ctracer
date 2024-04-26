#include "shape.h"
#include "intersection.h"
#include "equality.h"

#include <math.h>

Shape NewSphere(Tuple3 cp, double radius) {
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

Shape NewPlane(Tuple3 pnt, Tuple3 normal) {
    Shape s;
    s.material = NewMaterial(NewTuple3(1.0, 0.8, 0.6, 1.0));
    s.type = PLANE;

    Matrix4x4 translation = TranslationMatrix(pnt[0], pnt[1], pnt[2]);

    //Find Normal transform
    normal = TupleNormalize(normal);
    Tuple3 cp = TupleCrossProduct(normal, NewVec3(0, 1, 0));
    double cos_theta = TupleDotProduct(normal, NewVec3(0, 1, 0));

    Matrix4x4 v_brack = SkewSymmetricCPMatrix(cp);
    Matrix4x4 v_brack_2 = MatrixMultiply(v_brack, v_brack);
    double q = 1 / (1 + cos_theta);

    Matrix4x4 rotation = MatrixAdd(IdentityMatrix(), MatrixAdd(v_brack, MatrixScalarMultiply(v_brack_2, q)));

    s.transformation = MatrixMultiply(IdentityMatrix(), translation);
    s.transformation = MatrixMultiply(s.transformation, rotation);
    s.inverse_transform = MatrixInvert(s.transformation);

    return s;
}

void ApplyTransformation(Shape* s, Matrix4x4 t) {
    s->transformation = MatrixMultiply(s->transformation, t);
    s->inverse_transform = MatrixInvert(s->transformation);
}