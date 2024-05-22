#ifndef CANVAS_H
#define CANVAS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tuple.h"
#include "set.h"

/**
 * Stores the output of a rendered frame 
 */
typedef struct
{
    /** @private Buffer of pixel colors. 
     * This buffer is shared memory, so that multiple
     * child processes can render different sections of a scene
     */
    Tuple3 *buffer;

    /** @private Width of canvas in pixels*/
    unsigned canvas_width;

    /** @private Height of canvas in pixels*/
    unsigned canvas_height;
} Canvas;

/**
 * @memberof Canvas
 * Generates a canvas of the given height and width 
 * 
 * @param 'Canvas *c' The canvas struct to be filled in
 * @param 'unsigned width' the width, in pixels, of the canvas
 * @param 'unsigned height' the height, in pixels, of the canvas
 */
void ConstructCanvas(Canvas *c, unsigned width, unsigned height);

/**
 * @memberof Canvas
 * Cleans up a canvas' memory allocations. After DeconstructCanvas() is called on a
 * canvas, any attempts to write to the canvas will result in a SEGFAULT
 * 
 * @param 'Canvas *c' The canvas to deconstruct
 */
void DeconstructCanvas(Canvas *c);

/**
 * @memberof Canvas
 * Writes a color to a given pixel
 * 
 * @param 'Tuple3 color' A tuple representing the red, green, and blue components of a color. These
 * color components must be scaled in between 0.0 (no intensity) and 1.0 (maximum intesity)
 * @param 'unsigned x' The x coordinate to write the color to
 * @param 'unsigned y' The y coordinate to write the color to
 */
void WritePixel(Canvas *c, Tuple3 color, unsigned x, unsigned y);

/**
 * @memberof Canvas
 * Similar to WritePixel(), but uses an offset equal to (canvas_width * y) + x, rather than x and y
 * coordinates
 * 
 * @param 'Tuple3 color' A tuple representing the red, green, and blue components of a color. These
 * color components must be scaled in between 0.0 (no intensity) and 1.0 (maximum intesity)
 * @param 'unsigned i' The offset into the canvas to write the color to
 */
void DirectWritePixel(Canvas *c, Tuple3 color, unsigned i);

/**
 * @memberof Canvas
 * Writes a canvas to an output *.ppm file. If the file already contains data, the
 * file will be emptied before the canvas is writen to it.
 * 
 * @param 'Canvas *c' The canvvas to write to a file
 * @param 'char *filename' The name of the file to write to
 */
void WriteToPPM(Canvas *c, const char *filename);

#endif
