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

void CloneNode(Node* destination, Node* source) {
    memset(destination, 0, sizeof(Node));
    CloneSet(&destination->shapes, &source->shapes);

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

void AddShapeToTree(Tree* tree, Shape shape) {
    AppendValue(&tree->start.shapes, &shape);
    CalculateBounds(tree);
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

void TreeNodeBounds(Node* n) {
    Tuple3 min = NewPnt3(INFINITY, INFINITY, INFINITY);
    Tuple3 max = NewPnt3(-INFINITY, -INFINITY, -INFINITY);

    for (unsigned i = 0; i < n->shapes.length; i++) {
        Shape* this_shape = Index(&n->shapes, i);
        Bounds bounds = ShapeBounds(this_shape);

        min = _mm256_min_pd(bounds.minimum_bound, min);
        max = _mm256_max_pd(bounds.maximum_bound, max);
    }

    for (unsigned i = 0; i < n->children.length; i++) {
        Node* child = Index(&n->children, i);
        TreeNodeBounds(child);

        min = _mm256_min_pd(child->bounds.minimum_bound, min);
        max = _mm256_max_pd(child->bounds.maximum_bound, max);
    }

    Bounds out = {
        .minimum_bound = _mm256_min_pd(min, max),
        .maximum_bound = _mm256_max_pd(min, max),
    };

    n->bounds = out;
}

void CalculateBounds(Tree* tree) {
    TreeNodeBounds(&tree->start);
}