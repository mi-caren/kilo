/* Tiny Vec — generic dynamic array without vtable dispatch.
 *
 * Calls vec functions directly by name instead of going through a
 * VecDriver function pointer table. Requires TYPE as the first
 * argument to all dispatch macros.
 *
 * The struct has no driver pointer and no iterator driver field,
 * making it smaller than the full Vec variant. Direct calls allow
 * the compiler to inline vec operations at -O2/-O3, eliminating
 * the indirect-call overhead of the vtable.
 *
 * Important: compile with -ffunction-sections -fdata-sections and
 * link with -Wl,--gc-sections. Without these, the compiler keeps
 * all function bodies with external linkage even when fully inlined,
 * resulting in a larger binary than the full variant. With gc-sections
 * the linker strips the unreferenced bodies, producing the smallest
 * binary. (Tested: 52K tiny+gc vs 59K full+gc at -O3.) */

#ifdef AEOLUS_FULL
#error "Cannot include tiny aeolus headers alongside full headers"
#endif
#define AEOLUS_TINY

#ifndef TINY_VEC_H
#define TINY_VEC_H

/* Link-time guard: forces the linker to pull _aeolus_variant_tiny from
 * the tiny library. If linked against libaeolus.a instead, this
 * produces an "undefined reference" error. */
extern const char _aeolus_variant_tiny;
static __attribute__((used, retain)) const char* _aeolus_check_vec = &_aeolus_variant_tiny;

#include "aeolus/shared/vec.h"
#include "aeolus/tiny/iter.h"

#define VEC_STRUCT_DEF(TYPE)\
    typedef struct {\
        VEC_STRUCT_BASE(TYPE)\
    } Vec(TYPE)

/* ********* tiny vec_init *********** */
#define VEC_INIT_FUNC_IMPL(TYPE)\
    VEC_INIT_FUNC_SIGNATURE(TYPE) {\
        if (vec_init_base(TYPE, vec, initial_size) == NULL)\
            return NULL;\
        return vec;\
    }

#define vec_empty(TYPE, SELF)                         VEC_EMPTY_FUNC_NAME(TYPE)(SELF)
#define vec_push(TYPE, SELF, EL)                      VEC_PUSH_FUNC_NAME(TYPE)(SELF, EL)
#define vec_repeat_append(TYPE, SELF, EL, N)          VEC_REPEAT_APPEND_FUNC_NAME(TYPE)(SELF, EL, N)
#define vec_set(TYPE, SELF, VAL, POS)                 VEC_SET_FUNC_NAME(TYPE)(SELF, VAL, POS)
#define vec_get(TYPE, SELF, POS)                      VEC_GET_FUNC_NAME(TYPE)(SELF, POS)
#define vec_insert(TYPE, SELF, EL, POS)               VEC_INSERT_FUNC_NAME(TYPE)(SELF, EL, POS)
#define vec_remove(TYPE, SELF, POS)                   VEC_REMOVE_FUNC_NAME(TYPE)(SELF, POS)
#define vec_last(TYPE, SELF)                          VEC_LAST_FUNC_NAME(TYPE)(SELF)
#define vec_first(TYPE, SELF)                         VEC_FIRST_FUNC_NAME(TYPE)(SELF)
/* Returns a pointer to the popped element. The pointer is valid only
 * until the next mutation (push, insert, pop) of the vec. */
#define vec_pop(TYPE, SELF)                           VEC_POP_FUNC_NAME(TYPE)(SELF)
#define vec_free(TYPE, SELF)                          VEC_FREE_FUNC_NAME(TYPE)(SELF)

#define VEC_DEFS(TYPE)\
    VEC_DEFS_SHARED(TYPE)\
    ITER_DEFS(Vec(TYPE), TYPE)

#define VEC_IMPL(TYPE)\
    VEC_IMPL_SHARED(TYPE)\
    VEC_INIT_FUNC_IMPL(TYPE)\
    VEC_ITER_IMPL(TYPE)\

VEC_DEFS(char)
VEC_DEFS(unsigned int)

#endif
