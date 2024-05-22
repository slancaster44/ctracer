#ifndef MATERIAL_H
#define MATERIAL_H

#include "tuple.h"
#include "set.h"
#include "pattern.h"

typedef struct Scene Scene;

/**
 * Shader function type. All shader functions must adhere to this signature
 * 
 * @param 'Scene* s' The scene being shaded
 * @param 'Set *intersections' The set of intersections at a given pixel being shaded
 * @param 'unsigned long idx' The index into 'Set *intersections' of the intersection being shaded
 * @param 'int limit' The recursion depth for recursive shader functions
 */
typedef Tuple3 (*Shader)(Scene *s, Set *intersections, unsigned long idx, int limit);

/**
 * PhongShader - Shader function that uses the Phong reflection model
 * 
 * @param 'Scene* s' The scene being shaded
 * @param 'Set *intersections' The set of intersections at a given pixel being shaded
 * @param 'unsigned long idx' The index into 'Set *intersections' of the intersection being shaded
 * @param 'int limit' The maximum recursion depth
 */
Tuple3 PhongShader(Scene *s, Set *intersections, unsigned long idx, int limit);

/**
 * Represents a shape's material
 * These values can be used by a Shader function to generate a color for a intersection
 */
typedef struct
{
    /** The material's pattern (e.g. solid, striped, gradient etc)*/
    Pattern pattern;

    /** The ambient reflection of the material. Scaled from dull at 0.0 to bright at 1.0 */
    double ambient_reflection;

    /** The diffuse reflection of the material. Scaled from dull at 0.0 to bright at 1.0 */
    double diffuse_reflection;

    /** The spdcular reflection of the material. Scaled from dull at 0.0 to bright at 1.0 */
    double specular_reflection;

    /** The shininess of the material. Scaled from no shininess at 0 to very shiny at around 200 */
    double shininess;

    /** The general reflection of the material */
    double general_reflection;

    /** The refractive index of the material */
    double refractive_index;

    /** The transparency of the material */
    double transparency;

    /** The material's shader function. Assigning different shader
     * functions to different materials will allow individual shapes to be
     * shaded differently
     */
    Shader shader;
} Material;

/**
 * @memberof Material
 * Generates a solid material of a given color. The other material fields are filled
 * in with sensiable defaults, and can be manipulated as desired
 * 
 * @param 'Tuple3 color' The color of the generated color
 * @returns 'Material' the generated material
 * 
 * @line
 * 
 * Default Values
 * - Material.pattern = NewSolidPattern(color);
 * - Material.ambient_reflection = 0.1;
 * - Material.diffuse_reflection = 0.9;
 * - Material.specular_reflection = 0.9;
 * - Material.general_reflection = 0.0;
 * - Material.shininess = 200;
 * - Material.refractive_index = 1.0;
 * - Material.transparency = 0.0;
 * - Material.shader = PhongShader;
 *  
*/
Material NewMaterial(Tuple3 color);

#endif
