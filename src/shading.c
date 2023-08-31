#include "shading.h"

Tuple3 PhongShading(Material m, Light l, Tuple3 position, Tuple3 eye_vector, Tuple3 surface_normal) {
    Tuple3 effective_color = TupleMultiply(m.color, l.color);


    Tuple3 light_pos_vector = TupleNormalize(TupleSubtract(l.origin, position));
    float light_dot_normal = TupleDotProduct(light_pos_vector, surface_normal); //Measure of light to surface angle

    
}