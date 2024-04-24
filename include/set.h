#ifndef SET_H
#define SET_H

typedef struct {
    void* data __attribute__ ((aligned (32)));
    unsigned long length;
    unsigned long capacity;
    unsigned long data_width;
} Set;

void ConstructSet(Set* s, unsigned data_width);
void DeconstructSet(Set* s);
unsigned long AppendValue(Set* s, void* value);
void* Index(Set* s, unsigned long index);
void CopyOut(Set* s, unsigned long index, void* out);

#endif 