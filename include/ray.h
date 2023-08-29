#ifndef RAY_H
#define RAY_H

#include "tuple.h"

typedef struct {
    Tuple3 origin;
    Tuple3 direction;
} Ray;

#endif