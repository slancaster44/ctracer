#ifndef TREE_H
#define TREE_H

#include "set.h"
#include "ray.h"

typedef struct Node {
    struct Node* parent;
    Set   shapes;
    Set   children;
} Node;

typedef struct {
    Node start;
} Tree;

void ConstructTree(Tree* tree);
void DeconstructTree(Tree* tree);
void CloneTree(Tree* destination, Tree* source);
void CopyInChild(Tree* parent, Tree* child);

void IntersectTree(Tree* tree, Ray r, Set* intersections);

void PropagateTransform(Tree* tree, Matrix4x4 transform);

#endif