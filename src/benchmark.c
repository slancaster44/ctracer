#include <time.h>
#include <stdio.h>

#include "matrix.h"
#include "shape.h"

#define BENCHMARK_CYCLES 8192
#define BENCHMARK(fn, width) {                          \
    double average = 0;                                 \
    for (int i = 0; i < BENCHMARK_CYCLES; i++) {        \
        clock_t start = clock();                        \
        for (int j = 0; j < width; j++)                 \
            fn;                                         \
        clock_t end = clock();                          \
        average += (double) (end - start);              \
    }                                                   \
    printf("%s: %lf clocks\n", #fn, average / (double) BENCHMARK_CYCLES);\
}

void BenchmarkMatrixEqual() {
    Matrix4x4 m1 = {
        contents: {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    Matrix4x4 m2 = {
        contents: {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    BENCHMARK(MatrixEqual(m1, m2), 8192);
}

void BenchmarkMatrixFuzzyEqual() {
        Matrix4x4 m1 = {
        contents: {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    Matrix4x4 m2 = {
        contents: {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    BENCHMARK(MatrixFuzzyEqual(m1, m2), 8192);
}

void BenchmarkMatrixMultiply() {
        Matrix4x4 m1 = {
        contents: {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    Matrix4x4 m2 = {
        contents: {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    BENCHMARK(MatrixMultiply(m1, m2), 8192);
}

void BenchmarkMatrixTranspose() {
            Matrix4x4 m1 = {
        contents: {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    BENCHMARK(MatrixTranspose(m1), 8192);
}

void BenchmarkMatrixInvert() {
    Matrix4x4 m1 = {
        contents: {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    BENCHMARK(MatrixInvert(m1), 8192);
}

void BenchmarkMatrixTupleMultiply() {
    Matrix4x4 m1 = {
        contents: {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    Tuple3 t1 = NewTuple3(1, 2, 3, 4);

    BENCHMARK(MatrixTupleMultiply(m1, t1), 8192);
}

void BenchmarkRaySphere() {
    Shape sphere;
    ConstructSphere(&sphere, NewPnt3(0, 0, 0));

    Ray r1 = {
        origin: NewPnt3(0, 0, -5),
        direction: NewVec3(0, 0, 1),
    };

    BENCHMARK(Intersect(sphere, r1), 8192);
}

int main() {
    BenchmarkMatrixEqual();
    BenchmarkMatrixFuzzyEqual();
    BenchmarkMatrixMultiply();
    BenchmarkMatrixInvert();
    BenchmarkMatrixTranspose();
    BenchmarkMatrixTupleMultiply();
    BenchmarkRaySphere();
    return 0;
}