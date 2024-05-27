#ifndef SET_H
#define SET_H

#include <stdbool.h>

/**
 * A dynamically allocated array of data
 */
typedef struct
{
    /** @private */
    void* data;

    /** The number of elements in the set. @note Don't mutate this value */
    unsigned long length;

    /** @private */
    unsigned long capacity;

    /** @private */
    unsigned data_width;
} Set;

/**
 * @memberof Set
 * Compare two set elements for sorted insert
*/
typedef bool (*Comparator)(void* v1, void* v2);

/**
 * @memberof Set
 * Constructs a set
 * 
 * @param 'Set *s' The set to initialize
 * @param 'unsigned data_width' The width (in bytes) of the data to be stored
*/
void ConstructSet(Set *s, unsigned data_width);

/**
 * @memberof Set
 * Empties the given set, and reinitializes its feilds
 */
void ReconstructSet(Set *s);

/**
 * @memberof Set
 * Deconstruct a set, and free all allocated memory
 */
void DeconstructSet(Set *s);

/**
 * @memberof Set
 * Add an element to a given set
 * 
 * @param 'Set *s' The set to append to
 * @param 'void *value' A pointer to the data to copy into the set
 * @returns The index at which the appended value is stored
 */
unsigned long AppendValue(Set *s, void *value);

/**
 * @memberof Set
 * Get a pointer to a given element in the given set
 * 
 * @param 'Set *s' The set that contains the data
 * @param 'unsigned long index' The offset the data is located at
 * @returns A pointer to the requested data 
 */
void *Index(Set *s, unsigned long index);

/**
 * @memberof Set
 * Copy a set element out of the given set, and into a another memory area
 * 
 * @param 'Set *s' The set to copy the element out of
 * @param 'unsigned long index' The offset of the required data
 * @param 'void *out' The memory area to copy the data to
*/
void CopyOut(Set *s, unsigned long index, void *out);

/**
 * @memberof Set
 * Copy the source set to the destination set. The data will be copied too, however
 * if that data contains references to other objects, those references will not be
 * copied
 * 
 * @param 'Set *destination' The set to copy data to
 * @param 'Set *source' The set to copy from
 * 
 * @note The destination set should not be initialized before calling CloneSet()
 */
void CloneSet(Set *destination, Set *source);

/**
 * @memberof Set
 * Swap the element located at offset 'ix' with the element located
 * at offset 'iy.'
 */
void SwapElements(Set *s, unsigned long ix, unsigned long iy);

/**
 * @memberof Set
 * Assuming 's' is sorted, insert value into the set in order
 * 
 * @param Set *s set to insert into
 * @param void *value the value to insert
 * @param Comparator c the function to compare values with
*/
void InsertSorted(Set *s, void* value, Comparator c);

/**
 * @memberof Set
 * Sort the given set according to the given comparator
*/
void QuickSort(Set *s, Comparator cmp);

#endif
