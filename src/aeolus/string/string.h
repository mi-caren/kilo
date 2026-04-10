#ifdef AEOLUS_TINY
#error "Cannot include full aeolus headers alongside tiny headers"
#endif
#define AEOLUS_FULL

#ifndef STRING_H
#define STRING_H

#include <bits/types/struct_iovec.h>

#include "aeolus/iter/iter.h"
#include "aeolus/vec/common.h"

typedef struct {
    VEC_STRUCT_BASE(char)
    ITER_DRIVER(String);
} String;

ITER_DEFS(String, char)

#include "aeolus/string/common.h"

#endif
