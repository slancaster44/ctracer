#ifndef MATERIAL_H
#define MATERIAL_H

#include "tuple.h"
#include "intersection.h"
#include "pattern.h"
#include "scene.h"

typedef Tuple3 (*Shader)(Scene* s, Intersection* i);
Tuple3 PhongShader(Scene* s, Intersection* i);

typedef struct {
    Pattern pattern;
    float ambient_reflection;
    float diffuse_reflection;
    float specular_reflection;
    float shininess;
    float general_reflection;
    Shader shader;
} Material;

Material NewMaterial(Tuple3 color);

#endif