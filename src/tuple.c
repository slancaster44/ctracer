#include <immintrin.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "equality.h"
#include "tuple.h"

Tuple3 NewVec3(double x, double y, double z)
{
    return NewTuple3(x, y, z, 0.0);
}

Tuple3 NewPnt3(double x, double y, double z)
{
    return NewTuple3(x, y, z, 1.0);
}

Tuple3 NewTuple3(double x, double y, double z, double w)
{
    return _mm256_set_pd(w, z, y, x);
}

Tuple3 NewColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    Tuple3 c = NewTuple3((double)r, (double)g, (double)b, (double)a);
    return TupleScalarDivide(c, 255);
}

bool TupleHasNaNs(Tuple3 t1)
{
    Tuple3 tester = _mm256_set1_pd(0);
    __m256d nans_cmp = _mm256_cmp_pd(t1, tester, _CMP_UNORD_Q);
    unsigned mask1 = (unsigned)_mm256_movemask_pd(nans_cmp);

    __m256d neg_nans_cmp = _mm256_cmp_pd(t1, tester, _CMP_UNORD_Q);
    unsigned mask2 = (unsigned)_mm256_movemask_pd(neg_nans_cmp);

    return mask1 != 0 || mask2 != 0;
}

void PrintTuple(Tuple3 t)
{
    printf("[%f %f %f %f]\n", t[0], t[1], t[2], t[3]);
}

int TupleEqual(Tuple3 t1, Tuple3 t2)
{
    __m256d res = _mm256_cmp_pd(t1, t2, _CMP_EQ_OQ);
    unsigned bitmask = (unsigned)_mm256_movemask_pd(res);

    return (bitmask == 0xf);
}

bool TupleContains(Tuple3 t1, double n)
{
    __m256d res = _mm256_cmp_pd(t1, _mm256_set1_pd(n), _CMP_EQ_OQ);
    unsigned bitmask = (unsigned)_mm256_movemask_pd(res);

    return bitmask > 0;
}

bool TupleHasInf(Tuple3 t1)
{
    return TupleContains(t1, INFINITY) || TupleContains(t1, -INFINITY);
}

bool TupleHasInfOrNans(Tuple3 t1)
{
    return TupleHasInf(t1) || TupleHasNaNs(t1);
}

int TupleFuzzyEqual(Tuple3 t1, Tuple3 t2)
{
    __m256d diff = _mm256_sub_pd(t1, t2);
    __m256d mask = (__m256d)_mm256_set1_epi64x(0x7FFFFFFFFFFFFFFF);
    diff = _mm256_and_pd(diff, mask);

    __m256d epsilon = _mm256_set1_pd(EQUALITY_EPSILON);
    __m256d cmp = _mm256_cmp_pd(diff, epsilon, _CMP_LT_OQ);

    unsigned res = (unsigned)_mm256_movemask_pd(cmp);
    return (res == 0xf);
}

Tuple3 TupleScalarMultiply(Tuple3 t1, double scalar)
{
    return _mm256_mul_pd(t1, _mm256_set1_pd(scalar));
}

Tuple3 TupleScalarDivide(Tuple3 t1, double scalar)
{
    return _mm256_div_pd(t1, _mm256_set1_pd(scalar));
}

Tuple3 TupleScalarSubtract(Tuple3 t1, double scalar)
{
    return _mm256_sub_pd(t1, _mm256_set1_pd(scalar));
}

Tuple3 TupleScalarAdd(Tuple3 t1, double scalar)
{
    return _mm256_add_pd(t1, _mm256_set1_pd(scalar));
}

Tuple3 TupleNegate(Tuple3 t1)
{
    return _mm256_sub_pd(_mm256_set1_pd(0), t1);
}

double TupleMagnitude(Tuple3 t1)
{
    Tuple3 r0 = _mm256_mul_pd(t1, t1);
    Tuple3 r1 = _mm256_hadd_pd(r0, r0);
    return sqrt(r1[1] + r1[2]);
}

// TODO: No Tests
double TupleFloorSum(Tuple3 t1)
{
    Tuple3 floor = _mm256_floor_pd(t1);
    Tuple3 r1 = _mm256_hadd_pd(floor, floor);
    return r1[1] + r1[2];
}

double TupleDotProduct(Tuple3 t1, Tuple3 t2)
{
    Tuple3 r0 = TupleMultiply(t1, t2);
    Tuple3 r1 = _mm256_hadd_pd(r0, r0);
    return r1[1] + r1[2];
}

Tuple3 TupleCrossProduct(Tuple3 t1, Tuple3 t2)
{
    Tuple3 r0 = SWIZZLE_M256(t1, 1, 2, 0, 3);
    Tuple3 r1 = SWIZZLE_M256(t2, 2, 0, 1, 3);
    Tuple3 s0 = _mm256_mul_pd(r0, r1);

    Tuple3 r2 = SWIZZLE_M256(t1, 2, 0, 1, 3);
    Tuple3 r3 = SWIZZLE_M256(t2, 1, 2, 0, 3);
    Tuple3 s1 = _mm256_mul_pd(r2, r3);

    return _mm256_sub_pd(s0, s1);
}

Tuple3 TupleNormalize(Tuple3 t1)
{
    double mag = TupleMagnitude(t1);
    return _mm256_div_pd(t1, _mm256_set1_pd(mag));
}

void InfinitiesMask(Tuple3 t1, __mmask8 *infs, __mmask8 *neg_infs)
{
    __m256d infinity_cmp = _mm256_cmp_pd(t1, _mm256_set1_pd(INFINITY), _CMP_EQ_OQ);
    *infs |= (__mmask8)_mm256_movemask_pd(infinity_cmp);

    __m256d neg_infinity_cmp = _mm256_cmp_pd(t1, _mm256_set1_pd(-INFINITY), _CMP_EQ_OQ);
    *neg_infs |= (__mmask8)_mm256_movemask_pd(neg_infinity_cmp);
}

Tuple3 TupleMultiply(Tuple3 t1, Tuple3 t2)
{
    __mmask8 neg_infs = 0;
    __mmask8 infs = 0;

    InfinitiesMask(t1, &infs, &neg_infs);
    InfinitiesMask(t2, &infs, &neg_infs);

    Tuple3 src = _mm256_mask_mov_pd(_mm256_set1_pd(INFINITY), neg_infs, _mm256_set1_pd(-INFINITY));

    return _mm256_mask_mul_pd(src, ~(neg_infs | infs), t1, t2); // Preserve inifinites
}

Tuple3 TupleDivide(Tuple3 t1, Tuple3 t2)
{
    return _mm256_div_pd(t1, t2);
}

Tuple3 TupleAdd(Tuple3 t1, Tuple3 t2)
{
    return _mm256_add_pd(t1, t2);
}

Tuple3 TupleSubtract(Tuple3 t1, Tuple3 t2)
{
    return _mm256_sub_pd(t1, t2);
}

Tuple3 TupleReflect(Tuple3 t1, Tuple3 normal)
{
    double angle = 2 * TupleDotProduct(t1, normal);
    return TupleSubtract(t1, TupleScalarMultiply(normal, angle));
}

// TODO: No Test
double MinComponent(Tuple3 t1)
{
    return fmin(t1[0], fmin(t1[1], t1[2]));
}

// TODO: No test
double MaxComponent(Tuple3 t1)
{
    return fmax(t1[0], fmax(t1[1], t1[2]));
}
