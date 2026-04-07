#include <errno.h>
#include <stdio.h>
#include <threads.h>
#include <unistd.h>
#include <stdlib.h>

#include "result.h"

static thread_local Err _res_try_err = NULL;

void _res_set_try_err(Err err) {
    _res_try_err = err;
}

Err _res_get_try_err() {
    return _res_try_err;
}


static void (*_die_hook)(void) = NULL;

void set_die_hook(void (*hook)(void)) {
    _die_hook = hook;
}

void die(const char *msg) {
    if (_die_hook) _die_hook();
    if (errno) {
        perror(msg);
    } else {
        fprintf(stderr, "%s\n\r", msg);
    }
    exit(EXIT_FAILURE);
}

RESULT_IMPL(void)
RESULT_IMPL(int)
RESULT_IMPL(unsigned int)
RESULT_IMPL(char)
