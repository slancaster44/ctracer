#include <math.h>

#include "shading.h"
#include "scene.h"
#include "equality.h"
#define BLACK NewColor(0, 0, 0, 0)

static int recursion_count = 0;

Tuple3 PhongShading(ShadingJob sj) {


    Tuple3 effective_color = TupleMultiply(sj.material.color, sj.light.color);
    Tuple3 ambient = TupleScalarMultiply(effective_color, sj.material.ambient_reflection);

    Tuple3 diffuse;
    Tuple3 specular;

    Tuple3 light_pos_vector = TupleNormalize(TupleSubtract(sj.light.origin, sj.position));
    float light_dot_normal = TupleDotProduct(light_pos_vector, sj.surface_normal); //Measure of light to surface angle

    if (light_dot_normal < 0.0 || sj.shadow) {
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

    Tuple3 general_reflection = BLACK;
    if (!FloatEquality(0, sj.material.general_reflection) && sj.scene_ptr != NULL) {
        recursion_count++;
        if (recursion_count > (1 << 3)) {
            recursion_count = 0;
        } else {
            general_reflection = ColorFor(sj.scene_ptr, NewRay(sj.position, sj.reflection_vector));
            general_reflection = TupleScalarMultiply(general_reflection, sj.material.general_reflection);
        }

    }

    return TupleAdd(general_reflection, TupleAdd(ambient, TupleAdd(diffuse, specular)));
}

