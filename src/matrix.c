#include "matrix.h"
#include "equality.h"

#include <math.h>
#include <stdio.h>

bool MatrixEqual(Matrix4x4 m1, Matrix4x4 m2)
{
    __mmask8 cmp0 = _mm512_cmp_pd_mask(m1.chunks[0], m2.chunks[0], _CMP_EQ_OQ);
    __mmask8 cmp1 = _mm512_cmp_pd_mask(m1.chunks[1], m2.chunks[1], _CMP_EQ_OQ);

    return cmp1 == 0xff && cmp0 == 0xff;
}

bool MatrixFuzzyEqual(Matrix4x4 m1, Matrix4x4 m2)
{
    __m512d diff0 = _mm512_sub_pd(m1.chunks[0], m2.chunks[0]);
    __m512d diff1 = _mm512_sub_pd(m1.chunks[1], m2.chunks[1]);

    __m512d abs0 = _mm512_abs_pd(diff0);
    __m512d abs1 = _mm512_abs_pd(diff1);

    __m512d epsilon = _mm512_set1_pd(EQUALITY_EPSILON);

    __mmask8 cmp0 = _mm512_cmp_pd_mask(abs0, epsilon, _CMP_LT_OQ);
    __mmask8 cmp1 = _mm512_cmp_pd_mask(abs1, epsilon, _CMP_LT_OQ);

    return cmp1 == 0xff && cmp0 == 0xff;

}

static inline __m256d matrixMultHelper(int a, Matrix4x4 m1, Matrix4x4 m2)
{
    __m256d s0 = _mm256_mul_pd(_mm256_set1_pd(m1.contents[a][0]), m2.contents[0]);
    __m256d s1 = _mm256_mul_pd(_mm256_set1_pd(m1.contents[a][1]), m2.contents[1]);
    __m256d s2 = _mm256_mul_pd(_mm256_set1_pd(m1.contents[a][2]), m2.contents[2]);
    __m256d s3 = _mm256_mul_pd(_mm256_set1_pd(m1.contents[a][3]), m2.contents[3]);

    return _mm256_add_pd(s0, _mm256_add_pd(s1, _mm256_add_pd(s2, s3)));
}

Matrix4x4 MatrixMultiply(Matrix4x4 m1, Matrix4x4 m2)
{
    Matrix4x4 out;
    out.contents[0] = matrixMultHelper(0, m1, m2);
    out.contents[1] = matrixMultHelper(1, m1, m2);
    out.contents[2] = matrixMultHelper(2, m1, m2);
    out.contents[3] = matrixMultHelper(3, m1, m2);

    return out;
}

Matrix4x4 MatrixScalarMultiply(Matrix4x4 m1, double f1)
{
    __m512d scale = _mm512_set1_pd(f1);

    Matrix4x4 m2;
    m2.chunks[0] = _mm512_mul_pd(m1.chunks[0], scale);
    m2.chunks[1] = _mm512_mul_pd(m1.chunks[1], scale);

    return m2;
}

// TODO: No test
Matrix4x4 MatrixAdd(Matrix4x4 m1, Matrix4x4 m2)
{
    Matrix4x4 out;
    out.chunks[0] = _mm512_add_pd(m1.chunks[0], m2.chunks[0]);
    out.chunks[1] = _mm512_add_pd(m1.chunks[1], m2.chunks[1]);

    return out;
}

Tuple3 MatrixTupleMultiply(Matrix4x4 m1, Tuple3 t1)
{
    __m512d factor = DISTRIBUTE_256(t1, t1);
    __m512d r0 = _mm512_mul_pd(m1.chunks[0], factor);
    __m512d r1 = _mm512_mul_pd(m1.chunks[1], factor);

    Tuple3 out;
    out[0] = r0[0] + r0[1] + r0[2] + r0[3];
    out[1] = r0[4] + r0[5] + r0[6] + r0[7];
    out[2] = r1[0] + r1[1] + r1[2] + r1[3];
    out[3] = r1[4] + r1[5] + r1[6] + r1[7];

    return out;
}

Tuple3 MatrixTupleMultiplyPerserveInf(Matrix4x4 m1, Tuple3 t1)
{
    Tuple3 out;
    out[0] = TupleDotProductPreserveInf(m1.contents[0], t1);
    out[1] = TupleDotProductPreserveInf(m1.contents[1], t1);
    out[2] = TupleDotProductPreserveInf(m1.contents[2], t1);
    out[3] = TupleDotProductPreserveInf(m1.contents[3], t1);

    return out;
}


// a 256d translation of _MM_TRANSPOSE4_PS()
Matrix4x4 MatrixTranspose(Matrix4x4 m1)
{

    __m256d tmp0, tmp1, tmp2, tmp3;
    tmp0 = _mm256_unpacklo_pd(m1.contents[0], m1.contents[1]);
    tmp2 = _mm256_unpacklo_pd(m1.contents[2], m1.contents[3]);
    tmp1 = _mm256_unpackhi_pd(m1.contents[0], m1.contents[1]);
    tmp3 = _mm256_unpackhi_pd(m1.contents[2], m1.contents[3]);

    m1.contents[0] = SHUFFLE_M256(tmp0, tmp2, 0, 1, 0, 1);
    m1.contents[1] = SHUFFLE_M256(tmp1, tmp3, 0, 1, 0, 1);
    m1.contents[2] = SHUFFLE_M256(tmp0, tmp2, 2, 3, 2, 3);
    m1.contents[3] = SHUFFLE_M256(tmp1, tmp3, 2, 3, 2, 3);

    return m1;
}

static inline __m256d invertHelper(__m256d m1)
{
    __m256d out = SWIZZLE_M256(m1, 3, 1, 2, 0);
    double det = m1[0] * m1[3] - m1[1] * m1[2];
    out[1] = -out[1];
    out[2] = -out[2];

    if (det == 0)
    {
        PrintTuple(m1);
        printf("Warning: Failed to Generate Inverse Matrix\n");
    }

    return TupleScalarMultiply(out, 1 / det);
}

static inline __m256d invertMultHelper(__m256d m1, __m256d m2)
{
    __m256d r0 = _mm256_mul_pd(SWIZZLE_M256(m1, 0, 0, 3, 2),
                               SWIZZLE_M256(m2, 0, 1, 2, 1));
    __m256d r1 = _mm256_mul_pd(SWIZZLE_M256(m1, 1, 1, 2, 3),
                               SWIZZLE_M256(m2, 2, 3, 0, 3));
    return _mm256_add_pd(r0, r1);
}

// Blockwise matrix inversion, see the wikipedia page on inverse matricies for more
Matrix4x4 MatrixInvert(Matrix4x4 m1)
{
    __m256d A = SHUFFLE_M256(m1.contents[0], m1.contents[1], 0, 1, 0, 1);
    __m256d B = SHUFFLE_M256(m1.contents[0], m1.contents[1], 2, 3, 2, 3);
    __m256d C = SHUFFLE_M256(m1.contents[2], m1.contents[3], 0, 1, 0, 1);
    __m256d D = SHUFFLE_M256(m1.contents[2], m1.contents[3], 2, 3, 2, 3);

    __m256d Ainv = invertHelper(A);
    __m256d CAinv = invertMultHelper(C, Ainv); //
    __m256d schur_compliment = invertHelper(TupleSubtract(D, invertMultHelper(CAinv, B)));
    __m256d Bschur_inv = invertMultHelper(B, schur_compliment);

    /*
        [X, Y,
         Z, W]
    */
    __m256d W = schur_compliment;
    __m256d Z = invertMultHelper(TupleNegate(schur_compliment), CAinv);
    __m256d Y = invertMultHelper(TupleNegate(Ainv), Bschur_inv);
    __m256d X = TupleAdd(Ainv, invertMultHelper(invertMultHelper(Ainv, Bschur_inv), CAinv));

    Matrix4x4 output;
    output.contents[0] = SHUFFLE_M256(X, Y, 0, 1, 0, 1);
    output.contents[1] = SHUFFLE_M256(X, Y, 2, 3, 2, 3);
    output.contents[2] = SHUFFLE_M256(Z, W, 0, 1, 0, 1);
    output.contents[3] = SHUFFLE_M256(Z, W, 2, 3, 2, 3);

    return output;
}

void PrintMatrix(Matrix4x4 m1)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%f ", m1.contents[i][j]);
        }
        printf("\n");
    }
}

Matrix4x4 TranslationMatrix(double x, double y, double z)
{
    Matrix4x4 out = {
        .contents = {
            {1, 0, 0, x},
            {0, 1, 0, y},
            {0, 0, 1, z},
            {0, 0, 0, 1},
        }};

    return out;
}

Matrix4x4 ScalingMatrix(double x, double y, double z)
{
    Matrix4x4 out = {
        .contents = {
            {x, 0, 0, 0},
            {0, y, 0, 0},
            {0, 0, z, 0},
            {0, 0, 0, 1},
        }};

    return out;
}

Matrix4x4 RotationXMatrix(double theta)
{
    Matrix4x4 out = {
        .contents = {
            {1, 0, 0, 0},
            {0, cos(theta), -sin(theta), 0},
            {0, sin(theta), cos(theta), 0},
            {0, 0, 0, 1}}};

    return out;
}

Matrix4x4 RotationYMatrix(double theta)
{
    Matrix4x4 out = {
        .contents = {
            {cos(theta), 0, sin(theta), 0},
            {0, 1, 0, 0},
            {-sin(theta), 0, cos(theta), 0},
            {0, 0, 0, 1}}};

    return out;
}

Matrix4x4 RotationZMatrix(double theta)
{
    Matrix4x4 out = {
        .contents = {
            {cos(theta), -sin(theta), 0, 0},
            {sin(theta), cos(theta), 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}}};

    return out;
}

Matrix4x4 RotationMatrix(double rot_x, double rot_y, double rot_z)
{
    Matrix4x4 yaw = RotationXMatrix(rot_x);
    Matrix4x4 pitch = RotationYMatrix(rot_y);
    Matrix4x4 roll = RotationZMatrix(rot_z);

    return MatrixMultiply(MatrixMultiply(yaw, pitch), roll);
}

Matrix4x4 ShearingMatrix(double xy, double xz, double yx, double yz, double zx, double zy)
{
    Matrix4x4 out = {
        .contents = {
            {1, xy, xz, 0},
            {yx, 1, yz, 0},
            {zx, zy, 1, 0},
            {0, 0, 0, 1}}};

    return out;
}

Matrix4x4 IdentityMatrix()
{
    Matrix4x4 result = {
        .contents = {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1},
        }};

    return result;
}

Matrix4x4 ZeroMatrix()
{
        Matrix4x4 result = {
        .contents = {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        }};

    return result;
}

Matrix4x4 ViewMatrix(Tuple3 from, Tuple3 to, Tuple3 upvec)
{
    Tuple3 forward = TupleNormalize(TupleSubtract(to, from));
    upvec = TupleNormalize(upvec);
    Tuple3 left = TupleCrossProduct(forward, upvec);
    upvec = TupleCrossProduct(left, forward);

    Matrix4x4 result = {
        .contents = {
            {left[0], left[1], left[2], 0},
            {upvec[0], upvec[1], upvec[2], 0},
            {-forward[0], -forward[1], -forward[2], 0},
            {0, 0, 0, 1}}};

    return MatrixMultiply(result, TranslationMatrix(-from[0], -from[1], -from[2]));
}

Matrix4x4 RectifyMatrix(Matrix4x4 m)
{
    for (int i = 0; i < 4; i++)
    {
        m.contents[i][i] = m.contents[i][i] == 0.0 ? EQUALITY_EPSILON : m.contents[i][i];
    }

    return m;
}
