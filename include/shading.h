#ifndef SHADE_H
#define SHADE_H

#include "tuple.h"
#include "light.h"
#include "material.h"

Tuple3 PhongShading(Material m, Light l, Tuple3 position, Tuple3 eye_vector, Tuple3 surface_normal);
Tuple3 StepShading(Material m, Light l, Tuple3 position, Tuple3 eye_vector, Tuple3 surface_normal, float width);

#endif