#include <stdio.h>
#include <math.h>

#include "tuple.h"
#include "trig.h"
#include "matrix.h"

void Fail(const char* msg) {
    printf("\033[0;31m[FAIL]\033[0;37m %s\n", msg);
}

void Pass(const char* msg) {
    printf("\033[0;32m[PASS]\033[0;37m %s\n", msg);
}

void TestMatrixEqual() {
    Matrix4x4 m1 = {
        contents: {
            {-1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    Matrix4x4 m2 = {
        contents: {
            {-1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    if (!MatrixEqual(m1, m2)) {
        Fail("Matrix Equality");
    } else {
        Pass("Matrix Equality");
    }

    m2.contents[0][0] = -1.0001;
    if (MatrixEqual(m1, m2) || !MatrixFuzzyEqual(m1, m2)) {
        Fail("Matrix Fuzzy Equality");
    } else {
        Pass("Matrix Fuzzy Equality");
    }
}

void TestMatrixMultiply() {
    Matrix4x4 m1 = {
        contents: {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 8, 7 ,6},
            {5, 4, 3, 2}
        }
    };

    Matrix4x4 m2 = {
        contents: {
            {-2, 1, 2, 3},
            {3, 2, 1, -1},
            {4, 3, 6, 5},
            {1, 2, 7, 8}
        }
    };

    Matrix4x4 res = {
        contents: {
            {20, 22, 50, 48},
            {44, 54, 114, 108},
            {40, 58, 110, 102},
            {16, 26, 46, 42}
        }
    };

    if (!MatrixFuzzyEqual(MatrixMultiply(m1, m2), res)) {
        Fail("Matrix Multiplication");
    } else {
        Pass("Matrix Multiplication");
    }
}

void TestMatrixTranspose() {
    Matrix4x4 m1 = {
        contents: {
            {0, 9, 3, 0},
            {9, 8, 0, 8},
            {1, 8, 5, 3},
            {0, 0, 5, 8}
        }
    };

    Matrix4x4 res = {
        contents: {
            {0, 9, 1, 0},
            {9, 8, 8, 0},
            {3, 0, 5, 5},
            {0, 8, 3, 8}
        }
    };

    if (!MatrixEqual(MatrixTranspose(m1), res)) {
        Fail("Matrix Transposition");
    } else {
        Pass("Matrix Transposition");
    }
}

void TestMatrixInvert() {
    Matrix4x4 m1 = {
        contents: {
            {8, -5, 9, 2},
            {7, 5, 6, 1},
            {-6, 0, 9, 6},
            {-3, 0, -9, -4}
        }
    };

    Matrix4x4 m2 = {
        contents: {
            {-0.15385, -0.15385, -0.28205, -0.53846},
            {-0.07692, 0.12308, 0.02564, 0.03077},
            {0.35897, 0.35897, 0.43590, 0.92308},
            {-0.69231, -0.69231, -0.76923, -1.92308}
        }
    };

    if (!MatrixFuzzyEqual(MatrixInvert(m1), m2)) {
        Fail("Matrix Inversion");
        PrintMatrix(MatrixInvert(m1));
    } else {
        Pass("Matrix Inversion");
    }
}

void TestMatrixTranslation() {
    
}

void TestMatrixVectorMultiply() {
    Matrix4x4 m1 = {
        contents: {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1}
        }
    };

    Tuple3 t1 = NewTuple3(1, 2, 3, 1);
    Tuple3 res = NewTuple3(18, 24, 33, 1);

    if (!TupleEqual(res, MatrixTupleMultiply(m1, t1))) {
        Fail("Matrix & Tuple Multiplication");
    } else {
        Pass("Matrix & Tuple Multiplication");
    }

}

void TestCos() {
    if (!FloatEquality(_cos(0.0), 1.0)) {
        Fail("Cos Test");
    } else {
        Pass("Cos Test");
    }
}

void TestSin() {
    if (!FloatEquality(_sin(0.0), 0.0) || !FloatEquality(_sin(M_PI / 2), 1.0) || !FloatEquality(_sin(M_PI * 3/2), -1.0) || !FloatEquality(_sin(M_PI * 2 / 3), 0.8660)) {
        printf("%f %f %f %f\n", _sin(0.0), _sin(M_PI/2), _sin(M_PI * 3 / 2), _sin(M_PI * 3 / 4));
        Fail("Sin Test");
    } else {
        Pass("Sin Test");
    }
}

void TestTan() {
    if (!FloatEquality(_tan(M_PI), 0.0)) {
        Fail("Tan Test");
    } else {
        Pass("Tan Test");
    }
}

void TestTupleEqual() {
    if (!TupleEqual(NewVec3(1, 2, 3), NewVec3(1, 2, 3)) || TupleEqual(NewVec3(1, 2, 3), NewVec3(3, 2, 1))) {
        Fail("Tuple Equality");
    } else {
        Pass("Tuple Equality");
    }
}

void TestScalarMultiply() {
    Tuple3 t1 = NewVec3(2.0, 4.0, 6.0);
    Tuple3 result = TupleScalarMultiply(t1, 2);

    if (!TupleEqual(result, NewVec3(4, 8, 12))) {
        Fail("Tuple Scalar Multiplication");
    } else {
        Pass("Tuple Scalar Multiplication");
    }
}

void TestScalarDivide() {
    Tuple3 t1 = NewVec3(2.0, 4.0, 6.0);
    Tuple3 result = TupleScalarDivide(t1, 2);

    if (!TupleEqual(result, NewVec3(1, 2, 3))) {
        Fail("Tuple Scalar Division");
    } else {
        Pass("Tuple Scalar Division");
    }
}

void TestTupleNegate() {
    Tuple3 t1 = NewVec3(1, 2, 3);
    Tuple3 result = TupleNegate(t1);

    if (!TupleEqual(result, NewVec3(-1, -2, -3))) {
        Fail("Tuple Negation");
    } else {
        Pass("Tuple Negation");
    }
}

void TestTupleMagnitude() {
    Tuple3 t1 = NewVec3(1, 2, 3);
    float result = TupleMagnitude(t1);

    if (!FloatEquality(result, sqrtf(14))) {
        Fail("Tuple Magnitude");
    } else {
        Pass("Tuple Magnitude");
    }
}

void TestTupleDotProduct() {
    Tuple3 t1 = NewVec3(1, 2, 3);
    Tuple3 t2 = NewVec3(2, 3, 4);
    
    float result = TupleDotProduct(t1, t2);
    if (!FloatEquality(result, 20)) {
        Fail("Tuple Dot Product");
    } else {
        Pass("Tuple Dot Product");
    }
}

void TestTupleNormalize() {
    Tuple3 t1 = NewVec3(100, 100, 100);
    Tuple3 result = TupleNormalize(t1);

    if (abs(TupleMagnitude(result) - 1) > 0.0001) {
        Fail("Tuple Normalization");
    } else {
        Pass("Tuple Normalization");
    }
}

void TestTupleMultiply() {
    Tuple3 t1 = NewPnt3(1, 2, 3);
    Tuple3 t2 = NewPnt3(1, 2, 3);

    Tuple3 result = TupleMultiply(t1, t2);

    if (!TupleEqual(result, NewPnt3(1, 4, 9))) {
        Fail("Tuple Multiplication");
    } else {
        Pass("Tuple Multiplication");
    }
}

void TestTupleDivide() {
    Tuple3 t1 = NewPnt3(1, 2, 3);
    Tuple3 t2 = NewPnt3(1, 2, 3);

    Tuple3 result = TupleDivide(t1, t2);

    if (!TupleEqual(result, NewPnt3(1, 1, 1))) {
        Fail("Tuple Division");
    } else {
        Pass("Tuple Division");
    }
}

void TestTupleAdd() {
    Tuple3 t1 = NewVec3(1, 2, 3);
    Tuple3 t2 = NewVec3(1, 2, 3);

    Tuple3 result = TupleAdd(t1, t2);

    if (!TupleEqual(result, NewVec3(2, 4, 6))) {
        Fail("Tuple Addition");
    } else {
        Pass("Tuple Addition");
    }
}

void TestTupleSubtract() {
    Tuple3 t1 = NewVec3(1, 2, 3);
    Tuple3 t2 = NewVec3(1, 2, 3);

    Tuple3 result = TupleSubtract(t1, t2);

    if (!TupleEqual(result, NewVec3(0, 0, 0))) {
        Fail("Tuple Subtraction");
    } else {
        Pass("Tuple Subtraction");
    }
}

int main() {
    TestMatrixEqual();
    TestMatrixMultiply();
    TestMatrixTranspose();
    TestMatrixInvert();
    TestMatrixTranslation();
    TestMatrixVectorMultiply();
    TestCos();
    TestSin();
    TestTan();
    TestTupleEqual();
    TestScalarMultiply();
    TestScalarDivide();
    TestTupleNegate();
    TestTupleMagnitude();
    TestTupleDotProduct();
    TestTupleNormalize();
    TestTupleMultiply();
    TestTupleDivide();
    TestTupleAdd();
    TestTupleSubtract();

    return 0;
}