#ifndef ALIGN_H
#define ALIGN_H

/**
 * @def align
 * Aligns a field, used to align vectors to the correct place 
 */
#define align __attribute__((aligned(sizeof(__BIGGEST_ALIGNMENT__))))

#endif
