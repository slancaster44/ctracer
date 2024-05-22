#include "equality.h"

bool FloatEquality(double f1, double f2)
{
    return fabs(f1 - f2) < EQUALITY_EPSILON;
}
