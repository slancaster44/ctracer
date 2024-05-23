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

    return TransformBounds(b, s->transformation);
}

bool IsInBounds(Bounds b, Ray r)
{
    if (TupleHasInfOrNans(b.maximum_bound) || TupleHasInfOrNans(b.minimum_bound))
    {
        return true;
    }

    Tuple3 tmin_numerators = TupleSubtract(b.minimum_bound, r.origin);
    Tuple3 tmax_numerators = TupleSubtract(b.maximum_bound, r.origin);

    tmin_numerators = TupleDivide(tmin_numerators, r.direction);
    tmax_numerators = TupleDivide(tmax_numerators, r.direction);

    Tuple3 tmaxs = _mm256_max_pd(tmin_numerators, tmax_numerators);
    Tuple3 tmins = _mm256_min_pd(tmin_numerators, tmax_numerators);

    __m512d partial_result = DISTRIBUTE_256(tmins, tmaxs);

    double tmin = _mm512_mask_reduce_max_pd(0x8F, partial_result); // Mask 1000_1111; Exclude 'w' and elements from tmaxs
    double tmax = _mm512_mask_reduce_min_pd(0xF8, partial_result);

    return tmax > tmin;
}

Bounds TransformBounds(Bounds b, Matrix4x4 m)
{
    Bounds new_bounds = {
        .minimum_bound = NewPnt3(INFINITY, INFINITY, INFINITY),
        .maximum_bound = NewPnt3(-INFINITY, -INFINITY, -INFINITY)};

    Tuple3 points[8] align = {
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

    return new_bounds;
}

Tuple3 Centroid(Bounds b)
{
    Tuple3 hwd = TupleSubtract(b.maximum_bound, b.minimum_bound);
    Tuple3 center_point_offset = TupleScalarDivide(hwd, 2);
    Tuple3 center_point = TupleSubtract(b.maximum_bound, center_point_offset);

    return center_point;
}
