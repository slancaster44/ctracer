#include "shape.h"


void ConstructSphere(Shape* s, Tuple3 cp, float radius) {

    s->type = SPHERE;

    Matrix4x4 center_point_translation = TranslationMatrix(cp[0], cp[1], cp[2]);
    Matrix4x4 radius_scaling = ScalingMatrix(radius, radius, radius);

    s->transformation = MatrixMultiply(MatrixMultiply(IdentityMatrix(), radius_scaling), center_point_translation);
    s->inverse_transform = MatrixInvert(s->transformation);
}

void ApplyTransformation(Shape* s, Matrix4x4 t) {
    s->transformation = MatrixMultiply(s->transformation, t);
    s->inverse_transform = MatrixInvert(s->transformation);
}