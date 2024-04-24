#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "ray.h"

#define MAX_NUMBER_INTERSECTIONS 2

typedef struct Shape Shape; //Prevents circular type dependencies intersection->shape->material->shader->intersection
typedef struct {
    Shape* shape_ptr;
    Ray ray;

    double ray_times[MAX_NUMBER_INTERSECTIONS];
    int count;
} Intersection;

Intersection NewIntersection(Shape* s, Ray r);
Intersection Intersect(Shape* s, Ray r);
Intersection IntersectPlane(Shape* s, Ray r);
Intersection IntersectSphere(Shape* s, Ray r);

#endif