#include "bounds.h"
#include "tree.h"
#include "intersection.h"
#include "equality.h"

#include <float.h>

Bounds SphereBounds()
{
    Bounds b = {
        .minimum_bound = NewPnt3(-1, -1, -1),
        .maximum_bound = NewPnt3(1, 1, 1),
    };

    return b;
}

Bounds CubeBounds()
{
    Bounds b = {
        .minimum_bound = NewPnt3(-0.5, -0.5, -0.5),
        .maximum_bound = NewPnt3(0.5, 0.5, 0.5),
    };

    return b;
}

Bounds PlaneBounds()
{
    Bounds b = {
        .minimum_bound = NewPnt3(-INFINITY, -INFINITY, -INFINITY),
        .maximum_bound = NewPnt3(INFINITY, INFINITY, INFINITY),
    };

    return b;
}

Bounds ShapeBounds(Shape *s)
{
    Bounds b;

    switch (s->type)
    {
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

    b = TransformBounds(b, s->inverse_transform);
    return b;
}

bool IsInBounds(Bounds b, Ray r)
{
    if (TupleHasInfOrNans(b.maximum_bound) || TupleHasInfOrNans(b.minimum_bound))
    {
        return true;
    }

    Intersection cube_intersect = Intersect(&b.as_cube, r);
    return cube_intersect.count > 0;
}

Bounds TransformBounds(Bounds b, Matrix4x4 m)
{
    Bounds new_bounds = {
        .minimum_bound = NewPnt3(INFINITY, INFINITY, INFINITY),
        .maximum_bound = NewPnt3(-INFINITY, -INFINITY, -INFINITY)};

    Tuple3 points[8] __attribute__((aligned(sizeof(Tuple3)))) = {
        // 'k' must be an immediate, so this cannot be inside the loop
        _mm256_blend_pd(b.maximum_bound, b.minimum_bound, 0),
        _mm256_blend_pd(b.maximum_bound, b.minimum_bound, 1),
        _mm256_blend_pd(b.maximum_bound, b.minimum_bound, 2),
        _mm256_blend_pd(b.maximum_bound, b.minimum_bound, 3),
        _mm256_blend_pd(b.maximum_bound, b.minimum_bound, 4),
        _mm256_blend_pd(b.maximum_bound, b.minimum_bound, 5),
        _mm256_blend_pd(b.maximum_bound, b.minimum_bound, 6),
        _mm256_blend_pd(b.maximum_bound, b.minimum_bound, 7),
    };

    for (int i = 0; i < 8; i++)
    {
        Tuple3 this_corner = points[i];
        this_corner = MatrixTupleMultiply(m, this_corner);

        new_bounds.maximum_bound = _mm256_max_pd(new_bounds.maximum_bound, this_corner);
        new_bounds.minimum_bound = _mm256_min_pd(new_bounds.minimum_bound, this_corner);
    }

    GenerateBoundingCube(&new_bounds);
    return new_bounds;
}

Tuple3 Centroid(Bounds b)
{
    Tuple3 hwd = TupleSubtract(b.maximum_bound, b.minimum_bound);
    Tuple3 center_point_offset = TupleScalarDivide(hwd, 2);
    Tuple3 center_point = TupleSubtract(b.maximum_bound, center_point_offset);

    return center_point;
}

void GenerateBoundingCube(Bounds *b)
{
    if (TupleHasInfOrNans(b->maximum_bound) || TupleHasInfOrNans(b->minimum_bound))
    {
        b->as_cube = NewCube(NewPnt3(0, 0, 0), DBL_MAX);
        return;
    }

    // Find cube that matches the bounding box
    Tuple3 hwd = TupleSubtract(b->maximum_bound, b->minimum_bound);

    Matrix4x4 cube_scaling_matrix = ScalingMatrix(hwd[0], hwd[1], hwd[2]);

    Tuple3 center_point_offset = TupleScalarDivide(hwd, 2);
    Tuple3 center_point = TupleSubtract(b->maximum_bound, center_point_offset);
    Matrix4x4 cube_translation_matrix = TranslationMatrix(center_point[0], center_point[1], center_point[2]);

    Matrix4x4 cube_transform = MatrixMultiply(cube_translation_matrix, cube_scaling_matrix);

    Shape cube = {.type = CUBE, .transformation = IdentityMatrix(), .inverse_transform = IdentityMatrix()};
    ApplyTransformation(&cube, cube_transform);

    for (int i = 0; i < 4; i++)
    {
        if (TupleHasInfOrNans(cube_transform.contents[i]))
        {
            cube = NewCube(NewPnt3(0, 0, 0), DBL_MAX);
            break;
        }
    }

    b->as_cube = cube;
}
