#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"
#include "ray.h"

/**
 * Represents a scene's camera. Fields in this struct are
 * calculated by NewCamera() and CameraApplyTransformation(), and should
 * not be directly manipulated
 */
typedef struct
{
    /**
     * @private
     * A transformation to be applied to the camera
     */
    Matrix4x4 view_transformation;

    /**
     * @private
     * The inverse of view_transformation, cached here to prevent extra recalculations
     * of the inverse transformation
     */
    Matrix4x4 inverse_view_transformation;

    /** @private Number of pixels wide the camera is */
    unsigned width;

    /** @private Number of pixels high the camera is */
    unsigned height;

    /** @private Camera's field of view */
    double fov;

    /** @private Value that is cached internally to facilitate camera to world coordinate conversions */
    double half_width;

    /** @private Value that is cached internally to facilitate camera to world coordinate conversions */
    double half_height;

    /** @private Size in world coordinates of a camera pixel */
    double pixel_size;
} Camera;


/**
 * @memberof Camera
 * Constructs a camera, centered at the origin, with the given height, width, and field of view
 * 
 * @param 'unsigned width' width of the camera in pixels
 * @param 'unsigned height' height of camera in pixels
 * @param 'double fov' Camera's field of view
*/
Camera NewCamera(unsigned width, unsigned height, double fov);

/**
 * @memberof Camera
 * Applies a transformation to a given camera 
 * 
 * @param 'Camera *c' The camera to transform
 * @param 'Matrix4x4 t' The transformation matrix to apply to the matrix
 */
void CameraApplyTransformation(Camera *c, Matrix4x4 t);

/**
 * @memberof Camera
 * Generates a ray for a given pixel
 * 
 * @param 'Camera *c' The camera the ray will be gerated for
 * @param 'unsigned x' The x coord of the pixel the ray will pass through
 * @param 'unsigned y' the y coord of the pixel the ray will pass through
 */
Ray RayForPixel(Camera *c, unsigned x, unsigned y);

#endif
