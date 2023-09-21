#include "light.h"

Light NewLight(Tuple3 origin) {
    Light l;
    l.origin = origin;
    l.color = NewColor(255, 255, 255, 255);
    return l;
}