#ifndef SHADE_H
#define SHADE_H

#include "tuple.h"
#include "light.h"
#include "scene.h"
#include "material.h"

typedef struct _ShadingJob {
    Material material;
    Light light;
    Tuple3 position;
    Tuple3 eye_vector;
    Tuple3 surface_normal;
    Tuple3 reflection_vector;
    Scene* scene_ptr;
    float shadow;
} ShadingJob;

//Take a shading job, and convert it to a color
Tuple3 PhongShading(ShadingJob lj);

#endif
