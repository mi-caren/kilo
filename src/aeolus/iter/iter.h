/* Iterator — generic iterator interface via function pointer tables.
 *
 * To make a type iterable:
 *   1. Add ITER_DRIVER(MyType) as a field in the struct
 *   2. Call ITER_DEFS(MyType, ItemType) in the header to declare types
 *   3. Call ITER_IMPL(MyType, ...) in the .c file with bodies for
 *      curr, begin, end, prev, next
 *   4. Call ITER_INIT(MyType, instance) to assign the driver
 *
 * Usage:
 *   for (EACH(item, &my_collection)) { ... }      // forward iteration
 *   for (EACH_REV(item, &my_collection)) { ... }   // reverse iteration
 *
 * Iterator functions return pointers to elements (IterItem*), not
 * copies. Returning by value would force a copy on every step, which
 * is wasteful for large element types. It would also be dangerous
 * for types that contain pointers to owned memory (like Vec or String):
 * the copy would share the same backing memory as the original,
 * and mutating or freeing either one would corrupt the other.
 * With a pointer, the caller reads in place or copies explicitly.
 *
 * The iterator state (cursor) is stored on the collection itself,
 * not on a separate iterator struct. This means nested iteration
 * over the same collection is not supported. */

#ifdef AEOLUS_TINY
#error "Cannot include full aeolus headers alongside tiny headers"
#endif
#define AEOLUS_FULL

#ifndef ITERATOR_H
#define ITERATOR_H

#include "aeolus/iter/common.h"

#define ITER_DRIVER_DEF(ITERABLE)\
    struct Iterator(ITERABLE) {\
        IterItem(ITERABLE)* (*curr)  (ITERABLE* const self);\
        IterItem(ITERABLE)* (*begin) (ITERABLE* const self);\
        IterItem(ITERABLE)* (*end)   (ITERABLE* const self);\
        IterItem(ITERABLE)* (*prev)  (ITERABLE* const self);\
        IterItem(ITERABLE)* (*next)  (ITERABLE* const self);\
    }

#define ITER_DRIVER(ITERABLE)   const struct Iterator(ITERABLE)* iter
#define ITER_INIT(ITERABLE, IT)\
    static const struct Iterator(ITERABLE)\
    CAT(Iterator(ITERABLE), _iter) = {\
        .curr  = ITER_CURR_FUNC_NAME(ITERABLE),\
        .begin = ITER_BEGIN_FUNC_NAME(ITERABLE),\
        .end   = ITER_END_FUNC_NAME(ITERABLE),\
        .prev  = ITER_PREV_FUNC_NAME(ITERABLE),\
        .next  = ITER_NEXT_FUNC_NAME(ITERABLE),\
    };\
    (IT)->iter = &CAT(Iterator(ITERABLE), _iter)

#define iter_curr(SELF)                 (SELF)->iter->curr(SELF)
#define iter_begin(SELF)                (SELF)->iter->begin(SELF)
#define iter_end(SELF)                  (SELF)->iter->end(SELF)
#define iter_prev(SELF)                 (SELF)->iter->prev(SELF)
#define iter_next(SELF)                 (SELF)->iter->next(SELF)

#define ITER_DEFS(ITERABLE, TYPE)\
    typedef TYPE IterItem(ITERABLE);\
    ITER_DRIVER_DEF(ITERABLE);\

#define EACH(EL, IT) \
    auto EL = iter_begin(IT); EL != NULL; EL = iter_next(IT)

#define EACH_REV(EL, IT) \
    auto EL = iter_end(IT); EL != NULL; EL = iter_prev(IT)


#endif
