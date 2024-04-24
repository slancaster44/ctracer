#include "camera.h"
#include <math.h>

Camera NewCamera(unsigned width, unsigned height, double fov) {
    Camera c;

    c.width = width;
    c.height = height;
    c.fov = fov;

    c.view_transformation = IdentityMatrix();
    c.inverse_view_transformation = IdentityMatrix();

    double half_view = tan(fov / 2);
    double aspect_ratio = (double) width / (double) height;

    if (aspect_ratio >= 1) {
        c.half_width = half_view;
        c.half_height = half_view / aspect_ratio;
    } else {
        c.half_width = half_view * aspect_ratio;
        c.half_height = half_view;
    }

    c.pixel_size = (c.half_width * 2) / (double) c.width;

    return c;
}

void CameraApplyTransformation(Camera* c, Matrix4x4 t) {
    c->view_transformation = t;
    c->inverse_view_transformation = MatrixInvert(t);
}

Ray RayForPixel(Camera* c, unsigned x, unsigned y) {
    double offset_x = ((double) x + 0.5) * c->pixel_size;
    double offset_y = ((double) y + 0.5) * c->pixel_size;

    double world_x = c->half_width - offset_x;
    double world_y = c->half_height - offset_y;

    Tuple3 pixel_location = MatrixTupleMultiply(c->inverse_view_transformation, NewPnt3(world_x, world_y, -1));
    Tuple3 ray_origin = MatrixTupleMultiply(c->inverse_view_transformation, NewPnt3(0, 0, 0));
    Tuple3 ray_direction = TupleNormalize(TupleSubtract(pixel_location, ray_origin));

    Ray r = NewRay(ray_origin, ray_direction);

    return r;
}