#ifndef CANVAS_H
#define CANVAS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tuple.h"
#include "set.h"

typedef struct
{
    Tuple3 *buffer;
    unsigned canvas_width;
    unsigned canvas_height;
} Canvas;

void ConstructCanvas(Canvas *c, unsigned width, unsigned height);
void DeconstructCanvas(Canvas *c);
void WritePixel(Canvas *c, Tuple3 color, unsigned x, unsigned y);
void DirectWritePixel(Canvas *c, Tuple3 color, unsigned i);
void WriteToPPM(Canvas *c, const char *filename);

#endif
