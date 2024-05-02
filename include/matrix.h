#ifndef MATRIX_H
#define MATRIX_H

#include <immintrin.h>
#include <stdio.h>

#include "tuple.h"

typedef union {
    __m256d contents[4] __attribute__ ((aligned (32)));
} Matrix4x4;

int MatrixEqual(Matrix4x4 m1, Matrix4x4 m2);
int MatrixFuzzyEqual(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 MatrixAdd(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 MatrixScalarMultiply(Matrix4x4 m1, double f1);
Matrix4x4 MatrixMultiply(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 MatrixTranspose(Matrix4x4 m1);
Matrix4x4 MatrixInvert(Matrix4x4 m1);

Matrix4x4 TranslationMatrix(double x, double y, double z);
Matrix4x4 RotationXMatrix(double theta);
Matrix4x4 RotationYMatrix(double theta);
Matrix4x4 RotationZMatrix(double theta);
Matrix4x4 RotationMatrix(double rot_x, double rot_y, double rot_z);
Matrix4x4 ScalingMatrix(double x, double y, double z);
Matrix4x4 ShearingMatrix(double xy, double xz, double yx, double yyz, double zx, double zy);

Tuple3 MatrixTupleMultiply(Matrix4x4 m1, Tuple3 t1);
void PrintMatrix(Matrix4x4 m1);

Matrix4x4 IdentityMatrix();
Matrix4x4 ViewMatrix(Tuple3 from, Tuple3 to, Tuple3 upvec);
Matrix4x4 SkewSymmetricCPMatrix(Tuple3 t1);
Matrix4x4 RectifyMatrix(Matrix4x4 m);

#endif