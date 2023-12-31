#include "material.h"

Material NewMaterial(Tuple3 color) {
    Material m;
    m.color = color;
    m.ambient_reflection = 0.1f;
    m.diffuse_reflection = 0.9f;
    m.specular_reflection = 0.9f;
    m.shininess = 200;

    return m;
}