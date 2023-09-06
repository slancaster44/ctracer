#include "shape.h"
#include "intersection.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

void IntersectSphere(Shape* s, Ray r, Intersection* result) {   
    r = RayTransform(r, s->inverse_transform);

    Tuple3 sphere_to_ray = TupleSubtract(r.origin, NewPnt3(0, 0, 0));
    float a = TupleDotProduct(r.direction, r.direction);
    float b = 2 * TupleDotProduct(r.direction, sphere_to_ray);
    float c = TupleDotProduct(sphere_to_ray, sphere_to_ray) - 1;
    float discriminant = (b * b) - 4 * a * c;
    
    if (discriminant < 0) {
        return;
        
    } else if (discriminant == 0) {
        result->count = 1;
        result->ray_times[0] = -b / (a * 2);

    } else {
        result->count = 2;

        float a_2 = a * 2;
        float d_sqrt = sqrtf(discriminant);

        //I found this necessary to do. However, none
        //of the literature indicated I have to do it,
        //this could be indicative of a bug somewhere else

        result->ray_times[0] = (-b - d_sqrt) / a_2;
        result->ray_times[1] = (-b + d_sqrt) / a_2;

        //if (result->ray_times[0] < 0 && result->ray_times[1] < 0) {

        //}
    }
}


Intersection Intersect(Shape* s, Ray r) {
    Intersection result;
    ConstructIntersection(&result, s, r);

    switch(s->type) {
    case SPHERE:
        IntersectSphere(s, r, &result);
        break;
    default:
        printf("Invalid shape type\n");
        exit(1);
    }

    return result;
}

void ConstructIntersection(Intersection* i, Shape* s, Ray r) {
    i->count = 0;
    i->shape_ptr = s;
    i->ray = r;
    memset(i->ray_times, 0, sizeof(float) * MAX_NUMBER_INTERSECTIONS);
}