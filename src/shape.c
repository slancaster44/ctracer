#include "shape.h"

#include <stdio.h>
#include <math.h>

void ConstructSphere(Shape* s, Tuple3 center_point) {
    Sphere sphere;
    sphere.center_point = center_point;

    s->type = SPHERE;
    s->as.sphere = sphere;
}

Intersections IntersectSphere(Shape s, Ray r) {
    Intersections result;
    result.shape = s;
    result.ray = r;

    Sphere sphere = s.as.sphere;

    Tuple3 sphere_to_ray = TupleSubtract(r.origin, sphere.center_point);
    float a = TupleDotProduct(r.direction, r.direction);
    float b = 2 * TupleDotProduct(r.direction, sphere_to_ray);
    float c = TupleDotProduct(sphere_to_ray, sphere_to_ray) - 1;
    float discriminant = (b * b) - 4 * a * c;

    if (discriminant < 0) {
        result.count = 0;
        
    } else if (discriminant == 0) {
        result.count = 1;
        result.ray_times[0] = -b / (a * 2);

    } else {
        result.count = 2;

        float a_2 = a * 2;
        float d_sqrt = sqrtf(discriminant);

        result.ray_times[0] = (-b - d_sqrt) / a_2;
        result.ray_times[1] = (-b + d_sqrt) / a_2;
    }

    return result;
}

Intersections Intersect(Shape s, Ray r) {
    switch(s.type) {
    case SPHERE:
        return IntersectSphere(s, r);
        break;
    default:
        printf("Invalid shape type\n");
        exit(1);
    }
}