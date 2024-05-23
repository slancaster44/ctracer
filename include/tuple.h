#ifndef TUPLE_H
#define TUPLE_H

#include <immintrin.h>
#include <stdbool.h>

#include "alignment.h"

typedef unsigned char uint8_t;

/**
 * @struct Tuple3
 * A vector of four elements. \n
 * Used to represent:
 * - Points in 3D space (x, y, z, 1.0)
 * - Vectors in 3D space (x, y, z, 0.0)
 * - Colors (r, g, b, _)
*/
typedef __m256d Tuple3 align;

#define SHUFFLE_M256(v1, v2, p1, p2, p3, p4) \
    _mm256_permutex2var_pd(v1, _mm256_set_epi64x((p4 + 4), (p3 + 4), (p2), (p1)), v2)
#define SWIZZLE_M256(val, p1, p2, p3, p4) \
    SHUFFLE_M256(val, val, p1, p2, p3, p4)
#define ADJOINT_M256(val) \
    SWIZZLE_M256(val, 0, 2, 1, 3)
#define DISTRIBUTE_256(A, B) \
    _mm512_insertf64x4(_mm512_insertf64x4(_mm512_set1_pd(0.0), A, 0), B, 1)

/**
 * @memberof Tuple3
 * Generate a vector in 3D space with the given x, y and z elements
 */
Tuple3 NewVec3(double x, double y, double z);

/**
 * @memberof Tuple3
 * Generate a point in 3D space with the given x, y and z elements
 */
Tuple3 NewPnt3(double x, double y, double z);

/**
 * @memberof Tuple3
 * Generate a tuple with the given x, y, z and w elements
 */
Tuple3 NewTuple3(double x, double y, double z, double w);

/**
 * @memberof Tuple3
 * Generate a color tuple from four 8-bit values. These values
 * should be scaled between 0 (lowest intesity) and 255 (highest intesity).
 * The resulting tuple will be transformed to have values scaled from 0.0 
 * (lowest intensity) and 1.0 (highest intensity)
 */
Tuple3 NewColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/** @memberof Tuple3 
 * Print a tuple to the console, with a trailing newline character
 */
void PrintTuple(Tuple3 t);

/**
 * @memberof Tuple3
 * Returns true if every element in 't1' is exactly equal to
 * every element in 't2'
*/
bool TupleEqual(Tuple3 t1, Tuple3 t2);

/**
 * @memberof Tuple3
 * Compares two tuples for equality with EQUALITY_EPSILON.
 * Returns true if, for every element 'n' in t1 and t2,
 * <code> abs(t1[n] - t2[n]) < EQUALITY_EPSILON </code>
 */
bool TupleFuzzyEqual(Tuple3 t1, Tuple3 t2);

/**
 * @memberof Tuple3
 * Multiplies every element in 't1' by the given scalar
 */
Tuple3 TupleScalarMultiply(Tuple3 t1, double scalar);

/**
 * @memberof Tuple3
 * Divides every element in 't1' by the given scalar
 */
Tuple3 TupleScalarDivide(Tuple3 t1, double scalar);

/**
 * @memberof Tuple3
 * Multiplies every element in 't1' by -1.0
 */
Tuple3 TupleNegate(Tuple3 t1);

/**
 * @memberof Tuple3
 * Assuming 't1' represents a vector, the return value will be the
 * magnitude of that vector.
 */
double TupleMagnitude(Tuple3 t1);

/**
 * @memberof Tuple3
 * Assuming 't1' and 't2' are vectors, the return value will be
 * the dot product of 't1' and 't2.'
*/
double TupleDotProduct(Tuple3 t1, Tuple3 t2);


/**
 * @memberof Tuple3
 * Assuming 't1' and 't2' are vectors, the return value will be
 * the dot product of 't1' and 't2.'
 * 
 * @note Values of +/- infinity will remain unchanged
*/
double TupleDotProductPreserveInf(Tuple3 t1, Tuple3 t2);

/**
 * @memberof Tuple3
 * Assuming 't1' and 't2' are vectors, the return value will be
 * the cross product of 't1' and 't2.'
*/
Tuple3 TupleCrossProduct(Tuple3 t1, Tuple3 t2);

/**
 * @memberof Tuple3
 * Scale the given tuple such that the magnitude equals 1.0. Return the result.
*/
Tuple3 TupleNormalize(Tuple3 t1);

/**
 * @memberof Tuple3
 * Returns a result such that, for every index 'n' in 't1' and 't2,'
 * <code> result[n] = t1[n] * t2[n] </code>
*/
Tuple3 TupleMultiply(Tuple3 t1, Tuple3 t2);

/**
 * @memberof Tuple3
 * Returns a result such that, for every index 'n' in 't1' and 't2,'
 * <code> result[n] = t1[n] * t2[n] </code>
 * 
 * @note All values of +/- inifinity will remain uncahnged
*/
Tuple3 TupleMultiplyPreserveInf(Tuple3 t1, Tuple3 t2);


/**
 * @memberof Tuple3
 * Returns a result such that, for every index 'n' in 't1' and 't2,'
 * <code> result[n] = t1[n] / t2[n] </code>
*/
Tuple3 TupleDivide(Tuple3 t1, Tuple3 t2);

/**
 * @memberof Tuple3
 * Returns a result such that, for every index 'n' in 't1' and 't2,'
 * <code> result[n] = t1[n] + t2[n] </code>
*/
Tuple3 TupleAdd(Tuple3 t1, Tuple3 t2);

/**
 * @memberof Tuple3
 * Returns a result such that, for every index 'n' in 't1' and 't2,'
 * <code> result[n] = t1[n] - t2[n] </code>
*/
Tuple3 TupleSubtract(Tuple3 t1, Tuple3 t2);

/**
 * @memberof Tuple3
 * Reflect the given vector, 't1,' around the given normal
*/
Tuple3 TupleReflect(Tuple3 t1, Tuple3 normal);

/**
 * @memberof Tuple3
 * Returns a result such that, for every index 'n' in 't1,'
 * <code> result[n] = t1[n] - x </code>
*/
Tuple3 TupleScalarSubtract(Tuple3 t, double x);

/**
 * @memberof Tuple3
 * Returns a result such that, for every index 'n' in 't1,'
 * <code> result[n] = t1[n] + x </code>
*/
Tuple3 TupleScalarAdd(Tuple3 t, double x);

/**
 * @memberof Tuple3
 * Returns a result such that, 
 * <code> result = floor(t1[0]) + floor(t1[1]) + floor(t1[2]) + floor(t1[3]) </code>
*/
double TupleFloorSum(Tuple3 t1);

/**
 * @memberof Tuple3
 * Returns true if any element in 't1' is not a number (NaN)
*/
bool TupleHasNaNs(Tuple3 t1);

/**
 * @memberof Tuple3
 * Returns true if any elment of 't1' is equal to positive infinity or negative infinity
*/
bool TupleHasInf(Tuple3 t1);

/**
 * @memberof Tuple3
 * Returns true if TupleHasInf() or TupleHasNaNs() would return true
*/
bool TupleHasInfOrNans(Tuple3 t1);

/**
 * @memberof Tuple3
 * Returns true if any element in 't1' is equal to 'n'
*/
bool TupleContains(Tuple3 t1, double n);

/**
 * @memberof Tuple3
 * Returns the largest element in 't1'
*/
double MaxComponent(Tuple3 t1);

/**
 * @memberof Tuple3
 * Returns the smallest element in t1.
*/
double MinComponent(Tuple3 t1);

#endif
