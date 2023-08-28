#ifndef CANVAS_H
#define CANVAS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tuple.h"

typedef struct {
    Tuple3* buffer;
    size_t canvas_width;
    size_t canvas_height;
} Canvas;


void ConstructCanvas(Canvas* c, size_t width, size_t height);
void DeconstructCanvas(Canvas* c);
void WritePixel(Canvas* c, Tuple3 color, size_t x, size_t y);
void WriteToPPM(Canvas* c, const char* filename);

#endif