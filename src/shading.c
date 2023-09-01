#include <math.h>

#include "shading.h"
#define BLACK NewColor(0, 0, 0, 0)

Tuple3 PhongShading(Material m, Light l, Tuple3 position, Tuple3 eye_vector, Tuple3 surface_normal) {
    Tuple3 effective_color = TupleMultiply(m.color, l.color);
    Tuple3 ambient = TupleScalarMultiply(effective_color, m.ambient_reflection);

    Tuple3 diffuse;
    Tuple3 specular;

    Tuple3 light_pos_vector = TupleNormalize(TupleSubtract(l.origin, position));
    float light_dot_normal = TupleDotProduct(light_pos_vector, surface_normal); //Measure of light to surface angle

    if (light_dot_normal < 0.0) {
        diffuse = BLACK;
        specular = BLACK;

    } else {
        diffuse = TupleScalarMultiply(effective_color, m.diffuse_reflection * light_dot_normal);

        Tuple3 reflect_vector = TupleReflect(TupleNegate(light_pos_vector), surface_normal);
        float reflect_dot_eye = TupleDotProduct(reflect_vector, eye_vector);

        if (reflect_dot_eye <= 0.0) {
            specular = BLACK;
        } else {
            float factor = powf(reflect_dot_eye, m.shininess);
            specular = TupleScalarMultiply(l.color, m.specular_reflection * factor);
        }
    }
    
    return TupleAdd(ambient, TupleAdd(diffuse, specular));
}