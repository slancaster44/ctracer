#include <stdio.h>
#include <math.h>

#include "tuple.h"
#include "equality.h"
#include "matrix.h"
#include "canvas.h"
#include "shape.h"
#include "material.h"
#include "light.h"
#include "scene.h"
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

#define TEST(CONDITION, MSG) {  \
    if (CONDITION) {            \
        Pass(MSG);              \
    } else {                    \
        Fail(MSG);              \
    }                           \
}                               \

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
    for (unsigned int i = 0; i < s.length; i++) {
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
    for (unsigned int i = 0; i < s.length; i++) {
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
        .contents = {
            {-1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }
    };

    Matrix4x4 m2 = {
        .contents = {
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

    m2.contents[0][0] = -1.0001f;
    if (MatrixEqual(m1, m2) || !MatrixFuzzyEqual(m1, m2)) {
        Fail("Matrix Fuzzy Equality");
    } else {
        Pass("Matrix Fuzzy Equality");
    }
}

void TestMatrixMultiply() {
    Matrix4x4 m1 = {
        .contents = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 8, 7 ,6},
            {5, 4, 3, 2}
        }
    };

    Matrix4x4 m2 = {
        .contents = {
            {-2, 1, 2, 3},
            {3, 2, 1, -1},
            {4, 3, 6, 5},
            {1, 2, 7, 8}
        }
    };

    Matrix4x4 res = {
        .contents = {
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
        .contents = {
            {0, 9, 3, 0},
            {9, 8, 0, 8},
            {1, 8, 5, 3},
            {0, 0, 5, 8}
        }
    };

    Matrix4x4 res = {
        .contents = {
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
        .contents = {
            {8, -5, 9, 2},
            {7, 5, 6, 1},
            {-6, 0, 9, 6},
            {-3, 0, -9, -4}
        }
    };

    Matrix4x4 m2 = {
        .contents = {
            {-0.15385f, -0.15385f, -0.28205f, -0.53846f},
            {-0.07692f, 0.12308f, 0.02564f, 0.03077f},
            {0.35897f, 0.35897f, 0.43590f, 0.92308f},
            {-0.69231f, -0.69231f, -0.76923f, -1.92308f}
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
        .contents = {
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

    if (!FloatEquality(TupleMagnitude(result), 1.0)) {
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
        .origin = NewPnt3(2, 3, 4),
        .direction = NewVec3(1, 0, 0),
    };

    if (!TupleEqual(RayPosition(r, 2.5), NewPnt3(4.5, 3, 4))) {
        Fail("Ray Position Finding");
    } else {
        Pass("Ray Position Finding");
    }
}

void TestRaySphereIntersection() {
    Shape sphere = NewSphere(NewPnt3(0, 0, 0), 1.0);

    Ray r1 = {
        .origin = NewPnt3(0, 0, -5),
        .direction = NewVec3(0, 0, 1),
    };

    Intersection intersection = Intersect(&sphere, r1);

    if (!FloatEquality(4.0, intersection.ray_times[0]) || !FloatEquality(6.0, intersection.ray_times[1])) {
        Fail("Ray/Sphere Intersection, Normal");
        printf("\t%f\n", intersection.ray_times[1]);
    } else {
        Pass("Ray/Sphere Intersection, Normal");
    }

    Ray r2 = {
        .origin = NewPnt3(0, 1, -5),
        .direction = NewVec3(0, 0, 1),
    };

    Intersection i2 = Intersect(&sphere, r2);

    if (!FloatEquality(i2.ray_times[0], 5.0) || i2.count != 1) {
        Fail("Ray/Sphere Intersection, Tangent");
        printf("\t%f\n", i2.ray_times[0]);
    } else {
        Pass("Ray/Sphere Intersection, Tangent");
    }
    
    Ray r3 = {
        .origin = NewPnt3(0, 0, 0),
        .direction = NewVec3(0, 0, 1),
    };

    Intersection i3 = Intersect(&sphere, r3);

    if (!FloatEquality(i3.ray_times[0], -1) || !FloatEquality(1.0, i3.ray_times[1])) {
        Fail("Ray/Sphere Intersection, Inside");
        printf("\t%f\n", i3.ray_times[0]);
    } else {
        Pass("Ray/Sphere Intersection, Inside");
    }


    Ray r4 = {
        .origin = NewPnt3(0, 0, 5),
        .direction = NewVec3(0, 0, 1),
    };

    Intersection i4 = Intersect(&sphere, r4);

    if (!FloatEquality(i4.ray_times[0], -6) || !FloatEquality(-4.0, i4.ray_times[1])) {
        Fail("Ray/Sphere Intersection, Behind");
        printf("\t%f\n", i4.ray_times[0]);
    } else {
        Pass("Ray/Sphere Intersection, Behind");
    }

    Ray r5 = {
        .origin = NewPnt3(1.1f, 1.1f, 0),
        .direction = NewVec3(0, 0, 1),
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
        .origin = NewPnt3(1, 2, 3),
        .direction = NewVec3(0, 1, 0),
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
    Shape s = NewSphere(NewPnt3(0, 0, 0), 1.0);

    if (!TupleFuzzyEqual(NormalAt(&s, NewPnt3(1, 0, 0)), NewVec3(1, 0, 0))) {
        PrintTuple(NormalAt(&s, NewPnt3(1, 0, 0)));
        PrintTuple(NewVec3(1, 0, 0));
        Fail("Sphere Normal");
    } else {
        Pass("Sphere Normal");
    }


    s.transformation = TranslationMatrix(0, 1, 0);
    s.inverse_transform = MatrixInvert(s.transformation); 

    Tuple3 n = NormalAt(&s, NewPnt3(0, 1.70711f, -0.70711f));
    Tuple3 expected_out = NewVec3(0, 0.70711f, -0.70711f);

    if (!TupleFuzzyEqual(n, expected_out)) {
        PrintTuple(n);
        Fail("Sphere Normal, Translation");
    } else {
        Pass("Sphere Normal, Translation");
    }

    s.transformation = MatrixMultiply(ScalingMatrix(1, 0.5, 1), RotationZMatrix((float) M_PI / 5.0f));
    s.inverse_transform = MatrixInvert(s.transformation);

    n = NormalAt(&s, NewPnt3(0, sqrtf(2) / 2, -sqrtf(2) / 2));
    expected_out = NewVec3(0.0f, 0.97014f, -0.24254f);

    if (!TupleFuzzyEqual(n, expected_out)) {
        PrintTuple(n);
        Fail("Sphere Normal, Scaling & Rotation");
    } else {
        Pass("Sphere Normal, Scaling & Rotation");
    }
}

void TestSphereGeometry() {
    Shape s = NewSphere(NewPnt3(0, 0, 0), 6.0);

    Ray r = {
        .origin = NewPnt3(0, 6, 0),
        .direction = NewVec3(0, 0, 1),
    };

    Intersection res = Intersect(&s, r);

    if (res.ray_times[0] != 0) {
        Fail("Sphere geometry, radius");
        printf("\t%d: %f, %f\n", res.count, res.ray_times[0], res.ray_times[1]);
    } else {
        Pass("Sphere geometry, radius");
    }

    s = NewSphere(NewPnt3(1, 1, 0), 1.0);
    Ray r2 = {
        .origin = NewPnt3(1, 1, -3),
        .direction = NewVec3(0, 0, 1),
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
    m->ambient_reflection = 0.1f;
    m->diffuse_reflection = 0.9f;
    m->specular_reflection = 0.9f;
    m->shininess = 200;
}

void TestSceneCreation() {
    Camera c;
    Light l;

    Scene s;
    ConstructScene(&s, c, l);
    DeconstructScene(&s);

    Pass("Deconstruct Empty Scene");

    ConstructScene(&s, c, l);
    
    Shape sphere = NewSphere(NewPnt3(0, 0, 0), 1.0);
    AddShape(&s, sphere);

    DeconstructScene(&s);

    Pass("Deconstruct Scene with Shapes");
}

void ConstructDefaultScene(Scene* s) {
    Camera c;
    Light l = NewLight(NewPnt3(-10, 10, -10));
    ConstructScene(s, c, l);
    
    Shape s1 = NewSphere(NewPnt3(0, 0, 0), 1.0);
    s1.material.diffuse_reflection = 0.7f;
    s1.material.specular_reflection = 0.2f;

    Shape s2 = NewSphere(NewPnt3(0, 0, 0), 0.5);

    AddShape(s, s1);
    AddShape(s, s2);
}

void TestReflection() {
    //Reflective surface
    Scene reflective_scene;
    ConstructDefaultScene(&reflective_scene);

    Shape new_plane = NewPlane(NewPnt3(0, 0, 0), NewVec3(0, 1, 0));
    new_plane.material = NewMaterial(NewColor(255, 255, 255, 255));
    new_plane.material.general_reflection = 0.5;
    ApplyTransformation(&new_plane, TranslationMatrix(0, -1, 0));
    AddShape(&reflective_scene, new_plane);

    Ray r2 = NewRay(NewPnt3(0, 0, -3), NewVec3(0, -sqrtf(2.0f) / 2, sqrtf(2.0f) / 2));
    Tuple3 result = ColorFor(&reflective_scene, r2);
    PrintTuple(result);
}

void TestScene() {
    Scene s;
    ConstructDefaultScene(&s);

    Ray r = NewRay(NewPnt3(0, 0, -5), NewVec3(0, 0, 1));

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
        .contents = {
            {-0.50709f, 0.50709f, 0.67612f, -2.36643f},
            {0.76772f, 0.60609f, 0.12122f, -2.82843f},
            {-0.35857f, 0.59761f, -0.71714f, 0},
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
    Camera c = NewCamera(200, 125, (float) M_PI_2);

    if (!FloatEquality(0.01f, c.pixel_size)) {
        Fail("Pixel Size, Horizontal Canvas");
    } else {
        Pass("Pixel Size, Horizontal Canvas");
    }

    c = NewCamera(125, 200, (float) M_PI_2);

    if (!FloatEquality(0.01f, c.pixel_size)) {
        Fail("Pixel Size, Vertical Canvas");
    } else {
        Pass("Pixel Size, Vertical Canvas");
    }
 
    c = NewCamera(201, 101, (float) M_PI_2);
    Ray r = RayForPixel(&c, 100, 50);

    if (!TupleFuzzyEqual(NewPnt3(0, 0, 0), r.origin) || !TupleFuzzyEqual(NewVec3(0, 0, -1), r.direction)) {
        Fail("Camera, Center of Canvas");
        PrintTuple(r.origin);
        PrintTuple(r.direction);
    } else {
        Pass("Camera, Center of Canvas");
    }

    c = NewCamera(201, 101, (float) M_PI_2);
    r = RayForPixel(&c, 0, 0);

    if (!TupleFuzzyEqual(NewPnt3(0, 0, 0), r.origin) || !TupleFuzzyEqual(NewVec3(0.66519f, 0.33259f, -0.66851f), r.direction)) {
        Fail("Camera, Corner of Canvas");
        PrintTuple(r.origin);
        PrintTuple(r.direction);
    } else {
        Pass("Camera, Corner of Canvas");
    }

    c = NewCamera(201, 101, (float) M_PI_2);
    CameraApplyTransformation(&c, MatrixMultiply(RotationYMatrix((float) M_PI_4), TranslationMatrix(0, -2, 5)));
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

    DeconstructScene(&sc);
}

void TestPlane() {

    bool passed = true;
    for (float x = 0.1f; x < 1; x += 0.1f) {
        for (float y = 0.0f; y < 1; y += 0.1f) {
            for (float z = 0.0f; z < 1; z += 0.1f) {
                Tuple3 normal = TupleNormalize(NewVec3(x, y, z));
                Shape test_plane = NewPlane(NewPnt3(0, 0, 0), normal);

                Tuple3 result = NormalAt(&test_plane, NewPnt3(0, 0, 0));

                if (!TupleFuzzyEqual(result, normal)) {
                    Fail("Plane Normal");
                    PrintMatrix(test_plane.transformation);
                    PrintTuple(normal);
                    PrintTuple(result);
                    passed = false;
                }
            }
        }
    }

    if (passed) {
        Pass("Plane Normal");
    }

    Shape plane = NewPlane(NewPnt3(0, 0, 0), NewVec3(0, 1, 0));
    Ray r = NewRay(NewPnt3(0, 10, 0), NewVec3(0, 0, 1));
    Intersection result = Intersect(&plane, r);
    TEST(result.count == 0, "Plane Intersection, parallel ray")

    r = NewRay(NewPnt3(0, 0, 0), NewVec3(0, 0, 1));
    result = Intersect(&plane, r);
    TEST(result.count == 0, "Plane Intersection, coplanar ray")

    r = NewRay(NewPnt3(0, 1, 0), NewVec3(0, -1, 0));
    result = Intersect(&plane, r);
    TEST(FloatEquality(1.0, result.ray_times[0]), "Plane Intersection, Intersecting from above");


    r = NewRay(NewPnt3(0, -1, 0), NewVec3(0, 1, 0));
    result = Intersect(&plane, r);
    TEST(FloatEquality(1.0, result.ray_times[0]), "Plane Intersection, Intersecting from above");
}

void TestSceneReading() {
    Scene s;
    ReadScene(&s, "./scenes/three_spheres.json");

    TEST(s.camera.width == 3840, "Reading json, camera width");
    TEST(s.camera.height == 2160, "Reading json, camera height");
    TEST(s.camera.fov == 1.047f, "Reading json, camera fov");
    Matrix4x4 expected_camera_transform = ViewMatrix(NewPnt3(0, 1.5, -5), NewPnt3(0, 1, 0), NewVec3(0, 1, 0));
    TEST(MatrixFuzzyEqual(expected_camera_transform, s.camera.view_transformation), "Reading json, camera transform");

    TEST(TupleEqual(s.light.origin, NewPnt3(-10, 10, -10)), "Reading json, light origin");
    TEST(TupleEqual(s.light.color, NewColor(255, 255, 255, 255)), "Reading json, light color");

    Shape* s1 = Index(&s.shapes, 0);
    TEST(s1->type == SPHERE, "Reading json, shape type");

    Matrix4x4 s1_expected = {
        .contents = {
            {0.33f, 0, 0, -1.4f},
            {0, 0.33f,  0, 0.33f},
            {0, 0, 0.33f, -0.75f},
            {0, 0, 0, 1.0f}
        }
    };
    TEST(MatrixFuzzyEqual(s1_expected, s1->transformation), "Reading json, shape transformation");
    TEST(MatrixFuzzyEqual(MatrixInvert(s1_expected), s1->inverse_transform), "Reading json, inverse transformation");
    TEST(s.shapes.length == 4, "Reading json, shape list length");

    TEST(FloatEquality(s1->material.ambient_reflection, 0.1f), "Reading json, ambient reflection");
    TEST(FloatEquality(s1->material.diffuse_reflection, 0.9f), "Reading json, diffuse reflection");
    TEST(FloatEquality(s1->material.specular_reflection, 0.9f), "Reading json, specular reflection");
    TEST(s1->material.general_reflection == 0.0, "Reading json, general reflection");
    TEST(s1->material.shininess == 200, "Reading json, material shininess");

    TEST(TupleFuzzyEqual(NewPnt3(0.635f, 0, 1), s1->material.color), "Reading json, color");
    TEST(PhongShader == s1->material.shader, "Reading json, shader function");
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
    TestSceneCreation();
    TestScene();
    TestViewMatrix();
    TestCamera();
    TestShadow();
    TestPlane();
    TestReflection();

    TestSceneReading();

    printf("Test(s) Passed: %d\nTests(s) Failed: %d\n", num_passed, num_failed);

    return 0;
}