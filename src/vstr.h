#ifndef VECTOR_H

#include "vec.h"

typedef struct Str Str;
VEC_INCLUDE(VStr, vstr, Str, BY_REF);
VEC_INCLUDE(VrStr, vrstr, Str, BY_REF);

void vrstr_sort(VrStr *vec, size_t *counts);

#define VECTOR_H
#endif

