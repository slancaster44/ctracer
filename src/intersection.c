#include "shape.h"
#include "intersection.h"
#include "equality.h"

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>

Intersection NewIntersection(Shape* s, Ray r) {
    Intersection i;
    i.count = 0;
    i.shape_ptr = s;
    i.ray = r;

    for (int idx = 0; idx < MAX_NUMBER_INTERSECTIONS; idx++) {
        i.ray_times[idx] = DBL_MAX;
    }
    
    return i;
}

Intersection IntersectPlane(Shape* s, Ray r) {
    Intersection result = NewIntersection(s, r);
    r = RayTransform(r, s->inverse_transform);

    /* Because the ray has been transformed into "shape space"
     * then we can assume that the plane is on the x=0. As a result
     * a y value near zero indcates the ray is either on the plane, or
     * it is parallel to the plane
     */
    if (FloatEquality(r.direction[1], 0)) {
        return result;
    }
    
    result.count = 1;
    result.ray_times[0] = -r.origin[1] / r.direction[1];
    
    return result;
}

Intersection IntersectSphere(Shape* s, Ray r) {
    Intersection result = NewIntersection(s, r);   
    r = RayTransform(r, s->inverse_transform);

    Tuple3 sphere_to_ray = TupleSubtract(r.origin, NewPnt3(0, 0, 0));
    double a = TupleDotProduct(r.direction, r.direction);
    double b = 2 * TupleDotProduct(r.direction, sphere_to_ray);
    double c = TupleDotProduct(sphere_to_ray, sphere_to_ray) - 1;
    double discriminant = (b * b) - 4 * a * c;
    
    if (discriminant < 0) {
        return result;
        
    } else if (discriminant == 0) {
        result.count = 1;
        result.ray_times[0] = -b / (a * 2);

    } else {
        result.count = 2;

        double a_2 = a * 2;
        double d_sqrt = sqrt(discriminant);

        result.ray_times[0] = (-b - d_sqrt) / a_2;
        result.ray_times[1] = (-b + d_sqrt) / a_2;
    }

    return result;
}

Intersection IntersectCube(Shape* s, Ray r) {
    Intersection result = NewIntersection(s, r);
    r = RayTransform(r, s->inverse_transform);

    Tuple3 tmin_numerators = TupleSubtract(_mm256_set1_pd(-1.0), r.origin);
    Tuple3 tmax_numerators = TupleSubtract(_mm256_set1_pd(1.0), r.origin);

    tmin_numerators = TupleDivide(tmin_numerators, r.direction);
    tmax_numerators = TupleDivide(tmax_numerators, r.direction);

    Tuple3 tmaxs = _mm256_max_pd(tmin_numerators, tmax_numerators);
    Tuple3 tmins = _mm256_min_pd(tmin_numerators, tmax_numerators);

    __m512d partial_result = DISTRIBUTE_256(tmins, tmaxs);

    double tmin = _mm512_mask_reduce_max_pd(0x8F, partial_result); //Mask 1000_1111; Exclude 'w' and elements from tmaxs
    double tmax = _mm512_mask_reduce_min_pd(0xF8, partial_result);

    result.ray_times[0] = tmin;
    result.ray_times[1] = tmax;
    result.count = tmin > tmax ? 0 : 2;

    return result;
}

Intersection Intersect(Shape* s, Ray r) {
    Intersection result;

    switch (s->type) {
    case SPHERE:
        result = IntersectSphere(s, r);
        break;
    case PLANE:
        result = IntersectPlane(s, r);
        break;
    case CUBE:
        result = IntersectCube(s, r);
        break;
    default:
        printf("Cannot intersect shape of type '%d'\n", s->type);
        exit(1);
    }

    if (result.ray_times[0] > result.ray_times[1]) {
        double tmp = result.ray_times[1];
        result.ray_times[1] = result.ray_times[0];
        result.ray_times[0] = tmp;
    }

    return result;
}

