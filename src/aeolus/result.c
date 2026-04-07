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


static void (*_errdbg_pre_hook)(void) = NULL;
static void (*_errdbg_post_hook)(void) = NULL;

void set_errdbg_hooks(void (*pre)(void), void (*post)(void)) {
    _errdbg_pre_hook = pre;
    _errdbg_post_hook = post;
}

void errdbg(const char *msg, Err err, const char *filename, int linenumber) {
#ifndef NDEBUG
    if (_errdbg_pre_hook) _errdbg_pre_hook();
    fprintf(stderr, "[errdbg] %s:%d\t %s: %s\n\r", filename, linenumber, msg, err);
    if (_errdbg_post_hook) _errdbg_post_hook();
#else
    (void)msg; (void)err; (void)filename; (void)linenumber;
#endif
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
