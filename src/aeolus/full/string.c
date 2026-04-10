#include <bits/types/struct_iovec.h>
#include <stdlib.h>
#include <string.h>

#include "aeolus/string.h"
#include "aeolus/vec.h"

#include "aeolus/shared/string_impl.h"

__attribute__((weak)) const char _aeolus_variant_full = 0;

String str_new_with_cap(size_t initial_size) {
    if (initial_size < 2) initial_size = 2;
    String str;
    vec_init_with_cap(char, _str_vec(&str), initial_size);
    vec_push_fn(char, _str_vec(&str), '\0');
    ITER_INIT(String, &str);
    return str;
}
