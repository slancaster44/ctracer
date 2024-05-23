#include <stdlib.h>
#include <string.h>

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
    void* x = Index(s, ix);
    void* y = Index(s, iy);

    unsigned char tmp[s->data_width] align;
    memcpy(tmp, x, s->data_width);

    memcpy(x, y, s->data_width); //This memcpy is generating the issue
    memcpy(y, tmp, s->data_width);
}
