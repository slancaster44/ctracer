#ifndef TUPLE_H
#define TUPLE_H

#include <immintrin.h>
#include <stdbool.h>

typedef unsigned char uint8_t;
typedef __m256d Tuple3 __attribute__ ((aligned (32)));

#define SHUFFLE_M256(v1, v2, p1, p2, p3, p4) \
    _mm256_permutex2var_pd(v1, _mm256_set_epi64x((p4 + 4), (p3 + 4), (p2), (p1)), v2)
#define SWIZZLE_M256(val, p1, p2, p3, p4) \
    SHUFFLE_M256(val, val, p1, p2, p3, p4)
#define ADJOINT_M256(val) \
    SWIZZLE_M256(val, 0,2,1,3)
#define DISTRIBUTE_256(A, B) \
    _mm512_insertf64x4(_mm512_insertf64x4(_mm512_set1_pd(0.0), A, 0), B, 1)

Tuple3 NewVec3(double x, double y, double z);
Tuple3 NewPnt3(double x, double y, double z);
Tuple3 NewTuple3(double x, double y, double z, double w);
Tuple3 NewColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void PrintTuple(Tuple3 t);
int TupleEqual(Tuple3 t1, Tuple3 t2);
int TupleFuzzyEqual(Tuple3 t1, Tuple3 t2);
Tuple3 TupleScalarMultiply(Tuple3 t1, double scalar);
Tuple3 TupleScalarDivide(Tuple3 t1, double scalar);
Tuple3 TupleNegate(Tuple3 t1);
double TupleMagnitude(Tuple3 t1);
double TupleDotProduct(Tuple3 t1, Tuple3 t2);
Tuple3 TupleCrossProduct(Tuple3 t1, Tuple3 t2);
Tuple3 TupleNormalize(Tuple3 t1);
Tuple3 TupleMultiply(Tuple3 t1, Tuple3 t2);
Tuple3 TupleDivide(Tuple3 t1, Tuple3 t2);
Tuple3 TupleAdd(Tuple3 t1, Tuple3 t2);
Tuple3 TupleSubtract(Tuple3 t1, Tuple3 t2);
Tuple3 TupleReflect(Tuple3 t1, Tuple3 normal);
Tuple3 TupleScalarSubtract(Tuple3 t, double n);
Tuple3 TupleScalarAdd(Tuple3 t, double n);
double TupleFloorSum(Tuple3 t1);
bool TupleHasNaNs(Tuple3 t1);
double MaxComponent(Tuple3 t1); 
double MinComponent(Tuple3 t1); 


#endif