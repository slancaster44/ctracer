#ifndef PATTERN_H
#define PATTERN_H

#include "tuple.h"
#include "matrix.h"

/**
 * Tags to be applied to a pattern to indicate what type
 * of pattern is being represented 
 */
typedef enum
{
    /** A single, solid color */
    SOLID,
    /** Two colors alternating every 1.0 units along the x axis */
    STRIPED,
    /** Two colors alternating every 1.0 units along the x and y axes*/
    CHECKERED,
    /** Two colors alternating in a ring-like pattern from the center of the surface */
    RINGED,
    /** A slow fade from one color to another */
    GRADIENT,
} PATTERN_TYPE;

/**
 * A material's pattern
 */
typedef struct
{
    /** 
     * The first color used for a pattern. This color is the only 
     * color used if the PATTERN_TYPE is SOLID
     */
    Tuple3 color_a;

    /**
     * The second color used for a pattern. This color is not used
     * if the PATTERN_TYPE is SOLID 
     */
    Tuple3 color_b;

    /** The pattern's type, see PATTERN_TYPE for more */
    PATTERN_TYPE type;

    /** @private */
    Matrix4x4 transform;
    /** @private */
    Matrix4x4 inverse_transform;
} Pattern;

typedef struct Shape Shape; //For prevent circular dependencies

/**
 * @memberof Shape
 * Returns the color of the shape's material pattern on a given location. @note This function
 * is does not do the final color calculation, this is done by a @ref Shader function. The Shader function
 * will manipulate the color returned by PatternColorAt() to calculate the final color.
 * 
 * @param 'Shape *s' the shape to find the color for
 * @param 'Tuple3 position' the location to find the color for
 * @returns A Tuple3 where the elements represent the red, blue and green channels of a color
 */
Tuple3 PatternColorAt(Shape *s, Tuple3 position);

/**
 * @memberof Pattern
 * Constructs a new solid pattern of a given color
 * 
 * @param 'Tuple3 color' The color of the solid pattern, where the first three channels represent
 * the red, green and blue components on a scale of 0.0 to 1.0.
 * @returns A solid pattern of the given color
*/
Pattern NewSolidPattern(Tuple3 color);

/**
 * @memberof Pattern
 * Constructs a new two-color pattern
 * 
 * @param 'Tuple3 color_a' The first color
 * @param 'Tuple3 color_b' The second color
 * @param 'PATTERN_TYPE type' The type of pattern
 * @returns A pattern of the given type with the given color
*/
Pattern NewPattern(Tuple3 color_a, Tuple3 color_b, PATTERN_TYPE type);

/**
 * @memberof Pattern
 * Transforms the pattern 'p,' by the given transformation matrix, and returns
 * the result
 * 
 * @param 'Pattern p' The pattern to be transformed
 * @param 'Matrix4x4 m' The transformation to be applied to the pattern
 * @returns The transformed pattern
 */
Pattern TransformPattern(Pattern p, Matrix4x4 m);

#endif
