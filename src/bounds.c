#include "bounding.h"
#include "tree.h"
#include "intersection.h"
#include "equality.h"

void GenerateBoundingCube(Bounds* b);

Bounds SphereBounds() {
    Bounds b = {
        .minimum_bound = NewPnt3(-1, -1, -1),
        .maximum_bound = NewPnt3(1, 1, 1),
    };

    return b;
}

Bounds CubeBounds() {
    Bounds b = {
        .minimum_bound = NewPnt3(-0.5, -0.5, -0.5),
        .maximum_bound = NewPnt3(0.5, 0.5, 0.5),
    };

    return b;
}

Bounds PlaneBounds() {
    Bounds b = {
        .minimum_bound = NewPnt3(-INFINITY, -EQUALITY_EPSILON, -INFINITY),
        .maximum_bound = NewPnt3(INFINITY, EQUALITY_EPSILON, INFINITY),
    };

    return b;
}

Bounds ShapeBounds(Shape* s) {
    Bounds b;

    switch (s->type) {
    case CUBE:
        b = CubeBounds();
        break;
    case PLANE:
        b = PlaneBounds();
        break;
    case SPHERE:
        b = SphereBounds();
        break;
    default:
        printf("Unable to calculate bounding box");
    }

    b = TransformBounds(b, s->transformation);
    GenerateBoundingCube(&b);

    return b;
}

bool IsInBounds(Bounds b, Ray r) {
    Intersection cube_intersect = Intersect(&b.as_cube, r);
    return cube_intersect.count > 0;
}

Bounds TransformBounds(Bounds b, Matrix4x4 m) {
    b.maximum_bound = MatrixTupleMultiply(m, b.maximum_bound);
    b.minimum_bound = MatrixTupleMultiply(m, b.minimum_bound);
    return b;
}

Tuple3 Centroid(Bounds b) {
    Tuple3 hwd = TupleSubtract(b.maximum_bound, b.minimum_bound);
    Tuple3 center_point_offset = TupleScalarDivide(hwd, 2);
    Tuple3 center_point = TupleSubtract(b.maximum_bound, center_point_offset);

    return center_point;
}

void GenerateBoundingCube(Bounds* b) {
    //Find cube that matches the bounding box
    Tuple3 hwd = TupleSubtract(b->maximum_bound, b->minimum_bound);
    Matrix4x4 cube_scaling_matrix = ScalingMatrix(hwd[0], hwd[1], hwd[2]);

    Tuple3 center_point_offset = TupleScalarDivide(hwd, 2);
    Tuple3 center_point = TupleSubtract(b->maximum_bound, center_point_offset);
    Matrix4x4 cube_translation_matrix = TranslationMatrix(center_point[0], center_point[1], center_point[2]);

    Matrix4x4 cube_transform = MatrixMultiply(cube_translation_matrix, cube_scaling_matrix);
    Shape cube = { .type = CUBE, .transformation = IdentityMatrix(), .inverse_transform = IdentityMatrix() };
    ApplyTransformation(&cube, cube_transform);

    b->as_cube = cube;
}