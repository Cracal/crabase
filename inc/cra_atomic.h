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

#if !defined(__STDC_NO_ATOMICS__)

// stdatomic
#include <stdatomic.h>
#define CRA_USE_STDATOMIC

#elif defined(CRA_COMPILER_GNUC) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))

// __atomic
#define CRA_USE___ATOMIC

#elif defined(CRA_COMPILER_MSVC)

// Interlocked
#define CRA_USE_INTERLOCKED

#elif defined(CRA_COMPILER_GNUC)

// __sync
#define CRA_USE___SYNC

#else

#error "No atomic implementation available"

#endif

// ========================== type ==========================

#if defined(CRA_USE_STDATOMIC)

typedef _Atomic(void *)  cra_atomic_ptr_t;
typedef _Atomic(int32_t) cra_atomic_int32_t;
typedef _Atomic(int64_t) cra_atomic_int64_t;
typedef atomic_flag      cra_atomic_flag_t;

#define CRA_ATOMIC_FLAG_INIT ATOMIC_FLAG_INIT

#elif defined(CRA_USE_INTERLOCKED)

typedef PVOID  cra_atomic_ptr_t;
typedef LONG   cra_atomic_int32_t;
typedef LONG64 cra_atomic_int64_t;
typedef CHAR   cra_atomic_flag_t;

#define CRA_ATOMIC_FLAG_INIT 0

#else

typedef void   *cra_atomic_ptr_t;
typedef int32_t cra_atomic_int32_t;
typedef int64_t cra_atomic_int64_t;
typedef bool    cra_atomic_flag_t;

#define CRA_ATOMIC_FLAG_INIT false

#endif

// ========================== memory order ==========================

#if defined(CRA_USE_STDATOMIC)

typedef enum
{
    CRA_MO_RELAXED = memory_order_relaxed,
    CRA_MO_CONSUME = memory_order_consume,
    CRA_MO_ACQUIRE = memory_order_acquire,
    CRA_MO_RELEASE = memory_order_release,
    CRA_MO_ACQ_REL = memory_order_acq_rel,
    CRA_MO_SEQ_CST = memory_order_seq_cst
} CraMO_e;

#elif defined(CRA_USE___ATOMIC)

typedef enum
{
    CRA_MO_RELAXED = __ATOMIC_RELAXED,
    CRA_MO_CONSUME = __ATOMIC_CONSUME,
    CRA_MO_ACQUIRE = __ATOMIC_ACQUIRE,
    CRA_MO_RELEASE = __ATOMIC_RELEASE,
    CRA_MO_ACQ_REL = __ATOMIC_ACQ_REL,
    CRA_MO_SEQ_CST = __ATOMIC_SEQ_CST
} CraMO_e;

#else

typedef enum
{
    CRA_MO_RELAXED,
    CRA_MO_CONSUME,
    CRA_MO_ACQUIRE,
    CRA_MO_RELEASE,
    CRA_MO_ACQ_REL,
    CRA_MO_SEQ_CST
} CraMO_e;

#endif

// ========================== functions ==========================

#if defined(CRA_USE_STDATOMIC)

// return *p
static inline void *
cra_atomic_load_ptr(cra_atomic_ptr_t *p, CraMO_e mo)
{
    return atomic_load_explicit(p, mo);
}

// return *p
static inline int32_t
cra_atomic_load32(cra_atomic_int32_t *p, CraMO_e mo)
{
    return atomic_load_explicit(p, mo);
}

// return *p
static inline int64_t
cra_atomic_load64(cra_atomic_int64_t *p, CraMO_e mo)
{
    return atomic_load_explicit(p, mo);
}

// *p = v
static inline void
cra_atomic_store_ptr(cra_atomic_ptr_t *p, void *v, CraMO_e mo)
{
    atomic_store_explicit(p, v, mo);
}

// *p = v
static inline void
cra_atomic_store32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    atomic_store_explicit(p, v, mo);
}

// *p = v
static inline void
cra_atomic_store64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    atomic_store_explicit(p, v, mo);
}

// o = *p; *p += v; return o;
static inline int32_t
cra_atomic_add32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    return atomic_fetch_add_explicit(p, v, mo);
}

// o = *p; *p += v; return o;
static inline int64_t
cra_atomic_add64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    return atomic_fetch_add_explicit(p, v, mo);
}

// o = *p; *p -= v; return o;
static inline int32_t
cra_atomic_sub32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    return atomic_fetch_sub_explicit(p, v, mo);
}

// o = *p; *p -= v; return o;
static inline int64_t
cra_atomic_sub64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    return atomic_fetch_sub_explicit(p, v, mo);
}

// return (*p)++;
static inline int32_t
cra_atomic_inc32(cra_atomic_int32_t *p, CraMO_e mo)
{
    return atomic_fetch_add_explicit(p, 1, mo);
}

// return (*p)++;
static inline int64_t
cra_atomic_inc64(cra_atomic_int64_t *p, CraMO_e mo)
{
    return atomic_fetch_add_explicit(p, 1, mo);
}

// return (*p)--;
static inline int32_t
cra_atomic_dec32(cra_atomic_int32_t *p, CraMO_e mo)
{
    return atomic_fetch_sub_explicit(p, 1, mo);
}

// return (*p)--;
static inline int64_t
cra_atomic_dec64(cra_atomic_int64_t *p, CraMO_e mo)
{
    return atomic_fetch_sub_explicit(p, 1, mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak_ptr(cra_atomic_ptr_t *p, void *oldval, void *newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return atomic_compare_exchange_weak_explicit(p, &oldval, newval, succ_mo, fail_mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak32(cra_atomic_int32_t *p, int32_t oldval, int32_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return atomic_compare_exchange_weak_explicit(p, &oldval, newval, succ_mo, fail_mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak64(cra_atomic_int64_t *p, int64_t oldval, int64_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return atomic_compare_exchange_weak_explicit(p, &oldval, newval, succ_mo, fail_mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong_ptr(cra_atomic_ptr_t *p, void *oldval, void *newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return atomic_compare_exchange_strong_explicit(p, &oldval, newval, succ_mo, fail_mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong32(cra_atomic_int32_t *p, int32_t oldval, int32_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return atomic_compare_exchange_strong_explicit(p, &oldval, newval, succ_mo, fail_mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong64(cra_atomic_int64_t *p, int64_t oldval, int64_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return atomic_compare_exchange_strong_explicit(p, &oldval, newval, succ_mo, fail_mo);
}

// return (*p == false) ? (*p = true, false) : true
static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p, CraMO_e mo)
{
    return atomic_flag_test_and_set_explicit(p, mo);
}

// *p = false
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p, CraMO_e mo)
{
    atomic_flag_clear_explicit(p, mo);
}

#elif defined(CRA_USE___ATOMIC)

// return *p
static inline void *
cra_atomic_load_ptr(cra_atomic_ptr_t *p, CraMO_e mo)
{
    return __atomic_load_n(p, mo);
}

// return *p
static inline int32_t
cra_atomic_load32(cra_atomic_int32_t *p, CraMO_e mo)
{
    return __atomic_load_n(p, mo);
}

// return *p
static inline int64_t
cra_atomic_load64(cra_atomic_int64_t *p, CraMO_e mo)
{
    return __atomic_load_n(p, mo);
}

// *p = v
static inline void
cra_atomic_store_ptr(cra_atomic_ptr_t *p, void *v, CraMO_e mo)
{
    __atomic_store_n(p, v, mo);
}

// *p = v
static inline void
cra_atomic_store32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    __atomic_store_n(p, v, mo);
}

// *p = v
static inline void
cra_atomic_store64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    __atomic_store_n(p, v, mo);
}

// o = *p; *p += v; return o;
static inline int32_t
cra_atomic_add32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    return __atomic_fetch_add(p, v, mo);
}

// o = *p; *p += v; return o;
static inline int64_t
cra_atomic_add64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    return __atomic_fetch_add(p, v, mo);
}

// o = *p; *p -= v; return o;
static inline int32_t
cra_atomic_sub32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    return __atomic_fetch_sub(p, v, mo);
}

// o = *p; *p -= v; return o;
static inline int64_t
cra_atomic_sub64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    return __atomic_fetch_sub(p, v, mo);
}

// return (*p)++;
static inline int32_t
cra_atomic_inc32(cra_atomic_int32_t *p, CraMO_e mo)
{
    return __atomic_fetch_add(p, 1, mo);
}

// return (*p)++;
static inline int64_t
cra_atomic_inc64(cra_atomic_int64_t *p, CraMO_e mo)
{
    return __atomic_fetch_add(p, 1, mo);
}

// return (*p)--;
static inline int32_t
cra_atomic_dec32(cra_atomic_int32_t *p, CraMO_e mo)
{
    return __atomic_fetch_sub(p, 1, mo);
}

// return (*p)--;
static inline int64_t
cra_atomic_dec64(cra_atomic_int64_t *p, CraMO_e mo)
{
    return __atomic_fetch_sub(p, 1, mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak_ptr(cra_atomic_ptr_t *p, void *oldval, void *newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return __atomic_compare_exchange_n(p, &oldval, newval, true, succ_mo, fail_mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak32(cra_atomic_int32_t *p, int32_t oldval, int32_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return __atomic_compare_exchange_n(p, &oldval, newval, true, succ_mo, fail_mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak64(cra_atomic_int64_t *p, int64_t oldval, int64_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return __atomic_compare_exchange_n(p, &oldval, newval, true, succ_mo, fail_mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong_ptr(cra_atomic_ptr_t *p, void *oldval, void *newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return __atomic_compare_exchange_n(p, &oldval, newval, false, succ_mo, fail_mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong32(cra_atomic_int32_t *p, int32_t oldval, int32_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return __atomic_compare_exchange_n(p, &oldval, newval, false, succ_mo, fail_mo);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong64(cra_atomic_int64_t *p, int64_t oldval, int64_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    return __atomic_compare_exchange_n(p, &oldval, newval, false, succ_mo, fail_mo);
}

// return (*p == false) ? (*p = true, false) : true
static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p, CraMO_e mo)
{
    return __atomic_test_and_set(p, mo);
}

// *p = false
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p, CraMO_e mo)
{
    __atomic_clear(p, mo);
}

#elif defined(CRA_USE_INTERLOCKED)

// return *p
static inline void *
cra_atomic_load_ptr(cra_atomic_ptr_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedCompareExchangePointer(p, NULL, NULL);
}

// return *p
static inline int32_t
cra_atomic_load32(cra_atomic_int32_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedCompareExchange(p, 0, 0);
}

// return *p
static inline int64_t
cra_atomic_load64(cra_atomic_int64_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedCompareExchange64(p, 0, 0);
}

// *p = v
static inline void
cra_atomic_store_ptr(cra_atomic_ptr_t *p, void *v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    InterlockedExchangePointer(p, v);
}

// *p = v
static inline void
cra_atomic_store32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    InterlockedExchange(p, v);
}

// *p = v
static inline void
cra_atomic_store64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    InterlockedExchange64(p, v);
}

// o = *p; *p += v; return o;
static inline int32_t
cra_atomic_add32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedExchangeAdd(p, v);
}

// o = *p; *p += v; return o;
static inline int64_t
cra_atomic_add64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedExchangeAdd64(p, v);
}

// o = *p; *p -= v; return o;
static inline int32_t
cra_atomic_sub32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedExchangeAdd(p, -v);
}

// o = *p; *p -= v; return o;
static inline int64_t
cra_atomic_sub64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedExchangeAdd64(p, -v);
}

// return (*p)++;
static inline int32_t
cra_atomic_inc32(cra_atomic_int32_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedIncrement(p) - 1;
}

// return (*p)++;
static inline int64_t
cra_atomic_inc64(cra_atomic_int64_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedIncrement64(p) - 1;
}

// return (*p)--;
static inline int32_t
cra_atomic_dec32(cra_atomic_int32_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedDecrement(p) + 1;
}

// return (*p)--;
static inline int64_t
cra_atomic_dec64(cra_atomic_int64_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedDecrement64(p) + 1;
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak_ptr(cra_atomic_ptr_t *p, void *oldval, void *newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return InterlockedCompareExchangePointer(p, newval, oldval) == oldval;
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak32(cra_atomic_int32_t *p, int32_t oldval, int32_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return InterlockedCompareExchange(p, newval, oldval) == oldval;
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak64(cra_atomic_int64_t *p, int64_t oldval, int64_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return InterlockedCompareExchange64(p, newval, oldval) == oldval;
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong_ptr(cra_atomic_ptr_t *p, void *oldval, void *newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return InterlockedCompareExchangePointer(p, newval, oldval) == oldval;
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong32(cra_atomic_int32_t *p, int32_t oldval, int32_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return InterlockedCompareExchange(p, newval, oldval) == oldval;
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong64(cra_atomic_int64_t *p, int64_t oldval, int64_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return InterlockedCompareExchange64(p, newval, oldval) == oldval;
}

// return (*p == false) ? (*p = true, false) : true
static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return InterlockedExchange8(p, 1);
}

// *p = false
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    InterlockedAnd8(p, 0);
}

#elif defined(CRA_USE___SYNC)

// return *p
static inline void *
cra_atomic_load_ptr(cra_atomic_ptr_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_fetch_and_add(p, 0);
}

// return *p
static inline int32_t
cra_atomic_load32(cra_atomic_int32_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_fetch_and_add(p, 0);
}

// return *p
static inline int64_t
cra_atomic_load64(cra_atomic_int64_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_fetch_and_add(p, 0);
}

// *p = v
static inline void
cra_atomic_store_ptr(cra_atomic_ptr_t *p, void *v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    __sync_lock_test_and_set(p, v);
}

// *p = v
static inline void
cra_atomic_store32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    __sync_lock_test_and_set(p, v);
}

// *p = v
static inline void
cra_atomic_store64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    __sync_lock_test_and_set(p, v);
}

// o = *p; *p += v; return o;
static inline int32_t
cra_atomic_add32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_fetch_and_add(p, v);
}

// o = *p; *p += v; return o;
static inline int64_t
cra_atomic_add64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_fetch_and_add(p, v);
}

// o = *p; *p -= v; return o;
static inline int32_t
cra_atomic_sub32(cra_atomic_int32_t *p, int32_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_fetch_and_sub(p, v);
}

// o = *p; *p -= v; return o;
static inline int64_t
cra_atomic_sub64(cra_atomic_int64_t *p, int64_t v, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_fetch_and_sub(p, v);
}

// return (*p)++;
static inline int32_t
cra_atomic_inc32(cra_atomic_int32_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_fetch_and_add(p, 1);
}

// return (*p)++;
static inline int64_t
cra_atomic_inc64(cra_atomic_int64_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_fetch_and_add(p, 1);
}

// return (*p)--;
static inline int32_t
cra_atomic_dec32(cra_atomic_int32_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_fetch_and_sub(p, 1);
}

// return (*p)--;
static inline int64_t
cra_atomic_dec64(cra_atomic_int64_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_fetch_and_sub(p, 1);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak_ptr(cra_atomic_ptr_t *p, void *oldval, void *newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return __sync_val_compare_and_swap(p, oldval, newval);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak32(cra_atomic_int32_t *p, int32_t oldval, int32_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return __sync_val_compare_and_swap(p, oldval, newval);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_weak64(cra_atomic_int64_t *p, int64_t oldval, int64_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return __sync_val_compare_and_swap(p, oldval, newval);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong_ptr(cra_atomic_ptr_t *p, void *oldval, void *newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return __sync_val_compare_and_swap(p, oldval, newval);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong32(cra_atomic_int32_t *p, int32_t oldval, int32_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return __sync_val_compare_and_swap(p, oldval, newval);
}

// return (*p == oldval) ? (*p = newval, true) : false
static inline bool
cra_atomic_cas_strong64(cra_atomic_int64_t *p, int64_t oldval, int64_t newval, CraMO_e succ_mo, CraMO_e fail_mo)
{
    CRA_UNUSED_VALUE(succ_mo);
    CRA_UNUSED_VALUE(fail_mo);
    return __sync_val_compare_and_swap(p, oldval, newval);
}

// return (*p == false) ? (*p = true, false) : true
static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    return __sync_lock_test_and_set(p, 1);
}

// *p = false
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p, CraMO_e mo)
{
    CRA_UNUSED_VALUE(mo);
    __sync_lock_release(p);
}

#endif

// ========================== generic ==========================

#define CRA_ATOMIC_GENERIC2(_p, _name)                                                                         \
    _Generic((_p), cra_atomic_int32_t *: cra_atomic_##_name##32, cra_atomic_int64_t *: cra_atomic_##_name##64)
#define CRA_ATOMIC_GENERIC3(_p, _name)              \
    _Generic((_p),                                  \
      cra_atomic_int32_t *: cra_atomic_##_name##32, \
      cra_atomic_int64_t *: cra_atomic_##_name##64, \
      cra_atomic_ptr_t *: cra_atomic_##_name##_ptr)

// return *p
#define cra_atomic_load(_p, _mo)      CRA_ATOMIC_GENERIC3(_p, load)(_p, _mo)
// *p = v
#define cra_atomic_store(_p, _v, _mo) CRA_ATOMIC_GENERIC3(_p, store)(_p, _v, _mo)
// o = *p; *p += v; return o;
#define cra_atomic_add(_p, _v, _mo)   CRA_ATOMIC_GENERIC2(_p, add)(_p, _v, _mo)
// o = *p; *p -= v; return o;
#define cra_atomic_sub(_p, _v, _mo)   CRA_ATOMIC_GENERIC2(_p, sub)(_p, _v, _mo)
// return (*p)++;
#define cra_atomic_inc(_p, _mo)       CRA_ATOMIC_GENERIC2(_p, inc)(_p, _mo)
// return (*p)--;
#define cra_atomic_dec(_p, _mo)       CRA_ATOMIC_GENERIC2(_p, dec)(_p, _mo)
// return (*p == oldval) ? (*p = newval, true) : false
#define cra_atomic_cas_weak(_p, _oldval, _newval, _succ_mo, _fail_mo)           \
    CRA_ATOMIC_GENERIC3(_p, cas_weak)(_p, _oldval, _newval, _succ_mo, _fail_mo)
// return (*p == oldval) ? (*p = newval, true) : false
#define cra_atomic_cas_strong(_p, _oldval, _newval, _succ_mo, _fail_mo)           \
    CRA_ATOMIC_GENERIC3(_p, cas_strong)(_p, _oldval, _newval, _succ_mo, _fail_mo)

#endif