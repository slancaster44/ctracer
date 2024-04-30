#ifndef MATERIAL_H
#define MATERIAL_H

#include "tuple.h"
#include "set.h"
#include "pattern.h"
#include "scene.h"

typedef Tuple3 (*Shader)(Scene* s, Set* intersections, unsigned long idx, int limit);
Tuple3 PhongShader(Scene* s, Set* intersections, unsigned long idx, int limit);

typedef struct {
    Pattern pattern;
    double ambient_reflection;
    double diffuse_reflection;
    double specular_reflection;
    double shininess;
    double general_reflection;
    double refractive_index;
    double transparency;
    Shader shader;
} Material;

Material NewMaterial(Tuple3 color);

#endif