#ifndef SCENE_H
#define SCENE_H

#include <stdbool.h>

#include "light.h"
#include "tree.h"
#include "camera.h"
#include "canvas.h"

typedef struct Scene {
    Tree shapes;
    Light light;
    Camera camera;
} Scene;


void ConstructScene(Scene* s, Camera c, Light);
void DeconstructScene(Scene* s);
void IntersectScene(Scene* s, Ray r, Set* intersection_set);
bool IsInShadow(Scene *s, Tuple3 location);

void ReadScene(Scene* s, const char* filename);
Tuple3 ColorFor(Scene *s, Ray r);
Tuple3 ColorForLimited(Scene *s, Ray r, int limit);
void RenderScene(Scene* s, Canvas* c);
void RenderSceneUnthreaded(Scene*  s, Canvas* c);



#endif