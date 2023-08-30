#include "shape.h"

Tuple3 SphereNormalAt(Shape s, Tuple3 p) {
    p = MatrixTupleMultiply(s.inverse_transform, p);

    Tuple3 obj_normal = p;
    obj_normal[3] = 0;

    Tuple3 world_normal = MatrixTupleMultiply(MatrixTranspose(s.inverse_transform), obj_normal);
    world_normal[3] = 0;
    
    return TupleNormalize(world_normal);
}

Tuple3 NormalAt(Shape s, Tuple3 pnt) {
    switch (s.type) {
    case SPHERE:
        return SphereNormalAt(s, pnt);
        break;
    default:
        printf("Cannot find normal for unkown shape");
        break;
    }
}