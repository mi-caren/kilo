/* Tiny Iterator — generic iterator without vtable dispatch.
 *
 * Calls iterator functions directly by name instead of going through
 * a function pointer table. Requires the ITERABLE type as the first
 * argument to all dispatch macros.
 *
 * The struct does not need an ITER_DRIVER field, and no ITER_INIT
 * call is required. */

#ifdef AEOLUS_FULL
#error "Cannot include tiny aeolus headers alongside full headers"
#endif
#define AEOLUS_TINY

#ifndef TINY_ITER_H
#define TINY_ITER_H

extern const char _aeolus_variant_tiny;
static __attribute__((used, retain)) const char* _aeolus_check_iter = &_aeolus_variant_tiny;

#include "aeolus/shared/iter.h"

#define iter_curr(ITERABLE, SELF)    ITER_CURR_FUNC_NAME(ITERABLE)(SELF)
#define iter_begin(ITERABLE, SELF)   ITER_BEGIN_FUNC_NAME(ITERABLE)(SELF)
#define iter_end(ITERABLE, SELF)     ITER_END_FUNC_NAME(ITERABLE)(SELF)
#define iter_prev(ITERABLE, SELF)    ITER_PREV_FUNC_NAME(ITERABLE)(SELF)
#define iter_next(ITERABLE, SELF)    ITER_NEXT_FUNC_NAME(ITERABLE)(SELF)

#define EACH(ITERABLE, EL, IT) \
    IterItem(ITERABLE)* EL = iter_begin(ITERABLE, IT); EL != NULL; EL = iter_next(ITERABLE, IT)

#define EACH_REV(ITERABLE, EL, IT) \
    IterItem(ITERABLE)* EL = iter_end(ITERABLE, IT); EL != NULL; EL = iter_prev(ITERABLE, IT)

/* Tiny ITER_DEFS — no driver struct, just the item typedef
 * and forward declarations for the iterator functions. */
#define ITER_DEFS(ITERABLE, TYPE)\
    typedef TYPE IterItem(ITERABLE);\
    ITER_CURR_FUNC_SIGNATURE(ITERABLE);\
    ITER_BEGIN_FUNC_SIGNATURE(ITERABLE);\
    ITER_END_FUNC_SIGNATURE(ITERABLE);\
    ITER_PREV_FUNC_SIGNATURE(ITERABLE);\
    ITER_NEXT_FUNC_SIGNATURE(ITERABLE);\

#endif
