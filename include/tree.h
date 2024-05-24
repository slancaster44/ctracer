#ifndef TREE_H
#define TREE_H

#include "set.h"
#include "ray.h"
#include "material.h"
#include "shape.h"
#include "bounds.h"

/** @private A tree node */
typedef struct Node
{
    /** @private A link back to the parent node */
    struct Node *parent;

    /** @private The set of shapes belonging to this node */
    Set shapes;

    /** @private The set of child nodes */
    Set children;

    /** @private The node's bounding box */
    Bounds bounds;
} Node;

/** A tree of shapes */
typedef struct Tree
{
    /** @private The root node in the tree */
    Node start;
} Tree;

/** 
 * @memberof Tree
 * Initialize the given tree. This includes allocating memory for the
 * tree's shapes and child nodes
 */
void ConstructTree(Tree *tree);

/**
 * @memberof Tree
 * Empty the tree and reinitialize it. Equivalent to 
 * <code> DeconstructTree(tree); ConstructTree(tree); </code>
 */
void ReconstructTree(Tree *tree);

/**
 * @memberof Tree
 * Deallocate the tree's memory areas. Attempts to manipulate the
 * tree after DeconstructTree() is called will likely result in
 * a SEGFAULT
 */
void DeconstructTree(Tree *tree);

/**
 * @memberof Tree
 * Copy the source tree to the destination tree, including all
 * shapes and child nodes.
 * 
 * @note The destination node should be uninitialized before 
 * CloneTree() is called
 */
void CloneTree(Tree *destination, Tree *source);

/**
 * @memberof Tree
 * Clone the child tree, and insert it as a child node to 
 * the parent tree's root node
 */
void CopyInChild(Tree *parent, Tree *child);

/**
 * @memberof Tree
 * Intersect all shapes in the given tree with the given
 * ray. All resulting intersections wil be placed into the
 * intersections set.
 * 
 * @note 'Set *intersections' should be initialized before
 * IntersectTree() is called.
*/
void IntersectTree(Tree *tree, Ray r, Set *intersections);

/**
 * @memberof Tree
 * Apply the given transformation to all shapes in the given
 * tree.
 */
void PropagateTransform(Tree *tree, Matrix4x4 transform);

/**
 * @memberof Tree
 * Apply the given material to all shapes in the given 
 * tree
 */
void PropagateMaterial(Tree *tree, Material material);

/**
 * @memberof Tree
 * Add the given shape to the root node of the given tree
 */
void AddShapeToTree(Tree *tree, Shape *shape);

/**
 * @memberof Tree
 * @private
 * Calculate the bounding boxes for every node in the given tree.
 * Store those bounding boxes on the nodes
 */
void CalculateBounds(Tree *tree);

/**
 * @memberof Tree
 * @private
 * Generate a BVH based on 'src' and store the new tree
 * in 'dst.' This new tree will be more optimized for fast
 * rendering
*/
void GenerateBVH(Tree *dst, Tree *src);

/**
 * @memberof Tree
 * Print the given tree to the console
 */
void PrintTree(Tree *tree);
#endif
