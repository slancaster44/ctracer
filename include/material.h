#ifndef MATERIAL_H
#define MATERIAL_H

#include "tuple.h"

typedef struct _ShadingJob ShadingJob;
typedef Tuple3 (*Shader)(ShadingJob sj);

typedef struct {
    Tuple3 color;
    float ambient_reflection;
    float diffuse_reflection;
    float specular_reflection;
    float shininess;
    float general_reflection;
    Shader shader;
} Material;

Material NewMaterial(Tuple3 color);

#endif