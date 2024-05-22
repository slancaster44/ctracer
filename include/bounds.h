#ifndef BOUNDING_H
#define BOUNDING_H

#include "tuple.h"
#include "shape.h"

/**
 * Bounding box representation.
 * Stores an object's bounding box as a minimum & maximum point
 */
typedef struct
{
     /** Minimum bounding point */
    Tuple3 minimum_bound;  

    /** Maximum bounding point */
    Tuple3 maximum_bound;        
} Bounds;

/**
 * @memberof Shape
 * 
 * Generates the Bounds for a given shape
 * @param 'Shape *s' a pointer to the shape that Bounds must be generated for.
 * @returns 'Bounds' a bounding box enclosing the given shape
 */
Bounds ShapeBounds(Shape *s);

/**
 * @memberof Bounds
 * Transforms a given Bounds by the Matrix4x4 'm'. Returns the resulting transformed bounding box
 * 
 * @param 'Bounds b' The bounding volume to be transformed
 * @param 'Matrix4x4 m' The transformation to apply to the volume 
 * @returns 'Bounds' the transformed bounding box
 */
Bounds TransformBounds(Bounds b, Matrix4x4 m);

/**
 * @memberof Bounds
 * Returns 'true' if the given ray intersects the given bounding volume
 * 
 * @param 'Bounds b' The bounding box to intersect.
 * @param 'Ray r' The ray to test bounding box intersection on
 * @returns 'bool' True if the ray intersects the bounding box
 */
bool IsInBounds(Bounds b, Ray r);

/**
 * @memberof Bounds
 * Returns a point at the center of the given bounding box
 * 
 * @param 'Bounds b' the bounding box you wish to know the center of
 * @returns 'Tuple3' the point at the center of the bounding box
 */
Tuple3 Centroid(Bounds b);

#endif
