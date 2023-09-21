#include "tuple.h"
#include "canvas.h"
#include "scene.h"
#include "intersection.h"
#include "shading.h"

#include <math.h>

void AssignDefaultTestMaterial(Material* m) {
    m->color = NewColor(255, 255, 255, 255);
    m->ambient_reflection = 0.1f;
    m->diffuse_reflection = 0.9f;
    m->specular_reflection = 0.9f;
    m->shininess = 200;
}

void DemoCanvas() {
    Tuple3 start = NewPnt3(0, 1, 0);
    Tuple3 velocity = TupleScalarMultiply(TupleNormalize(NewVec3(1, 1.8f, 0)), 11.25f);

    Tuple3 gravity = NewVec3(0, -0.1, 0);

    Canvas c;
    ConstructCanvas(&c, 900, 550);

    while (start[1] > 0) {
        WritePixel(&c, NewColor(255, 255, 255, 255), (int) start[0], (int) start[1], 0);
        
        start = TupleAdd(start, velocity);
        velocity = TupleAdd(velocity, gravity);
    }

    WriteToPPM(&c, "canvas_test.ppm");
    DeconstructCanvas(&c);
}

void DemoShadeSphere() {
    Material m;
    AssignDefaultTestMaterial(&m);
    m.color = NewColor(0, 100, 200, 255);

    Light l;
    l.origin = NewPnt3(-200, -200, -400);
    l.color = NewColor(255, 255, 255, 255);
    
    Canvas c;
    ConstructCanvas(&c, 800, 600);

    Shape s = NewSphere(NewPnt3(0, 0, 0), 250); 
    s.material = m;

    Ray r;
    r.direction = NewVec3(0, 0, 1);

    for (int x = 0; x < 800; x++) {
        for (int y = 0; y < 600; y++) {
            r.origin = NewPnt3(x-400, y - 300, -200);
            Intersection i = Intersect(&s, r);

            if (i.count > 0) {

                Tuple3 pos = RayPosition(r, i.ray_times[0]);
                Tuple3 norm = NormalAt(&s, pos);
                Tuple3 eyev = TupleNegate(r.direction);

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

    WriteToPPM(&c, "sphere.ppm");
    DeconstructCanvas(&c);
}

void DemoSphereScene() {
    Material m;
    AssignDefaultTestMaterial(&m);
    m.color = NewColor(255, 0, 128, 255);

    Light l;
    l.origin = NewPnt3(1000, 2000, -2000);
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
    DemoCanvas();
    DemoShadeSphere();
    DemoSphereScene();
}