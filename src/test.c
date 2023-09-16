#include <stdio.h>
#include <math.h>

#include "tuple.h"
#include "trig.h"
#include "matrix.h"
#include "canvas.h"
#include "shape.h"
#include "material.h"
#include "light.h"
#include "scene.h"
#include "shading.h"
#include "intersection.h"
#include "set.h"

static int num_failed;
static int num_passed;

void Fail(const char* msg) {
    num_failed ++;
    printf("\033[0;31m[FAIL]\033[0;37m %s\n", msg);
}

void Pass(const char* msg) {
    num_passed ++;
    printf("\033[0;32m[PASS]\033[0;37m %s\n", msg);
}

void TestSet() {
    Set s;
    ConstructSet(&s, sizeof(int));

    int l = 1;
    AppendValue(&s, &l);
    AppendValue(&s, &l);
    AppendValue(&s, &l);
    AppendValue(&s, &l);
    AppendValue(&s, &l);

    int out = 0;
    for (int i = 0; i < s.length; i++) {
        int* n;
        n = Index(&s, i);
        out += *n;
    }

    if (out != 5) {
        Fail("Set Indexing");
        printf("\t%d\n", out);
    } else {
        Pass("Set Indexing");
    }

    out = 0;
    for (int i = 0; i < s.length; i++) {
        int n;
        CopyOut(&s, i, &n);
        out += n;
    }

    if (out != 5) {
        Fail("Set Copy Out");
        printf("\t%d\n", out);
    } else {
        Pass("Set Copy Out");
    }

    DeconstructSet(&s);
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



void TestRay() {
    Ray r = {
        origin: NewPnt3(2, 3, 4),
        direction: NewVec3(1, 0, 0),
    };

    if (!TupleEqual(RayPosition(r, 2.5), NewPnt3(4.5, 3, 4))) {
        Fail("Ray Position Finding");
    } else {
        Pass("Ray Position Finding");
    }
}

void TestRaySphereIntersection() {
    Shape sphere;
    ConstructSphere(&sphere, NewPnt3(0, 0, 0), 1.0);

    Ray r1 = {
        origin: NewPnt3(0, 0, -5),
        direction: NewVec3(0, 0, 1),
    };

    Intersection intersection = Intersect(&sphere, r1);

    if (!FloatEquality(4.0, intersection.ray_times[0]) || !FloatEquality(6.0, intersection.ray_times[1])) {
        Fail("Ray/Sphere Intersection, Normal");
        printf("\t%f\n", intersection.ray_times[1]);
    } else {
        Pass("Ray/Sphere Intersection, Normal");
    }

    Ray r2 = {
        origin: NewPnt3(0, 1, -5),
        direction: NewVec3(0, 0, 1),
    };

    Intersection i2 = Intersect(&sphere, r2);

    if (!FloatEquality(i2.ray_times[0], 5.0) || i2.count != 1) {
        Fail("Ray/Sphere Intersection, Tangent");
        printf("\t%f\n", i2.ray_times[0]);
    } else {
        Pass("Ray/Sphere Intersection, Tangent");
    }
    
    Ray r3 = {
        origin: NewPnt3(0, 0, 0),
        direction: NewVec3(0, 0, 1),
    };

    Intersection i3 = Intersect(&sphere, r3);

    if (!FloatEquality(i3.ray_times[0], -1) || !FloatEquality(1.0, i3.ray_times[1])) {
        Fail("Ray/Sphere Intersection, Inside");
        printf("\t%f\n", i3.ray_times[0]);
    } else {
        Pass("Ray/Sphere Intersection, Inside");
    }


    Ray r4 = {
        origin: NewPnt3(0, 0, 5),
        direction: NewVec3(0, 0, 1),
    };

    Intersection i4 = Intersect(&sphere, r4);

    if (!FloatEquality(i4.ray_times[0], -6) || !FloatEquality(-4.0, i4.ray_times[1])) {
        Fail("Ray/Sphere Intersection, Behind");
        printf("\t%f\n", i4.ray_times[0]);
    } else {
        Pass("Ray/Sphere Intersection, Behind");
    }

    Ray r5 = {
        origin: NewPnt3(1.1, 1.1, 0),
        direction: NewVec3(0, 0, 1),
    };

    Intersection i5 = Intersect(&sphere, r5);

    if (i5.count != 0) {
        Fail("Ray/Sphere Intersection, No Hit");
        printf("\t%d\n", i5.count);
    } else {
        Pass("Ray/Sphere Intersection, No Hit");
    }
}

void TestRayTransform() {
    Ray r = {
        origin: NewPnt3(1, 2, 3),
        direction: NewVec3(0, 1, 0),
    };

    Matrix4x4 scaling = ScalingMatrix(2, 3, 4);
    Ray r2 = RayTransform(r, scaling);

    if (!TupleFuzzyEqual(r2.direction, NewVec3(0, 3, 0)) || !TupleFuzzyEqual(r2.origin, NewPnt3(2, 6, 12))) {
        Fail("Ray Transformation");
    } else {
        Pass("Ray Transformation");
    }
}

void SphereNormal() {
    Shape s;
    ConstructSphere(&s, NewPnt3(0, 0, 0), 1.0);

    if (!TupleFuzzyEqual(NormalAt(&s, NewPnt3(1, 0, 0)), NewVec3(1, 0, 0))) {
        PrintTuple(NormalAt(&s, NewPnt3(1, 0, 0)));
        PrintTuple(NewVec3(1, 0, 0));
        Fail("Sphere Normal");
    } else {
        Pass("Sphere Normal");
    }


    s.transformation = TranslationMatrix(0, 1, 0);
    s.inverse_transform = MatrixInvert(s.transformation); 

    Tuple3 n = NormalAt(&s, NewPnt3(0, 1.70711, -0.70711));
    Tuple3 expected_out = NewVec3(0, 0.70711, -0.70711);

    if (!TupleFuzzyEqual(n, expected_out)) {
        PrintTuple(n);
        Fail("Sphere Normal, Translation");
    } else {
        Pass("Sphere Normal, Translation");
    }

    s.transformation = MatrixMultiply(ScalingMatrix(1, 0.5, 1), RotationZMatrix(M_PI / 5));
    s.inverse_transform = MatrixInvert(s.transformation);

    n = NormalAt(&s, NewPnt3(0, sqrtf(2) / 2, -sqrtf(2) / 2));
    expected_out = NewVec3(0, 0.97014, -0.24254);

    if (!TupleFuzzyEqual(n, expected_out)) {
        PrintTuple(n);
        Fail("Sphere Normal, Scaling & Rotation");
    } else {
        Pass("Sphere Normal, Scaling & Rotation");
    }
}

void TestSphereGeometry() {
    Shape s;
    ConstructSphere(&s, NewPnt3(0, 0, 0), 6.0);

    Ray r = {
        origin: NewPnt3(0, 6, 0),
        direction: NewVec3(0, 0, 1),
    };

    Intersection res = Intersect(&s, r);

    if (res.ray_times[0] != 0) {
        Fail("Sphere geometry, radius");
        printf("\t%d: %f, %f\n", res.count, res.ray_times[0], res.ray_times[1]);
    } else {
        Pass("Sphere geometry, radius");
    }

    ConstructSphere(&s, NewPnt3(1, 1, 0), 1.0);
    Ray r2 = {
        origin: NewPnt3(1, 1, -3),
        direction: NewVec3(0, 0, 1),
    };

    res = Intersect(&s, r2);
    if (res.ray_times[0] != 2) {
        PrintTuple(RayPosition(r2, res.ray_times[0]));
        printf("%d: %f", res.count, res.ray_times[0]);
        Fail("Sphere geometry, origin");
    } else {
        Pass("Sphere geometry, origin");
    }
}

void AssignDefaultTestMaterial(Material* m) {
    m->color = NewColor(255, 255, 255, 255);
    m->ambient_reflection = 0.1;
    m->diffuse_reflection = 0.9;
    m->specular_reflection = 0.9;
    m->shininess = 200;
}

void TestPhongShading() {
    Material m;
    AssignDefaultTestMaterial(&m);
    Tuple3 position = NewPnt3(0, 0, 0);

    Tuple3 eyev, normalv, result;
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
        .surface_normal = normalv,
        .shadow = 0,
    };

    result = PhongShading(sj);

    if (!TupleFuzzyEqual(NewTuple3(1.9, 1.9, 1.9, 1.9), result)) {
        PrintTuple(result);
        Fail("Phong Shading, Behind");
    } else {
        Pass("Phong Shading, Behind");
    }

    float rt2_2 = sqrtf(2) / 2;

    eyev = NewVec3(0, rt2_2, -rt2_2);

    ShadingJob sj6 = {
        .material = m,
        .light = light,
        .position = position,
        .eye_vector = eyev,
        .surface_normal = normalv,
        .shadow = 0,
    };

    result = PhongShading(sj6);

    if (!TupleFuzzyEqual(NewTuple3(1.0, 1.0, 1.0, 1.0), result)) {
        PrintTuple(result);
        Fail("Phong Shading, Angled Viewer");
    } else {
        Pass("Phong Shading, Angled Viewer");
    }

    eyev = NewVec3(0, 0, -1);
    light.origin = NewPnt3(0, 10, -10);

    ShadingJob sj1 = {
        .material = m,
        .light = light,
        .position = position,
        .eye_vector = eyev,
        .surface_normal = normalv,
        .shadow = 0,
    };

    result = PhongShading(sj1);

    if (!TupleFuzzyEqual(NewTuple3(0.7364, 0.7364, 0.7364, 0.7364), result)) {
        PrintTuple(result);
        Fail("Phong Shading, Angled Light");
    } else {
        Pass("Phong Shading, Angled Light");
    }

    eyev = NewVec3(0, -rt2_2, -rt2_2);

    ShadingJob sj2 = {
        .material = m,
        .light = light,
        .position = position,
        .eye_vector = eyev,
        .surface_normal = normalv,
        .shadow = 0,
    };

    result = PhongShading(sj2);

    if (!TupleFuzzyEqual(NewTuple3(1.6364, 1.6364, 1.6364, 1.6364), result)) {
        PrintTuple(result);
        Fail("Phong Shading, Angled Viewer & Light");
    } else {
        Pass("Phong Shading, Angled Viewer & Light");
    }

    eyev = NewVec3(0, 0, -1);
    light.origin = NewPnt3(0, 0, 10);

    ShadingJob sj3 = {
        .material = m,
        .light = light,
        .position = position,
        .eye_vector = eyev,
        .surface_normal = normalv,
        .shadow = 0,
    };

    result = PhongShading(sj3);

    if (!TupleFuzzyEqual(NewTuple3(0.1, 0.1, 0.1, 0.1), result)) {
        PrintTuple(result);
        Fail("Phong Shading, Light Behind Surface");
    } else {
        Pass("Phong Shading, Light Behind Surface");
    }
}



void TestSceneCreation() {
    Camera c;
    Light l;

    Scene s;
    ConstructScene(&s, c, l);
    DeconstructScene(&s);

    Pass("Deconstruct Empty Scene");

    ConstructScene(&s, c, l);
    
    Shape sphere;

    ConstructSphere(&sphere, NewPnt3(0, 0, 0), 1.0);
    AddShape(&s, sphere);

    DeconstructScene(&s);

    Pass("Deconstruct Scene with Shapes");
}

void ConstructDefaultScene(Scene* s) {
    Camera c;
    Light l;
    ConstructLight(&l, NewPnt3(-10, 10, -10));
    ConstructScene(s, c, l);
    
    Shape s1;
    ConstructSphere(&s1, NewPnt3(0, 0, 0), 1.0);
    s1.material.diffuse_reflection = 0.7;
    s1.material.specular_reflection = 0.2;

    Shape s2;
    ConstructSphere(&s2, NewPnt3(0, 0, 0), 0.5);

    AddShape(s, s1);
    AddShape(s, s2);
}

void TestScene() {
    Scene s;
    ConstructDefaultScene(&s);

    Ray r;
    ConstructRay(&r, NewPnt3(0, 0, -5), NewVec3(0, 0, 1));

    Set is;
    ConstructSet(&is, sizeof(Intersection));

    IntersectScene(&s, r, &is);

    if (is.length != 2) {
        Fail("Scene, Number of Intersections");
        return;
    } else {
        Pass("Scene, Number of Intersections");
    }

    Intersection* i = Index(&is, 0);
    Intersection* i2 = Index(&is, 1);

    if (!FloatEquality(i->ray_times[0], 4.0) || !FloatEquality(i->ray_times[1], 6.0)) {
        Fail("Scene, First Object");
    } else {
        Pass("Scene, First Object");
    }

    if (!FloatEquality(i2->ray_times[0], 4.5) || !FloatEquality(i2->ray_times[1], 5.5)) {
        Fail("Scene, Second Object");
    } else {
        Pass("Scene, Second Object");
    }

    DeconstructSet(&is);
    DeconstructScene(&s);
}

void TestViewMatrix() {
    Matrix4x4 m1 = ViewMatrix(NewPnt3(0, 0, 0), NewPnt3(0, 0, -1), NewVec3(0, 1, 0));
    Matrix4x4 expected = IdentityMatrix();

    if (!MatrixFuzzyEqual(m1, expected)) {
        Fail("View Matrix, Identity");
        PrintMatrix(m1);
    } else {
        Pass("View Matrix, Identity");
    }

    Matrix4x4 m2 = ViewMatrix(NewPnt3(0, 0, 0), NewPnt3(0, 0, 1), NewVec3(0, 1, 0));
    Matrix4x4 expected2 = ScalingMatrix(-1, 1, -1);

    if (!MatrixFuzzyEqual(m2, expected2)) {
        Fail("View Matrix, Scaling");
    } else {
        Pass("View Matrix, Scaling");
    }

    Matrix4x4 m3 = ViewMatrix(NewPnt3(0, 0, 8), NewPnt3(0, 0, 0), NewVec3(0, 1, 0));
    Matrix4x4 expected3 = TranslationMatrix(0, 0, -8);

    if (!MatrixFuzzyEqual(m3, expected3)) {
        Fail("View Matrix, Translation");
        PrintMatrix(m3);
        printf("\n");
        PrintMatrix(expected3);
    } else {
        Pass("View Matrix, Translation");
    }

    Matrix4x4 m4 = ViewMatrix(NewPnt3(1, 3, 2), NewPnt3(4, -2, 8), NewVec3(1, 1, 0));
    Matrix4x4 expected4 = {
        contents: {
            {-0.50709, 0.50709, 0.67612, -2.36643},
            {0.76772, 0.60609, 0.12122, -2.82843},
            {-0.35857, 0.59761, -0.71714, 0},
            {0, 0, 0, 1}
        }
    };

    if (!MatrixFuzzyEqual(m4, expected4)) {
        Fail("View Matrix, Arbitrary");
        PrintMatrix(m4);
        printf("\n");
        PrintMatrix(expected4);
    } else {
        Pass("View Matrix, Arbitrary");
    }
}

void TestCamera() {
    Camera c;
    ConstructCamera(&c, 200, 125, M_PI/2);

    if (!FloatEquality(0.01, c.pixel_size)) {
        Fail("Pixel Size, Horizontal Canvas");
    } else {
        Pass("Pixel Size, Horizontal Canvas");
    }

    ConstructCamera(&c, 125, 200, M_PI/2);

    if (!FloatEquality(0.01, c.pixel_size)) {
        Fail("Pixel Size, Vertical Canvas");
    } else {
        Pass("Pixel Size, Vertical Canvas");
    }
 
    ConstructCamera(&c, 201, 101, M_PI / 2);
    Ray r = RayForPixel(&c, 100, 50);

    if (!TupleFuzzyEqual(NewPnt3(0, 0, 0), r.origin) || !TupleFuzzyEqual(NewVec3(0, 0, -1), r.direction)) {
        Fail("Camera, Center of Canvas");
        PrintTuple(r.origin);
        PrintTuple(r.direction);
    } else {
        Pass("Camera, Center of Canvas");
    }

    ConstructCamera(&c, 201, 101, M_PI / 2);
    r = RayForPixel(&c, 0, 0);

    if (!TupleFuzzyEqual(NewPnt3(0, 0, 0), r.origin) || !TupleFuzzyEqual(NewVec3(0.66519, 0.33259, -0.66851), r.direction)) {
        Fail("Camera, Corner of Canvas");
        PrintTuple(r.origin);
        PrintTuple(r.direction);
    } else {
        Pass("Camera, Corner of Canvas");
    }

    ConstructCamera(&c, 201, 101, M_PI / 2);
    CameraApplyTransformation(&c, MatrixMultiply(RotationYMatrix(M_PI / 4), TranslationMatrix(0, -2, 5)));
    r = RayForPixel(&c, 100, 50);
    float r2_2 = sqrtf(2) / 2;
 
    if (!TupleFuzzyEqual(NewPnt3(0, 2, -5), r.origin) || !TupleFuzzyEqual(NewVec3(r2_2, 0, -r2_2), r.direction)) {
        Fail("Camera, Transformed");
        PrintTuple(r.origin);
        PrintTuple(r.direction);
    } else {
        Pass("Camera, Transformed");
    }
}

void TestShadow() {
    ShadingJob sj = {
        .eye_vector = NewVec3(0, 0, -1),
        .surface_normal = NewVec3(0, 0, -1),
        .shadow = 1,
        .position = NewPnt3(0, 0, 0),
    };

    AssignDefaultTestMaterial(&sj.material);
    ConstructLight(&sj.light, NewPnt3(0, 0, -10));

    Tuple3 color = PhongShading(sj);
    
    if (!TupleFuzzyEqual(NewTuple3(0.1, 0.1, 0.1, 0.1), color)) {
        Fail("Shadow Shading");
        PrintTuple(color);
    } else {
        Pass("Shadow Shading");
    }

    Scene sc;
    ConstructDefaultScene(&sc);

    if (IsInShadow(&sc, NewPnt3(0, 10, 0))) {
        Fail("Shadow Test, Out of Shadow");
    } else {
        Pass("Shadow Test, Out of Shadow");
    }

    if (!IsInShadow(&sc, NewPnt3(10, -10, 10))) {
        Fail("Shadow Test, In Shadow");
    } else {
        Pass("Shadow Test, In Shadow");
    }

    if (IsInShadow(&sc, NewPnt3(-20, 20, -20))) {
        Fail("Shadow Test, Object behind light");
    } else {
        Pass("Shadow Test, Object behind light");
    }

    if (IsInShadow(&sc, NewPnt3(-2, 2, -2))) {
        Fail("Shadow Test, Object behind point");
    } else {
        Pass("Shadow Test, Object behind point");
    }
}

int main() {
    num_failed = 0;
    num_passed = 0;

    TestSet();
    TestMatrixEqual();
    TestMatrixMultiply();
    TestMatrixTranspose();
    TestMatrixInvert();
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

    TestRay();
    TestRaySphereIntersection();
    TestRayTransform();
    SphereNormal();
    TestSphereGeometry();
    TestPhongShading();
    TestSceneCreation();
    TestScene();
    TestViewMatrix();
    TestCamera();
    TestShadow();

    printf("Test(s) Passed: %d\nTests(s) Failed: %d\n", num_passed, num_failed);

    return 0;
}