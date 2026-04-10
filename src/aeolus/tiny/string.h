/* Tiny String — string type for the driverless aeolus variant.
 *
 * Inlines VEC_STRUCT_BASE(char) directly instead of wrapping a
 * Vec(char). This avoids pulling in vec.h, keeping the header
 * dependency chain light: only vec/common.h is needed for the
 * struct fields. The .c implementation uses _fn macros to call
 * vec functions via a cast, so vec operations are still reused. */

#ifdef AEOLUS_FULL
#error "Cannot include tiny aeolus headers alongside full headers"
#endif
#define AEOLUS_TINY

#ifndef TINY_STRING_H
#define TINY_STRING_H

extern const char _aeolus_variant_tiny;
static __attribute__((used, retain)) const char* _aeolus_check_string = &_aeolus_variant_tiny;

#include <bits/types/struct_iovec.h>

#include "aeolus/tiny/iter.h"
#include "aeolus/shared/vec.h"

typedef struct {
    VEC_STRUCT_BASE(char)
} String;

ITER_DEFS(String, char)

#include "aeolus/shared/string.h"

#endif
