#ifndef SCENE_H
#define SCENE_H

#include <stdbool.h>

#include "light.h"
#include "tree.h"
#include "camera.h"
#include "canvas.h"

/**
 * Represents a scene to be rendered
 */
typedef struct Scene
{
    /** A tree of shapes. At the start of rendering, this 
     * tree is converted into a bounding volume hierachy
     */
    Tree shapes;

    /** A light to light up the scene */
    Light light;

    /** The camera that will capture the scene */
    Camera camera;
} Scene;

/**
 * @memberof Scene
 * Fill out the given scene with a the camera and light, intializes
 * the shapes tree.
 */
void ConstructScene(Scene *s, Camera c, Light);

/**
 * @memberof Scene
 * Deallocate the given scene's shape tree. All attempts add shapes to,
 * or render the scene afterwards will result in a SEGFAULT
*/
void DeconstructScene(Scene *s);

/**
 * @memberof Scene
 * Intersect the given scene with the given ray. All resulting intersection
 * objects will be added to the intersection_set.
 * 
 * Note: intersection_set must be initalized with 
 * <code> ConstructSet(&my_intersections, sizeof(Intersection)) </code>
 * before attempting to intersect the scene
 */
void IntersectScene(Scene *s, Ray r, Set *intersection_set);

/**
 * @memberof Scene
 * Returns true if a the given location is in a shadow in the
 * scene
*/
bool IsInShadow(Scene *s, Tuple3 location);

/**
 * @memberof Scene
 * Read a scene from the given metadata file
 * 
 * Note: This is a replacement for ConstructScene(). ConstructScene() should
 * not be called on the scene before or after ReadScene() is
 */
void ReadScene(Scene *s, const char *filename);

/**
 * @memberof Scene
 * Returns the color at on a given ray for a given scene 
 */
Tuple3 ColorFor(Scene *s, Ray r);

/**
 * @memberof Scene
 * Like ColorFor(), however recursion that might occur due to reflection
 * will be limited to the given limit
 * 
 * @param 'Scene *s' The scene to find a color for
 * @param 'Ray r' The ray to intersect the scene with
 * @param 'int limit' The maximum recursion depth
 * @returns The color at the ray-scene intersection
*/
Tuple3 ColorForLimited(Scene *s, Ray r, int limit);

/**
 * @memberof Scene
 * Render a given scene to the given canvas
 */
void RenderScene(Scene *s, Canvas *c);

/**
 * @memberof Scene
 * Render the given scene to the given canvas without
 * multi-processing
 */
void RenderSceneUnthreaded(Scene *s, Canvas *c);

/**
 * @memberof Scene
 * Replace the given scene's shape tree with the
 * given tree
 */
void ReplaceTree(Scene *s, Tree *t);
#endif
