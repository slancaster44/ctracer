#include "shape.h"
#include "pattern.h"
#include "equality.h"
#include <math.h>
#include <stdio.h>

Tuple3 PatternColorAt(Shape* s, Tuple3 pos_orig) {

    Pattern p = s->material.pattern;
    if (p.type == SOLID) {
        return p.color_a;
    }

    Tuple3 position = MatrixTupleMultiply(s->inverse_transform, pos_orig);
    position = MatrixTupleMultiply(s->material.pattern.inverse_transform, position);

    switch (p.type) {
    case STRIPED: ;
        return fmod(floor(position[0]), 2) == 0 ? p.color_a : p.color_b;
        break;
    case RINGED: ;
        double magn = sqrt((position[0] * position[0]) + (position[1] * position[1]));
        return fmod(floor(magn), 2.0) == 0 ? p.color_a : p.color_b;
        break;
    case CHECKERED: ;
        position[3] = 0.0; //The final element in the vector should not contribute
        double sum = TupleFloorSum(position);
        return fmod(sum, 2.0) == 0.0 ? p.color_a : p.color_b;
        break;
    case GRADIENT: ;
        double position_scalar = (position[0] + 1) / 2.0; //Distance from -1 to 1
        Tuple3 color_offset = TupleSubtract(p.color_b, p.color_a);
        return TupleAdd(p.color_a, TupleScalarMultiply(color_offset, position_scalar));
        break;
    default: ;
        printf("Unknown pattern\n");
        exit(1);
    }
}

Pattern NewSolidPattern(Tuple3 color) {
    Pattern p = {
        .color_a = color,
        .color_b = NewColor(0, 0, 0, 0),
        .type = SOLID,
    };

    p = TransformPattern(p, IdentityMatrix());

    return p;
}

Pattern NewStripedPattern(Tuple3 color_a, Tuple3 color_b) {
    Pattern p = {
        .color_a = color_a,
        .color_b = color_b,
        .type = STRIPED,
        .transform = IdentityMatrix()
    };

    p = TransformPattern(p, IdentityMatrix());

    return p;
}

Pattern TransformPattern(Pattern p, Matrix4x4 m) {
    p.transform = m;
    p.inverse_transform = MatrixInvert(m);
    return p;
}