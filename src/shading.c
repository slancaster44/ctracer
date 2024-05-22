#include <math.h>
#include <string.h>

#include "scene.h"
#include "equality.h"
#include "shape.h"
#include "intersection.h"
#include "float.h"

#define BLACK NewColor(0, 0, 0, 0)

bool IsAllFalse(bool *set, size_t length)
{
    for (int i = 0; i < length; i++)
    {
        if (set[i])
        {
            return false;
        }
    }

    return true;
}

/* Generates two numbers required for calculating refraction
 * n[0]:
 */
void CalculateRefractionRatio(Set *intersections, unsigned long idx, double *n)
{
    size_t mapping_length = intersections->length;
    size_t mapping_size = intersections->length * sizeof(bool);
    bool *mapping = alloca(mapping_size);
    memset(mapping, false, mapping_size);

    Shape *last_added = NULL;

    for (unsigned long i = 0; i < intersections->length; i++)
    {
        if (i == idx)
        {
            if (IsAllFalse(mapping, mapping_length))
            {
                n[0] = 1.0;
            }
            else
            {
                n[0] = last_added->material.refractive_index;
            }
        }

        Intersection *intr = Index(intersections, i);
        last_added = mapping[i] && last_added == intr->shape_ptr ? NULL : intr->shape_ptr;
        mapping[i] = !mapping[i];

        if (i == idx)
        {
            if (IsAllFalse(mapping, mapping_length))
            {
                n[1] = 1.0;
            }
            else
            {
                n[1] = last_added->material.refractive_index;
            }

            break;
        }
    }
}

Tuple3 PhongShader(Scene *s, Set *intersections, unsigned long idx, int limit)
{
    if (limit == 0)
    {
        return BLACK;
    }

    Intersection *i = Index(intersections, idx);

    Tuple3 eyev = TupleNegate(i->ray.direction);
    Tuple3 pos = RayPosition(i->ray, i->ray_times[0]);
    Tuple3 normal = NormalAt(i->shape_ptr, pos);
    if (TupleDotProduct(normal, eyev) < 0)
    {
        normal = TupleNegate(normal);
    }

    Tuple3 offset_normal = TupleScalarMultiply(normal, EQUALITY_EPSILON);
    Tuple3 over_pos = TupleAdd(pos, offset_normal); // move the position out a little to handle floating point errors
    Tuple3 under_pos = TupleSubtract(pos, offset_normal);

    Material material = i->shape_ptr->material;
    Tuple3 color = PatternColorAt(i->shape_ptr, pos);
    Tuple3 effective_color = TupleMultiply(color, s->light.color);
    Tuple3 ambient = TupleScalarMultiply(effective_color, material.ambient_reflection);

    Tuple3 diffuse;
    Tuple3 specular;

    Tuple3 light_pos_vector = TupleNormalize(TupleSubtract(s->light.origin, over_pos));
    double light_dot_normal = TupleDotProduct(light_pos_vector, normal); // Measure of light to surface angle

    if (light_dot_normal < 0.0 || IsInShadow(s, over_pos))
    {
        diffuse = BLACK;
        specular = BLACK;
    }
    else
    {

        diffuse = TupleScalarMultiply(effective_color, material.diffuse_reflection * light_dot_normal);

        Tuple3 reflect_vector = TupleReflect(TupleNegate(light_pos_vector), normal);
        double reflect_dot_eye = TupleDotProduct(reflect_vector, eyev);

        if (reflect_dot_eye < 0.0)
        {
            specular = BLACK;
        }
        else
        {
            double factor = pow(reflect_dot_eye, material.shininess);
            specular = TupleScalarMultiply(s->light.color, material.specular_reflection * factor);
        }
    }

    Tuple3 general_reflection = BLACK;
    if (material.general_reflection != 0)
    {
        Tuple3 reflectv = TupleReflect(i->ray.direction, normal);
        general_reflection = ColorForLimited(s, NewRay(over_pos, reflectv), limit - 1);
        general_reflection = TupleScalarMultiply(general_reflection, material.general_reflection);
    }

    Tuple3 refraction_color = BLACK;
    double *n = alloca(2 * sizeof(double));
    CalculateRefractionRatio(intersections, idx, n);

    double n_ratio = n[0] / n[1];
    double cos_i = TupleDotProduct(eyev, normal);
    double sin2_t = (n_ratio * n_ratio) * (1 - (cos_i * cos_i));
    double cos_t = sqrt(1.0 - sin2_t);

    if (sin2_t < (1.0 + EQUALITY_EPSILON))
    {

        Tuple3 eyev_alt = TupleScalarMultiply(eyev, n_ratio);
        Tuple3 norm_alt = TupleScalarMultiply(normal, n_ratio * cos_i - cos_t);
        Tuple3 direction = TupleSubtract(norm_alt, eyev_alt);

        Ray refract_ray = NewRay(under_pos, direction);
        refraction_color = ColorForLimited(s, refract_ray, limit - 1);

        refraction_color = TupleScalarMultiply(refraction_color, material.transparency);
    }

    if (material.general_reflection > 0 && material.transparency > 0)
    {
        double cos = cos_i;
        double reflectance;

        if (n[0] > n[1])
        {
            if (sin2_t > 1.0)
            {
                reflectance = 1.0;
            }
            else
            {
                cos = cos_t;
                double r0 = (n[0] - n[1]) / (n[0] + n[2]);
                r0 = r0 * r0;

                reflectance = pow(r0 + (1 - r0) * (1 - cos), 5);
            }
        }

        general_reflection = TupleScalarMultiply(general_reflection, reflectance);
        refraction_color = TupleScalarMultiply(refraction_color, 1 - reflectance);
    }

    return TupleAdd(refraction_color, TupleAdd(general_reflection, TupleAdd(ambient, TupleAdd(diffuse, specular))));
}
