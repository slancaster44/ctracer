#include "scene.h"
#include "intersection.h"

void ConstructScene(Scene* s) {
    ConstructSet(&(s->shapes), sizeof(Shape));
}

void DeconstructScene(Scene* s) {
    DeconstructSet(&(s->shapes));
}

unsigned AddShape(Scene* s, Shape sp) {
    return AppendValue(&(s->shapes), &sp);
}

void IntersectScene(Scene* s, Ray r, Set* intersection_set) {
    for (int i = 0; i < s->shapes.length; i++) {

        Shape this_shape;
        CopyOut(&(s->shapes), i, &this_shape);

        Intersection i = Intersect(this_shape, r);
        AppendValue(intersection_set, &i);
    }
}