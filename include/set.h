#ifndef SET_H
#define SET_H

typedef struct {
    void* data;
    unsigned length;
    unsigned capacity;
    unsigned data_width;
} Set;

void ConstructSet(Set* s, unsigned data_width);
void DeconstructSet(Set* s);
unsigned AppendValue(Set* s, void* value);
void* Index(Set* s, unsigned index);
void CopyOut(Set* s, unsigned index, void* out);

#endif 