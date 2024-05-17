#include "tuple.h"
#include "canvas.h"
#include "scene.h"
#include "material.h"
#include "shape.h"
#include "intersection.h"

#include <math.h>
#include <time.h>
#include <stdlib.h>

void AssignDefaultTestMaterial(Material* m) {
    m->pattern = NewSolidPattern(NewColor(255, 255, 255, 255));
    m->ambient_reflection = 0.1;
    m->diffuse_reflection = 0.9;
    m->specular_reflection = 0.9;
    m->shininess = 200;
}

void DemoCanvas() {
    Tuple3 start = NewPnt3(0, 1, 0);
    Tuple3 velocity = TupleScalarMultiply(TupleNormalize(NewVec3(1, 1.8, 0)), 11.25);

    Tuple3 gravity = NewVec3(0, -0.1, 0);

    Canvas c;
    ConstructCanvas(&c, 900, 550);

    while (start[1] > 0) {
        WritePixel(&c, NewColor(255, 255, 255, 255), (int) start[0], (int) start[1]);
        
        start = TupleAdd(start, velocity);
        velocity = TupleAdd(velocity, gravity);
    }

    WriteToPPM(&c, "./renderings/canvas_test.ppm");
    DeconstructCanvas(&c);
}

void DemoSphereScene() {
    Material m;
    AssignDefaultTestMaterial(&m);
    m.pattern = NewSolidPattern(NewColor(255, 0, 128, 255));

    Light l;
    l.origin = NewPnt3(1000, 2000, -2000);
    l.color = NewColor(255, 255, 255, 255);
    
    Canvas c;
    ConstructCanvas(&c, 800, 600);

    Shape s = NewSphere(NewPnt3(0, 0, 2), 500); 
    s.material = m;

    Shape s2 = NewSphere(NewPnt3(1, 1, -5), 50);
    m.pattern = NewSolidPattern(NewColor(0, 255, 128, 255));

    s2.material = m;

    Camera ca = NewCamera(800, 600, 3.1415 / 3);
    CameraApplyTransformation(&ca, ViewMatrix(NewPnt3(0, 0, -600), NewPnt3(0, 0, 0), NewVec3(0, -1, 0)));

    Scene sc;
    ConstructScene(&sc, ca, l);
    AddShape(&sc, s);
    AddShape(&sc, s2);

    RenderScene(&sc, &c);

    WriteToPPM(&c, "./renderings/sphere_scene.ppm");

    DeconstructScene(&sc);
    DeconstructCanvas(&c);
}

void DemoPlane() {
    Camera c = NewCamera(800, 600, 3.1415 / 3);
    CameraApplyTransformation(&c, ViewMatrix(NewPnt3(0, 100, 250), NewPnt3(0, 0, 0), NewVec3(0, 1, 0)));

    Light l = NewLight( NewPnt3(750, 1200, 2000));

    Scene s;
    ConstructScene(&s, c, l);
   
    Shape sphere2 = NewSphere(NewPnt3(-0.5, 40, 100), 50);
    AssignDefaultTestMaterial(&(sphere2.material));
    sphere2.material.pattern = NewSolidPattern(NewColor(0, 0, 255, 255));
    sphere2.material.general_reflection = 0.5;
    AddShape(&s, sphere2);

    Shape sphere = NewSphere(NewPnt3(0, 100, 175), 20);
    AssignDefaultTestMaterial(&(sphere.material));
    sphere.material.pattern = NewSolidPattern(NewColor(0, 255, 128, 255));
    AddShape(&s, sphere);

    Shape plane = NewPlane(NewPnt3(0, -10, 0), NewVec3(0, 1, 0));
    AssignDefaultTestMaterial(&(plane.material));
    AddShape(&s, plane);


    Canvas canvas;
    ConstructCanvas(&canvas, 800, 600);

    RenderScene(&s, &canvas);
    WriteToPPM(&canvas, "./renderings/plane_scene.ppm");

    DeconstructCanvas(&canvas);
    DeconstructScene(&s);
}

void DemoJsonScene() {
    Scene s;
    ReadScene(&s, "./scenes/three_spheres.json");

    Canvas canvas;
    ConstructCanvas(&canvas, s.camera.width, s.camera.height);

    RenderScene(&s, &canvas);
    WriteToPPM(&canvas, "./renderings/three_spheres.ppm");

    DeconstructCanvas(&canvas);
    DeconstructScene(&s);
}

void DemoUnthreaded() {
    Scene s;
    ReadScene(&s, "./scenes/three_spheres.json");

    Canvas canvas;
    ConstructCanvas(&canvas, s.camera.width, s.camera.height);

    RenderSceneUnthreaded(&s, &canvas);
    WriteToPPM(&canvas, "./renderings/three_spheres.ppm");

    DeconstructCanvas(&canvas);
    DeconstructScene(&s);
}

#define NUM_RAND_SHAPES 75
void BusyScene() {
    Light l = NewLight( NewPnt3(750, 1200, 2000));
    Camera c = NewCamera(800, 600, 3.1415 / 3);
    CameraApplyTransformation(&c, ViewMatrix(NewPnt3(0, 100, 250), NewPnt3(0, 0, 0), NewVec3(0, 1, 0)));

    Scene s;
    ConstructScene(&s, c, l);

    double i = 4.0;

    for (int j = 0; j < NUM_RAND_SHAPES; j++) {
        i = ((((double) i * -1.0) * 0.66 + j) / 1.0);
        i = fmodf(i, 20);

        while (i == 0 || fmod(i * i, i) == 0.0) { i += 0.87; }

        double x = i * i;
        double y = fmod(x, i);
        double z = pow(i, y);

        Shape sphere = NewSphere(NewPnt3(x, y, z), i);
        sphere.material.pattern.color_a = NewTuple3(i * 1, i * 2, i * 3, 1.0);
        sphere.material.general_reflection = 1.34;
        AddShape(&s, sphere);
    }

    Canvas canvas;
    ConstructCanvas(&canvas, 800, 600);

    RenderScene(&s, &canvas);
    WriteToPPM(&canvas, "./renderings/random_spheres.ppm");

    DeconstructCanvas(&canvas);
    DeconstructScene(&s);
}

int main() {
    //DemoCanvas();
    //DemoJsonScene();
    //DemoPlane();
    //DemoSphereScene();
    //DemoUnthreaded();

    BusyScene();
}