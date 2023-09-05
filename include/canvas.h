#ifndef CANVAS_H
#define CANVAS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tuple.h"
#include "set.h"

typedef struct {
    Tuple3* buffer;
    float* depth_buffer;
    size_t canvas_width;
    size_t canvas_height;
} Canvas;


void ConstructCanvas(Canvas* c, size_t width, size_t height);
void DeconstructCanvas(Canvas* c);
void WritePixel(Canvas* c, Tuple3 color, size_t x, size_t y, float depth);
void WriteToPPM(Canvas* c, const char* filename);

#endif