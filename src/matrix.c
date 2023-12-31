#include "matrix.h"
#include "equality.h"

#include <math.h>
#include <stdio.h>

#define SWIZZLE_M128(val, p1, p2, p3, p4) \
    _mm_permute_ps(val, p1 | (p2 << 2) | (p3 << 4) | (p4 << 6))
#define SHUFFLE_M128(v1, v2, p1, p2, p3, p4) \
    _mm_shuffle_ps(v1, v2, p1 | (p2 << 2) | (p3 << 4) | (p4 << 6))
#define ADJOINT_M128(val) \
    SWIZZLE_M128(val, 0,2,1,3)
#define SET_M256(a, b, c, d, e, f, g, h) \
    _mm256_set_ps(h, g, f, e, d, c, b, a)


int MatrixEqual(Matrix4x4 m1, Matrix4x4 m2) {
    __m256 cmp1 = _mm256_cmp_ps(m1.chunks[0], m2.chunks[0], _CMP_EQ_OQ);
    __m256 cmp2 = _mm256_cmp_ps(m1.chunks[1], m2.chunks[1], _CMP_EQ_OQ);

    unsigned r0 = (unsigned) _mm256_movemask_ps(cmp1);
    unsigned r1 = (unsigned) _mm256_movemask_ps(cmp2);

    return (r0 == 0xff) && (r1 == 0xff);
}

int MatrixFuzzyEqual(Matrix4x4 m1, Matrix4x4 m2) {
    __m256 diff0 = _mm256_sub_ps(m1.chunks[0], m2.chunks[0]);
    __m256 diff1 = _mm256_sub_ps(m1.chunks[1], m2.chunks[1]);

    //Mask Out Sign Bits
    __m256 mask = (__m256) _mm256_set1_epi32(0x7FFFFFFF);
    diff0 = _mm256_and_ps(diff0, mask);
    diff1 = _mm256_and_ps(diff1, mask);

    //Compare
    __m256 epsilon = _mm256_set1_ps(EQUALITY_EPSILON);
    __m256 cmp0 = _mm256_cmp_ps(diff0, epsilon, _CMP_LT_OQ);
    __m256 cmp1 = _mm256_cmp_ps(diff1, epsilon, _CMP_LT_OQ);

    unsigned r0 = (unsigned) _mm256_movemask_ps(cmp0);
    unsigned r1 = (unsigned) _mm256_movemask_ps(cmp1);

    return (r0 == 0xff) && (r1 == 0xff);
}

static inline __m128 matrixMultHelper(int a, Matrix4x4 m1, Matrix4x4 m2) {
    __m128 s0 = _mm_mul_ps(_mm_set1_ps(m1.contents[a][0]), m2.contents[0]);
    __m128 s1 = _mm_mul_ps(_mm_set1_ps(m1.contents[a][1]), m2.contents[1]);
    __m128 s2 = _mm_mul_ps(_mm_set1_ps(m1.contents[a][2]), m2.contents[2]);
    __m128 s3 = _mm_mul_ps(_mm_set1_ps(m1.contents[a][3]), m2.contents[3]);

    return _mm_add_ps(s0, _mm_add_ps(s1, _mm_add_ps(s2, s3)));
}

Matrix4x4 MatrixMultiply(Matrix4x4 m1, Matrix4x4 m2) {
    Matrix4x4 out;
    out.contents[0] = matrixMultHelper(0, m1, m2);
    out.contents[1] = matrixMultHelper(1, m1, m2);
    out.contents[2] = matrixMultHelper(2, m1, m2);
    out.contents[3] = matrixMultHelper(3, m1, m2);

    return out;
}

Matrix4x4 MatrixScalarMultiply(Matrix4x4 m1, float f1) {
    __m256 scale = _mm256_set1_ps(f1);

    Matrix4x4 m2;
    m2.chunks[0] = _mm256_mul_ps(m1.chunks[0], scale);
    m2.chunks[1] = _mm256_mul_ps(m1.chunks[1], scale);

    return m2;
}

Matrix4x4 MatrixAdd(Matrix4x4 m1, Matrix4x4 m2) {
    Matrix4x4 out;
    out.chunks[0] = _mm256_add_ps(m1.chunks[0], m2.chunks[0]);
    out.chunks[1] = _mm256_add_ps(m1.chunks[1], m2.chunks[1]);

    return out;
}

Tuple3 MatrixTupleMultiply(Matrix4x4 m1, Tuple3 t1) {
    Tuple3 out;
    out[0] = _mm_dp_ps(m1.contents[0], t1, 0xff)[0];
    out[1] = _mm_dp_ps(m1.contents[1], t1, 0xff)[0];
    out[2] = _mm_dp_ps(m1.contents[2], t1, 0xff)[0];
    out[3] = _mm_dp_ps(m1.contents[3], t1, 0xff)[0];

    return out;
}

Matrix4x4 MatrixTranspose(Matrix4x4 m1) {
    _MM_TRANSPOSE4_PS(m1.contents[0], m1.contents[1], m1.contents[2], m1.contents[3]);
    return m1;
}

static inline __m128 invertHelper(__m128 m1) {
    __m128 out = SWIZZLE_M128(m1, 3, 1, 2, 0);
    float det = m1[0] * m1[3] - m1[1] * m1[2];
    out[1] = -out[1];
    out[2] = -out[2];
    
    if (det == 0) {
        printf("Warning: Failed to Generate Inverse Matrix\n");
    }

    return TupleScalarMultiply(out, 1/det);
}

static inline __m128 invertMultHelper(__m128 m1, __m128 m2) {
    __m128 r0 = _mm_mul_ps(SWIZZLE_M128(m1, 0, 0, 3, 2), 
                            SWIZZLE_M128(m2, 0, 1, 2, 1));
    __m128 r1 = _mm_mul_ps(SWIZZLE_M128(m1, 1, 1, 2, 3), 
                            SWIZZLE_M128(m2, 2, 3, 0, 3));
    return _mm_add_ps(r0, r1);
}

//Blockwise matrix inversion, see the wikipedia page on inverse matricies for more
Matrix4x4 MatrixInvert(Matrix4x4 m1) {
    __m128 A = SHUFFLE_M128(m1.contents[0], m1.contents[1], 0, 1, 0, 1);
    __m128 B = SHUFFLE_M128(m1.contents[0], m1.contents[1], 2, 3, 2, 3);
    __m128 C = SHUFFLE_M128(m1.contents[2], m1.contents[3], 0, 1, 0, 1);
    __m128 D = SHUFFLE_M128(m1.contents[2], m1.contents[3], 2, 3, 2, 3);

    __m128 Ainv = invertHelper(A);
    __m128 CAinv = invertMultHelper(C, Ainv); //
    __m128 schur_compliment = invertHelper(TupleSubtract(D, invertMultHelper(CAinv, B)));
    __m128 Bschur_inv = invertMultHelper(B, schur_compliment);

    /*
        [X, Y,
         Z, W]
    */
   __m128 W = schur_compliment;
   __m128 Z = invertMultHelper(TupleNegate(schur_compliment), CAinv);
   __m128 Y = invertMultHelper(TupleNegate(Ainv), Bschur_inv);
   __m128 X = TupleAdd(Ainv, invertMultHelper(invertMultHelper(Ainv, Bschur_inv), CAinv));

    Matrix4x4 output;
    output.contents[0] = SHUFFLE_M128(X, Y, 0, 1, 0, 1);
    output.contents[1] = SHUFFLE_M128(X, Y, 2, 3, 2, 3);
    output.contents[2] = SHUFFLE_M128(Z, W, 0, 1, 0, 1);
    output.contents[3] = SHUFFLE_M128(Z, W, 2, 3, 2, 3);

    return output;
}

void PrintMatrix(Matrix4x4 m1) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f ", m1.contents[i][j]);
        }
        printf("\n");
    }
}

Matrix4x4 TranslationMatrix(float x, float y, float z) {
    Matrix4x4 out = {
        .contents = {
            {1, 0, 0, x},
            {0, 1, 0, y},
            {0, 0, 1, z},
            {0, 0, 0, 1},
        }
    };

    return out;
}

Matrix4x4 ScalingMatrix(float x, float y, float z) {
    Matrix4x4 out = {
        .contents = {
            {x, 0, 0, 0},
            {0, y, 0, 0},
            {0, 0, z, 0},
            {0, 0, 0, 1},
        }
    };

    return out;
}

Matrix4x4 RotationXMatrix(float theta) {
    Matrix4x4 out = {
        .contents = {
            {1, 0, 0, 0},
            {0, cosf(theta), -sinf(theta), 0},
            {0, sinf(theta), cosf(theta), 0},
            {0, 0, 0, 1}
        }
    };

    return out;
}

Matrix4x4 RotationYMatrix(float theta) {
    Matrix4x4 out = {
        .contents = {
            {cosf(theta), 0, sinf(theta), 0},
            {0, 1, 0, 0},
            {-sinf(theta), 0, cosf(theta), 0},
            {0, 0, 0, 1}
        }
    };

    return out;
}

Matrix4x4 RotationZMatrix(float theta) {
    Matrix4x4 out = {
        .contents = {
            {cosf(theta), -sinf(theta), 0, 0},
            {sinf(theta), cosf(theta), 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        }
    };

    return out;
}

Matrix4x4 RotationMatrix(float rot_x, float rot_y, float rot_z)  {
    Matrix4x4 yaw = RotationXMatrix(rot_x);
    Matrix4x4 pitch = RotationYMatrix(rot_y);
    Matrix4x4 roll = RotationZMatrix(rot_z);

    return MatrixMultiply(MatrixMultiply(yaw, pitch), roll);
}

Matrix4x4 ShearingMatrix(float xy, float xz, float yx, float yz, float zx, float zy) {
    Matrix4x4 out = {
        .contents = {
            {1, xy, xz, 0},
            {yx, 1, yz, 0},
            {zx, zy, 1, 0},
            {0, 0, 0, 1}
        }
    };

    return out;
}

Matrix4x4 IdentityMatrix() {
    Matrix4x4 result = {
        .contents = {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1},
        }
    };

    return result;
}

Matrix4x4 ViewMatrix(Tuple3 from, Tuple3 to, Tuple3 upvec) {
    Tuple3 forward = TupleNormalize(TupleSubtract(to, from));
    upvec = TupleNormalize(upvec);
    Tuple3 left = TupleCrossProduct(forward, upvec);
    upvec = TupleCrossProduct(left, forward);

    Matrix4x4 result = {
        .contents = {
            {left[0], left[1], left[2], 0},
            {upvec[0], upvec[1], upvec[2], 0},
            {-forward[0], -forward[1], -forward[2], 0},
            {0, 0, 0, 1}
        }
    };

    return MatrixMultiply(result, TranslationMatrix(-from[0], -from[1], -from[2]));
}

Matrix4x4 SkewSymmetricCPMatrix(Tuple3 t1) {
    Matrix4x4 result = {
        .contents = {
            {0, t1[2], -t1[1], 0},
            {-t1[2], 0, t1[0], 0},
            {t1[1], -t1[0], 0, 0},
            {0, 0, 0, 1},
        }
    };

    return result;
}