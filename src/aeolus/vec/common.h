#ifndef VEC_FUNCTIONS_H
#define VEC_FUNCTIONS_H

#include <stdlib.h>

#define VEC_STRUCT_BASE(TYPE) \
    size_t cap;\
    size_t len;\
    size_t curr;\
    TYPE* items;\

/* ********* vec_init *********** */
#define VEC_INIT_FUNC_NAME(TYPE)        CAT(Vec(TYPE), _init)
#define VEC_INIT_FUNC_SIGNATURE(TYPE)   Vec(TYPE)* VEC_INIT_FUNC_NAME(TYPE)(Vec(TYPE)* vec, size_t initial_size)

#define VEC_INIT_BASE_FUNC_NAME(TYPE)        CAT(Vec(TYPE), _init_base)
#define VEC_INIT_BASE_FUNC_SIGNATURE(TYPE)   Vec(TYPE)* VEC_INIT_BASE_FUNC_NAME(TYPE)(Vec(TYPE)* vec, size_t initial_size)
#define VEC_INIT_BASE_FUNC_IMPL(TYPE)\
    VEC_INIT_BASE_FUNC_SIGNATURE(TYPE) {\
        size_t cap = vec_cap_from_size(initial_size);\
        TYPE* items = malloc(sizeof(TYPE) * cap);\
        if (items == NULL) return NULL;\
        vec->cap = cap;\
        vec->len = 0;\
        vec->curr = 0;\
        vec->items = items;\
        return vec;\
    }

#define vec_init_base(TYPE, SELF, CAP)      VEC_INIT_BASE_FUNC_NAME(TYPE)(SELF, CAP)
#define vec_init(TYPE, SELF)                VEC_INIT_FUNC_NAME(TYPE)(SELF, 1)
#define vec_init_with_cap(TYPE, SELF, CAP)  VEC_INIT_FUNC_NAME(TYPE)(SELF, CAP)
/* Frees the items array only. Use for stack-allocated vecs (via vec_init). */
#define vec_deinit(SELF) do { free((SELF)->items); } while(0)

#define vec_new(TYPE)                   VEC_NEW_FUNC_NAME(TYPE)(1)
#define vec_new_with_cap(TYPE, CAP)     VEC_NEW_FUNC_NAME(TYPE)(CAP)

/* ********* vec_new *********** */
#define VEC_NEW_FUNC_NAME(TYPE)         CAT(Vec(TYPE), _new)
#define VEC_NEW_FUNC_SIGNATURE(TYPE)    Vec(TYPE)* VEC_NEW_FUNC_NAME(TYPE)(size_t initial_size)
#define VEC_NEW_FUNC_IMPL(TYPE)\
    VEC_NEW_FUNC_SIGNATURE(TYPE) {\
        Vec(TYPE)* vec = malloc(sizeof(Vec(TYPE)));\
        if (vec == NULL) return NULL;\
        if (VEC_INIT_FUNC_NAME(TYPE)(vec, initial_size) == NULL) {\
            free(vec);\
            return NULL;\
        }\
        return vec;\
    }

/* ********* vec_empty *********** */
#define VEC_EMPTY_FUNC_NAME(TYPE)            CAT(Vec(TYPE), _empty)
#define VEC_EMPTY_FUNC_SIGNATURE(TYPE)       void VEC_EMPTY_FUNC_NAME(TYPE)(Vec(TYPE)* self)
#define VEC_EMPTY_FUNC_IMPL(TYPE)\
    VEC_EMPTY_FUNC_SIGNATURE(TYPE) {\
        self->len = 0;\
    }

/* ********* vec_push *********** */
#define VEC_PUSH_FUNC_NAME(TYPE)            CAT(Vec(TYPE), _push)
#define VEC_PUSH_FUNC_SIGNATURE(TYPE)       Vec(TYPE)* VEC_PUSH_FUNC_NAME(TYPE)(Vec(TYPE)* self, TYPE el)
#define VEC_PUSH_FUNC_IMPL(TYPE)\
    VEC_PUSH_FUNC_SIGNATURE(TYPE) {\
        if (self->len == self->cap) {\
            if (vec_grow_fn(TYPE, self) == NULL)\
                return NULL;\
        }\
        self->items[self->len] = el;\
        self->len++;\
        return self;\
    }

/* ********* vec_repeat_append *********** */
#define VEC_REPEAT_APPEND_FUNC_NAME(TYPE)            CAT(Vec(TYPE), _repeat_append)
#define VEC_REPEAT_APPEND_FUNC_SIGNATURE(TYPE)       Vec(TYPE)* VEC_REPEAT_APPEND_FUNC_NAME(TYPE)(Vec(TYPE)* self, TYPE el, size_t n)
#define VEC_REPEAT_APPEND_FUNC_IMPL(TYPE)\
    VEC_REPEAT_APPEND_FUNC_SIGNATURE(TYPE) {\
        size_t total_space = self->len + n;\
        if (total_space > self->cap) {\
            if (vec_make_space_fn(TYPE, self, total_space) == NULL)\
                return NULL;\
        }\
\
        for (size_t i = 0; i < n; i++) {\
            self->items[self->len + i] = el;\
        }\
        self->len += n;\
        return self;\
    }

/* ********* static vec_realloc *********** */
#define VEC_REALLOC_FUNC_NAME(TYPE)            CAT(Vec(TYPE), _realloc)
#define VEC_REALLOC_FUNC_SIGNATURE(TYPE)       Vec(TYPE)* VEC_REALLOC_FUNC_NAME(TYPE)(Vec(TYPE)* vec, size_t size)
#define VEC_REALLOC_FUNC_IMPL(TYPE)\
    VEC_REALLOC_FUNC_SIGNATURE(TYPE) {\
        if (vec == NULL || vec->items == NULL) return NULL;\
        TYPE* new = realloc(\
            vec->items,\
            sizeof(TYPE) * size\
        );\
        if (new == NULL) return NULL;\
        vec->items = new;\
        vec->cap = size;\
        return vec;\
    }

#define vec_realloc_fn(TYPE, VEC, SIZE)        VEC_REALLOC_FUNC_NAME(TYPE)(VEC, SIZE)
#define vec_grow_fn(TYPE, VEC)                 vec_realloc_fn(TYPE, VEC, VEC->cap * 2)
#define vec_make_space_fn(TYPE, VEC, SPACE)    vec_realloc_fn(TYPE, VEC, vec_cap_from_size(SPACE))

/* ********* vec_set *********** */
#define VEC_SET_FUNC_NAME(TYPE)            CAT(Vec(TYPE), _set)
#define VEC_SET_FUNC_SIGNATURE(TYPE)       TYPE* VEC_SET_FUNC_NAME(TYPE)(Vec(TYPE)* self, TYPE val, size_t pos)
#define VEC_SET_FUNC_IMPL(TYPE)\
    VEC_SET_FUNC_SIGNATURE(TYPE) {\
        if (pos >= self->len) return NULL;\
        self->items[pos] = val;\
        return &self->items[pos];\
    }

/* ********* vec_get *********** */
#define VEC_GET_FUNC_NAME(TYPE)            CAT(Vec(TYPE), _get)
#define VEC_GET_FUNC_SIGNATURE(TYPE)       TYPE* VEC_GET_FUNC_NAME(TYPE)(Vec(TYPE)* self, size_t pos)
#define VEC_GET_FUNC_IMPL(TYPE)\
    VEC_GET_FUNC_SIGNATURE(TYPE) {\
        if (pos >= self->len) return NULL;\
        return &self->items[pos];\
    }

/* ********* vec_insert *********** */
#define VEC_INSERT_FUNC_NAME(TYPE)            CAT(Vec(TYPE), _insert)
#define VEC_INSERT_FUNC_SIGNATURE(TYPE)       Vec(TYPE)* VEC_INSERT_FUNC_NAME(TYPE)(Vec(TYPE)* self, TYPE el, size_t pos)
#define VEC_INSERT_FUNC_IMPL(TYPE)\
    VEC_INSERT_FUNC_SIGNATURE(TYPE) {\
        if (pos > self->len)\
            return NULL;\
        if (self->len == self->cap) {\
            if (vec_grow_fn(TYPE, self) == NULL)\
                return NULL;\
        }\
        memmove(\
            &self->items[pos+1],\
            &self->items[pos],\
            sizeof(TYPE) * (self->len - pos)\
        );\
        self->items[pos] = el;\
        self->len++;\
        return self;\
    }

/* ********* vec_remove *********** */
#define VEC_REMOVE_FUNC_NAME(TYPE)            CAT(Vec(TYPE), _remove)
#define VEC_REMOVE_FUNC_SIGNATURE(TYPE)       Vec(TYPE)* VEC_REMOVE_FUNC_NAME(TYPE)(Vec(TYPE)* self, size_t pos)
#define VEC_REMOVE_FUNC_IMPL(TYPE)\
    VEC_REMOVE_FUNC_SIGNATURE(TYPE) {\
        if (pos >= self->len) return NULL;\
        memmove(\
            &self->items[pos],\
            &self->items[pos+1],\
            sizeof(TYPE) * (self->len - pos - 1)\
        );\
        self->len--;\
        return self;\
    }

/* ********* vec_last *********** */
#define VEC_LAST_FUNC_NAME(TYPE)           CAT(Vec(TYPE), _last)
#define VEC_LAST_FUNC_SIGNATURE(TYPE)      TYPE* VEC_LAST_FUNC_NAME(TYPE)(Vec(TYPE)* self)
#define VEC_LAST_FUNC_IMPL(TYPE)\
    VEC_LAST_FUNC_SIGNATURE(TYPE) {\
        if (self->len == 0) return NULL;\
        return &self->items[self->len - 1];\
    }

/* ********* vec_first *********** */
#define VEC_FIRST_FUNC_NAME(TYPE)           CAT(Vec(TYPE), _first)
#define VEC_FIRST_FUNC_SIGNATURE(TYPE)      TYPE* VEC_FIRST_FUNC_NAME(TYPE)(Vec(TYPE)* self)
#define VEC_FIRST_FUNC_IMPL(TYPE)\
    VEC_FIRST_FUNC_SIGNATURE(TYPE) {\
        if (self->len == 0) return NULL;\
        return &self->items[0];\
    }

/* ********* vec_pop *********** */
/* Returns a pointer to the popped element. The pointer is valid only
 * until the next mutation (push, insert, pop) of the vec. */
#define VEC_POP_FUNC_NAME(TYPE)           CAT(Vec(TYPE), _pop)
#define VEC_POP_FUNC_SIGNATURE(TYPE)      TYPE* VEC_POP_FUNC_NAME(TYPE)(Vec(TYPE)* self)
#define VEC_POP_FUNC_IMPL(TYPE)\
    VEC_POP_FUNC_SIGNATURE(TYPE) {\
        if (self->len == 0) return NULL;\
        TYPE* el = vec_last_fn(TYPE, self);\
        self->len--;\
        if (self->len == 0)\
            self->curr = 0;\
        else if (self->curr >= self->len)\
            self->curr = self->len - 1;\
        return el;\
    }

/* ********* vec_free *********** */
#define VEC_FREE_FUNC_NAME(TYPE)           CAT(Vec(TYPE), _free)
#define VEC_FREE_FUNC_SIGNATURE(TYPE)      void VEC_FREE_FUNC_NAME(TYPE)(Vec(TYPE)* self)
#define VEC_FREE_FUNC_IMPL(TYPE)\
    VEC_FREE_FUNC_SIGNATURE(TYPE) {\
        free(self->items);\
        free(self);\
    }

size_t vec_cap_from_size(size_t size);

#define VEC_DEFS_COMMON(TYPE)\
    VEC_STRUCT_DEF(TYPE);\
    VEC_INIT_FUNC_SIGNATURE(TYPE);\
    VEC_NEW_FUNC_SIGNATURE(TYPE);\


#define VEC_IMPL_COMMON(TYPE)\
    static VEC_REALLOC_FUNC_IMPL(TYPE)\
    static VEC_INIT_BASE_FUNC_IMPL(TYPE)\
    VEC_EMPTY_FUNC_IMPL(TYPE)\
    VEC_PUSH_FUNC_IMPL(TYPE)\
    VEC_REPEAT_APPEND_FUNC_IMPL(TYPE)\
    VEC_SET_FUNC_IMPL(TYPE)\
    VEC_GET_FUNC_IMPL(TYPE)\
    VEC_INSERT_FUNC_IMPL(TYPE)\
    VEC_REMOVE_FUNC_IMPL(TYPE)\
    VEC_LAST_FUNC_IMPL(TYPE)\
    VEC_FIRST_FUNC_IMPL(TYPE)\
    VEC_POP_FUNC_IMPL(TYPE)\
    VEC_FREE_FUNC_IMPL(TYPE)\
    VEC_NEW_FUNC_IMPL(TYPE)\

#endif