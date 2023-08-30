#include "shape.h"

#include <stdio.h>
#include <math.h>

Intersections IntersectSphere(Shape s, Ray r) {
    Intersections result;
    result.shape = s;
    result.ray = r;
    result.count = 0;

    Ray r_transformed = RayTransform(r, s.inverse_transform);


    Tuple3 l = TupleSubtract(r_transformed.origin, NewPnt3(0, 0, 0));
    float l_2 = TupleDotProduct(l, l);
    float _s = TupleDotProduct(l, r_transformed.direction);

    float m_2 = l_2 - (_s * _s);
    float q_2 = 1 - m_2;
    float q = sqrtf(q_2);
    float r_2 = q_2 + m_2;

    if (m_2 > r_2) {
        return result;
    }

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

