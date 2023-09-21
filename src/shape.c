#include "shape.h"


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

void ApplyTransformation(Shape* s, Matrix4x4 t) {
    s->transformation = MatrixMultiply(s->transformation, t);
    s->inverse_transform = MatrixInvert(s->transformation);
}