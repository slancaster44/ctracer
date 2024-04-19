#ifndef SCENE_H
#define SCENE_H

#include <stdbool.h>

#include "shape.h"
#include "light.h"
#include "set.h"
#include "camera.h"
#include "canvas.h"

typedef struct {
    Set shapes;
    Light light;
    Camera camera;
} Scene;


void ConstructScene(Scene* s, Camera c, Light);
void DeconstructScene(Scene* s);
unsigned long AddShape(Scene* s, Shape sp);
void IntersectScene(Scene* s, Ray r, Set* intersection_set);
bool IsInShadow(Scene *s, Tuple3 location);

#endif