#ifndef LIGHT_H
#define LIGHT_H

#include "tuple.h"

typedef struct
{
    Tuple3 origin;
    Tuple3 color;
} Light;

Light NewLight(Tuple3 origin);

#endif
