#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "ray.h"
#include "shape.h"

#define MAX_NUMBER_INTERSECTIONS 2

typedef struct {
    Shape* shape_ptr;
    Ray ray;

    float ray_times[MAX_NUMBER_INTERSECTIONS];
    int count;
} Intersection;

Intersection Intersect(Shape* s, Ray r);

#endif