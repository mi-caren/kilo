#ifdef AEOLUS_TINY
#error "Cannot include full aeolus headers alongside tiny headers"
#endif
#define AEOLUS_FULL

#ifndef STRING_H
#define STRING_H

extern const char _aeolus_variant_full;
static __attribute__((used, retain)) const char* _aeolus_check_string = &_aeolus_variant_full;

#include <bits/types/struct_iovec.h>

#include "aeolus/iter.h"
#include "aeolus/shared/vec.h"

typedef struct {
    VEC_STRUCT_BASE(char)
    ITER_DRIVER(String);
} String;

ITER_DEFS(String, char)

#include "aeolus/shared/string.h"

#endif
