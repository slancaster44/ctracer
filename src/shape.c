#include "shape.h"

#include <math.h>

Shape NewSphere(Tuple3 cp, float radius) {
    Shape s;
    s.material = NewMaterial(NewTuple3(0.8f, 1.0f, 0.6f, 1.0f));
    s.type = SPHERE;

    Matrix4x4 center_point_translation = TranslationMatrix(cp[0], cp[1], cp[2]);
    Matrix4x4 radius_scaling = ScalingMatrix(radius, radius, radius);

    s.transformation = MatrixMultiply(radius_scaling, center_point_translation);
    s.inverse_transform = MatrixInvert(s.transformation);

    return s;
}

Shape NewPlane(Tuple3 pnt, Tuple3 normal) {
    Shape s;
    s.material = NewMaterial(NewTuple3(1.0f, 0.8f, 0.6f, 1.0f));
    s.type = PLANE;

    Matrix4x4 translation = TranslationMatrix(pnt[0], pnt[1], pnt[2]);

    //Find Normal transform
    normal = TupleNormalize(normal);
    Tuple3 cp = TupleCrossProduct(normal, NewVec3(0, 1, 0));
    float cos_theta = TupleDotProduct(normal, NewVec3(0, 1, 0));

    Matrix4x4 v_brack = SkewSymmetricCPMatrix(cp);
    Matrix4x4 v_brack_2 = MatrixMultiply(v_brack, v_brack);
    float q = 1 / (1 + cos_theta);

    Matrix4x4 rotation = MatrixAdd(IdentityMatrix(), MatrixAdd(v_brack, MatrixScalarMultiply(v_brack_2, q)));

    s.transformation = MatrixMultiply(translation, rotation);
    s.inverse_transform = MatrixInvert(s.transformation);

    return s;
}

void ApplyTransformation(Shape* s, Matrix4x4 t) {
    s->transformation = MatrixMultiply(s->transformation, t);
    s->inverse_transform = MatrixInvert(s->transformation);
}