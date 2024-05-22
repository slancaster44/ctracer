#include "tree.h"
#include "intersection.h"
#include "shape.h"
#include "bounds.h"
#include <string.h>
#include <math.h>

void ConstructTree(Tree *tree)
{
    tree->start.parent = NULL;
    ConstructSet(&tree->start.shapes, sizeof(Shape));
    ConstructSet(&tree->start.children, sizeof(Node));
}

void ReconstructTree(Tree *tree)
{
    DeconstructTree(tree);
    ConstructTree(tree);
}

void DeconstructNode(Node *n)
{
    n->parent = NULL;

    for (unsigned i = 0; i < n->children.length; i++)
    {
        Node *child = Index(&n->children, i);
        DeconstructNode(child);
    }

    DeconstructSet(&n->children);
    DeconstructSet(&n->shapes);
}

void DeconstructTree(Tree *tree)
{
    DeconstructNode(&tree->start);
}

void PrintNode(Node *n, const char *prefix)
{
    printf("%s|- Node with '%ld' shape(s) and '%ld' children\n", prefix, n->shapes.length, n->children.length);

    double x1 = n->bounds.minimum_bound[0];
    double y1 = n->bounds.minimum_bound[1];
    double z1 = n->bounds.minimum_bound[2];

    double x2 = n->bounds.maximum_bound[0];
    double y2 = n->bounds.maximum_bound[1];
    double z2 = n->bounds.maximum_bound[2];

    printf("%s\\- Bounds from [%f %f %f] to [%f %f %f]\n", prefix, x1, y1, z1, x2, y2, z2);

    size_t prefix_size = strlen(prefix) + 3; // +1 for null terminator, +1 for new tab
    char *new_prefix = alloca(prefix_size);
    memcpy(new_prefix, prefix, prefix_size);
    new_prefix[prefix_size - 3] = '|';
    new_prefix[prefix_size - 2] = '\t';
    new_prefix[prefix_size - 1] = '\0';

    for (unsigned i = 0; i < n->children.length; i++)
    {
        Node *child = Index(&n->children, i);
        PrintNode(child, new_prefix);
    }
}

void PrintTree(Tree *t)
{
    PrintNode(&t->start, "");
}

void CloneNode(Node *destination, Node *source)
{
    memset(destination, 0, sizeof(Node));
    CloneSet(&destination->shapes, &source->shapes);

    if (destination->children.data == NULL)
    {
        ConstructSet(&destination->children, sizeof(Node));
    }

    for (unsigned i = 0; i < source->children.length; i++)
    {
        Node new_child;

        CloneNode(&new_child, Index(&source->children, i));
        new_child.parent = destination;

        AppendValue(&destination->children, &new_child);
    }
}

void CloneTree(Tree *destination, Tree *source)
{
    CloneNode(&destination->start, &source->start);
}

void CopyInChild(Tree *parent, Tree *child)
{
    unsigned long idx = AppendValue(&parent->start.children, &child->start);

    Node *child_ptr = Index(&parent->start.children, idx);
    CloneNode(child_ptr, &child->start);
    child_ptr->parent = &parent->start;
}

void AddShapeToTree(Tree *tree, Shape *shape)
{
    AppendValue(&tree->start.shapes, shape);
}

void PropagateTransformOnNode(Node *node, Matrix4x4 transform)
{
    for (unsigned long i = 0; i < node->shapes.length; i++)
    {
        Shape *shape_ptr = Index(&node->shapes, i);
        ApplyTransformation(shape_ptr, transform);
    }

    for (unsigned long i = 0; i < node->children.length; i++)
    {
        Node *child = Index(&node->children, i);
        PropagateTransformOnNode(child, transform);
    }
}

void PropagateTransform(Tree *tree, Matrix4x4 transform)
{
    PropagateTransformOnNode(&tree->start, transform);
}

void PropagateMaterialOnNode(Node *node, Material mat)
{
    for (unsigned long i = 0; i < node->shapes.length; i++)
    {
        Shape *shape_ptr = Index(&node->shapes, i);
        shape_ptr->material = mat;
    }

    for (unsigned long i = 0; i < node->children.length; i++)
    {
        Node *child = Index(&node->children, i);
        PropagateMaterialOnNode(child, mat);
    }
}

void PropagateMaterial(Tree *tree, Material material)
{
    PropagateMaterialOnNode(&tree->start, material);
}

void IntersectNode(Node *n, Ray r, Set *intersections)
{
    for (unsigned i = 0; i < n->children.length; i++)
    {
        Node *child = Index(&n->children, i);
        IntersectNode(child, r, intersections);
    }

    if (!IsInBounds(n->bounds, r))
    {
        return;
    }

    for (unsigned i = 0; i < n->shapes.length; i++)
    {
        Shape *this_shape = Index(&n->shapes, i);
        Intersection intersection = Intersect(this_shape, r);

        if (intersection.count != 0)
        {
            AppendValue(intersections, &intersection);
        }
    }
}

void IntersectTree(Tree *tree, Ray r, Set *intersections)
{
    IntersectNode(&tree->start, r, intersections);
}

Bounds SetBounds(Set *s)
{
    Tuple3 min = NewPnt3(INFINITY, INFINITY, INFINITY);
    Tuple3 max = NewPnt3(-INFINITY, -INFINITY, -INFINITY);

    for (unsigned i = 0; i < s->length; i++)
    {
        Shape *this_shape = Index(s, i);
        Bounds bounds = ShapeBounds(this_shape);

        min = _mm256_min_pd(bounds.minimum_bound, min);
        max = _mm256_max_pd(bounds.maximum_bound, max);
    }

    Bounds out = {
        .minimum_bound = min,
        .maximum_bound = max,
    };

    GenerateBoundingCube(&out);
    return out;
}

void TreeNodeBounds(Node *n)
{
    Bounds out = SetBounds(&n->shapes);

    for (unsigned i = 0; i < n->children.length; i++)
    {
        Node *child = Index(&n->children, i);
        TreeNodeBounds(child);

        out.minimum_bound = _mm256_min_pd(child->bounds.minimum_bound, out.minimum_bound);
        out.maximum_bound = _mm256_max_pd(child->bounds.maximum_bound, out.maximum_bound);
    }

    GenerateBoundingCube(&out);
    n->bounds = out;
}

void CalculateBounds(Tree *tree)
{
    TreeNodeBounds(&tree->start);
}

unsigned NumberOfParents(Node *n)
{
    unsigned count = 0;

    while (n != NULL)
    {
        count++;
        n = n->parent;
    }

    return count;
}

void GetShapeSets(Set *bound, Set *unbound, Node *src)
{
    for (unsigned i = 0; i < src->shapes.length; i++)
    {

        Shape *this_shape = Index(&src->shapes, i);
        Bounds b = ShapeBounds(this_shape);

        if (TupleHasInfOrNans(b.maximum_bound) || TupleHasInfOrNans(b.minimum_bound))
        {
            AppendValue(unbound, this_shape);
        }
        else
        {
            AppendValue(bound, this_shape);
        }
    }

    for (unsigned i = 0; i < src->children.length; i++)
    {
        GetShapeSets(bound, unbound, Index(&src->children, i));
    }
}

void SortByXCoord(Set *shapes, unsigned long lo, unsigned long hi)
{
    if (lo >= hi || lo < 0)
    {
        return;
    }

    Shape *pivot_shape = Index(shapes, lo);
    double pivot_x_coord = MatrixTupleMultiply(pivot_shape->inverse_transform, NewPnt3(0, 0, 0))[0];

    unsigned long i = lo;
    for (unsigned long j = lo; j < hi; j++)
    {
        Shape *this_shape = Index(shapes, j);
        Tuple3 this_center_point = MatrixTupleMultiply(this_shape->inverse_transform, NewPnt3(0, 0, 0));
        if (this_center_point[0] <= pivot_x_coord)
        {
            SwapElements(shapes, i, j);
            i++;
        }
    }

    SwapElements(shapes, i, hi);
    SortByXCoord(shapes, lo, i - 1);
    SortByXCoord(shapes, i + 1, hi);
}

#define SHAPES_PER_CHILD 4
void GenerateBVH(Tree *dst, Tree *src)
{
    Set bound_shapes;
    ConstructSet(&bound_shapes, sizeof(Shape));

    GetShapeSets(&bound_shapes, &dst->start.shapes, &src->start);
    SortByXCoord(&bound_shapes, 0, bound_shapes.length);

    unsigned long num_groups = bound_shapes.length / SHAPES_PER_CHILD;
    for (unsigned long i = 0; i < num_groups; i++)
    {
        Tree child;
        ConstructTree(&child);

        for (unsigned long j = 0; j < SHAPES_PER_CHILD; j++)
        {
            AddShapeToTree(&child, Index(&bound_shapes, (i * SHAPES_PER_CHILD) + j));
        }

        CopyInChild(dst, &child);
    }

    Tree final_child;
    ConstructTree(&final_child);

    unsigned long num_leftovers = num_groups == 0 ? bound_shapes.length : bound_shapes.length % num_groups;
    for (unsigned long j = 0; j < num_leftovers; j++)
    {
        unsigned long idx = (num_groups * SHAPES_PER_CHILD) + j;
        AddShapeToTree(&final_child, Index(&bound_shapes, idx));
    }

    if (num_leftovers != 0)
    {
        CopyInChild(dst, &final_child);
    }
}
