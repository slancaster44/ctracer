#ifndef MATRIX_H
#define MATRIX_H

#include <immintrin.h>
#include <stdio.h>
#include <stdbool.h>

#include "tuple.h"
#include "alignment.h"

/**
 * A union representing a 4 by 4 matrix
 */
typedef union
{
    /** 
     * @name contents
     *  The contents of the matrix represented as 4 256-bit vectors of four doubles 
     */
    __m256d contents[4] align;

    /**
     * @name chunks
     * The contents of the matrix represented as 2 512-bit vectors of eight doubles
    */
   __m512d chunks[2] align;
} Matrix4x4;

/**
 * @memberof Matrix4x4
 * Compares two matricies for perfect equality
 * 
 * @param 'Matrix4x4 m1' The first matrix to be compared
 * @param 'Matrix4x4 m2' The second matrix to be compared
 * @returns Returns 'true' if every element in m1 is exactly equal to
 * every element in m2
 */
bool MatrixEqual(Matrix4x4 m1, Matrix4x4 m2);

/**
 * @memberof Matrix4x4
 * Compares two matricies for equality with EQUALITY_EPSILON.
 * 
 * @param 'Matrix4x4 m1' The first matrix to be compared
 * @param 'Matrix4x4 m2' The second matrix to be compared
 * @returns Returns 'true' if every element in m1 is close enough
 * to m2 to be considered equal
 */
bool MatrixFuzzyEqual(Matrix4x4 m1, Matrix4x4 m2);

/**
 * @memberof Matrix4x4
 * Adds every element of m1 to m2, returns the result 
 * 
 * @param 'Matrix4x4 m1' The first addend
 * @param 'Matrix4x4 m2' The second addend
 * @returns Returns a Matrix4x4 where for every element n,
 * <code> result[n] = m1[n] + m2[n] </code>
 */
Matrix4x4 MatrixAdd(Matrix4x4 m1, Matrix4x4 m2);

/**
 * @memberof Matrix4x4
 * Multiply every element in m1 by f1, return the result
 * 
 * @param 'Matrix4x4 m1' the matrix to be scaled
 * @param 'double f1' Scalar to multiply the matrix by
 * @returns Returns a Matrix4x4 where for ever element n, <code> result[n] = m1[n] * f1 </code>
 */
Matrix4x4 MatrixScalarMultiply(Matrix4x4 m1, double f1);

/**
 * @memberof Matrix4x4
 * Matrix multiply m1 by m2
 * 
 * @param 'Matrix4x4 m1' Multiplier
 * @param 'Matrix4x4 m2' Multiplicand
 * @returns 'm1' multiplied by 'm2' 
 */
Matrix4x4 MatrixMultiply(Matrix4x4 m1, Matrix4x4 m2);

/**
 * @memberof Matrix4x4
 * Transpose m1, return the result
 * 
 * @param 'Matrix4x4 m1' Matrix to be transposed
 * @returns The transposed result 
 */
Matrix4x4 MatrixTranspose(Matrix4x4 m1);

/**
 * @memberof Matrix4x4
 * Inverts m1, returns the result
 * 
 * @param 'Matrix4x4 m1' The matrix to invert
 * @returns The inverted result
 */
Matrix4x4 MatrixInvert(Matrix4x4 m1);

/**
 * @memberof Matrix4x4
 * Generates a translation matrix
 * 
 * @param 'double x' Translation factor in the x direction
 * @param 'double y' Translation factor in the y direction
 * @param 'double z' Translation factor in the z direction
 * @returns The matrix representing the given translation
*/
Matrix4x4 TranslationMatrix(double x, double y, double z);

/**
 * @memberof Matrix4x4
 * Generates a rotation matrix around the 'x' axis
 * 
 * @param 'double theta' The rotation angle in radians
 * @returns The matrix representing the given rotation
 */
Matrix4x4 RotationXMatrix(double theta);

/**
 * @memberof Matrix4x4
 * Generates a rotation matrix around the 'y' axis
 * 
 * @param 'double theta' The rotation angle in radians
 * @returns The matrix representing the given rotation
 */
Matrix4x4 RotationYMatrix(double theta);

/**
 * @memberof Matrix4x4
 * Generates a rotation matrix around the 'z' axis
 * 
 * @param 'double theta' The rotation angle in radians
 * @returns The matrix representing the given rotation
 */
Matrix4x4 RotationZMatrix(double theta);

/**
 * @memberof Matrix4x4
 * Generates a rotation matrix around all axes
 * 
 * @param 'double rot_x' The rotation angle in radians around the 'x' axis
 * @param 'double rot_y' The rotation angle in radians around the 'y' axis
 * @param 'double rot_z' The rotation angle in radians around the 'z' axis
 * @returns The matrix given by: 
 * <code> RotationXMatrix(rot_x) * RotationYMatrix(rot_y) * RotationZMatrix(rot_z) </code>
 */
Matrix4x4 RotationMatrix(double rot_x, double rot_y, double rot_z);

/**
 * @memberof Matrix4x4
 * Generates a scaling matrix
 * 
 * @param 'double x' The scaling factor along the 'x' axis
 * @param 'double y' The scaling factor along the 'y' axis
 * @param 'double z' The scaling factor along the 'z' axis
 * @returns The matrix representing the given scaling
 */
Matrix4x4 ScalingMatrix(double x, double y, double z);

/**
 * @memberof Matrix4x4
 * Generates a shearing matrix
 * 
 * @param 'double xy' Factor by which the 'y' coordinate will effect the output 'x' coordinate when the transformation
 * is applied
 * @param 'double xz' Factor by which the 'z' coordinate will effect the output 'x' coordinate when the transformation
 * is applied
 * @param 'double yx' Factor by which the 'x' coordinate will effect the output 'y' coordinate when the transformation
 * is applied
 * @param 'double yz' Factor by which the 'z' coordinate will effect the output 'y' coordinate when the transformation
 * is applied
 * @param 'double zx' Factor by which the 'x' coordinate will effect the output 'z' coordinate when the transformation
 * is applied
 * @param 'double zy' Factor by which the 'y' coordinate will effect the output 'z' coordinate when the transformation
 * is applied
 * 
 * @returns The resulting shearing matrix
 */
Matrix4x4 ShearingMatrix(double xy, double xz, double yx, double yz, double zx, double zy);

/**
 * @memberof Tuple3
 * Multiply the given matrix by the given tuple, return the result. Used to apply transformation
 * matricies to tuples
 * 
 * @param 'Matrix4x4 m1' The transformation to manipulate 't1' by
 * @param 'Tuple3 t1' The tuple to transform
 * @returns The transformed tuple
 */
Tuple3 MatrixTupleMultiply(Matrix4x4 m1, Tuple3 t1);

/**
 * @memberof Matrix4x4
 * Print the given matrix to the console 
 */
void PrintMatrix(Matrix4x4 m1);

/**
 * @memberof Matrix4x4
 * @returns The identity matrix
*/
Matrix4x4 IdentityMatrix();

/**
 * @memberof Matrix4x4
 * Generate a view transformation matrix. These are often applied to a camera to
 * correctly orient the camera
 * 
 * @param 'Tuple3 from' The location of the camera
 * @param 'Tuple3 to' The location the camera points to
 * @param 'Tuple3 upvec' The upwards direction of the camera
 * @returns A transformation matrix to correctly orient a camera
 */
Matrix4x4 ViewMatrix(Tuple3 from, Tuple3 to, Tuple3 upvec);

/**
 * @memberof Matrix4x4
 * Convert a matrix that cannot be inverted into a very similar matrix that
 * is invertable. This is achieved by adding the EQUALITY_EPSILON to all 
 * zeroed fields on the diagonal axis of the matrix.
 * 
 * @param 'Matrix4x4 m' The matrix to be rectified
 * @returns The rectified matrix
*/
Matrix4x4 RectifyMatrix(Matrix4x4 m);

#endif
