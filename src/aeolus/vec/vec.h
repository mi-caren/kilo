/* Vec — generic dynamic array.
 *
 * Vecs can be stack-allocated (vec_init/vec_deinit) or heap-allocated
 * (vec_new/vec_free).
 *
 * Pushing a stack-allocated vec into another vec creates a shallow copy:
 * both copies share the same items array but have independent
 * len/cap/curr fields. Mutating the original after the push will desync
 * the metadata, and a realloc can leave the copy with a dangling items
 * pointer. Pushing a heap-allocated vec (pointer) avoids this because
 * only the pointer is copied — there is always a single vec struct. */

#ifdef AEOLUS_TINY
#error "Cannot include full aeolus headers alongside tiny headers"
#endif
#define AEOLUS_FULL

#ifndef VEC_H
#define VEC_H

#include <stdbool.h>
#include <stdlib.h>

#include "aeolus/utils.h"
#include "aeolus/iter/iter.h"
#include "aeolus/vec/common.h"

#define Vec(TYPE) GenericName(TYPE, Vec)

#define VEC_STRUCT_DEF(TYPE)\
    typedef struct {\
        VEC_STRUCT_BASE(TYPE)\
        const struct VecDriver(TYPE)* drv;\
        ITER_DRIVER(Vec(TYPE));\
    } Vec(TYPE)

#define VecDriver(TYPE)     CAT(Vec(TYPE), Driver)


#define VEC_DRIVER_DEF(TYPE)\
    struct VecDriver(TYPE) {\
        void       (*empty)         (Vec(TYPE)* self);\
        Vec(TYPE)* (*push)          (Vec(TYPE)* self, TYPE el);\
        TYPE*      (*pop)           (Vec(TYPE)* self);\
        Vec(TYPE)* (*repeat_append) (Vec(TYPE)* self, TYPE el, size_t n);\
        TYPE*      (*set)           (Vec(TYPE)* self, TYPE val, size_t idx);\
        TYPE*      (*get)           (Vec(TYPE)* self, size_t idx);\
        Vec(TYPE)* (*insert)        (Vec(TYPE)* self, TYPE el, size_t pos);\
        Vec(TYPE)* (*remove)        (Vec(TYPE)* self, size_t pos);\
        TYPE*      (*last)          (Vec(TYPE)* self);\
        TYPE*      (*first)         (Vec(TYPE)* self);\
        void       (*free)          (Vec(TYPE)* self);\
    }

/* ********* vec_init *********** */
#define VEC_INIT_FUNC_IMPL(TYPE)\
    VEC_INIT_FUNC_SIGNATURE(TYPE) {\
        if (vec_init_base(TYPE, vec, initial_size) == NULL)\
            return NULL;\
        static const struct VecDriver(TYPE) vec_driver = {\
            .empty         = VEC_EMPTY_FUNC_NAME(TYPE),\
            .push          = VEC_PUSH_FUNC_NAME(TYPE),\
            .pop           = VEC_POP_FUNC_NAME(TYPE),\
            .repeat_append = VEC_REPEAT_APPEND_FUNC_NAME(TYPE),\
            .set           = VEC_SET_FUNC_NAME(TYPE),\
            .get           = VEC_GET_FUNC_NAME(TYPE),\
            .insert        = VEC_INSERT_FUNC_NAME(TYPE),\
            .remove        = VEC_REMOVE_FUNC_NAME(TYPE),\
            .last          = VEC_LAST_FUNC_NAME(TYPE),\
            .first         = VEC_FIRST_FUNC_NAME(TYPE),\
            .free          = VEC_FREE_FUNC_NAME(TYPE),\
        };\
        vec->drv = &vec_driver;\
        ITER_INIT(Vec(TYPE), vec);\
        return vec;\
    }


#define vec_empty(SELF)             (SELF)->drv->empty(SELF)
#define vec_push(SELF, EL)             (SELF)->drv->push(SELF, EL)
#define vec_repeat_append(SELF, EL, N)              (SELF)->drv->repeat_append(SELF, EL, N)
#define vec_set(SELF, VAL, POS)             (SELF)->drv->set(SELF, VAL, POS)
#define vec_get(SELF, POS)             (SELF)->drv->get(SELF, POS)
#define vec_insert(SELF, EL, POS)              (SELF)->drv->insert(SELF, EL, POS)
#define vec_remove(SELF, POS)             (SELF)->drv->remove(SELF, POS)
#define vec_last(SELF)                (SELF)->drv->last(SELF)
#define vec_first(SELF)                (SELF)->drv->first(SELF)
/* Returns a pointer to the popped element. The pointer is valid only
 * until the next mutation (push, insert, pop) of the vec. */
#define vec_pop(SELF)                (SELF)->drv->pop(SELF)
#define vec_free(SELF)                (SELF)->drv->free(SELF)

#define VEC_DEFS(TYPE)\
    VEC_DEFS_COMMON(TYPE)\
    VEC_DRIVER_DEF(TYPE);\
    ITER_DEFS(Vec(TYPE), TYPE)\

#define VEC_IMPL(TYPE)\
    VEC_IMPL_COMMON(TYPE)\
    VEC_ITER_IMPL(TYPE)\
    VEC_INIT_FUNC_IMPL(TYPE)\


VEC_DEFS(char)
VEC_DEFS(unsigned int)


#endif