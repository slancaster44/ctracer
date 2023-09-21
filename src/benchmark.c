#include <time.h>
#include <stdio.h>

#include "matrix.h"
#include "shape.h"
#include "shading.h"
#include "light.h"
#include "intersection.h"
#include "canvas.h"
#include "scene.h"
#include "set.h"

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
        .contents = {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    Matrix4x4 m2 = {
        .contents = {
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
        .contents = {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    Matrix4x4 m2 = {
        .contents = {
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
        .contents = {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    Matrix4x4 m2 = {
        .contents = {
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
        .contents = {
            {1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    BENCHMARK(MatrixTranspose(m1), 1920, BENCHMARK_CYCLES);
}

void BenchmarkMatrixInvert() {
    Matrix4x4 m1 = ScalingMatrix(1, 2, 3);
    BENCHMARK(MatrixInvert(m1), 1920, BENCHMARK_CYCLES);
}

void BenchmarkMatrixTupleMultiply() {
    Matrix4x4 m1 = {
        .contents = {
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
    Shape sphere = NewSphere(NewPnt3(0, 0, 0), 1.0);

    Ray r1 = {
        .origin = NewPnt3(0, 0, -5),
        .direction = NewVec3(0, 0, 1),
    };

    BENCHMARK(Intersect(&sphere, r1), 1920, BENCHMARK_CYCLES);
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

    ShadingJob sj = {
        .material = m,
        .light = light,
        .position = position,
        .eye_vector = eyev,
        .surface_normal = normalv
    };

    BENCHMARK(PhongShading(sj), 1920, BENCHMARK_CYCLES);
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

    Shape s = NewSphere(NewPnt3(0, 0, 0), 250); 
    s.material = m;

    Ray r;
    r.direction = NewVec3(0, 0, 1);

    for (int x = 0; x < 800; x++) {
        for (int y = 0; y < 600; y++) {
            r.origin = NewPnt3(x-400, y - 300, 0);
            Intersection i = Intersect(&s, r);

            if (i.count > 0) {

                Tuple3 pos = RayPosition(r, i.ray_times[0]);
                Tuple3 norm = NormalAt(&s, pos);
                Tuple3 eyev = TupleSubtract(NewPnt3(0, 0, -1), pos);

                ShadingJob sj = {
                    .material = s.material,
                    .light = l,
                    .position = pos,
                    .eye_vector = eyev,
                    .surface_normal = norm,
                };

                Tuple3 color = PhongShading(sj);
                WritePixel(&c, color, x, y, 0);
            }
        }
    }

    DeconstructCanvas(&c);
}

void ConstructDefaultScene(Scene* s) {
    Camera c = NewCamera(1920, 1080, 3.14 / 4);
    CameraApplyTransformation(&c, ViewMatrix(NewPnt3(0, 0, -1200), NewPnt3(0, 0, 0), NewVec3(0, 1, 0)));

    Light l = NewLight(NewPnt3(-300, -300, -300));
    ConstructScene(s, c, l);
    
    Shape s1 = NewSphere(NewPnt3(0, 0, 0), 1.0);
    s1.material.diffuse_reflection = 0.7;
    s1.material.specular_reflection = 0.2;

    Shape s2 = NewSphere(NewPnt3(0, 0, 0), 0.5);

    AddShape(s, s1);
    AddShape(s, s2);
}

void BenchmarkScene() {
    Ray r = NewRay(NewPnt3(0, 0, -5), NewVec3(0, 0, 1));

    Set is;
    ConstructSet(&is, sizeof(Intersection));

    Scene s;
    ConstructDefaultScene(&s);

    BENCHMARK(IntersectScene(&s, r, &is), 1920 * 1080, 5);

    DeconstructSet(&is);
    DeconstructScene(&s);
}

void DemoSphereScene() {
    Material m;
    AssignDefaultTestMaterial(&m);
    m.color = NewColor(255, 0, 128, 255);

    Light l;
    l.origin = NewPnt3(-1000, -2000, -750);
    l.color = NewColor(255, 255, 255, 255);
    
    Canvas c;
    ConstructCanvas(&c, 800, 600);

    Shape s = NewSphere(NewPnt3(0, 0, 2), 500); 
    s.material = m;

    Shape s2 = NewSphere(NewPnt3(1, 1, -5), 50);
    m.color = NewColor(0, 255, 128, 255);
    s2.material = m;

    Camera ca = NewCamera(800, 600, 3.1415 / 3);
    CameraApplyTransformation(&ca, ViewMatrix(NewPnt3(0, 0, -600), NewPnt3(0, 0, 0), NewVec3(0, -1, 0)));

    Scene sc;
    ConstructScene(&sc, ca, l);
    AddShape(&sc, s);
    AddShape(&sc, s2);

    RenderScene(&sc, &c);

    WriteToPPM(&c, "sphere_scene.ppm");

    DeconstructScene(&sc);
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
    BENCHMARK(DemoSphereScene(), 1, 10);
    BenchmarkScene();
    return 0;
}