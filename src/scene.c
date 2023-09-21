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

bool IsInShadow(Scene *s, Tuple3 location) {
    Tuple3 pnt_light_vec = TupleSubtract(s->light.origin, location);

    float distance = TupleMagnitude(pnt_light_vec);
    Tuple3 direction = TupleNormalize(pnt_light_vec);

    Ray ray = NewRay(location, direction);

    Set intersections;
    ConstructSet(&intersections, sizeof(Intersection)); //TODO: Cleanup memory, deoncstruct
    IntersectScene(s, ray, &intersections);

    for (size_t i = 0; i < intersections.length; i++){
        Intersection* this_intersection = Index(&intersections, i);
        if (this_intersection->ray_times[0] < distance && this_intersection->ray_times[0] > 0) {
            return true;
        }

    }

    return false;
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

        for (int j = 0; j < intersections.length; j++) {
            Intersection* this_intersection = Index(&intersections, j);
            if (this_intersection->ray_times[0] < 0) {
                continue;
            }
            
            Tuple3 pos = RayPosition(this_intersection->ray, this_intersection->ray_times[0]);
            Tuple3 eyev = TupleNegate(r.direction);
            Tuple3 normal = NormalAt(this_intersection->shape_ptr, pos);
            
            if (TupleDotProduct(normal, eyev) < 0) {
                normal = TupleNegate(normal);
            }

            ShadingJob lj = {
                .material = this_intersection->shape_ptr->material,
                .light = s->light,
                .position = pos,
                .eye_vector = eyev,
                .surface_normal = normal,
            	.shadow = IsInShadow(s, pos),
	    };

            Tuple3 color = PhongShading(lj);
            float depth = TupleMagnitude(TupleSubtract(r.origin, pos));

            DirectWritePixel(c, color, i, depth);
        }

        DeconstructSet(&intersections);
    }
}


void RenderScene(Scene* s, Canvas* c) {
    unsigned canvas_size = (c->canvas_height * c->canvas_width);
    unsigned chunk_size = canvas_size / get_nprocs();
    unsigned num_chunks = canvas_size / chunk_size;

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

    unsigned leftovers_start = num_chunks * chunk_size;
    if (leftovers_start != canvas_size) {
        RenderSceneSection(s, c, leftovers_start, canvas_size + 1, c->canvas_width);
    }
}
