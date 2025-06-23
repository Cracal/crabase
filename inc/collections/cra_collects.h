/**
 * @file cra_collects.h
 * @author Cracal
 * @brief collections
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_COLLECTS_H__
#define __CRA_COLLECTS_H__
#include <math.h>
#include <float.h>
#include "cra_defs.h"

typedef void (*cra_remove_val_fn)(void *val);
typedef bool (*cra_match_fn)(void *val, void *arg);
typedef void (*cra_deep_copy_val_fn)(const void *from, void *to);

#if 1 // compare functions

typedef int (*cra_compare_fn)(const void *a, const void *b);

#define __CRA_COMPARE_FUNC(_name, _type)                          \
    static inline int cra_compare_##_name(_type a, _type b)       \
    {                                                             \
        return a == b ? 0 : (a > b ? 1 : -1);                     \
    }                                                             \
    static inline int cra_compare_##_name##_p(_type *a, _type *b) \
    {                                                             \
        return cra_compare_##_name(*a, *b);                       \
    }

__CRA_COMPARE_FUNC(int, int)
__CRA_COMPARE_FUNC(uint, unsigned int)
__CRA_COMPARE_FUNC(ssize_t, ssize_t)
__CRA_COMPARE_FUNC(size_t, size_t)

__CRA_COMPARE_FUNC(int8_t, int8_t)
__CRA_COMPARE_FUNC(int16_t, int16_t)
__CRA_COMPARE_FUNC(int32_t, int32_t)
__CRA_COMPARE_FUNC(int64_t, int64_t)
__CRA_COMPARE_FUNC(uint8_t, uint8_t)
__CRA_COMPARE_FUNC(uint16_t, uint16_t)
__CRA_COMPARE_FUNC(uint32_t, uint32_t)
__CRA_COMPARE_FUNC(uint64_t, uint64_t)

static inline int cra_compare_float(float a, float b)
{
    return fabsf(a - b) < FLT_EPSILON ? 0 : (a > b ? 1 : -1);
}

static inline int cra_compare_float_p(float *a, float *b)
{
    return cra_compare_float(*a, *b);
}

static inline int cra_compare_double(double a, double b)
{
    return fabs(a - b) < DBL_EPSILON ? 0 : (a > b ? 1 : -1);
}

static inline int cra_compare_double_p(double *a, double *b)
{
    return cra_compare_double(*a, *b);
}

#define cra_compare_string strcmp
static inline int cra_compare_string_p(const char **a, const char **b)
{
    return cra_compare_string(*a, *b);
}

__CRA_COMPARE_FUNC(ptr, const void *)

#undef __CRA_COMPARE_FUNC

#endif // end compare functions

#if 1 // hash functions

typedef ssize_t cra_hash_t;
#define CRA_HASH_MAX SSIZE_MAX
#define CRA_HASH_MIN SSIZE_MIN
typedef cra_hash_t (*cra_hash_fn)(const void *val);

#define __CRA_HASH_FUNC1(_name, _type)                        \
    static inline cra_hash_t cra_hash_##_name(_type val)      \
    {                                                         \
        return (cra_hash_t)val == -1 ? -2 : (cra_hash_t)val;  \
    }                                                         \
    static inline cra_hash_t cra_hash_##_name##_p(_type *val) \
    {                                                         \
        return cra_hash_##_name(*val);                        \
    }
#define __CRA_HASH_FUNC2(_name, _type)                        \
    static inline cra_hash_t cra_hash_##_name(_type val)      \
    {                                                         \
        return (cra_hash_t)(val == -1 ? -2 : val);            \
    }                                                         \
    static inline cra_hash_t cra_hash_##_name##_p(_type *val) \
    {                                                         \
        return cra_hash_##_name(*val);                        \
    }

__CRA_HASH_FUNC2(int, int)
__CRA_HASH_FUNC2(ssize_t, ssize_t)
__CRA_HASH_FUNC1(uint, unsigned int)
__CRA_HASH_FUNC1(size_t, size_t)

__CRA_HASH_FUNC2(int8_t, int8_t)
__CRA_HASH_FUNC2(int16_t, int16_t)
__CRA_HASH_FUNC2(int32_t, int32_t)
__CRA_HASH_FUNC2(int64_t, int64_t)
__CRA_HASH_FUNC1(uint8_t, uint8_t)
__CRA_HASH_FUNC1(uint16_t, uint16_t)
__CRA_HASH_FUNC1(uint32_t, uint32_t)
__CRA_HASH_FUNC1(uint64_t, uint64_t)

static inline cra_hash_t cra_hash_float(float val)
{
    if (!isnan(val))
    {
        union
        {
            int32_t i;
            float f;
        } u;
        u.f = val;
        return u.i == -1 ? -2 : u.i;
    }
    return 0x7fc00000;
}

static inline cra_hash_t cra_hash_float_p(float *val) { return cra_hash_float(*val); }

static inline cra_hash_t cra_hash_double(double val)
{
    if (!isnan(val))
    {
        union
        {
            cra_hash_t i;
            double d;
        } u;
        u.d = val;
        return u.i == -1 ? -2 : u.i;
    }
    return 0x7ff8000000000000L;
}

static inline cra_hash_t cra_hash_double_p(double *val) { return cra_hash_double(*val); }

CRA_API cra_hash_t cra_hash_string1(const char *val);
CRA_API cra_hash_t cra_hash_string2(const char *val);
static inline cra_hash_t cra_hash_string1_p(const char **val) { return cra_hash_string1(*val); }
static inline cra_hash_t cra_hash_string2_p(const char **val) { return cra_hash_string2(*val); }

__CRA_HASH_FUNC1(ptr, const void *)

#undef __CRA_HASH_FUNC1
#undef __CRA_HASH_FUNC2

#endif // end hash functions

#endif