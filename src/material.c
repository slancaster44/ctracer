#include "material.h"

void ConstructMaterial(Material* m, Tuple3 color) {
    m->color = color;
    m->ambient_reflection = 0.1;
    m->diffuse_reflection = 0.9;
    m->specular_reflection = 0.9;
    m->shininess = 200;
}