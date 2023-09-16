#include <math.h>

#include "shading.h"
#define BLACK NewColor(0, 0, 0, 0)

Tuple3 PhongShading(ShadingJob sj) {
    Tuple3 effective_color = TupleMultiply(sj.material.color, sj.light.color);
    Tuple3 ambient = TupleScalarMultiply(effective_color, sj.material.ambient_reflection);

    if (sj.shadow) {
        return ambient; //Multiply this by inverse of sj.shadow as a float?
    }

    Tuple3 diffuse;
    Tuple3 specular;

    Tuple3 light_pos_vector = TupleNormalize(TupleSubtract(sj.light.origin, sj.position));
    float light_dot_normal = TupleDotProduct(light_pos_vector, sj.surface_normal); //Measure of light to surface angle


    if (light_dot_normal < 0.0) {
        diffuse = BLACK;
        specular = BLACK;

    } else {

        diffuse = TupleScalarMultiply(effective_color, sj.material.diffuse_reflection * light_dot_normal);

        Tuple3 reflect_vector = TupleReflect(TupleNegate(light_pos_vector), sj.surface_normal);
        float reflect_dot_eye = TupleDotProduct(reflect_vector, sj.eye_vector);

        if (reflect_dot_eye < 0.0) {
            specular = BLACK;
        } else {
            float factor = powf(reflect_dot_eye, sj.material.shininess);
            specular = TupleScalarMultiply(sj.light.color, sj.material.specular_reflection * factor);
        }
    }
    
    return TupleAdd(ambient, TupleAdd(diffuse, specular));
}

