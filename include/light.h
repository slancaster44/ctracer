#ifndef LIGHT_H
#define LIGHT_H

#include "tuple.h"

/**
 * Represents a scene's point light
 */
typedef struct
{
    /** Light's origin */
    Tuple3 origin;

    /** Light's color */
    Tuple3 color;
} Light;

/**
 * @memberof Light
 * Generates a white point light at the given origin
 * 
 * @param 'Tuple3 origin' The location of the light in world space
 * @returns 'Light' the generated point light
 */
Light NewLight(Tuple3 origin);

#endif
