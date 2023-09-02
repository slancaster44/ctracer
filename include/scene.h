#ifndef SCENE_H
#define SCENE_H

#include "shape.h"
#include "light.h"
#include "set.h"

typedef struct {
    Set shapes;
    Light l;
} Scene;

void ConstructScene(Scene* s);
void DeconstructScene(Scene* s);
unsigned AddShape(Scene* s, Shape sp);
void IntersectScene(Scene* s, Ray r, Set* intersection_set);

#endif