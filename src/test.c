#include <stdio.h>
#include <math.h>

#include "tuple.h"
#include "tree.h"
#include "equality.h"
#include "matrix.h"
#include "canvas.h"
#include "shape.h"
#include "material.h"
#include "light.h"
#include "scene.h"
#include "intersection.h"
#include "set.h"
#include "bounds.h"

static int num_failed;
static int num_passed;

void Fail(const char *msg)
{
    num_failed++;
    printf("\033[0;31m[FAIL]\033[0;37m %s\n", msg);
}

void Pass(const char *msg)
{
    num_passed++;
    printf("\033[0;32m[PASS]\033[0;37m %s\n", msg);
}

#define TEST(CONDITION, MSG) \
    {                        \
        if (CONDITION)       \
        {                    \
            Pass(MSG);       \
        }                    \
        else                 \
        {                    \
            Fail(MSG);       \
        }                    \
    }

void TestSet()
{
    Set s;
    ConstructSet(&s, sizeof(int));

    int l = 1;
    AppendValue(&s, &l);
    AppendValue(&s, &l);
    AppendValue(&s, &l);
    AppendValue(&s, &l);
    AppendValue(&s, &l);

    int out = 0;
    for (unsigned int i = 0; i < s.length; i++)
    {
        int *n;
        n = Index(&s, i);
        out += *n;
    }

    if (out != 5)
    {
        Fail("Set Indexing");
        printf("\t%d\n", out);
    }
    else
    {
        Pass("Set Indexing");
    }

    out = 0;
    for (unsigned int i = 0; i < s.length; i++)
    {
        int n;
        CopyOut(&s, i, &n);
        out += n;
    }

    if (out != 5)
    {
        Fail("Set Copy Out");
        printf("\t%d\n", out);
    }
    else
    {
        Pass("Set Copy Out");
    }

    Set s2;
    CloneSet(&s2, &s);
    TEST(s2.length == s.length, "Set cloning, set length");

    for (unsigned i = 0; i < s2.length; i++)
    {
        int *exp = Index(&s, i);
        int *res = Index(&s2, i);

        TEST(*exp == *res && exp != res, "Set cloning")
    }

    DeconstructSet(&s);
}

void TestMatrixEqual()
{
    Matrix4x4 m1 = {
        .contents = {
            {-1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }};

    Matrix4x4 m2 = {
        .contents = {
            {-1, 2, 3, 4},
            {2, 4, 4, 2},
            {8, 6, 4, 1},
            {0, 0, 0, 1},
        }};

    if (!MatrixEqual(m1, m2))
    {
        Fail("Matrix Equality");
    }
    else
    {
        Pass("Matrix Equality");
    }

    m2.contents[0][0] += EQUALITY_EPSILON / 2;
    TEST(MatrixFuzzyEqual(m1, m2), "Matrix Fuzzy Equality");
}

void TestMatrixMultiply()
{
    Matrix4x4 m1 = {
        .contents = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 8, 7, 6},
            {5, 4, 3, 2}}};

    Matrix4x4 m2 = {
        .contents = {
            {-2, 1, 2, 3},
            {3, 2, 1, -1},
            {4, 3, 6, 5},
            {1, 2, 7, 8}}};

    Matrix4x4 res = {
        .contents = {
            {20, 22, 50, 48},
            {44, 54, 114, 108},
            {40, 58, 110, 102},
            {16, 26, 46, 42}}};

    if (!MatrixFuzzyEqual(MatrixMultiply(m1, m2), res))
    {
        Fail("Matrix Multiplication");
    }
    else
    {
        Pass("Matrix Multiplication");
    }
}

void TestMatrixTranspose()
{
    Matrix4x4 m1 = {
        .contents = {
            {0, 9, 3, 0},
            {9, 8, 0, 8},
            {1, 8, 5, 3},
            {0, 0, 5, 8}}};

    Matrix4x4 res = {
        .contents = {
            {0, 9, 1, 0},
            {9, 8, 8, 0},
            {3, 0, 5, 5},
            {0, 8, 3, 8}}};

    if (!MatrixEqual(MatrixTranspose(m1), res))
    {
        Fail("Matrix Transposition");
    }
    else
    {
        Pass("Matrix Transposition");
    }
}

void TestMatrixInvert()
{ // TODO: More tests?
    Matrix4x4 m1 = {
        .contents = {
            {8, -5, 9, 2},
            {7, 5, 6, 1},
            {-6, 0, 9, 6},
            {-3, 0, -9, -4}}};

    Matrix4x4 m2 = {
        .contents = {
            {-0.15385, -0.15385, -0.28205, -0.53846f},
            {-0.07692, 0.12308, 0.02564, 0.03077f},
            {0.35897, 0.35897, 0.43590, 0.92308f},
            {-0.69231, -0.69231, -0.76923, -1.92308f}}};

    if (!MatrixFuzzyEqual(MatrixInvert(m1), m2))
    {
        PrintMatrix(MatrixInvert(m1));
        Fail("Matrix Inversion");
    }
    else
    {
        Pass("Matrix Inversion");
    }
}

void TestMatrixVectorMultiply()
{
    Matrix4x4 m1 = {
        .contents = {
            {0, 2, 3, -47},
            {4, 5, 6, 26},
            {7, -8, 9, 15},
            {12, 11, 10, 14}}};

    Tuple3 t1 = NewTuple3(19, 28, -37, 16);
    Tuple3 res = NewTuple3(-807, 410, -184, 390);

    if (!TupleEqual(res, MatrixTupleMultiply(m1, t1)))
    {
        Fail("Matrix & Tuple Multiplication");
    }
    else
    {
        Pass("Matrix & Tuple Multiplication");
    }
}

void TestTupleEqual()
{
    if (!TupleEqual(NewVec3(1, 2, 3), NewVec3(1, 2, 3)) || TupleEqual(NewVec3(1, 2, 3), NewVec3(3, 2, 1)))
    {
        Fail("Tuple Equality");
    }
    else
    {
        Pass("Tuple Equality");
    }
}

void TestScalarMultiply()
{
    Tuple3 t1 = NewVec3(2.0, 4.0, 6.0);
    Tuple3 result = TupleScalarMultiply(t1, 2);

    if (!TupleEqual(result, NewVec3(4, 8, 12)))
    {
        Fail("Tuple Scalar Multiplication");
    }
    else
    {
        Pass("Tuple Scalar Multiplication");
    }
}

void TestScalarDivide()
{
    Tuple3 t1 = NewVec3(2.0, 4.0, 6.0);
    Tuple3 result = TupleScalarDivide(t1, 2);

    if (!TupleEqual(result, NewVec3(1, 2, 3)))
    {
        Fail("Tuple Scalar Division");
    }
    else
    {
        Pass("Tuple Scalar Division");
    }
}

void TestTupleNegate()
{
    Tuple3 t1 = NewVec3(1, -2, 3);
    Tuple3 result = TupleNegate(t1);

    if (!TupleEqual(result, NewVec3(-1, 2, -3)))
    {
        Fail("Tuple Negation");
    }
    else
    {
        Pass("Tuple Negation");
    }
}

void TestTupleMagnitude()
{
    Tuple3 t1 = NewVec3(1, 2, 3);
    double result = TupleMagnitude(t1);

    if (!FloatEquality(result, sqrt(14)))
    {
        Fail("Tuple Magnitude");
    }
    else
    {
        Pass("Tuple Magnitude");
    }
}

void TestTupleDotProduct()
{
    Tuple3 t1 = NewVec3(1, 2, 3);
    Tuple3 t2 = NewVec3(2, 3, 4);

    double result = TupleDotProduct(t1, t2);
    if (!FloatEquality(result, 20))
    {
        printf("%f\n", result);
        Fail("Tuple Dot Product");
    }
    else
    {
        Pass("Tuple Dot Product");
    }
}

void TestTupleNormalize()
{
    Tuple3 t1 = NewVec3(100, 100, 100);
    Tuple3 result = TupleNormalize(t1);

    if (!FloatEquality(TupleMagnitude(result), 1.0))
    {
        Fail("Tuple Normalization");
    }
    else
    {
        Pass("Tuple Normalization");
    }
}

void TestTupleMultiply()
{
    Tuple3 t1 = NewPnt3(1, 2, 3);
    Tuple3 t2 = NewPnt3(1, 2, 3);

    Tuple3 result = TupleMultiply(t1, t2);

    if (!TupleEqual(result, NewPnt3(1, 4, 9)))
    {
        Fail("Tuple Multiplication");
    }
    else
    {
        Pass("Tuple Multiplication");
    }
}

void TestTupleDivide()
{
    Tuple3 t1 = NewPnt3(1, 2, 3);
    Tuple3 t2 = NewPnt3(1, 2, 3);

    Tuple3 result = TupleDivide(t1, t2);

    if (!TupleEqual(result, NewPnt3(1, 1, 1)))
    {
        Fail("Tuple Division");
    }
    else
    {
        Pass("Tuple Division");
    }
}

void TestTupleAdd()
{
    Tuple3 t1 = NewVec3(1, 2, 3);
    Tuple3 t2 = NewVec3(1, 2, 3);

    Tuple3 result = TupleAdd(t1, t2);

    if (!TupleEqual(result, NewVec3(2, 4, 6)))
    {
        Fail("Tuple Addition");
    }
    else
    {
        Pass("Tuple Addition");
    }
}

void TestTupleSubtract()
{
    Tuple3 t1 = NewVec3(1, 2, 3);
    Tuple3 t2 = NewVec3(1, 2, 3);

    Tuple3 result = TupleSubtract(t1, t2);

    if (!TupleEqual(result, NewVec3(0, 0, 0)))
    {
        Fail("Tuple Subtraction");
    }
    else
    {
        Pass("Tuple Subtraction");
    }
}

void TestRay()
{
    Ray r = {
        .origin = NewPnt3(2, 3, 4),
        .direction = NewVec3(1, 0, 0),
    };

    if (!TupleEqual(RayPosition(r, 2.5), NewPnt3(4.5, 3, 4)))
    {
        Fail("Ray Position Finding");
    }
    else
    {
        Pass("Ray Position Finding");
    }
}

void TestRaySphereIntersection()
{
    Shape sphere = NewSphere(NewPnt3(0, 0, 0), 1.0);

    Ray r1 = {
        .origin = NewPnt3(0, 0, -5),
        .direction = NewVec3(0, 0, 1),
    };

    Intersection intersection = Intersect(&sphere, r1);

    if (!FloatEquality(4.0, intersection.ray_times[0]) || !FloatEquality(6.0, intersection.ray_times[1]))
    {
        Fail("Ray/Sphere Intersection, Normal");
        printf("\t%f\n", intersection.ray_times[1]);
    }
    else
    {
        Pass("Ray/Sphere Intersection, Normal");
    }

    Ray r2 = {
        .origin = NewPnt3(0, 1, -5),
        .direction = NewVec3(0, 0, 1),
    };

    Intersection i2 = Intersect(&sphere, r2);

    if (!FloatEquality(i2.ray_times[0], 5.0) || i2.count != 1)
    {
        Fail("Ray/Sphere Intersection, Tangent");
        printf("\t%f\n", i2.ray_times[0]);
    }
    else
    {
        Pass("Ray/Sphere Intersection, Tangent");
    }

    Ray r3 = {
        .origin = NewPnt3(0, 0, 0),
        .direction = NewVec3(0, 0, 1),
    };

    Intersection i3 = Intersect(&sphere, r3);

    if (!FloatEquality(i3.ray_times[0], -1) || !FloatEquality(1.0, i3.ray_times[1]))
    {
        Fail("Ray/Sphere Intersection, Inside");
        printf("\t%f\n", i3.ray_times[0]);
    }
    else
    {
        Pass("Ray/Sphere Intersection, Inside");
    }

    Ray r4 = {
        .origin = NewPnt3(0, 0, 5),
        .direction = NewVec3(0, 0, 1),
    };

    Intersection i4 = Intersect(&sphere, r4);

    if (!FloatEquality(i4.ray_times[0], -6) || !FloatEquality(-4.0, i4.ray_times[1]))
    {
        Fail("Ray/Sphere Intersection, Behind");
        printf("\t%f\n", i4.ray_times[0]);
    }
    else
    {
        Pass("Ray/Sphere Intersection, Behind");
    }

    Ray r5 = {
        .origin = NewPnt3(1.1, 1.1, 0),
        .direction = NewVec3(0, 0, 1),
    };

    Intersection i5 = Intersect(&sphere, r5);

    if (i5.count != 0)
    {
        Fail("Ray/Sphere Intersection, No Hit");
        printf("\t%d\n", i5.count);
    }
    else
    {
        Pass("Ray/Sphere Intersection, No Hit");
    }
}

void TestRayTransform()
{
    Ray r = {
        .origin = NewPnt3(1, 2, 3),
        .direction = NewVec3(0, 1, 0),
    };

    Matrix4x4 scaling = ScalingMatrix(2, 3, 4);
    Ray r2 = RayTransform(r, scaling);

    if (!TupleFuzzyEqual(r2.direction, NewVec3(0, 3, 0)) || !TupleFuzzyEqual(r2.origin, NewPnt3(2, 6, 12)))
    {
        Fail("Ray Transformation");
    }
    else
    {
        Pass("Ray Transformation");
    }
}

void SphereNormal()
{
    Shape s = NewSphere(NewPnt3(0, 0, 0), 1.0);

    if (!TupleFuzzyEqual(NormalAt(&s, NewPnt3(1, 0, 0)), NewVec3(1, 0, 0)))
    {

        Fail("Sphere Normal");
    }
    else
    {
        Pass("Sphere Normal");
    }

    s.transformation = TranslationMatrix(0, 1, 0);
    s.inverse_transform = MatrixInvert(s.transformation);

    Tuple3 n = NormalAt(&s, NewPnt3(0, 1.70711, -0.70711));
    Tuple3 expected_out = NewVec3(0, 0.70711, -0.70711);

    if (!TupleFuzzyEqual(n, expected_out))
    {
        PrintTuple(expected_out);
        PrintTuple(n);
        Fail("Sphere Normal, Translation");
    }
    else
    {
        Pass("Sphere Normal, Translation");
    }

    s.transformation = MatrixMultiply(ScalingMatrix(1, 0.5, 1), RotationZMatrix((double)M_PI / 5.0));
    s.inverse_transform = MatrixInvert(s.transformation);

    n = NormalAt(&s, NewPnt3(0, sqrt(2) / 2, -sqrt(2) / 2));
    expected_out = TupleNormalize(NewVec3(0.0, 0.97014, -0.24254));

    if (!TupleFuzzyEqual(n, expected_out))
    {
        PrintTuple(expected_out);
        PrintTuple(n);
        Fail("Sphere Normal, Scaling & Rotation");
    }
    else
    {
        Pass("Sphere Normal, Scaling & Rotation");
    }
}

void TestSphereGeometry()
{
    Shape s = NewSphere(NewPnt3(0, 0, 0), 6.0);

    Ray r = {
        .origin = NewPnt3(0, 6, 0),
        .direction = NewVec3(0, 0, 1),
    };

    Intersection res = Intersect(&s, r);

    if (res.ray_times[0] != 0)
    {
        Fail("Sphere geometry, radius");
        printf("\t%d: %f, %f\n", res.count, res.ray_times[0], res.ray_times[1]);
    }
    else
    {
        Pass("Sphere geometry, radius");
    }

    s = NewSphere(NewPnt3(1, 1, 0), 1.0);
    Ray r2 = {
        .origin = NewPnt3(1, 1, -3),
        .direction = NewVec3(0, 0, 1),
    };

    res = Intersect(&s, r2);
    if (res.ray_times[0] != 2)
    {

        printf("%d: %f", res.count, res.ray_times[0]);
        Fail("Sphere geometry, origin");
    }
    else
    {
        Pass("Sphere geometry, origin");
    }
}

void AssignDefaultTestMaterial(Material *m)
{
    m->pattern = NewSolidPattern(NewColor(255, 255, 255, 255));
    m->ambient_reflection = 0.1;
    m->diffuse_reflection = 0.9;
    m->specular_reflection = 0.9;
    m->shininess = 200;
}

void TestSceneCreation()
{
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

void ConstructDefaultScene(Scene *s)
{
    Camera c;
    Light l = NewLight(NewPnt3(-10, 10, -10));
    ConstructScene(s, c, l);

    Shape s1 = NewSphere(NewPnt3(0, 0, 0), 1.0);
    s1.material.diffuse_reflection = 0.7;
    s1.material.specular_reflection = 0.2;

    Shape s2 = NewSphere(NewPnt3(0, 0, 0), 0.5);

    AddShape(s, s1);
    AddShape(s, s2);

    CalculateBounds(&s->shapes);
}

void TestReflection()
{
    // Reflective surface
    Scene reflective_scene;
    ConstructDefaultScene(&reflective_scene);

    Shape new_plane = NewPlane(NewPnt3(0, -1, 0), NewVec3(0, 1, 0));
    new_plane.material = NewMaterial(NewColor(255, 255, 255, 255));
    new_plane.material.general_reflection = 0.5;
    ApplyTransformation(&new_plane, TranslationMatrix(0, -1, 0));
    AddShape(&reflective_scene, new_plane);

    Ray r2 = NewRay(NewPnt3(0, 0, -3), NewVec3(0, -(sqrt(2.0) / 2), sqrt(2.0) / 2));
    CalculateBounds(&reflective_scene.shapes);
    Tuple3 result = ColorFor(&reflective_scene, r2);
    TEST(TupleFuzzyEqual(NewTuple3(0.739076, 0.749076, 0.729076, 0.749076), result), "Scene reflection");

    DeconstructScene(&reflective_scene);
}

void TestScene()
{
    Scene s;
    ConstructDefaultScene(&s);

    Ray r = NewRay(NewPnt3(0, 0, -5), NewVec3(0, 0, 1));

    Set is;
    ConstructSet(&is, sizeof(Intersection));

    IntersectScene(&s, r, &is);

    if (is.length != 2)
    {
        Fail("Scene, Number of Intersections");
        printf("\tNumber of intersections: %ld\n", is.length);
        return;
    }
    else
    {
        Pass("Scene, Number of Intersections");
    }

    Intersection *i = Index(&is, 0);
    Intersection *i2 = Index(&is, 1);

    if (!FloatEquality(i->ray_times[0], 4.0) || !FloatEquality(i->ray_times[1], 6.0))
    {
        Fail("Scene, First Object");
    }
    else
    {
        Pass("Scene, First Object");
    }

    if (!FloatEquality(i2->ray_times[0], 4.5) || !FloatEquality(i2->ray_times[1], 5.5))
    {
        Fail("Scene, Second Object");
    }
    else
    {
        Pass("Scene, Second Object");
    }

    DeconstructSet(&is);
    DeconstructScene(&s);
}

void TestViewMatrix()
{
    Matrix4x4 m1 = ViewMatrix(NewPnt3(0, 0, 0), NewPnt3(0, 0, -1), NewVec3(0, 1, 0));
    Matrix4x4 expected = IdentityMatrix();

    if (!MatrixFuzzyEqual(m1, expected))
    {
        PrintMatrix(m1);
        Fail("View Matrix, Identity");
    }
    else
    {
        Pass("View Matrix, Identity");
    }

    Matrix4x4 m2 = ViewMatrix(NewPnt3(0, 0, 0), NewPnt3(0, 0, 1), NewVec3(0, 1, 0));
    Matrix4x4 expected2 = ScalingMatrix(-1, 1, -1);

    if (!MatrixFuzzyEqual(m2, expected2))
    {
        Fail("View Matrix, Scaling");
    }
    else
    {
        Pass("View Matrix, Scaling");
    }

    Matrix4x4 m3 = ViewMatrix(NewPnt3(0, 0, 8), NewPnt3(0, 0, 0), NewVec3(0, 1, 0));
    Matrix4x4 expected3 = TranslationMatrix(0, 0, -8);

    if (!MatrixFuzzyEqual(m3, expected3))
    {
        Fail("View Matrix, Translation");

        printf("\n");
    }
    else
    {
        Pass("View Matrix, Translation");
    }

    Matrix4x4 m4 = ViewMatrix(NewPnt3(1, 3, 2), NewPnt3(4, -2, 8), NewVec3(1, 1, 0));
    Matrix4x4 expected4 = {
        .contents = {
            {-0.50709, 0.50709, 0.67612, -2.36643f},
            {0.76772, 0.60609, 0.12122, -2.82843f},
            {-0.35857, 0.59761, -0.71714, 0},
            {0, 0, 0, 1}}};

    if (!MatrixFuzzyEqual(m4, expected4))
    {
        Fail("View Matrix, Arbitrary");

        printf("\n");
    }
    else
    {
        Pass("View Matrix, Arbitrary");
    }
}

void TestCamera()
{
    Camera c = NewCamera(200, 125, (double)M_PI_2);

    if (!FloatEquality(0.01, c.pixel_size))
    {
        Fail("Pixel Size, Horizontal Canvas");
    }
    else
    {
        Pass("Pixel Size, Horizontal Canvas");
    }

    c = NewCamera(125, 200, (double)M_PI_2);

    if (!FloatEquality(0.01, c.pixel_size))
    {
        Fail("Pixel Size, Vertical Canvas");
    }
    else
    {
        Pass("Pixel Size, Vertical Canvas");
    }

    c = NewCamera(201, 101, (double)M_PI_2);
    Ray r = RayForPixel(&c, 100, 50);

    if (!TupleFuzzyEqual(NewPnt3(0, 0, 0), r.origin) || !TupleFuzzyEqual(NewVec3(0, 0, -1), r.direction))
    {
        Fail("Camera, Center of Canvas");
    }
    else
    {
        Pass("Camera, Center of Canvas");
    }

    c = NewCamera(201, 101, (double)M_PI_2);
    r = RayForPixel(&c, 0, 0);

    if (!TupleFuzzyEqual(NewPnt3(0, 0, 0), r.origin) || !TupleFuzzyEqual(NewVec3(0.66519, 0.33259, -0.66851), r.direction))
    {
        Fail("Camera, Corner of Canvas");
    }
    else
    {
        Pass("Camera, Corner of Canvas");
    }

    c = NewCamera(201, 101, (double)M_PI_2);
    CameraApplyTransformation(&c, MatrixMultiply(RotationYMatrix((double)M_PI_4), TranslationMatrix(0, -2, 5)));
    r = RayForPixel(&c, 100, 50);
    double r2_2 = sqrt(2) / 2;

    if (!TupleFuzzyEqual(NewPnt3(0, 2, -5), r.origin) || !TupleFuzzyEqual(NewVec3(r2_2, 0, -r2_2), r.direction))
    {
        Fail("Camera, Transformed");
    }
    else
    {
        Pass("Camera, Transformed");
    }
}

void TestShadow()
{

    Scene sc;
    ConstructDefaultScene(&sc);

    if (IsInShadow(&sc, NewPnt3(0, 10, 0)))
    {
        Fail("Shadow Test, Out of Shadow");
    }
    else
    {
        Pass("Shadow Test, Out of Shadow");
    }

    if (!IsInShadow(&sc, NewPnt3(10, -10, 10)))
    {
        Fail("Shadow Test, In Shadow");
    }
    else
    {
        Pass("Shadow Test, In Shadow");
    }

    if (IsInShadow(&sc, NewPnt3(-20, 20, -20)))
    {
        Fail("Shadow Test, Object behind light");
    }
    else
    {
        Pass("Shadow Test, Object behind light");
    }

    if (IsInShadow(&sc, NewPnt3(-2, 2, -2)))
    {
        Fail("Shadow Test, Object behind point");
    }
    else
    {
        Pass("Shadow Test, Object behind point");
    }

    DeconstructScene(&sc);
}

void TestPlane()
{

    bool passed = true;
    for (double x = 0.1; x < 1; x += 0.25)
    {
        for (double y = 0.0; y < 1; y += 0.25)
        {
            for (double z = 0.0; z < 1; z += 0.25)
            {
                Tuple3 normal = TupleNormalize(NewVec3(x, y, z));
                Shape test_plane = NewPlane(NewPnt3(0, 0, 0), normal);

                Tuple3 result = NormalAt(&test_plane, NewPnt3(0, 0, 0));

                if (!TupleFuzzyEqual(result, normal))
                {
                    PrintTuple(result);
                    PrintTuple(normal);
                    Fail("Plane Normal");
                    passed = false;
                }
            }
        }
    }

    if (passed)
    {
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

void TestSceneReading()
{
    Scene s;
    ReadScene(&s, "./scenes/three_spheres.json");

    TEST(s.camera.width == 3840, "Reading json, camera width");
    TEST(s.camera.height == 2160, "Reading json, camera height");
    TEST(s.camera.fov == 1.047, "Reading json, camera fov");
    Matrix4x4 expected_camera_transform = ViewMatrix(NewPnt3(0, 1.5, -5), NewPnt3(0, 1, 0), NewVec3(0, 1, 0));
    TEST(MatrixFuzzyEqual(expected_camera_transform, s.camera.view_transformation), "Reading json, camera transform");

    TEST(TupleEqual(s.light.origin, NewPnt3(-10, 10, -10)), "Reading json, light origin");
    TEST(TupleEqual(s.light.color, NewColor(255, 255, 255, 255)), "Reading json, light color");

    Shape *s1 = Index(&s.shapes.start.shapes, 0);
    TEST(s1->type == SPHERE, "Reading json, shape type");

    Matrix4x4 s1_expected = {
        .contents = {
            {0.33, 0, 0, -1.4f},
            {0, 0.33, 0, 0.33f},
            {0, 0, 0.33, -0.75f},
            {0, 0, 0, 1.0f}}};
    TEST(MatrixFuzzyEqual(s1_expected, s1->transformation), "Reading json, shape transformation");
    TEST(MatrixFuzzyEqual(MatrixInvert(s1_expected), s1->inverse_transform), "Reading json, inverse transformation");
    TEST(s.shapes.start.shapes.length == 4, "Reading json, shape list length");

    TEST(FloatEquality(s1->material.ambient_reflection, 0.1), "Reading json, ambient reflection");
    TEST(FloatEquality(s1->material.diffuse_reflection, 0.9), "Reading json, diffuse reflection");
    TEST(FloatEquality(s1->material.specular_reflection, 0.9), "Reading json, specular reflection");
    TEST(FloatEquality(s1->material.refractive_index, 1.0), "Reading json, refractive index");
    TEST(FloatEquality(s1->material.transparency, 0.0), "Reading json, transparency");
    TEST(s1->material.general_reflection == 0.0, "Reading json, general reflection");
    TEST(s1->material.shininess == 200, "Reading json, material shininess");

    TEST(PhongShader == s1->material.shader, "Reading json, shader function");

    DeconstructScene(&s);
}

void TestStripePattern()
{
    Tuple3 black = NewColor(0, 0, 0, 255);
    Tuple3 white = NewColor(255, 255, 255, 255);

    Pattern p1 = NewPattern(white, black, STRIPED);
    Shape s1 = NewSphere(NewPnt3(0, 0, 0), 1.0);
    s1.material.pattern = p1;

    Tuple3 r1 = PatternColorAt(&s1, NewPnt3(0, 0, 0));
    TEST(TupleFuzzyEqual(white, r1), "Stripe pattern, white stripe")

    Tuple3 r2 = PatternColorAt(&s1, NewPnt3(0, 1, 0));
    TEST(TupleFuzzyEqual(white, r2), "Stripe pattern, constant in y axis");

    Tuple3 r3 = PatternColorAt(&s1, NewPnt3(0, 0, 1));
    TEST(TupleFuzzyEqual(white, r3), "Stripe pattern. constant in z axis");

    Tuple3 r4 = PatternColorAt(&s1, NewPnt3(1, 0, 0));
    TEST(TupleFuzzyEqual(black, r4), "Stripe pattern, alters in x axis");
}

void TestScalarSubtract()
{
    Tuple3 t1 = NewTuple3(2, 3, 4, 5);
    Tuple3 expected = NewTuple3(1, 2, 3, 4);
    Tuple3 result = TupleScalarSubtract(t1, 1);

    TEST(TupleFuzzyEqual(expected, result), "Tuple scalar subtract");
}

void TestScalarAdd()
{
    Tuple3 t1 = NewTuple3(2, 3, 4, 5);
    Tuple3 expected = NewTuple3(3, 4, 5, 6);
    Tuple3 result = TupleScalarAdd(t1, 1);

    TEST(TupleFuzzyEqual(expected, result), "Tuple scalar add");
}

void TestCrossProduct()
{
    Tuple3 t1 = NewVec3(1, 2, 3);
    Tuple3 t2 = NewVec3(2, 3, 4);

    Tuple3 expected = NewVec3(-1, 2, -1);
    TEST(TupleFuzzyEqual(expected, TupleCrossProduct(t1, t2)), "Tuple cross product");

    expected = NewVec3(1, -2, 1);
    TEST(TupleFuzzyEqual(expected, TupleCrossProduct(t2, t1)), "Tuple cross product, reversed");
}

void TestVectorReflect()
{
    Tuple3 t1 = NewVec3(1, -1, 0);
    Tuple3 n1 = NewVec3(0, 1, 0);

    Tuple3 res = TupleReflect(t1, n1);
    Tuple3 exp = NewVec3(1, 1, 0);

    TEST(TupleFuzzyEqual(exp, res), "Tuple reflection");

    t1 = NewVec3(0, -1, 0);
    n1 = NewVec3(sqrt(2) / 2, sqrt(2) / 2, 0);

    res = TupleReflect(t1, n1);
    exp = NewVec3(1, 0, 0);
    TEST(TupleFuzzyEqual(exp, res), "Tuple reflection, second test");
}

void TestRefraction()
{
    // Test some refraction
    Scene s;
    ConstructDefaultScene(&s);

    Shape floor = NewPlane(NewPnt3(0, 0, 0), NewVec3(0, 1, 0));
    ApplyTransformation(&floor, TranslationMatrix(0, -1, 0));
    floor.material.transparency = 0.5;
    floor.material.refractive_index = 1.5;
    AddShape(&s, floor);

    Shape ball = NewSphere(NewPnt3(0, 0, 0), 1.0);
    ApplyTransformation(&ball, TranslationMatrix(0, -3.5, -0.5));
    ball.material.pattern.color_a = NewTuple3(1, 0, 0, 1);
    ball.material.ambient_reflection = 0.5;
    AddShape(&s, ball);

    CalculateBounds(&s.shapes);

    Ray r = NewRay(NewPnt3(0, 0, -3), NewVec3(0, -sqrt(2) / 2, sqrt(2) / 2));
    Tuple3 result = ColorFor(&s, r);
    Tuple3 expect = NewTuple3(0.936425, 0.549140, 0.411855, 0.936425);

    TEST(TupleFuzzyEqual(result, expect), "Refraction, shader test");

    DeconstructScene(&s);

    // Test no refraction
    Scene s2;
    ConstructDefaultScene(&s2);
    Shape *shape_1 = Index(&s2.shapes.start.shapes, 0);
    shape_1->material.transparency = 1.0;
    shape_1->material.refractive_index = 1.5;

    CalculateBounds(&s2.shapes);

    Ray r2 = NewRay(NewPnt3(0, 0, sqrt(2) / 2), NewVec3(0, 1, 0));
    Tuple3 result2 = ColorFor(&s2, r2);
    TEST(TupleFuzzyEqual(NewTuple3(0, 0, 0, 0), result2), "Refraction, none");
}

Shape NewGlassSphere()
{
    Shape sphere = NewSphere(NewPnt3(0, 0, 0), 1.0);
    sphere.material.transparency = 1.0;
    sphere.material.refractive_index = 1.5;

    return sphere;
}

void CalculateRefractionRatio(Set *intersections, unsigned long idx, double *n);
void GenerateSceneBVH(Scene *s);
void TestCalculateRefraction()
{
    Scene s;
    Camera c;
    Light l;
    ConstructScene(&s, c, l);

    Shape sphere_a = NewGlassSphere();
    ApplyTransformation(&sphere_a, ScalingMatrix(2, 2, 2));
    sphere_a.material.refractive_index = 1.5;
    AddShape(&s, sphere_a);

    Shape sphere_b = NewGlassSphere();
    ApplyTransformation(&sphere_b, TranslationMatrix(0, 0, -0.25));
    sphere_b.material.refractive_index = 2.0;
    AddShape(&s, sphere_b);

    Shape sphere_c = NewGlassSphere();
    ApplyTransformation(&sphere_c, TranslationMatrix(0, 0, 0.25));
    sphere_c.material.refractive_index = 2.5;
    AddShape(&s, sphere_c);

    GenerateSceneBVH(&s);

    Ray r = NewRay(NewPnt3(0, 0, -4), NewVec3(0, 0, 1));
    Set intersections;
    ConstructSet(&intersections, sizeof(Intersection));
    IntersectScene(&s, r, &intersections);

    TEST(intersections.length == 3, "Refraction, intersection count");

    double *results = alloca(2 * sizeof(double));
    CalculateRefractionRatio(&intersections, 0, results);
    TEST(results[0] == 1.0 && results[1] == 1.5, "Cacluate refraction ratio, 0"); //1 2.5

    CalculateRefractionRatio(&intersections, 1, results);
    TEST(results[0] == 1.5 && results[1] == 2.0, "Calculate refraction ratio, 1"); //2.5 1.5

    CalculateRefractionRatio(&intersections, 2, results);
    TEST(results[0] == 2.0 && results[1] == 2.5, "Calculate refraction ratio, 2"); //1.5 2.0

    DeconstructSet(&intersections);
    DeconstructScene(&s);
}

void TestCubeIntersection()
{
    Shape cube = NewCube(NewPnt3(0, 0, 0), 1.0); // The unit cube

    Tuple3 tests[7][3] = {
        {NewPnt3(5, 0.5, 0), NewVec3(-1, 0, 0), NewTuple3(4, 6, 0, 0)},
        {NewPnt3(-5, 0.5, 0), NewVec3(1, 0, 0), NewTuple3(4, 6, 0, 0)},
        {NewPnt3(0.5, 5, 0), NewVec3(0, -1, 0), NewTuple3(4, 6, 0, 0)},
        {NewPnt3(0.5, -5, 0), NewVec3(0, 1, 0), NewTuple3(4, 6, 0, 0)},
        {NewPnt3(0.5, 0, 5), NewVec3(0, 0, -1), NewTuple3(4, 6, 0, 0)},
        {NewPnt3(0.5, 0, -5), NewVec3(0, 0, 1), NewTuple3(4, 6, 0, 0)},
        {NewPnt3(0, 0.5, 0), NewVec3(0, 0, 1), NewTuple3(-1, 1, 0, 0)},
    };

    for (int j = 0; j < 7; j++)
    {
        Intersection intersect = Intersect(&cube, NewRay(tests[j][0], tests[j][1]));

        double exp_1 = tests[j][2][0];
        double exp_2 = tests[j][2][1];

        TEST(intersect.count == 2 && intersect.ray_times[0] == exp_1 && intersect.ray_times[1] == exp_2, "Cube intersection");
    }

    Tuple3 nohits[6][2] = {
        {NewPnt3(-2, 0, 0), NewVec3(0.2673, 0.5345, 0.8018)},
        {NewPnt3(0, -2, 0), NewVec3(0.8018, 0.2673, 0.5345)},
        {NewPnt3(0, 0, -2), NewVec3(0.5345, 0.8018, 0.2673)},
        {NewPnt3(2, 0, 2), NewVec3(0, 0, -1)},
        {NewPnt3(0, 2, 2), NewVec3(0, -1, 0)},
        {NewPnt3(2, 2, 0), NewVec3(-1, 0, 0)}};

    for (int k = 0; k < 6; k++)
    {
        Intersection intersect = Intersect(&cube, NewRay(nohits[k][0], nohits[k][1]));
        TEST(intersect.count == 0, "Cube intersection, no hit");
    }
}

void TestCubeNormal()
{
    Tuple3 tests[11][2] = {
        {NewPnt3(1, 0.5, -0.8), NewVec3(1, 0, 0)},
        {NewPnt3(-1, -0.2, 0.9), NewVec3(-1, 0, 0)},
        {NewPnt3(-0.4, 1, -0.1), NewVec3(0, 1, 0)},
        {NewPnt3(0.3, -1, -0.7), NewVec3(0, -1, 0)},
        {NewPnt3(-0.6, 0.3, 1), NewVec3(0, 0, 1)},
        {NewPnt3(0.4, 0.4, -1), NewVec3(0, 0, -1)},
        {NewPnt3(1, 1, 1), NewVec3(1, 0, 0)},
        {NewPnt3(-1, -1, -1), NewVec3(-1, 0, 0)},
        {NewPnt3(-2, -2, -1), NewVec3(-1, 0, 0)},
        {NewPnt3(-1.9, -1.7, -2.0), NewVec3(0, 0, -1)},
        {NewPnt3(0.9, 0.9, 0.9), NewVec3(1.0, 0, 0)}};

    Shape cube = NewCube(NewPnt3(0, 0, 0), 1.0);

    for (int i = 0; i < 11; i++)
    {
        Tuple3 test_pnt = tests[i][0];
        Tuple3 expected = tests[i][1];
        Tuple3 result = NormalAt(&cube, test_pnt);

        TEST(TupleFuzzyEqual(expected, result), "Cube normal test");
        if (!TupleFuzzyEqual(expected, result))
        {
            PrintTuple(result);
        }
    }
}

void TestTree()
{
    Tree parent;
    ConstructTree(&parent);

    Shape sphere = NewSphere(NewPnt3(0, 0, -3), 2.0);
    AddShapeToTree(&parent, &sphere);

    Tree child;
    ConstructTree(&child);

    Shape plane = NewPlane(NewPnt3(0, 0, 0), NewVec3(0, 1, 0));
    AddShapeToTree(&child, &plane);

    Shape cube = NewCube(NewPnt3(0, -5, 0), 4.0);
    AddShapeToTree(&child, &cube);

    CopyInChild(&parent, &child);
    DeconstructTree(&child);
    // Make sure we can get rid of the original child before referencing the tree's copy

    Node *child_ptr = Index(&parent.start.children, 0);
    Set *child_shapes = &child_ptr->shapes;

    TEST(child_shapes->length == 2, "Tree test, number of child shapes");

    Shape *res_plane = Index(child_shapes, 0);
    TEST(res_plane->type == plane.type, "Tree test, shape 1 type");
    TEST(MatrixEqual(res_plane->transformation, plane.transformation), "Tree test, shape 1 transform");

    Shape *res_cube = Index(child_shapes, 1);
    TEST(res_cube->type == cube.type, "Tree test, shape 2 type");
    TEST(MatrixEqual(res_cube->transformation, cube.transformation), "Tree test, shape 2 transform");

    Shape *res_sphere = Index(&parent.start.shapes, 0);
    TEST(res_sphere->type == sphere.type, "Tree test, shape 3 type");
    TEST(MatrixEqual(res_sphere->transformation, sphere.transformation), "Tree test, shape 3 transform");

    Matrix4x4 test_transform = TranslationMatrix(1, 2, 3);
    PropagateTransform(&parent, test_transform);

    TEST(MatrixFuzzyEqual(res_plane->transformation, test_transform), "Tree test, transformation propagation");

    Material new_material = NewMaterial(NewColor(255, 123, 221, 255));
    PropagateMaterial(&parent, new_material);

    TEST(TupleEqual(new_material.pattern.color_a, res_cube->material.pattern.color_a), "Tree test, material propagation");
    TEST(TupleEqual(new_material.pattern.color_b, res_cube->material.pattern.color_b), "Tree test, material propagation");

    DeconstructTree(&parent);
}

void TestBounds()
{
    Shape sphere = NewSphere(NewPnt3(0, 0, 0), 2.0);
    Bounds sphere_bounds = ShapeBounds(&sphere);

    Tuple3 sphere_exp_min = NewPnt3(-2, -2, -2);
    Tuple3 sphere_exp_max = NewPnt3(2, 2, 2);

    TEST(TupleFuzzyEqual(sphere_bounds.maximum_bound, sphere_exp_max) &&
             TupleFuzzyEqual(sphere_bounds.minimum_bound, sphere_exp_min),
         "Sphere bounding box");

    Shape cube = NewCube(NewPnt3(-3, -2, -1), 3.0);
    Bounds cube_bounds = ShapeBounds(&cube);

    Tuple3 cube_exp_min = NewPnt3(-4.5, -3.5, -2.5);
    Tuple3 cube_exp_max = NewPnt3(-1.5, -0.5, 0.5);

    TEST(TupleFuzzyEqual(cube_bounds.maximum_bound, cube_exp_max) &&
             TupleFuzzyEqual(cube_bounds.minimum_bound, cube_exp_min),
         "Cube bounding box");

    Tree t;
    ConstructTree(&t);

    AddShapeToTree(&t, &cube);
    AddShapeToTree(&t, &sphere);
    CalculateBounds(&t);
    Bounds b = t.start.bounds;

    TEST(TupleFuzzyEqual(b.minimum_bound, cube_exp_min) &&
             TupleFuzzyEqual(b.maximum_bound, sphere_exp_max),
         "Tree bounding box");

    DeconstructTree(&t);

    Bounds b2 = {
        .minimum_bound = NewPnt3(-1, -1, -1),
        .maximum_bound = NewPnt3(1, 1, 1)};

    Matrix4x4 bt = MatrixMultiply(RotationXMatrix(M_PI / 4.0), RotationYMatrix(M_PI / 4.0));
    Bounds result = TransformBounds(b2, bt);

    Tuple3 exp_min = NewPnt3(-1.414214, -1.707107, -1.707107);
    Tuple3 exp_max = NewPnt3(1.414214, 1.707107, 1.707107);

    TEST(TupleFuzzyEqual(exp_min, result.minimum_bound) &&
             TupleFuzzyEqual(exp_max, result.maximum_bound),
         "formation");
}

void DemoUnthreaded()
{
    Scene s;
    ReadScene(&s, "./scenes/three_spheres.json");

    Canvas canvas;
    ConstructCanvas(&canvas, s.camera.width, s.camera.height);

    RenderSceneUnthreaded(&s, &canvas);
    WriteToPPM(&canvas, "./renderings/three_spheres.ppm");

    DeconstructCanvas(&canvas);
    DeconstructScene(&s);
}

void TestTupleHasNans()
{
    Tuple3 t1 = NewTuple3(NAN, 0, 0, 0);
    bool r1 = TupleHasNaNs(t1);
    TEST(r1, "Has nans, single nan")

    Tuple3 t2 = NewTuple3(-NAN, 0, 0, 0);
    bool r2 = TupleHasNaNs(t2);
    TEST(r2, "Has nans, single negative nan")

    Tuple3 t3 = NewTuple3(NAN, NAN, NAN, NAN);
    bool r3 = TupleHasNaNs(t3);
    TEST(r3, "Has nans, all nans")

    Tuple3 t4 = NewTuple3(1, 1, 1, 1);
    bool r4 = TupleHasNaNs(t4);
    TEST(!r4, "Has nans, no nans")
}

void TestTupleHasInf()
{
    Tuple3 t1 = NewTuple3(INFINITY, 0, 0, 0);
    bool r1 = TupleHasInf(t1);
    TEST(r1, "Has infinties, single infinity")

    Tuple3 t2 = NewTuple3(-INFINITY, 0, 0, 0);
    bool r2 = TupleHasInf(t2);
    TEST(r2, "Has inf, single negative infinity")

    Tuple3 t3 = NewTuple3(INFINITY, INFINITY, INFINITY, INFINITY);
    bool r3 = TupleHasInf(t3);
    TEST(r3, "Has inf, only infinities")

    Tuple3 t4 = NewTuple3(1, 1, 1, 1);
    bool r4 = TupleHasInf(t4);
    TEST(!r4, "Has inf, no infinities")
}

void TestPlaneScene()
{
    Scene s;
    ConstructScene(&s, NewCamera(55, 55, 55), NewLight(NewPnt3(16, 6, 6)));

    Shape plane = NewPlane(NewPnt3(0, 0, 0), NewVec3(0, 1, 0));
    AddShape(&s, plane);

    Shape sphere = NewSphere(NewPnt3(0, 0, 0), 1.0);
    AddShape(&s, sphere);

    CalculateBounds(&s.shapes);

    Set intersections;
    ConstructSet(&intersections, sizeof(Intersection));

    Ray r = NewRay(NewPnt3(0, 1, 0), NewVec3(0, -1, 0));
    IntersectScene(&s, r, &intersections);

    Intersection *result = Index(&intersections, 1);
    TEST(FloatEquality(1.0, result->ray_times[0]), "Scene Intersection for planes");
}

void TestTriangle() {
    //Triangle Normal tests
    Shape triangle = NewTriangle(NewPnt3(0, 1, 0), NewPnt3(-1, 0, 0), NewPnt3(1, 0, 0));

    TEST(TupleFuzzyEqual(NormalAt(&triangle, NewPnt3(0, 0, 0)), NewVec3(0, 0, -1)), "Triangle Normal 1");
    TEST(TupleFuzzyEqual(NormalAt(&triangle, NewPnt3(0, 0.5, 0)), NewVec3(0, 0, -1)), "Triangle Normal 2");
    TEST(TupleFuzzyEqual(NormalAt(&triangle, NewPnt3(-0.5, 0.75, 0)), NewVec3(0, 0, -1)), "Triangle Normal 3");
    TEST(TupleFuzzyEqual(NormalAt(&triangle, NewPnt3(0.5, 0.25, 0)), NewVec3(0, 0, -1)), "Triangle Normal 4");

    //Triangle intersection tests
    Ray r0 = NewRay(NewPnt3(0, -1, -2), NewVec3(0, 1, 0));
    Intersection i0 = Intersect(&triangle, r0);
    TEST(i0.count == 0, "Triangle intersection, ray misses");

    Ray r1 = NewRay(NewPnt3(1, 1, -2), NewVec3(0, 0, 1));
    Intersection i1 = Intersect(&triangle, r1);
    TEST(i1.count == 0, "Triangle intersection, ray misses p1-p3 edge");

    Ray r2 = NewRay(NewPnt3(-1, 1, -2), NewVec3(0, 0, 1));
    Intersection i2 = Intersect(&triangle, r2);
    TEST(i2.count == 0, "Triangle intersection, ray misses p1-p2 edge");

    Ray r3 = NewRay(NewPnt3(0, -1, -2), NewVec3(0, 0, 1));
    Intersection i3 = Intersect(&triangle, r3);
    TEST(i3.count == 0, "Triangle intersection, ray misses p2-p3 edge");

    Ray r4 = NewRay(NewPnt3(0, 0.5, -2), NewVec3(0, 0, 1));
    Intersection i4 = Intersect(&triangle, r4);
    TEST(i4.count == 1 && FloatEquality(i4.ray_times[0], 2.0), "Triangle intersection, ray strikes triangle");
}

void TestReadObj() { 
    Light l = NewLight(NewPnt3(-10, 10, -10));
    Camera c = NewCamera(3840, 2160, 1.047);
    CameraApplyTransformation(&c, ViewMatrix(NewPnt3(0, 1.5, -5), NewPnt3(0, 0, 0), NewVec3(0, 1, 0)));

    Scene s;
    ConstructScene(&s, c, l);

    ReadObj(&s, "scenes/teapot.obj");
    TEST(s.shapes.start.shapes.length == 6320, "Read object file, number of faces");

    DeconstructScene(&s);
}

int DoTests()
{
    num_failed = 0;
    num_passed = 0;

    TestScalarSubtract();
    TestScalarAdd();
    TestCrossProduct();
    TestVectorReflect();

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
    TestStripePattern();
    TestRefraction();
    TestCalculateRefraction();
    TestCubeIntersection();
    TestCubeNormal();

    TestTree();
    TestBounds();
    TestPlaneScene();

    TestTupleHasNans();
    TestTupleHasInf();

    TestTriangle();
    TestReadObj();

    printf("Test(s) Passed: %d\nTests(s) Failed: %d\n", num_passed, num_failed);

    return 0;
}

int main()
{
    DoTests();
    return 0;
}
