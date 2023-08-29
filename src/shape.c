#include "shape.h"

#include <stdio.h>
#include <math.h>

void ConstructSphere(Shape* s, Tuple3 center_point) {
    Sphere sphere;
    sphere.center_point = center_point;

    s->type = SPHERE;
    s->as.sphere = sphere;

    s->transformation = IdentityMatrix();
    s->inverse_transform = IdentityMatrix();
}

Intersections IntersectSphere(Shape s, Ray r) {
    Intersections result;
    result.shape = s;
    result.ray = r;
    result.count = 0;

    Ray r_transformed = RayTransform(r, s.inverse_transform);

    Sphere sphere = s.as.sphere;

    Tuple3 l = TupleSubtract(r_transformed.origin, sphere.center_point);
    float l_2 = TupleDotProduct(l, l);
    float _s = TupleDotProduct(l, r_transformed.direction);

    float m_2 = l_2 - (_s * _s);
    float q = sqrtf(1 - m_2);

    if (l_2 > 1) { //Ray origin outside
        result.count = 2;
        result.ray_times[0] = -(_s + q);
        result.ray_times[1] = -(_s - q);
    } else { //Ray origin inside
        result.count = 2;
        result.ray_times[0] = _s - q;
        result.ray_times[1] = _s + q;
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