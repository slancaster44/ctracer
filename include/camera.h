#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"
#include "ray.h"

typedef struct {
    Matrix4x4 view_transformation;
    Matrix4x4 inverse_view_transformation;

    unsigned width;
    unsigned height;
    float fov;

    float half_width;
    float half_height;
    float pixel_size;
} Camera;

Camera NewCamera(unsigned width, unsigned height, float fov);
void CameraApplyTransformation(Camera* c, Matrix4x4 t);
Ray RayForPixel(Camera* c, unsigned x, unsigned y);

#endif
