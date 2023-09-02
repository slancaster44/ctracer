#include "light.h"

void ConstructLight(Light* l, Tuple3 origin) {
    l->origin = origin;
    l->color = NewColor(255, 255, 255, 255);
}