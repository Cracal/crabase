/**
 * @file cra_atomic.h
 * @author Cracal
 * @brief atomic
 * @version 0.1
 * @date 2024-09-17
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_ATOMIC_H__
#define __CRA_ATOMIC_H__
#include "cra_defs.h"

#ifndef __STDC_NO_ATOMICS__
#include <stdatomic.h>

typedef _Atomic int32_t cra_atomic_int32_t;
typedef _Atomic int64_t cra_atomic_int64_t;

static inline int32_t
cra_atomic_load32(cra_atomic_int32_t *p)
{
    return atomic_load(p);
}

static inline int64_t
cra_atomic_load64(cra_atomic_int64_t *p)
{
    return atomic_load(p);
}

static inline void
cra_atomic_store32(cra_atomic_int32_t *p, int32_t v)
{
    atomic_store(p, v);
}

static inline void
cra_atomic_store64(cra_atomic_int64_t *p, int64_t v)
{
    atomic_store(p, v);
}

// o = *p; *p += v; return o;
static inline int32_t
cra_atomic_add32(cra_atomic_int32_t *p, int32_t v)
{
    return atomic_fetch_add(p, v);
}

// o = *p; *p += v; return o;
static inline int64_t
cra_atomic_add64(cra_atomic_int64_t *p, int64_t v)
{
    return atomic_fetch_add(p, v);
}

// o = *p; *p -= v; return o;
static inline int32_t
cra_atomic_sub32(cra_atomic_int32_t *p, int32_t v)
{
    return atomic_fetch_sub(p, v);
}

// o = *p; *p -= v; return o;
static inline int64_t
cra_atomic_sub64(cra_atomic_int64_t *p, int64_t v)
{
    return atomic_fetch_sub(p, v);
}

// return (*)++;
static inline int32_t
cra_atomic_inc32(cra_atomic_int32_t *p)
{
    return atomic_fetch_add(p, 1);
}

// return (*)++;
static inline int64_t
cra_atomic_inc64(cra_atomic_int64_t *p)
{
    return atomic_fetch_add(p, 1);
}

// return (*)--;
static inline int32_t
cra_atomic_dec32(cra_atomic_int32_t *p)
{
    return atomic_fetch_sub(p, 1);
}

// return (*)--;
static inline int64_t
cra_atomic_dec64(cra_atomic_int64_t *p)
{
    return atomic_fetch_sub(p, 1);
}

// return (*p == cmp_val) ? (*p = set_val, true) : false
static inline bool
cra_atomic_compare_and_set32(cra_atomic_int32_t *p, int32_t cmp_val, int32_t set_val)
{
    return atomic_compare_exchange_strong(p, &(typeof(cmp_val)){ cmp_val }, set_val);
}

// return (*p == cmp_val) ? (*p = set_val, true) : false
static inline bool
cra_atomic_compare_and_set64(cra_atomic_int64_t *p, int64_t cmp_val, int64_t set_val)
{
    return atomic_compare_exchange_strong(p, &(typeof(cmp_val)){ cmp_val }, set_val);
}

typedef atomic_flag cra_atomic_flag_t;

#define CRA_ATOMIC_FLAG_INIT ATOMIC_FLAG_INIT

// return (*p == false) ? (*p = true, false) : true
static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p)
{
    return atomic_flag_test_and_set(p);
}

// *p = false
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p)
{
    atomic_flag_clear(p);
}

#else

#ifdef CRA_COMPILER_MSVC

typedef volatile LONG   cra_atomic_int32_t;
typedef volatile LONG64 cra_atomic_int64_t;

static inline void
cra_atomic_store32(cra_atomic_int32_t *p, int32_t v)
{
    InterlockedExchange(p, v);
}

static inline void
cra_atomic_store64(cra_atomic_int64_t *p, int64_t v)
{
    InterlockedExchange64(p, v);
}

// o = *p; *p += v; return o;
static inline int32_t
cra_atomic_add32(cra_atomic_int32_t *p, int32_t v)
{
    int32_t ret = *p;
    InterlockedAdd(p, v);
    return ret;
}

// o = *p; *p += v; return o;
static inline int64_t
cra_atomic_add64(cra_atomic_int64_t *p, int64_t v)
{
    int64_t ret = *p;
    InterlockedAdd64(p, v);
    return ret;
}

// o = *p; *p -= v; return o;
static inline int32_t
cra_atomic_sub32(cra_atomic_int32_t *p, int32_t v)
{
    int32_t ret = *p;
    InterlockedAdd(p, -v);
    return ret;
}

// o = *p; *p -= v; return o;
static inline int64_t
cra_atomic_sub64(cra_atomic_int64_t *p, int64_t v)
{
    int64_t ret = *p;
    InterlockedAdd64(p, -v);
    return ret;
}

// return (*)++;
static inline int32_t
cra_atomic_inc32(cra_atomic_int32_t *p)
{
    int32_t ret = *p;
    InterlockedIncrement(p);
    return ret;
}

// return (*)++;
static inline int64_t
cra_atomic_inc64(cra_atomic_int64_t *p)
{
    int64_t ret = *p;
    InterlockedIncrement64(p);
    return ret;
}

// return (*)--;
static inline int32_t
cra_atomic_dec32(cra_atomic_int32_t *p)
{
    int32_t ret = *p;
    InterlockedDecrement(p);
    return ret;
}

// return (*)--;
static inline int64_t
cra_atomic_dec64(cra_atomic_int64_t *p)
{
    int64_t ret = *p;
    InterlockedDecrement64(p);
    return ret;
}

// return (*p == cmp_val) ? (*p = set_val, true) : false
static inline bool
cra_atomic_compare_and_set32(cra_atomic_int32_t *p, int32_t cmp_val, int32_t set_val)
{
    return InterlockedCompareExchange(p, set_val, cmp_val) == cmp_val;
}

// return (*p == cmp_val) ? (*p = set_val, true) : false
static inline bool
cra_atomic_compare_and_set64(cra_atomic_int64_t *p, int64_t cmp_val, int64_t set_val)
{
    return InterlockedCompareExchange64(p, set_val, cmp_val) == cmp_val;
}

typedef LONG cra_atomic_flag_t;

// return (*p == false) ? (*p = true, false) : true
static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p)
{
    return InterlockedExchange(p, 1) == 1;
}

// *p = false
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p)
{
    InterlockedExchange(p, 0);
}

#elif defined(CRA_COMPILER_GNUC)

typedef volatile int32_t cra_atomic_int32_t;
typedef volatile int64_t cra_atomic_int64_t;

static inline void
cra_atomic_store32(cra_atomic_int32_t *p, int32_t v)
{
    *p = v;
}

static inline void
cra_atomic_store64(cra_atomic_int64_t *p, int64_t v)
{
    *p = v;
}

// o = *p; *p += v; return o;
static inline int32_t
cra_atomic_add32(cra_atomic_int32_t *p, int32_t v)
{
    return __sync_fetch_and_add(p, v);
}

// o = *p; *p += v; return o;
static inline int64_t
cra_atomic_add64(cra_atomic_int64_t *p, int64_t v)
{
    return __sync_fetch_and_add(p, v);
}

// o = *p; *p -= v; return o;
static inline int32_t
cra_atomic_sub32(cra_atomic_int32_t *p, int32_t v)
{
    return __sync_fetch_and_sub(p, v);
}

// o = *p; *p -= v; return o;
static inline int64_t
cra_atomic_sub64(cra_atomic_int64_t *p, int64_t v)
{
    return __sync_fetch_and_sub(p, v);
}

// return (*)++;
static inline int32_t
cra_atomic_inc32(cra_atomic_int32_t *p)
{
    return cra_atomic_add32(p, 1);
}

// return (*)++;
static inline int64_t
cra_atomic_inc64(cra_atomic_int64_t *p)
{
    return cra_atomic_add64(p, 1);
}

// return (*)--;
static inline int32_t
cra_atomic_dec32(cra_atomic_int32_t *p)
{
    return cra_atomic_sub32(p, 1);
}

// return (*)--;
static inline int64_t
cra_atomic_dec64(cra_atomic_int64_t *p)
{
    return cra_atomic_sub64(p, 1);
}

// return (*p == cmp_val) ? (*p = set_val, true) : false
static inline bool
cra_atomic_compare_and_set32(cra_atomic_int32_t *p, int32_t cmp_val, int32_t set_val)
{
    return __sync_val_compare_and_swap(p, cmp_val, set_val) == cmp_val;
}

// return (*p == cmp_val) ? (*p = set_val, true) : false
static inline bool
cra_atomic_compare_and_set64(cra_atomic_int64_t *p, int64_t cmp_val, int64_t set_val)
{
    return __sync_val_compare_and_swap(p, cmp_val, set_val) == cmp_val;
}

typedef int cra_atomic_flag_t;

// return (*p == false) ? (*p = true, false) : true
static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p)
{
    return __sync_lock_test_and_set(p, 1) != 0;
}

// *p = false
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p)
{
    __sync_lock_release(p);
}

#endif

static inline int32_t
cra_atomic_load32(cra_atomic_int32_t *p)
{
    return *p;
}

static inline int64_t
cra_atomic_load64(cra_atomic_int64_t *p)
{
    return *p;
}

#define CRA_ATOMIC_FLAG_INIT 0

#endif

#endif