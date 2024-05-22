#include "material.h"

Material NewMaterial(Tuple3 color)
{
    Material m;
    m.pattern = NewSolidPattern(color);
    m.ambient_reflection = 0.1;
    m.diffuse_reflection = 0.9;
    m.specular_reflection = 0.9;
    m.general_reflection = 0.0;
    m.shininess = 200;
    m.refractive_index = 1.0;
    m.transparency = 0.0;
    m.shader = PhongShader;

    return m;
}
