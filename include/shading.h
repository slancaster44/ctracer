#ifndef SHADE_H
#define SHADE_H

#include "tuple.h"
#include "light.h"
#include "material.h"

typedef struct {
    Material material;
    Light light;
    Tuple3 position;
    Tuple3 eye_vector;
    Tuple3 surface_normal;
    float shadow;
} ShadingJob;

Tuple3 PhongShading(ShadingJob lj);

#endif
