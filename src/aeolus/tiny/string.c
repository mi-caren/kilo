#include <bits/types/struct_iovec.h>
#include <stdlib.h>
#include <string.h>

#include "aeolus/tiny/string.h"
#include "aeolus/tiny/vec.h"

#include "aeolus/string/common_impl.h"

String str_new_with_cap(size_t initial_size) {
    if (initial_size < 2) initial_size = 2;
    String str;
    vec_init_with_cap(char, _str_vec(&str), initial_size);
    vec_push_fn(char, _str_vec(&str), '\0');
    return str;
}
