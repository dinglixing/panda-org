
#ifndef __LANG_VAL_INC__
#define __LANG_VAL_INC__

#include "config.h"

typedef uint64_t val_t;
typedef union {
    val_t   v;
    double  d;
} valnum_t;

/*
 *  Double-precision floating-point number, IEEE 754
 *
 *  64 bit (8 bytes) in total
 *  1  bit sign
 *  11 bits exponent
 *  52 bits mantissa
 *      7         6        5        4        3        2        1        0
 *  seeeeeee|eeeemmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm
 *
 * If an exponent is all-1 and mantissa is all-0, then it is an INFINITY:
 *  11111111|11110000|00000000|00000000|00000000|00000000|00000000|00000000
 *
 * If an exponent is all-1 and mantissa's MSB is 1, it is a quiet NaN:
 *  11111111|11111000|00000000|00000000|00000000|00000000|00000000|00000000
 *
 * object_t NaN-packing:
 *    sign and exponent is 0xfff
 *    4 bits specify type (tag), must be non-zero
 *    48 bits specify value
 *
 *  11111111|1111tttt|vvvvvvvv|vvvvvvvv|vvvvvvvv|vvvvvvvv|vvvvvvvv|vvvvvvvv
 *   NaN marker |type|  48-bit placeholder for values: pointers, strings
 *
 * On 64-bit platforms, pointers are really 48 bit only, so they can fit,
 * provided they are sign extended
 */
#define MAKE_TAG(s, t) \
  ((val_t)(s) << 63 | (val_t) 0x7ff0 <<48 | (val_t)(t) <<48)

#define TAG_INFINITE        MAKE_TAG(0, 0)
#define TAG_FUNC_SCRIPT     MAKE_TAG(1, 1) // define (script) function
#define TAG_FUNC_NATIVE     MAKE_TAG(1, 2) // static (native) function
#define TAG_STRING_S        MAKE_TAG(1, 3) // static string
#define TAG_STRING_I        MAKE_TAG(1, 4) // inline string
#define TAG_STRING_O        MAKE_TAG(1, 5) // owned (computed) string
#define TAG_UNDEFINED       MAKE_TAG(1, 6)
#define TAG_BOOLEAN         MAKE_TAG(1, 7)
#define TAG_NAN             MAKE_TAG(1, 8)

#define TAG_OBJECT          MAKE_TAG(1, 9)
#define TAG_ARRAY           MAKE_TAG(1, 0xA)
#define TAG_DICTIONARY      MAKE_TAG(1, 0xB)
#define TAG_BUFFER          MAKE_TAG(1, 0xC)

#define TAG_REFERENCE       MAKE_TAG(1, 0xE)

#define TAG_MASK            MAKE_TAG(1, 0xF)
#define VAR_MASK            (~MAKE_TAG(1, 0xF))

static inline double val_2_double(val_t *v) {
    return ((valnum_t*)v)->d;
}

static inline int val_2_integer(val_t *v) {
    return (int) (((valnum_t*)v)->d);
}

static inline intptr_t val_2_intptr(val_t *v) {
    return (intptr_t)(*v & VAR_MASK);
}

static inline val_t *val_2_reference(val_t *v) {
    return ((val_t *)(*v & VAR_MASK));
}

static inline val_t double_2_val(double d) {
    return ((valnum_t*)&d)->v;
}

static inline int val_is_number(val_t *v) {
    return (*v & TAG_INFINITE) != TAG_INFINITE;
}

static inline int val_is_nan(val_t *v) {
    return (*v & TAG_MASK) == TAG_NAN;
}

static inline int val_is_script(val_t *v) {
    return (*v & TAG_MASK) == TAG_FUNC_SCRIPT;
}

static inline int val_is_native(val_t *v) {
    return (*v & TAG_MASK) == TAG_FUNC_NATIVE;
}

static inline int val_is_function(val_t *v) {
    return val_is_script(v) || val_is_native(v);
}

static inline int val_is_undefined(val_t *v) {
    return (*v & TAG_MASK) == TAG_UNDEFINED;
}

static inline int val_is_boolean(val_t *v) {
    return (*v & TAG_MASK) == TAG_BOOLEAN;
}

static inline int val_is_owned_string(val_t *v) {
    return (*v & TAG_MASK) == TAG_STRING_O;
}

static inline int val_is_static_string(val_t *v) {
    return (*v & TAG_MASK) == TAG_STRING_S;
}

static inline int val_is_inline_string(val_t *v) {
    return (*v & TAG_MASK) == TAG_STRING_I;
}

static inline int val_is_string(val_t *v) {
    return val_is_owned_string(v) || val_is_static_string(v) || val_is_inline_string(v);
}

static inline const char *val_2_cstring(val_t *v) {
    uint64_t t = *v & TAG_MASK;

    if (t == TAG_STRING_I) {
        return ((const char *)v) + 2;
    } else
    if (t == TAG_STRING_O || t == TAG_STRING_S) {
        return (const char *) val_2_intptr(v);
    } else {
        return NULL;
    }
}

static inline int val_is_object(val_t *v) {
    return (*v & TAG_MASK) == TAG_OBJECT;
}

static inline int val_is_reference(val_t *v) {
    return (*v & TAG_MASK) == TAG_REFERENCE;
}

static inline int val_is_array(val_t *v) {
    return (*v & TAG_MASK) == TAG_ARRAY;
}

static inline int val_is_dictionary(val_t *v) {
    return (*v & TAG_MASK) == TAG_DICTIONARY;
}

static inline int val_is_true(val_t *v) {
    if (val_is_reference(v)) v = val_2_reference(v);

    return val_is_boolean(v) ? val_2_intptr(v) :
           val_is_number(v) ? val_2_double(v) != 0 :
           //val_is_string(v) ? string_is_true(val_2_string(v)) :
           //val_is_array(v) ? array_is_true(val_2_intptr(v)) :
           //val_is_dictionary(v) ? dictionary_is_true(val_2_intptr(v)) :
           //val_is_object(v) ? object_is_true(val_2_intptr(v)) :
           //val_is_function ? function_is_true(val_2_intptr(v)) :
           0; // "undefined" and "nan", always be false.
}

static inline val_t val_mk_number(double d) {
    return double_2_val(d);
}

static inline val_t val_mk_nan(void) {
    return TAG_NAN;
}

static inline val_t val_mk_undefined(void) {
    return TAG_UNDEFINED;
}

static inline val_t val_mk_script(intptr_t s) {
    return TAG_FUNC_SCRIPT | s;
}

static inline val_t val_mk_native(intptr_t n) {
    return TAG_FUNC_NATIVE | n;
}

static inline val_t val_mk_boolean(int v) {
    return TAG_BOOLEAN | (!!v);
}

static inline val_t val_mk_static_string(intptr_t s) {
    return TAG_STRING_S | s;
}

static inline val_t val_mk_owned_string(intptr_t s) {
    return TAG_STRING_O | s;
}

static inline val_t val_mk_reference(const val_t *ref) {
    return TAG_REFERENCE | (val_t) ref;
}

static inline val_t val_mk_array(void *ptr) {
    return TAG_ARRAY | (val_t) ptr;
}

static inline val_t val_mk_dictionary(void *ptr) {
    return TAG_DICTIONARY | (val_t) ptr;
}

static inline void val_set_nan(val_t *p) {
    *((uint64_t *)p) = TAG_NAN;
}

static inline void val_set_undefined(val_t *p) {
    *((uint64_t *)p) = TAG_UNDEFINED;
}

static inline void val_set_boolean(val_t *p, int b) {
    *((uint64_t *)p) = TAG_BOOLEAN | !!b;
}

static inline void val_set_number(val_t *p, double d) {
    *((double *)p) = d;
}

static inline void val_set_reference(val_t *p, val_t *r) {
    *((uint64_t *)p) = TAG_REFERENCE | (val_t) r;
}

static inline void val_set_string(val_t *p, intptr_t s) {
    *((uint64_t *)p) = TAG_STRING_S | s;
}

static inline void val_set_script(val_t *p, intptr_t s) {
    *((uint64_t *)p) = TAG_FUNC_SCRIPT | s;
}

static inline void val_set_native(val_t *p, intptr_t f) {
    *((uint64_t *)p) = TAG_FUNC_NATIVE | f;
}

static inline void val_set_array(val_t *p, intptr_t a) {
    *((uint64_t *)p) = TAG_ARRAY | a;
}

static inline void val_set_dictionary(val_t *p, intptr_t d) {
    *((uint64_t *)p) = TAG_DICTIONARY | d;
}

static inline void val_set_object(val_t *p, intptr_t o) {
    *((uint64_t *)p) = TAG_OBJECT | o;
}

#endif /* __LANG_VALUE_INC__ */

