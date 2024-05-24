#include "shape.h"

Tuple3 SphereNormalAt(Shape *s, Tuple3 p)
{
    return TupleSubtract(p, NewPnt3(0, 0, 0));
}

Tuple3 PlaneNormalAt(Shape *s, Tuple3 p)
{
    return NewVec3(0, 1, 0);
}

Tuple3 TriangleNormalAt(Shape *s, Tuple3 p)
{
    return UNIT_TRI_NORM;
}

Tuple3 CubeNormalAt(Shape *s, Tuple3 p)
{
    __m512d abs_vals_512 = _mm512_abs_pd(DISTRIBUTE_256(p, p));

    double cmax = _mm512_mask_reduce_max_pd(0x77, abs_vals_512);
    Tuple3 abs_vals = _mm512_extractf64x4_pd(abs_vals_512, 1);

    __mmask8 cmp_max = _mm256_cmp_pd_mask(abs_vals, _mm256_set1_pd(cmax), _CMP_EQ_OQ);
    int num_trailing_zeros = __builtin_ctz(cmp_max);
    __mmask8 mov_mask = (__mmask8)(1 << num_trailing_zeros);

    Tuple3 result = _mm256_mask_mov_pd(_mm256_set1_pd(0.0), mov_mask, p);
    return result;
}

Tuple3 NormalAt(Shape *s, Tuple3 pnt)
{
    pnt = MatrixTupleMultiply(s->inverse_transform, pnt);
    Tuple3 result;

    switch (s->type)
    {
    case SPHERE:
        result = SphereNormalAt(s, pnt);
        break;
    case PLANE:
        result = PlaneNormalAt(s, pnt);
        break;
    case CUBE:
        result = CubeNormalAt(s, pnt);
        break;
    case TRIANGLE:
        result = TriangleNormalAt(s, pnt);
        break;
    default:
        printf("Cannot find normal for unkown shape\n");
        break;
    }

    result = MatrixTupleMultiply(MatrixTranspose(s->inverse_transform), result);
    result[3] = 0;
    return TupleNormalize(result);
}
