#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "set.h"
#include "tuple.h"
#include "alignment.h"

#define SET_DEFAULT_CAPACITY 4

void ConstructSet(Set *s, unsigned data_width)
{
    memset(s, 0, sizeof(Set));
    s->data = aligned_alloc(__BIGGEST_ALIGNMENT__, data_width * SET_DEFAULT_CAPACITY);
    memset(s->data, 0, data_width * SET_DEFAULT_CAPACITY);

    s->length = 0;
    s->capacity = SET_DEFAULT_CAPACITY;
    s->data_width = data_width;
}

void DeconstructSet(Set *s)
{
    if (s->data != NULL)
    {
        free(s->data);
    }

    s->length = 0;
    s->capacity = 0;
}

unsigned long AppendValue(Set *s, void* value)
{
    if (s->length + 1 >= s->capacity)
    {
        s->capacity *= 3;
        s->data = reallocarray(s->data, s->capacity, s->data_width);
    }

    memcpy(s->data + (s->data_width * s->length), value, s->data_width);
    s->length++;

    return s->length - 1;
}

void* Index(Set *s, unsigned long index)
{
    if (index >= s->length)
    {
        printf("Index '%ld' out of range on set of length '%ld'\n", index, s->length);
        exit(1);
    }

    return s->data + (index * s->data_width);
}

void CopyOut(Set *s, unsigned long index, void* out)
{
    void* ptr = Index(s, index);
    memcpy(out, ptr, s->data_width);
}

void CloneSet(Set *dst, Set *src)
{
    ConstructSet(dst, src->data_width);
    for (unsigned i = 0; i < src->length; i++)
    {
        AppendValue(dst, Index(src, i));
    }
}

void ReconstructSet(Set *s)
{
    unsigned width = s->data_width;
    DeconstructSet(s);
    ConstructSet(s, width);
}

void SwapElements(Set *s, unsigned long ix, unsigned long iy)
{
    if (ix == iy)
    {
        return;
    }
    
    void* x = Index(s, ix);
    void* y = Index(s, iy);

    unsigned char tmp[s->data_width] align;
    memcpy(tmp, x, s->data_width);

    memcpy(x, y, s->data_width); //This memcpy is generating the issue
    memcpy(y, tmp, s->data_width);
}

void InsertSorted(Set *s, void *value, Comparator cmp)
{
    AppendValue(s, value);

    if (s->length == 1)
    {
        return;
    }

    //Move element down until it is in place
    for (unsigned long i = s->length - 1; i > 0 && cmp(Index(s, i), Index(s, i-1)); i--) 
    {
        SwapElements(s, i, i-1);
    }
}

void QuickSortHelper(Set *s, Comparator cmp, long lo, long hi)
{
    if (lo >= hi || lo < 0)
    {
        return;
    }

    void *pivot = Index(s, (unsigned long) hi);

    long i = lo;
    for (long j = lo; j <= hi - 1; j++)
    {
        void *value = Index(s, (unsigned long) j);
        if (!cmp(pivot, value))
        {
            SwapElements(s, (unsigned long) i, (unsigned long) j);
            i++;
        }
    }

    SwapElements(s, (unsigned long) i, (unsigned long) hi);
    QuickSortHelper(s, cmp, lo, i - 1);
    QuickSortHelper(s, cmp, i + 1, hi);
}

void QuickSort(Set *s, Comparator cmp)
{
    QuickSortHelper(s, cmp, 0, (long) s->length - 1);
}

void SortByNearestNeighbor(Set *s, Distance dst)
{
    
    for (unsigned long i = 0; i < s->length - 1; i++)
    {
        void *this_item = Index(s, i);

        unsigned long closest_idx;
        double distance = INFINITY;

        for (unsigned long j = i + 1; j < s->length; j++)
        {
            void *candidate = Index(s, j);
            double candidate_dst = dst(candidate, this_item);

            if (candidate_dst < distance)
            {
                distance = candidate_dst;
                closest_idx = j;
            }
        }

        SwapElements(s, i+1, closest_idx);
    }
}
