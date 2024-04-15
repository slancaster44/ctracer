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

//Take a shading job, and convert it to a color
typedef Tuple3 (*Shader)(ShadingJob sj);

Tuple3 PhongShading(ShadingJob lj);

#endif
