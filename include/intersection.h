#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "ray.h"

#define MAX_NUMBER_INTERSECTIONS 2

typedef struct Shape Shape; // Prevents circular type dependencies intersection->shape->material->shader->intersection

/**
 * Represents an intersection between a shape and a ray 
 */
typedef struct
{
    /** A pointer to the shape intersected */
    Shape *shape_ptr;

    /** The ray that intersects */
    Ray ray;

    /** The locations along 'Ray ray' that intersect 'Shape *shape_ptr' */
    double ray_times[MAX_NUMBER_INTERSECTIONS];

    /** The number of times 'Ray ray' intersected 'Shape *shape_ptr' */
    int count;
} Intersection;

/**
 * @private
 * @memberof Intersection
 * Constructs a new intersection object for a given shape and ray. This constructor oes not
 * calculate 'ray_times' or 'count,' but instead initializes them to default values
 * 
 * @param 'Shape* s' The shape belonging to the intersection
 * @param 'Ray r' The ray belonging to the intersection
 * @returns 'Intersection' An intersection without properly calculated 'ray_times' or 'count'
 * fields
*/
Intersection NewIntersection(Shape *s, Ray r);

/**
 * @memberof Shape
 * Constructs a new intersection object, and calculates the number and locations of those
 * intersections
 * 
 * @param 'Shape *s' The shape to be intersected
 * @param 'Ray r' The ray to intersect with
 * @returns 'Intersection' An intersection object containg the 'count' and 'ray_times' of
 * the intersections
 */
Intersection Intersect(Shape *s, Ray r);

/**
 * @memberof Intersection
 * Comparator for two intersections 
 *
 * @param 'Intersection *i1' First intersection to sort
 * @param 'Intersection *i2' Second intersection to sort
 * @returns True if i1.ray_times[0] < i2.ray_times[0]
*/
bool CompareIntersections(Intersection *i1, Intersection *i2);

#endif
