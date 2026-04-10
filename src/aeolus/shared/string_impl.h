/* Shared String function implementations.
 * Included by both string/string.c and tiny/string.c.
 * Requires String type and vec _fn macros to be available. */

#ifndef STRING_COMMON_IMPL_H
#define STRING_COMMON_IMPL_H

#include <assert.h>

/* Cast String* to Vec(char)* for calling vec functions.
 * Safe because String starts with VEC_STRUCT_BASE(char). */
#define _str_vec(self) ((Vec(char)*)(void*)(self))

STR_ITER_IMPL

String str_new() {
    return str_new_with_cap(2);
}

size_t str_len(String *self) {
    return self->len - 1;
}

char* str_chars(String* self) {
    return self->items;
}

char str_char_at(String* self, size_t pos) {
    assert(pos < self->len);
    return self->items[pos];
}

void str_set_at(String* self, size_t pos) {
    assert(pos < str_len(self));
    self->curr = pos;
}

size_t str_curri(String* self) {
    return self->curr;
}

void str_free(String* self) {
    free(self->items);
}

static void _str_push0(String* self) {
    vec_push_fn(char, _str_vec(self), '\0');
}

String str_from(char *chars) {
    size_t len = strlen(chars);
    String str = str_new_with_cap(len + 1);
    vec_pop_fn(char, _str_vec(&str));
    // TODO: create vec_from
    for (size_t i = 0; i < len; i ++)
        vec_push_fn(char, _str_vec(&str), chars[i]);
    _str_push0(&str);
    return str;
}

void str_empty(String *self) {
    vec_empty_fn(char, _str_vec(self));
    _str_push0(self);
}

String* str_insertc(String* self, char c, size_t pos) {
    if (pos > str_len(self)) return NULL;
    if (!vec_insert_fn(char, _str_vec(self), c, pos)) return NULL;
    return self;
}

String* str_insert(String* self, char* chars, size_t pos) {
    if (pos > str_len(self)) return NULL;
    // TODO: vec_insert_array
    for (size_t i = 0; i < strlen(chars); i++)
        vec_insert_fn(char, _str_vec(self), chars[i], pos+i);
    return self;
}

String* str_append(String* self, char* chars) {
    // TODO: vec_append
    vec_pop_fn(char, _str_vec(self));
    for (size_t i = 0; i < strlen(chars); i++)
        vec_push_fn(char, _str_vec(self), chars[i]);
    _str_push0(self);
    return self;
}

String* str_appendc(String* self, char c) {
    vec_pop_fn(char, _str_vec(self));
    vec_push_fn(char, _str_vec(self), c);
    _str_push0(self);
    return self;
}

String* str_appends(String*self, String* str) {
    vec_pop_fn(char, _str_vec(self));
    for (size_t i = 0; i < str_len(str); i++)
        vec_push_fn(char, _str_vec(self), str->items[i]);
    _str_push0(self);
    return self;
}

String* str_repeat_appendc(String* self, char c, size_t n) {
    vec_pop_fn(char, _str_vec(self));
    if (!vec_repeat_append_fn(char, _str_vec(self), c, n))
        return NULL;
    _str_push0(self);
    return self;
}

char str_remove(String* self, size_t pos) {
    if (pos >= str_len(self)) return '\0';
    char c = self->items[pos];
    vec_remove_fn(char, _str_vec(self), pos);
    return c;
}

void str_truncate(String* self, size_t new_len) {
    if (new_len > str_len(self))
        return;
    vec_pop_fn(char, _str_vec(self));
    while (self->len > new_len) {
        vec_pop_fn(char, _str_vec(self));
    }
    _str_push0(self);
}

#endif
