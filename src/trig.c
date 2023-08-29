#include "trig.h"
#include "sin_table.h"

#include <math.h>

const float TWO_PI = 2 * M_PI;

static inline float indexed_sin(int index) {
    return (sin_table[(int) index] + sin_table[(int) index + 1]) * 0.5;
}

static inline float indexed_cos(int index) {
    index = index + (SIN_TABLE_SIZE / 4);
    return indexed_sin(index);
}

float _sin(float theta) {
    float index = SIN_TABLE_SIZE * (fmodf(theta, TWO_PI) / TWO_PI);
    return indexed_sin((int) index);
}

float _cos(float theta) {
    float index = SIN_TABLE_SIZE * (fmodf(theta, TWO_PI) / TWO_PI);
    return indexed_cos(index);
}

float _tan(float theta) {
    float index = SIN_TABLE_SIZE * (fmodf(theta, TWO_PI) / TWO_PI);
    return indexed_sin(index) / indexed_cos(index);
}

int FloatEquality(float n1, float n2) {
    return fabsf(n1 - n2) < EQUALITY_EPSILON;
}
