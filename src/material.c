#include "material.h"

Material NewMaterial(Tuple3 color) {
    Material m;
    m.color = color;
    m.ambient_reflection = 0.1;
    m.diffuse_reflection = 0.9;
    m.specular_reflection = 0.9;
    m.shininess = 200;

    return m;
}