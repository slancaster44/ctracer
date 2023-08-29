#include "shape.h"

#include <stdio.h>

void ConstructSphere(Shape* s, Tuple3 center_point, float radius) {
    Sphere* sphere = (Sphere*) malloc(sizeof(Sphere));
    sphere->center_point = center_point;
    sphere->radius = radius;

    s->type = SPHERE;
    s->as.sphere = sphere;
}

void DeconstructShape(Shape* s) {
    switch (s->type) {
    case SPHERE:
        free(s->as.sphere);
        break;
    default:
        printf("Invalid shape type\n");
        exit(1);
    }
}