#ifndef RESULT_H
#define RESULT_H

#include "utils.h"
#include "generics.h"
#include <bits/types/error_t.h>
#include <string.h>

/* *********** RESULT *********** */

typedef const char* Err;

#define Res(TYPE) GenericName(TYPE, Result)

#define RESULT_STRUCT_DEF(TYPE)\
    typedef struct {\
        Err err;\
        IF_VOID(TYPE)(\
            ,\
            TYPE val;\
        )\
    } Res(TYPE)

#define RES_OK 0

void set_errdbg_hooks(void (*pre)(void), void (*post)(void));
void errdbg(const char *msg, Err err, const char *filename, int linenumber);

/* ********* OK *********** */
#define ok(TYPE, ...)\
    (Res(TYPE)) {\
        .err = NULL,\
        IF_VOID(TYPE) ( , .val = __VA_ARGS__)\
    }

/* ********* ERR *********** */
#define ERR_FUNC_NAME(TYPE)          CAT(Res(TYPE), _err)
#define ERR_FUNC_SIGNATURE(TYPE)     Res(TYPE) ERR_FUNC_NAME(TYPE)(Err err, const char* filename, int linenumber)
#define ERR_FUNC_IMPL(TYPE) \
    ERR_FUNC_SIGNATURE(TYPE) { \
        errdbg("error", err, filename, linenumber);\
        return (Res(TYPE)) {\
            .err = err,\
        };\
    }
#define err(TYPE, ERR)  ERR_FUNC_NAME(TYPE)(ERR, __FILE__, __LINE__)


void set_die_hook(void (*hook)(void));
void die(const char *msg);
/* ********* ERRDIE *********** */
#define ERRDIE_FUNC_NAME(TYPE)          CAT(Res(TYPE), _errdie)
#define ERRDIE_FUNC_SIGNATURE(TYPE)     TYPE ERRDIE_FUNC_NAME(TYPE)(Res(TYPE) res, const char* msg)
#define ERRDIE_FUNC_IMPL(TYPE) \
    ERRDIE_FUNC_SIGNATURE(TYPE) { \
        if (res.err) { \
            die(msg);\
        } \
        IF_VOID(TYPE)( \
            , \
            return res.val; \
        ) \
    }
#define errdie(TYPE, RES, MSG)            ERRDIE_FUNC_NAME(TYPE)(RES, MSG)


/* ********* TRY *********** */
#define TRY_FUNC_NAME(TYPE)             CAT(Res(TYPE), _try)
#define TRY_FUNC_SIGNATURE(TYPE)        TYPE TRY_FUNC_NAME(TYPE)(Res(TYPE) res)
#define TRY_FUNC_IMPL(TYPE) \
    TRY_FUNC_SIGNATURE(TYPE) { \
        _res_set_try_err(res.err); \
        IF_VOID(TYPE)( \
            , \
            return res.val; \
        ) \
    }
#define try(TYPE, EXPR) \
    TRY_FUNC_NAME(TYPE)(EXPR); \
    if (_res_get_try_err()) {\
        errdbg("  try", _res_get_try_err(), __FILE__, __LINE__);\
        return (Res(TYPE)) {\
            .err = _res_get_try_err(),\
        };\
    }

/* ********* CATCH *********** */
#define catch(TYPE, EXPR, ERR) \
    TRY_FUNC_NAME(TYPE)(EXPR); \
    Err ERR = _res_get_try_err();\
    if (ERR) errdbg("catch", ERR, __FILE__, __LINE__);\
    if (ERR)

#define is_ok(RES)                   (RES.err == NULL)
#define is_err(RES)                  (RES.err != NULL)

void _res_set_try_err(Err err);
Err _res_get_try_err();

#define RESULT_DEFS(TYPE)\
    RESULT_STRUCT_DEF(TYPE);\
    ERR_FUNC_SIGNATURE(TYPE);\
    ERRDIE_FUNC_SIGNATURE(TYPE);\
    TRY_FUNC_SIGNATURE(TYPE);\

#define RESULT_IMPL(TYPE)\
    ERR_FUNC_IMPL(TYPE)\
    ERRDIE_FUNC_IMPL(TYPE)\
    TRY_FUNC_IMPL(TYPE)\


RESULT_DEFS(void)
RESULT_DEFS(int)
RESULT_DEFS(unsigned int)
RESULT_DEFS(char)


#endif