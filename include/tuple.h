#ifndef TUPLE_H
#define TUPLE_H

#include <immintrin.h>



typedef unsigned char uint8_t;
typedef __m128 Tuple3;


Tuple3 NewVec3(float x, float y, float z);
Tuple3 NewPnt3(float x, float y, float z);
Tuple3 NewTuple3(float x, float y, float z, float w);
Tuple3 NewColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void PrintTuple(Tuple3 t);
int TupleEqual(Tuple3 t1, Tuple3 t2);
int TupleFuzzyEqual(Tuple3 t1, Tuple3 t2);
Tuple3 TupleScalarMultiply(Tuple3 t1, float scalar);
Tuple3 TupleScalarDivide(Tuple3 t1, float scalar);
Tuple3 TupleNegate(Tuple3 t1);
float TupleMagnitude(Tuple3 t1);
float TupleDotProduct(Tuple3 t1, Tuple3 t2);
Tuple3 TupleCrossProduct(Tuple3 t1, Tuple3 t2);
Tuple3 TupleNormalize(Tuple3 t1);
Tuple3 TupleMultiply(Tuple3 t1, Tuple3 t2);
Tuple3 TupleDivide(Tuple3 t1, Tuple3 t2);
Tuple3 TupleAdd(Tuple3 t1, Tuple3 t2);
Tuple3 TupleSubtract(Tuple3 t1, Tuple3 t2);
Tuple3 TupleReflect(Tuple3 t1, Tuple3 normal);
Tuple3 TupleScalarSubtract(Tuple3 t, float n);
Tuple3 TupleScalarAdd(Tuple3 t, float n);

#endif