#ifndef SET_H
#define SET_H

#include <stdbool.h>

typedef struct {
    void* data __attribute__ ((aligned (32)));
    unsigned long length;
    unsigned long capacity;
    unsigned data_width;
} Set;

void ConstructSet(Set* s, unsigned data_width);
void ReconstructSet(Set* s);
void DeconstructSet(Set* s);
unsigned long AppendValue(Set* s, void* value);
void* Index(Set* s, unsigned long index);
void CopyOut(Set* s, unsigned long index, void* out);
void CloneSet(Set* destination, Set* source);

#endif 