#include "str.h"
#include "vstr.h"

VEC_IMPLEMENT(VStr, vstr, Str, BY_REF, str_free);
VEC_IMPLEMENT(VrStr, vrstr, Str, BY_REF, 0);

void vrstr_sort(VrStr *vec, size_t *counts)
{
    /* shell sort, https://rosettacode.org/wiki/Sorting_algorithms/Shell_sort#C */
    size_t h, i, j, n = vrstr_length(vec);
    Str temp;
    size_t temp2 = 0;
    for (h = n; h /= 2;) {
        for (i = h; i < n; i++) {
            //t = a[i];
            temp = *vrstr_get_at(vec, i);
            if(counts) temp2 = counts[i];
            //for (j = i; j >= h && t < a[j - h]; j -= h) {
            for (j = i; j >= h && str_cmp_sortable(&temp, vrstr_get_at(vec, j-h)) < 0; j -= h) {
                vrstr_set_at(vec, j, vrstr_get_at(vec, j-h));
                if(counts) counts[j] = counts[j-h];
                //a[j] = a[j - h];
            }
            //a[j] = t;
            vrstr_set_at(vec, j, &temp);
            if(counts) counts[j] = temp2;
        }
    }
}


