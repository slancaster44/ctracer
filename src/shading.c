#include <math.h>

#include "scene.h"
#include "equality.h"
#include "shape.h"
#include "intersection.h"
#define BLACK NewColor(0, 0, 0, 0)

static int recursion_count = 0;
Tuple3 PhongShader(Scene* s, Intersection* i) {

    Tuple3 eyev = TupleNegate(i->ray.direction);
    Tuple3 pos = RayPosition(i->ray, i->ray_times[0]);
    Tuple3 normal = NormalAt(i->shape_ptr, pos);
    if (TupleDotProduct(normal, eyev) < 0) {
        normal = TupleNegate(normal);
    }
    Tuple3 reflectv = TupleReflect(i->ray.direction, normal);
    Material material = i->shape_ptr->material;

    Tuple3 color = PatternColorAt(i->shape_ptr, pos);

    Tuple3 effective_color = TupleMultiply(color, s->light.color);
    Tuple3 ambient = TupleScalarMultiply(effective_color, material.ambient_reflection);

    Tuple3 diffuse;
    Tuple3 specular;

    Tuple3 light_pos_vector = TupleNormalize(TupleSubtract(s->light.origin, pos));
    float light_dot_normal = TupleDotProduct(light_pos_vector, normal); //Measure of light to surface angle

    if (light_dot_normal < 0.0 || IsInShadow(s, pos)) {
        diffuse = BLACK;
        specular = BLACK;

    } else {

        diffuse = TupleScalarMultiply(effective_color, material.diffuse_reflection * light_dot_normal);

        Tuple3 reflect_vector = TupleReflect(TupleNegate(light_pos_vector), normal);
        float reflect_dot_eye = TupleDotProduct(reflect_vector, eyev);

        if (reflect_dot_eye < 0.0) {
            specular = BLACK;
        } else {
            float factor = powf(reflect_dot_eye, material.shininess);
            specular = TupleScalarMultiply(s->light.color, material.specular_reflection * factor);
        }
    }

    Tuple3 general_reflection = BLACK;
    if (!FloatEquality(0, material.general_reflection) && s != NULL) {
        recursion_count++;
        if (recursion_count > (1 << 3)) {
            recursion_count = 0;
        } else {
            general_reflection = ColorFor(s, NewRay(pos, reflectv));
            general_reflection = TupleScalarMultiply(general_reflection, material.general_reflection);
        }

    }

    return TupleAdd(general_reflection, TupleAdd(ambient, TupleAdd(diffuse, specular)));
}