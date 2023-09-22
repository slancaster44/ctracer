#include <stdlib.h>
#include <string.h>

#include "set.h"

#define SET_DEFAULT_CAPACITY 4

void ConstructSet(Set* s, unsigned data_width) {
    s->data = malloc(data_width * SET_DEFAULT_CAPACITY);
    s->length = 0;
    s->capacity = SET_DEFAULT_CAPACITY;
    s->data_width = data_width;
}

void DeconstructSet(Set* s) {
    s->length = 0;
    free(s->data);
}

unsigned long AppendValue(Set* s, void* value) {
    if (s->length + 1 >= s->capacity) {
        s->capacity *= 3;
        s->data = reallocarray(s->data, s->capacity, s->data_width);
    }

    memcpy(s->data + (s->data_width * s->length), value, s->data_width);
    s->length ++;

    return s->length-1;
}

void* Index(Set* s, unsigned long index) {
    return s->data + (index * s->data_width);
}

void CopyOut(Set* s, unsigned long index, void* out) {
    void* ptr = Index(s, index);
    memcpy(out, ptr, s->data_width);
}