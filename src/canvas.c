#include <string.h>
#include <float.h>
#include "shmem.h"
#include "canvas.h"

void ConstructCanvas(Canvas* c, unsigned width, unsigned height) {
    c->buffer = (Tuple3*) shmalloc(width * height * sizeof(Tuple3));
    c->canvas_width = width;
    c->canvas_height = height;

    c->depth_buffer = (float*) shmalloc(width * height * sizeof(float));
    for (int i = 0; i < width * height; i++) {
        c->depth_buffer[i] = FLT_MAX;
    }
}

void DeconstructCanvas(Canvas* c) {
    shfree(c->buffer, c->canvas_width * c->canvas_height * sizeof(Tuple3));
    shfree(c->depth_buffer, c->canvas_width * c->canvas_height * sizeof(float));
}

void WritePixel(Canvas* c, Tuple3 color, unsigned x, unsigned y, float depth) {
    unsigned location = (c->canvas_width * y) + x;
 
    if (depth <= c->depth_buffer[location]) {
        c->buffer[location] = color;
        c->depth_buffer[location] = depth;
    }
}

void DirectWritePixel(Canvas* c, Tuple3 color, unsigned location, float depth) {
    if (depth <= c->depth_buffer[location]) {
        c->buffer[location] = color;
        c->depth_buffer[location] = depth;
    }
}

void WriteToPPM(Canvas* c, const char* filename) {
    FILE* fp = fopen(filename, "w");

    fputc('P', fp);
    fputc('3', fp);
    fputc('\n', fp);

    char width_str [sizeof(int) * 8 + 1];
    char height_str [sizeof(int) * 8 + 1];

    sprintf(width_str, "%d", c->canvas_width);
    sprintf(height_str, "%d", c->canvas_height);

    fputs(width_str, fp);
    fputc(' ', fp);
    fputs(height_str, fp);
    fputc('\n', fp);
    fputs("255\n", fp);

    for (unsigned i = 0; i < c->canvas_height * c->canvas_width; i++) {
        float red = fminf(c->buffer[i][0] * 255, 255);
        float green = fminf(c->buffer[i][1] * 255, 255);
        float blue = fminf(c->buffer[i][2] * 255, 255);


        char red_str[4];
        char blue_str[4];
        char green_str[4];

        sprintf(red_str, "%d", (int) red);
        sprintf(blue_str, "%d", (int) green);
        sprintf(green_str, "%d", (int) blue);

        fputs(red_str, fp);
        fputc(' ', fp);
        fputs(blue_str, fp);
        fputc(' ', fp);        
        fputs(green_str, fp);
        fputc('\n', fp);
    }

    fclose(fp);
}