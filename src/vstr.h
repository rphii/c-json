#ifndef VECTOR_H

#include "vec.h"

typedef struct Str Str;
VEC_INCLUDE(VStr, vstr, Str, BY_REF, BASE);
VEC_INCLUDE(VrStr, vrstr, Str, BY_REF, BASE);
VEC_INCLUDE(VrStr, vrstr, Str, BY_REF, SORT);

#define VECTOR_H
#endif

