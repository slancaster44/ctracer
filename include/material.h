#ifndef MATERIAL_H
#define MATERIAL_H

#include "tuple.h"

typedef struct {
    Tuple3 color;
    float ambient_reflection;
    float diffuse_reflection;
    float specular_reflection;
    float shininess;
} Material;

void ConstructMaterial(Material* m, Tuple3 color);

#endif