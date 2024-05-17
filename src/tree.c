#include "tree.h"
#include "intersection.h"
#include "shape.h"
#include "bounding.h"
#include <string.h>
#include <math.h>

void ConstructTree(Tree* tree) {
    tree->start.parent = NULL;
    ConstructSet(&tree->start.shapes, sizeof(Shape));
    ConstructSet(&tree->start.children, sizeof(Node));
}

void ReconstructTree(Tree* tree) {
    DeconstructTree(tree);
    ConstructTree(tree);
}

void DeconstructNode(Node* n) {
    n->parent = NULL;

    for (unsigned i = 0; i < n->children.length; i++) {
        Node* child = Index(&n->children, i);
        DeconstructNode(child);
    }

    DeconstructSet(&n->children);
    DeconstructSet(&n->shapes);
}

void DeconstructTree(Tree* tree) {
    DeconstructNode(&tree->start);
}

void PrintNode(Node* n, const char* prefix) {
    if (n->parent == NULL) {
        printf("%s|- Parent node with '%ld' shape(s) and '%ld' children\n", prefix, n->shapes.length, n->children.length);
    } else {
            printf("%s|- Child node with '%ld' shape(s) and '%ld' children\n", prefix, n->shapes.length, n->children.length);
    }

    size_t prefix_size = strlen(prefix) + 3; // +1 for null terminator, +1 for new tab
    char* new_prefix = alloca(prefix_size);
    memcpy(new_prefix, prefix, prefix_size); 
    new_prefix[prefix_size - 3] = '|';
    new_prefix[prefix_size - 2] = '\t';
    new_prefix[prefix_size - 1] = '\0';

    for (unsigned i = 0; i < n->children.length; i++) {
        Node* child = Index(&n->children, i);
        PrintNode(child, new_prefix);
    }
}

void PrintTree(Tree* t) {
    PrintNode(&t->start, "");
}

void CloneNode(Node* destination, Node* source) {
    memset(destination, 0, sizeof(Node));
    CloneSet(&destination->shapes, &source->shapes);

    if (destination->children.data == NULL) {
        ConstructSet(&destination->children, sizeof(Node));
    }

    for (unsigned i = 0; i < source->children.length; i++) {
        Node new_child;
        
        CloneNode(&new_child, Index(&source->children, i));
        new_child.parent = destination;

        AppendValue(&destination->children, &new_child);
    }
}

void CloneTree(Tree* destination, Tree* source) {
    CloneNode(&destination->start, &source->start);
}

void CopyInChild(Tree* parent, Tree* child) {
    unsigned long idx = AppendValue(&parent->start.children, &child->start);

    Node* child_ptr = Index(&parent->start.children, idx);
    CloneNode(child_ptr, &child->start);
    child_ptr->parent = &parent->start;
}

void AddShapeToTree(Tree* tree, Shape* shape) {
    AppendValue(&tree->start.shapes, shape);
}

void PropagateTransformOnNode(Node* node, Matrix4x4 transform) {
    for (unsigned long i = 0; i < node->shapes.length; i++) {
        Shape* shape_ptr = Index(&node->shapes, i);
        ApplyTransformation(shape_ptr, transform);
    }

    for (unsigned long i = 0; i < node->children.length; i++) {
        Node* child = Index(&node->children, i);
        PropagateTransformOnNode(child, transform);
    }
}

void PropagateTransform(Tree* tree, Matrix4x4 transform) {
    PropagateTransformOnNode(&tree->start, transform);
}

void PropagateMaterialOnNode(Node* node, Material mat) {
    for (unsigned long i = 0; i < node->shapes.length; i++) {
        Shape* shape_ptr = Index(&node->shapes, i);
        shape_ptr->material = mat;
    }

    for (unsigned long i = 0; i < node->children.length; i++) {
        Node* child = Index(&node->children, i);
        PropagateMaterialOnNode(child, mat);
    }
}

void PropagateMaterial(Tree* tree, Material material) {
    PropagateMaterialOnNode(&tree->start, material);
}

void IntersectNode(Node* n, Ray r, Set* intersections) {
    for (unsigned i = 0; i < n->children.length; i++) {
        Node* child = Index(&n->children, i);
        IntersectNode(child, r, intersections);
    }

    if (!IsInBounds(n->bounds, r)) {
        return;
    }

    for (unsigned i = 0; i < n->shapes.length; i++) {
        Shape* this_shape = Index(&n->shapes, i);
        Intersection intersection = Intersect(this_shape, r);
        
        if (intersection.count != 0) {
            AppendValue(intersections, &intersection);
        }
    }
}

void IntersectTree(Tree* tree, Ray r, Set* intersections) {
    IntersectNode(&tree->start, r, intersections);
}

Bounds SetBounds(Set* s) {
    Tuple3 min = NewPnt3(INFINITY, INFINITY, INFINITY);
    Tuple3 max = NewPnt3(-INFINITY, -INFINITY, -INFINITY);


    for (unsigned i = 0; i < s->length; i++) {
        Shape* this_shape = Index(s, i);
        Bounds bounds = ShapeBounds(this_shape);

        min = _mm256_min_pd(bounds.minimum_bound, min);
        max = _mm256_max_pd(bounds.maximum_bound, max);
    }

    Bounds out = {
        .minimum_bound = _mm256_min_pd(min, max),
        .maximum_bound = _mm256_max_pd(min, max),
    };

    return out;
}

void TreeNodeBounds(Node* n) {
    Bounds out = SetBounds(&n->shapes);

    for (unsigned i = 0; i < n->children.length; i++) {
        Node* child = Index(&n->children, i);
        TreeNodeBounds(child);

        out.minimum_bound = _mm256_min_pd(child->bounds.minimum_bound, out.minimum_bound);
        out.maximum_bound = _mm256_max_pd(child->bounds.maximum_bound, out.maximum_bound);
    }

    n->bounds = out;
}

void CalculateBounds(Tree* tree) {
    TreeNodeBounds(&tree->start);
}

unsigned NumberOfParents(Node* n) {
    unsigned count = 0;

    while (n != NULL) {
        count++;
        n = n->parent;
    }

    return count;
}

void GetShapesSet(Set* dst, Node* src) {
    for (unsigned i = 0; i < src->shapes.length; i++) {
        AppendValue(dst, Index(&src->shapes, i));
    }

    for (unsigned i = 0; i < src->children.length; i++) {
        GetShapesSet(dst, Index(&src->children, i));
    }
}

#define MIN_SHAPES_PER_NODE 4
void GenerateBVHLimited(Tree* dst, Tree* src, unsigned limit) {
    if (limit == 0) {
        CopyInChild(dst, src);
        return;
    }

    Set all_shapes;
    ConstructSet(&all_shapes, sizeof(Shape));
    GetShapesSet(&all_shapes, &src->start);

    Bounds all_bounds = SetBounds(&all_shapes);
    Tuple3 hwd = TupleSubtract(all_bounds.maximum_bound, all_bounds.minimum_bound);
    unsigned longest_axis = hwd[0] > hwd[1] ? (hwd[0] > hwd[2] ? 0 : 2) : (hwd[1] > hwd[2] ? 1 : 2);
    double long_axis_size = hwd[longest_axis];
    double long_axis_divider = all_bounds.maximum_bound[longest_axis] - (long_axis_size / 2);

    Tree left_child;
    ConstructTree(&left_child);
    Tree right_child;
    ConstructTree(&right_child);

    for (unsigned i = 0; i < all_shapes.length; i++) {
        Shape* s = Index(&all_shapes, i);
        Tuple3 center = Centroid(ShapeBounds(s));
        
        if (center[longest_axis] < long_axis_divider || s->type ==  PLANE) {
            AddShapeToTree(&left_child, s);
        } else {
            AddShapeToTree(&right_child, s);
        }
    }

    if ((left_child.start.shapes.length >  MIN_SHAPES_PER_NODE || right_child.start.shapes.length > MIN_SHAPES_PER_NODE)) {
        GenerateBVHLimited(&left_child, &left_child, limit - 1);
        GenerateBVHLimited(&right_child, &right_child, limit - 1);

        ReconstructSet(&left_child.start.shapes); //child shape sets
        ReconstructSet(&right_child.start.shapes);
    } 
        
    CopyInChild(dst, &left_child);
    CopyInChild(dst, &right_child);

    DeconstructTree(&left_child);
    DeconstructTree(&right_child);
    DeconstructSet(&all_shapes);
}

#define MAX_BVH_DEPTH 6
void GenerateBVH(Tree* dst, Tree* src) {
    GenerateBVHLimited(dst, src, MAX_BVH_DEPTH);
}
