#include "scene.h"
#include "intersection.h"
#include "equality.h"
#include "material.h"
#include "shape.h"

#include <string.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <unistd.h>
#include <float.h>

void ConstructScene(Scene *s, Camera c, Light l)
{
    s->camera = c;
    s->light = l;
    ConstructTree(&(s->shapes));
}

void DeconstructScene(Scene *s)
{
    DeconstructTree(&s->shapes);
}

void AddShape(Scene *s, Shape sp)
{
    AddShapeToTree(&s->shapes, &sp);
}

void SetSceneCamera(Scene *s, Camera c)
{
    s->camera = c;
}

void ReplaceTree(Scene *s, Tree *t)
{
    ReconstructTree(&s->shapes);
    CloneTree(&s->shapes, t);
    CalculateBounds(&s->shapes);
}

bool IsInShadow(Scene *s, Tuple3 location)
{
    Tuple3 pnt_light_vec = TupleSubtract(s->light.origin, location);

    double distance = TupleMagnitude(pnt_light_vec);
    Tuple3 direction = TupleNormalize(pnt_light_vec);

    Ray ray = NewRay(location, direction);

    Set intersections;
    ConstructSet(&intersections, sizeof(Intersection));
    IntersectScene(s, ray, &intersections);

    for (unsigned i = 0; i < intersections.length; i++)
    {
        Intersection *this_intersection = Index(&intersections, i);
        if (this_intersection->ray_times[0] < distance && this_intersection->ray_times[0] > 0)
        {
            DeconstructSet(&intersections);
            return true;
        }
    }

    DeconstructSet(&intersections);
    return false;
}

void IntersectScene(Scene *s, Ray r, Set *intersection_set)
{
    IntersectTree(&s->shapes, r, intersection_set);
}

void RenderSceneSection(
    Scene *s,
    Canvas *c,
    unsigned start, unsigned end,
    unsigned canvas_width)
{
    for (unsigned i = start; i < end; i++)
    {
        unsigned x = i % canvas_width;
        unsigned y = (i - x) / canvas_width;

        Ray r = RayForPixel(&s->camera, x, y);
        Tuple3 color = ColorFor(s, r);
        DirectWritePixel(c, color, i);
    }
}

Tuple3 ColorFor(Scene *s, Ray r)
{
    return ColorForLimited(s, r, 8);
}

Tuple3 ColorForLimited(Scene *s, Ray r, int limit)
{

    Set intersections;
    ConstructSet(&intersections, sizeof(Intersection));
    IntersectScene(s, r, &intersections);

    double curDepth = FLT_MAX;
    Tuple3 curColor = NewColor(0, 0, 0, 0);

    for (unsigned j = 0; j < intersections.length; j++)
    {

        Intersection *this_intersection = Index(&intersections, j);
        if (this_intersection->ray_times[0] < 0)
        {
            continue;
        }

        Tuple3 pos = RayPosition(this_intersection->ray, this_intersection->ray_times[0]);
        double depth = TupleMagnitude(TupleSubtract(r.origin, pos));
        if (depth > curDepth)
        {
            continue;
        }

        curDepth = depth;
        curColor = this_intersection->shape_ptr->material.shader(s, &intersections, j, limit);
    }

    DeconstructSet(&intersections);
    return curColor;
}

void GenerateSceneBVH(Scene *s) {
    Tree bvh;
    ConstructTree(&bvh);

    GenerateBVH(&bvh, &s->shapes);
    ReplaceTree(s, &bvh);
    
    DeconstructTree(&bvh);
}

void RenderScene(Scene *s, Canvas *c)
{
    //GenerateSceneBVH(s); //Test scene with 75 spheres; No BVH: 48 seconds. With BVH: 8 seconds
    CalculateBounds(&s->shapes);

    unsigned canvas_size = (c->canvas_height * c->canvas_width);
    unsigned chunk_size = canvas_size / (unsigned)get_nprocs();
    unsigned num_chunks = canvas_size / chunk_size;

    Set pids;
    ConstructSet(&pids, sizeof(int));

    for (unsigned cur_chunk = 0; cur_chunk < num_chunks; cur_chunk++)
    {
        unsigned cur_chunk_start = cur_chunk * chunk_size;
        unsigned cur_chunk_end = cur_chunk_start + chunk_size;

        int pid = fork();
        if (pid == 0)
        { // Child renders its section
            RenderSceneSection(s, c, cur_chunk_start, cur_chunk_end, c->canvas_width);
            exit(0);
        }
        else
        {
            AppendValue(&pids, &pid); // Parent keeps track of child
        }
    }

    for (unsigned pid_index = 0; pid_index < pids.length; pid_index++)
    {
        int cur_pid;
        CopyOut(&pids, pid_index, &cur_pid);

        int exit_status;
        waitpid(cur_pid, &exit_status, WUNTRACED);
    }

    unsigned leftovers_start = num_chunks * chunk_size;
    if (leftovers_start != canvas_size)
    {
        RenderSceneSection(s, c, leftovers_start, canvas_size + 1, c->canvas_width);
    }
}

void RenderSceneUnthreaded(Scene *s, Canvas *c)
{
    GenerateSceneBVH(s);
    CalculateBounds(&s->shapes);
    RenderSceneSection(s, c, 0, c->canvas_height * c->canvas_width - 1, c->canvas_width);
}
