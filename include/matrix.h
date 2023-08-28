#ifndef MATRIX_H
#define MATRIX_H

#include <immintrin.h>
#include <stdio.h>

#include "tuple.h"

typedef union {
    __m128 contents[4];
    __m256 chunks[2];
} Matrix4x4;

int MatrixEqual(Matrix4x4 m1, Matrix4x4 m2);
int MatrixFuzzyEqual(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 MatrixMultiply(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 MatrixTranspose(Matrix4x4 m1);
Matrix4x4 MatrixInvert(Matrix4x4 m1);

Matrix4x4 TranslationMatrix(float x, float y, float z);
Matrix4x4 RotationXMatrix(float theta);
Matrix4x4 RotationYMatrix(float theta);
Matrix4x4 RotationZMatrix(float theta);
Matrix4x4 ScalingMatrix(float x, float y, float z);
Matrix4x4 ShearingMatrix(float xy, float xz, float yx, float yyz, float zx, float zy);

Tuple3 MatrixTupleMultiply(Matrix4x4 m1, Tuple3 t1);
void PrintMatrix(Matrix4x4 m1);

#endif