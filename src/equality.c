#include "equality.h"

bool FloatEquality(float f1, float f2) {
    return fabsf(f1 - f2) < EQUALITY_EPSILON;
}
