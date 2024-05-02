#include "tree.h"
#include "intersection.h"
#include "shape.h"
#include <string.h>

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
    for (unsigned i = 0; i < n->shapes.length; i++) {
        Shape* this_shape = Index(&n->shapes, i);
        Intersection intersection = Intersect(this_shape, r);
        
        if (intersection.count != 0) {
            AppendValue(intersections, &intersection);
        }
    }

    for (unsigned i = 0; i < n->children.length; i++) {
        Node* child = Index(&n->children, i);
        IntersectNode(child, r, intersections);
    }
}

void IntersectTree(Tree* tree, Ray r, Set* intersections) {
    IntersectNode(&tree->start, r, intersections);
}