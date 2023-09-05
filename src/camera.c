#include "camera.h"
#include "trig.h"

void ConstructCamera(Camera* c, unsigned width, unsigned height, float fov) {
    c->width = width;
    c->height = height;
    c->fov = fov;

    c->view_transformation = IdentityMatrix();
    c->inverse_view_transformation = IdentityMatrix();

    float half_view = _tan(fov / 2);
    float aspect_ratio = (float) width / (float) height;

    if (aspect_ratio >= 1) {
        c->half_width = half_view;
        c->half_height = half_view / aspect_ratio;
    } else {
        c->half_width = half_view * aspect_ratio;
        c->half_height = half_view;
    }

    c->pixel_size = (c->half_width * 2) / (float) c->width;
}

void CameraApplyTransformation(Camera* c, Matrix4x4 t) {
    c->view_transformation = t;
    c->inverse_view_transformation = MatrixInvert(t);
}

Ray RayForPixel(Camera* c, unsigned x, unsigned y) {
    float offset_x = ((float) x + 0.5) * c->pixel_size;
    float offset_y = ((float) y + 0.5) * c->pixel_size;

    float world_x = c->half_width - offset_x;
    float world_y = c->half_height - offset_y;

    Tuple3 pixel_location = MatrixTupleMultiply(c->inverse_view_transformation, NewPnt3(world_x, world_y, -1));
    Tuple3 ray_origin = MatrixTupleMultiply(c->inverse_view_transformation, NewPnt3(0, 0, 0));
    Tuple3 ray_direction = TupleNormalize(TupleSubtract(pixel_location, ray_origin));

    Ray r;
    ConstructRay(&r, ray_origin, ray_direction);

    return r;
}