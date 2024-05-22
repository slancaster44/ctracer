#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"
#include "ray.h"

typedef struct
{
    Matrix4x4 view_transformation;
    Matrix4x4 inverse_view_transformation;

    unsigned width;
    unsigned height;
    double fov;

    double half_width;
    double half_height;
    double pixel_size;
} Camera;

Camera NewCamera(unsigned width, unsigned height, double fov);
void CameraApplyTransformation(Camera *c, Matrix4x4 t);
Ray RayForPixel(Camera *c, unsigned x, unsigned y);

#endif
