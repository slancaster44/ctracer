#include "scene.h"
#include "intersection.h"
#include "shading.h"

#include <string.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <unistd.h>

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

void RenderSceneSection(Scene* s, 
    Canvas* c,
    unsigned start, unsigned end,
    unsigned canvas_width) 
{
    for (unsigned i = start; i < end; i++) {
        unsigned x = i % canvas_width;
        unsigned y = (i - x) / canvas_width;

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


void RenderScene(Scene* s, Canvas* c) {
    unsigned canvas_size = (c->canvas_height * c->canvas_width);
    unsigned chunk_size = canvas_size / get_nprocs();
    unsigned num_chunks = canvas_size / chunk_size;
    unsigned leftovers = canvas_size % chunk_size;

    Set pids;
    ConstructSet(&pids, sizeof(int));

    for (unsigned cur_chunk = 0; cur_chunk < num_chunks; cur_chunk++) {
        unsigned cur_chunk_start = cur_chunk * chunk_size;
        unsigned cur_chunk_end = cur_chunk_start + chunk_size;

        int pid = fork();
        if (pid == 0) { //Child renders its section
            RenderSceneSection(s, c, cur_chunk_start, cur_chunk_end, c->canvas_width);
            exit(0);
        }

        AppendValue(&pids, &pid);
    }

    for (unsigned pid_index = 0; pid_index < pids.length; pid_index++) {
        int cur_pid;
        CopyOut(&pids, pid_index, &cur_pid);

        int exit_status;
        waitpid(cur_pid, &exit_status, WUNTRACED);
    }

    //TODO Leftovers
}