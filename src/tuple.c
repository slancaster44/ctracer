#include <immintrin.h>
#include <math.h>
#include <stdio.h>

#include "trig.h"
#include "tuple.h"

Tuple3 NewVec3(float x, float y, float z) {
    return NewTuple3(x, y, z, 0.0);
}

Tuple3 NewPnt3(float x, float y, float z) {
    return NewTuple3(x, y, z, 1.0);
}

Tuple3 NewTuple3(float x, float y, float z, float w) {
    return _mm_set_ps(w, z, y, x);
}

Tuple3 NewColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return _mm_set_ps((float) r / 255, (float) g / 255, (float) b / 255, (float) a / 255);
}

void PrintTuple(Tuple3 t) {
    printf("[%f %f %f %f]", t[0], t[1], t[2], t[3]);
}

int TupleEqual(Tuple3 t1, Tuple3 t2) {
    __m128 res = _mm_cmpeq_ps(t1, t2);
    unsigned bitmask = _mm_movemask_ps(res);

    return (bitmask == 0xf);
}

int TupleFuzzyEqual(Tuple3 t1, Tuple3 t2) {
    __m128 diff = _mm_sub_ps(t1, t2);
    __m128 mask = (__m128) _mm_set1_epi32(0x7FFFFFFF);
    diff = _mm_and_ps(diff, mask);

    __m128 epsilon = _mm_set1_ps(EQUALITY_EPSILON);
    __m128 cmp = _mm_cmp_ps(diff, epsilon, _CMP_LT_OQ);

    unsigned res = _mm_movemask_ps(cmp);

    return (res == 0xf);
}

Tuple3 TupleScalarMultiply(Tuple3 t1, float scalar) {
    return _mm_mul_ps(t1, _mm_set1_ps(scalar));
}

Tuple3 TupleScalarDivide(Tuple3 t1, float scalar) {
    return _mm_div_ps(t1, _mm_set1_ps(scalar));
}

Tuple3 TupleNegate(Tuple3 t1) {
    return _mm_sub_ps(_mm_set1_ps(0), t1);
}

float TupleMagnitude(Tuple3 t1) {
    Tuple3 r0 = _mm_mul_ps(t1, t1);
    Tuple3 r1 = _mm_hadd_ps(r0, r0);
    Tuple3 r2 = _mm_hadd_ps(r1, r1);
    return sqrtf(r2[0]);
}

float TupleDotProduct(Tuple3 t1, Tuple3 t2) {
    Tuple3 r2 = _mm_dp_ps(t1, t2, 0xff);
    return r2[0];
}

Tuple3 TupleCrossProduct(Tuple3 t1, Tuple3 t2) {
    Tuple3 r0 = _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(3, 0, 2, 1));
    Tuple3 r1 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(3, 1, 0, 2));
    Tuple3 s0 = _mm_mul_ps(r0, r1);

    Tuple3 r2 = _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(3, 1, 0, 2));
    Tuple3 r3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(3, 0, 2, 1));
    Tuple3 s1 = _mm_mul_ps(r2, r3);

    return _mm_sub_ps(s0, s1);
}

Tuple3 TupleNormalize(Tuple3 t1) {
    float mag = TupleMagnitude(t1);
    return _mm_div_ps(t1, _mm_set1_ps(mag));
}

Tuple3 TupleMultiply(Tuple3 t1, Tuple3 t2) {
    return _mm_mul_ps(t1, t2);
}

Tuple3 TupleDivide(Tuple3 t1, Tuple3 t2) {
    return _mm_div_ps(t1, t2);
}

Tuple3 TupleAdd(Tuple3 t1, Tuple3 t2) {
    return _mm_add_ps(t1, t2);
}

Tuple3 TupleSubtract(Tuple3 t1, Tuple3 t2) {
    return _mm_sub_ps(t1, t2);
}

Tuple3 TupleReflect(Tuple3 t1, Tuple3 normal) {
    float x = 2 * TupleDotProduct(t1, normal);
    return TupleSubtract(t1, TupleScalarMultiply(normal, x));
}