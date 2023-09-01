#include <time.h>
#include <stdio.h>

#include "matrix.h"
#include "shape.h"
#include "shading.h"
#include "light.h"

#define BENCHMARK_CYCLES 1080
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

    BENCHMARK(MatrixEqual(m1, m2), 1920);
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

    BENCHMARK(MatrixFuzzyEqual(m1, m2), 1920);
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

    BENCHMARK(MatrixMultiply(m1, m2), 1920);
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

    BENCHMARK(MatrixTranspose(m1), 1920);
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

    BENCHMARK(MatrixInvert(m1), 1920);
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

    BENCHMARK(MatrixTupleMultiply(m1, t1), 1920);
}

void BenchmarkRaySphereIntersection() {
    Shape sphere;
    ConstructSphere(&sphere, NewPnt3(0, 0, 0), 1.0);

    Ray r1 = {
        origin: NewPnt3(0, 0, -5),
        direction: NewVec3(0, 0, 1),
    };

    BENCHMARK(Intersect(sphere, r1), 1920);
}

void AssignDefaultTestMaterial(Material* m) {
    m->color = NewColor(255, 255, 255, 255);
    m->ambient_reflection = 0.1;
    m->diffuse_reflection = 0.9;
    m->specular_reflection = 0.9;
    m->shininess = 200;
}

void BenchmarkPhongShading() {
    Material m;
    AssignDefaultTestMaterial(&m);
    Tuple3 position = NewPnt3(0, 0, 0);

    Tuple3 eyev, normalv;
    Light light;

    eyev = NewVec3(0, 0, -1);
    normalv = NewVec3(0, 0, -1);
    light.origin = NewPnt3(0, 0, -10);
    light.color = NewColor(255, 255, 255, 255);

    BENCHMARK(PhongShading(m, light, position, eyev, normalv), 1920);
}

int main() {
    BenchmarkMatrixEqual();
    BenchmarkMatrixFuzzyEqual();
    BenchmarkMatrixMultiply();
    BenchmarkMatrixInvert();
    BenchmarkMatrixTranspose();
    BenchmarkMatrixTupleMultiply();
    BenchmarkRaySphereIntersection();
    BenchmarkPhongShading();
    return 0;
}