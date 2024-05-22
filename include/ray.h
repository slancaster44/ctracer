#ifndef RAY_H
#define RAY_H

#include "tuple.h"
#include "matrix.h"

/**
 * Represents a ray that can be used to intersect
 * various objects
 */
typedef struct
{
    /** The ray's starting point */
    Tuple3 origin;

    /** The ray's direction vector */
    Tuple3 direction;
} Ray;

/**
 * @memberof Ray
 * Constructs a new ray with the given origin and direction
 */
Ray NewRay(Tuple3 origin, Tuple3 direction);

/**
 * @memberof Ray
 * Returns the coordinate at a given position along the ray
 * 
 * @param 'Ray r' The ray to travel along
 * @param 'double pos' The distance from the ray's origin to travel
 * @returns The point at the given position
 */
Tuple3 RayPosition(Ray r, double pos);

/**
 * @memberof Ray
 * Transform the given ray according to the given matrix. Return the result
 */
Ray RayTransform(Ray r, Matrix4x4 transformation);

#endif
