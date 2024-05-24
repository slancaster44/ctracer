#ifndef SHAPE_H
#define SHAPE_H

#include "tuple.h"
#include "ray.h"
#include "matrix.h"
#include "material.h"

/** Tags to indicate what type of shape is being represented */
typedef enum
{
    /** Untransformed, a shape with this tag will be the unit sphere */
    SPHERE,
    /** Untransfromed, a shape with this tag will be an infinite plane on the 'x' plane */
    PLANE,
    /** Untransformed, a shape with this tag will be a cube centered at (0, 0, 0) and a side length of 2.0 */
    CUBE,

    /** Untransformed, a shape with this tag will be a triangle with corners at (1, 0, 0) (0, 1, 0) (0, 0, 1) */
    TRIANGLE,
} SHAPE_TYPE;

/** Used to represent a shape in a scene */
typedef struct Shape
{
    /** @private */
    Matrix4x4 transformation;
    /** @private */
    Matrix4x4 inverse_transform;

    /** The shape's material */
    Material material;

    /** The shapes type tag, indicates the type of shape being represented */
    SHAPE_TYPE type;
} Shape;

/**
 * @memberof Shape
 * Transform a given shape using the given transformation matrix
 */
void ApplyTransformation(Shape *s, Matrix4x4 t);

/**
 * @memberof Shape
 * Generate a new sphere
 * 
 * @param 'Tuple3 center_point' The center point of the sphere
 * @param 'double radius' The radius of the sphere
 * @returns The sphere with the given center point and radius 
 */
Shape NewSphere(Tuple3 center_point, double radius);

/**
 * @memberof Shape
 * Generate a new plane
 * 
 * @param 'Tuple3 pnt' A point that will exist on generated plane
 * @param 'Tuple3 normal_vector' The surface normal on top of the plane
 * @returns A plane on the given point, facing the direction of the given surface normal vector
 */
Shape NewPlane(Tuple3 pnt, Tuple3 normal_vector);

/**
 * @memberof Shape
 * Generate a new cube
 * 
 * @param 'Tuple3 pnt' The center point of the cube
 * @param 'double size' The length of all the cube's sides
 * @returns A cube centered at the given point and with sides of length 'size' 
 */
Shape NewCube(Tuple3 pnt, double size);

/**
 * @memberof Shape
 * Generate a new triangle
 * @param 'Tuple3 p1, p2, p3' The three corners of the cube
 * @returns A triangle with corners at the given poinnts
 */
Shape NewTriangle(Tuple3 p1, Tuple3 p2, Tuple3 p3);

/** @private */
extern Tuple3 UNIT_TRI_P1;
/** @private */
extern Tuple3 UNIT_TRI_P2;
/** @private */
extern Tuple3 UNIT_TRI_P3;

/** @private */
extern Tuple3 UNIT_TRI_E1;
/** @private */
extern Tuple3 UNIT_TRI_E2;
/** @private */
extern Tuple3 UNIT_TRI_E3;

/** @private */
extern Matrix4x4 UNIT_TRI_EINV;
/** @private */
extern Tuple3 UNIT_TRI_NORM;

/**
 * @memberof Shape
 * Calculate the surface normal for a given shape at a given point 
 */
Tuple3 NormalAt(Shape *s, Tuple3 pnt);



/**
 * @memberof Scene
 * Add a given shape to the given scene. This shape will intiially be
 * addedd to the root node of the scene's shape tree. However, when RenderScene() is
 * called, the tree will be transformed into a bounding volume hierarchy 
 */
void AddShape(Scene *s, Shape sp);

#endif
