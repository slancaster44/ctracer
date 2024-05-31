#include <string.h>
#include <float.h>
#include "shmem.h"
#include "canvas.h"

void ConstructCanvas(Canvas *c, unsigned width, unsigned height)
{
    c->buffer = (Tuple3 *) shmalloc(width * height * sizeof(Tuple3));
    c->canvas_width = width;
    c->canvas_height = height;
}

void DeconstructCanvas(Canvas *c)
{
    shfree(c->buffer, c->canvas_width * c->canvas_height * sizeof(Tuple3));
}

void WritePixel(Canvas *c, Tuple3 color, unsigned x, unsigned y)
{
    unsigned location = (c->canvas_width * y) + x;
    DirectWritePixel(c, color, location);
}

void DirectWritePixel(Canvas *c, Tuple3 color, unsigned location)
{
    c->buffer[location] = color;
}

void WriteToPPM(Canvas *c, const char *filename)
{
    FILE *fp = fopen(filename, "w");

    fputc('P', fp);
    fputc('3', fp);
    fputc('\n', fp);

    char width_str[sizeof(int) * 8 + 1];
    char height_str[sizeof(int) * 8 + 1];

    sprintf(width_str, "%d", c->canvas_width);
    sprintf(height_str, "%d", c->canvas_height);

    fputs(width_str, fp);
    fputc(' ', fp);
    fputs(height_str, fp);
    fputc('\n', fp);
    fputs("255\n", fp);

    for (unsigned i = 0; i < c->canvas_height * c->canvas_width; i++)
    {
        double red = fmin(c->buffer[i][0] * 255, 255);
        double green = fmin(c->buffer[i][1] * 255, 255);
        double blue = fmin(c->buffer[i][2] * 255, 255);

        red = fmax(0, red);
        green = fmax(0, green);
        blue = fmax(0, blue);

        char red_str[11];
        char blue_str[11];
        char green_str[11];

        sprintf(red_str, "%d", (int)red);
        sprintf(blue_str, "%d", (int)green);
        sprintf(green_str, "%d", (int)blue);

        fputs(red_str, fp);
        fputc(' ', fp);
        fputs(blue_str, fp);
        fputc(' ', fp);
        fputs(green_str, fp);
        fputc('\n', fp);
    }

    fclose(fp);

    printf("Scene written to '%s'\n", filename);
}
