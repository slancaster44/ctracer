#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "ray.h"
#include "shape.h"

#define MAX_NUMBER_INTERSECTIONS 2

typedef struct {
    Shape shape;
    Ray ray;

    float ray_times[MAX_NUMBER_INTERSECTIONS];
    int count;
} Intersection;

void ConstructIntersection(Intersection* i, Shape s, Ray r);
Intersection Intersect(Shape s, Ray r);

#endif