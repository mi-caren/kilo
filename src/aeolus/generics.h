#ifndef GENERICS_H
#define GENERICS_H

/*
 * GenericName(TYPE, PREFIX) — produces a valid C identifier from a type
 * and a prefix. Single-token types use simple concatenation:
 *   GenericName(char, Vec) → Vecchar
 *   GenericName(void, Result) → Resultvoid
 *
 * Multi-token types (unsigned int, struct Foo) need special handling
 * because CAT can't paste spaces. These are detected by IF_UNSIGNED /
 * IF_STRUCT and routed through a deferred parenthesis trick:
 *   GenericName(unsigned int, Vec) → VecUnsignedint
 *   GenericName(struct Foo, Result) → ResultStructFoo
 *
 * The trick works by expanding e.g. CAT(GENERIC_, unsigned int, ...)
 * which produces GENERIC_unsigned int, ... — then GENERIC_unsigned
 * expands to GENERIC_UNSIGNED( which captures the remaining tokens
 * (int, PREFIX) as arguments to produce PREFIX##Unsigned##int.
 *
 * Pointer types (int*, char*) are NOT supported because the * is a
 * trailing token that the preprocessor cannot detect or capture with
 * prefix matching. Use a typedef instead:
 *   typedef int* IntPtr;
 *   Vec(IntPtr)
 */
#define GenericName(TYPE, PREFIX)\
    IF_UNSIGNED(TYPE)(\
        CAT(GENERIC_, TYPE, PAREN_CLOSE(PREFIX)),\
        IF_STRUCT(TYPE)(\
            CAT(GENERIC_, TYPE, PAREN_CLOSE(PREFIX)),\
            CAT(PREFIX, TYPE)\
        )\
    )


#define GENERIC_unsigned                GENERIC_UNSIGNED(
#define GENERIC_UNSIGNED(TYPE, PREFIX)  PREFIX##Unsigned##TYPE
#define GENERIC_struct                  GENERIC_STRUCT(
#define GENERIC_STRUCT(TYPE, PREFIX)    PREFIX##Struct##TYPE

#endif