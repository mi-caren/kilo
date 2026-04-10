#ifndef STRING_COMMON_H
#define STRING_COMMON_H

#include <unistd.h>

String str_new();
String str_new_with_cap(size_t initial_size);
String str_from(char* chars);

void str_empty(String* self);
size_t str_len(String* self);
char* str_chars(String* self);
String* str_insertc(String* self, char c, size_t pos);
String* str_insert(String* self, char* chars, size_t pos);
char str_remove(String* self, size_t pos);
/* chars MUST be 0 terminated */
String* str_append(String* self, char* chars);
String* str_appendc(String* self, char c);
String* str_appends(String*self, String* str);
String* str_repeat_appendc(String* self, char c, size_t n);
void str_truncate(String* self, size_t new_len);
char str_char_at(String* self, size_t pos);
void str_free(String* self);

// TODO: iterator methods
void str_set_at(String* self, size_t pos);
size_t str_curri(String* self);

#define STR_ITER_IMPL\
    ITER_IMPL(\
        String,\
        /* CURR_IMPL */ {\
            size_t len = str_len(self);\
            if (len == 0) return NULL;\
            if (self->curr >= len) return NULL;\
            return &self->items[self->curr];\
        },\
        /* BEGIN_IMPL */ {\
            self->curr = 0;\
            return ITER_CURR_FUNC_NAME(String)(self);\
        },\
        /* END_IMPL */ {\
            size_t len = str_len(self);\
            if (len == 0) return NULL;\
            self->curr = len - 1;\
            return ITER_CURR_FUNC_NAME(String)(self);\
        },\
        /* PREV_IMPL */ {\
            if (self->curr == 0) return NULL;\
            self->curr--;\
            return ITER_CURR_FUNC_NAME(String)(self);\
        },\
        /* NEXT_IMPL */ {\
            size_t len = str_len(self);\
            if (self->curr >= len - 1) return NULL;\
            self->curr++;\
            return ITER_CURR_FUNC_NAME(String)(self);\
        }\
    )

#endif
