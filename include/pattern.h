#ifndef PATTERN_H
#define PATTERN_H

#include "tuple.h"
#include "matrix.h"

typedef enum
{
    SOLID,
    STRIPED,
    CHECKERED,
    RINGED,
    GRADIENT,
} PATTERN_TYPE;

typedef struct
{
    Tuple3 color_a;
    Tuple3 color_b;
    PATTERN_TYPE type;
    Matrix4x4 transform;
    Matrix4x4 inverse_transform;
} Pattern;

typedef struct Shape Shape;
Tuple3 PatternColorAt(Shape *s, Tuple3 position);
Pattern NewSolidPattern(Tuple3 color);
Pattern NewStripedPattern(Tuple3 color_a, Tuple3 color_b);
Pattern TransformPattern(Pattern p, Matrix4x4 m);

#endif
