#ifdef AEOLUS_FULL
#error "Cannot include tiny aeolus headers alongside full headers"
#endif
#define AEOLUS_TINY

#ifndef TINY_STRING_H
#define TINY_STRING_H

#include <bits/types/struct_iovec.h>

#include "aeolus/tiny/iter.h"
#include "aeolus/vec/common.h"

typedef struct {
    VEC_STRUCT_BASE(char)
} String;

ITER_DEFS(String, char)

#include "aeolus/string/common.h"

#endif
