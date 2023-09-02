#include <time.h>
#include <stdio.h>

#include "matrix.h"
#include "shape.h"
#include "shading.h"
#include "light.h"
#include "canvas.h"

#define BENCHMARK_CYCLES 1080
#define BENCHMARK(fn, width, samples) {                          \
    double average = 0;                                 \
    for (int i = 0; i < samples; i++) {        \
        clock_t start = clock();                        \
        for (int j = 0; j < width; j++)                 \
            fn;                                         \
        clock_t end = clock();                          \
        average += (double) (end - start);              \
    }                                                   \
    printf("%s: %lf clocks\n", #fn, average / (double) samples);\
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

    BENCHMARK(MatrixEqual(m1, m2), 1920, BENCHMARK_CYCLES);
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

    BENCHMARK(MatrixFuzzyEqual(m1, m2), 1920, BENCHMARK_CYCLES);
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

    BENCHMARK(MatrixMultiply(m1, m2), 1920, BENCHMARK_CYCLES);
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

    BENCHMARK(MatrixTranspose(m1), 1920, BENCHMARK_CYCLES);
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

    BENCHMARK(MatrixInvert(m1), 1920, BENCHMARK_CYCLES);
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

    BENCHMARK(MatrixTupleMultiply(m1, t1), 1920, BENCHMARK_CYCLES);
}

void BenchmarkRaySphereIntersection() {
    Shape sphere;
    ConstructSphere(&sphere, NewPnt3(0, 0, 0), 1.0);

    Ray r1 = {
        origin: NewPnt3(0, 0, -5),
        direction: NewVec3(0, 0, 1),
    };

    BENCHMARK(Intersect(sphere, r1), 1920, BENCHMARK_CYCLES);
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

    BENCHMARK(PhongShading(m, light, position, eyev, normalv), 1920, BENCHMARK_CYCLES);
}

void TestShadeSphere() {
    Material m;
    AssignDefaultTestMaterial(&m);
    m.color = NewColor(255, 0, 128, 255);

    Light l;
    l.origin = NewPnt3(200, 200, -300);
    l.color = NewColor(255, 255, 255, 255);
    
    Canvas c;
    ConstructCanvas(&c, 800, 600);

    Shape s;
    ConstructSphere(&s, NewPnt3(0, 0, 0), 250); 
    s.material = m;

    Ray r;
    r.direction = NewVec3(0, 0, 1);

    for (int x = 0; x < 800; x++) {
        for (int y = 0; y < 600; y++) {
            r.origin = NewPnt3(x-400, y - 300, 0);
            Intersections i = Intersect(s, r);

            if (i.count > 0) {

                Tuple3 pos = RayPosition(r, i.ray_times[0]);
                Tuple3 norm = NormalAt(s, pos);
                Tuple3 eyev = TupleSubtract(NewPnt3(0, 0, -1), pos);

                Tuple3 color = PhongShading(s.material, l, pos, eyev, norm);
                WritePixel(&c, color, x, y);
            }
        }
    }

    DeconstructCanvas(&c);
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
    BENCHMARK(TestShadeSphere(), 1, 10)

    return 0;
}