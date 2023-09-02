#ifndef LIGHT_H
#define LIGHT_H

#include "tuple.h"

typedef struct {
    Tuple3 origin;
    Tuple3 color;
} Light;

void ConstructLight(Light* l, Tuple3 origin);

#endif