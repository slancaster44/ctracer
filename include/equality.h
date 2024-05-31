#ifndef EQUALITY_H
#define EQUALITY_H

#include <stdbool.h>
#include <math.h>

/**
 * An EPSILON value to be used in testing floating point equalites
 */
#define EQUALITY_EPSILON 0.0000075

/**
 * 'f1' and 'f2' will be considered equal to each other if 'f1' less
 * than 'EQUALITY_EPSILON' away from 'f2' (e.g. abs(f1 - f2) < EQUALITY_EPSILON)
 * 
 * @param 'f1' the first floating point number to compare
 * @param 'f2' the second floating point number to compare
 * @returns 'bool' True if the two floats are close enough to
 * be considered equal
 */
bool FloatEquality(double f1, double f2);
#endif
