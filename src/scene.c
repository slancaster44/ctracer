#include "scene.h"
#include "intersection.h"
#include "shading.h"

#include <string.h>

void ConstructScene(Scene* s, Camera c, Light l) {
    s->camera = c;
    s->light = l;
    ConstructSet(&(s->shapes), sizeof(Shape));
}

void DeconstructScene(Scene* s) {
    DeconstructSet(&(s->shapes));
}

unsigned AddShape(Scene* s, Shape sp) {
    return AppendValue(&(s->shapes), &sp);
}

void SetSceneCamera(Scene* s, Camera c) {
    s->camera = c;
}

void IntersectScene(Scene* s, Ray r, Set* intersection_set) {
    for (int i = 0; i < s->shapes.length; i++) {
        Shape* this_shape = Index(&s->shapes, i);
        Intersection i = Intersect(this_shape, r);
        
        if (i.count > 0) {
            AppendValue(intersection_set, &i);
        }
    }
}

void RenderScene(Scene* s, Canvas* c) {
    for (int x = 0; x < c->canvas_width; x++) {
        for (int y = 0; y < c->canvas_height; y++) {
            Ray r = RayForPixel(&s->camera, x, y);

            Set intersections;
            ConstructSet(&intersections, sizeof(Intersection));
            IntersectScene(s, r, &intersections); 

            for (int i = 0; i < intersections.length; i++) {
                Intersection* this_intersection = Index(&intersections, i);
                if (this_intersection->ray_times[0] < 0) {
                    continue;
                }
                
                Tuple3 pos = RayPosition(this_intersection->ray, this_intersection->ray_times[0]);
                Tuple3 eyev = TupleNegate(r.direction);
                Tuple3 normal = NormalAt(this_intersection->shape_ptr, pos);
                
                if (TupleDotProduct(normal, eyev) < 0) {
                    normal = TupleNegate(normal);
                }

                Tuple3 color = PhongShading(this_intersection->shape_ptr->material, s->light, pos, eyev, normal);
                
                float depth = TupleMagnitude(TupleSubtract(r.origin, pos));
                WritePixel(c, color, x, y, depth);
            }

            DeconstructSet(&intersections);
        }
    }
}