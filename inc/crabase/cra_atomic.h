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
#ifndef __CRABASE_CRA_ATOMIC_H__
#define __CRABASE_CRA_ATOMIC_H__
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

typedef atomic_flag cra_atomic_flag_t;
#define CRA_ATOMIC_FLAG_INIT ATOMIC_FLAG_INIT

#define __CRA_ATOMIC_DO_WITHOUT_PTR(XX, x) \
    XX(int32_t, int32_t, x)                \
    XX(int64_t, int64_t, x)
#define __CRA_ATOMIC_DO(XX, x)         \
    XX(void *, ptr_t, x)               \
    __CRA_ATOMIC_DO_WITHOUT_PTR(XX, x)

#define CRA_ATOMIC_XX(type, name, x) typedef _Atomic(type) cra_atomic_##name;
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, x)
#undef CRA_ATOMIC_XX

#elif defined(CRA_USE_INTERLOCKED)

typedef volatile CHAR cra_atomic_flag_t;
#define CRA_ATOMIC_FLAG_INIT 0

#define __CRA_ATOMIC_DO_WITHOUT_PTR(XX, x) \
    XX(LONG, int32_t, int32_t, x)          \
    XX(LONG64, int64_t, int64_t, x)
#define __CRA_ATOMIC_DO(XX, x)         \
    XX(PVOID, PVOID, ptr_t, x)         \
    __CRA_ATOMIC_DO_WITHOUT_PTR(XX, x)

#define CRA_ATOMIC_XX(type, t, name, x) typedef type cra_atomic_##name;
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, x)
#undef CRA_ATOMIC_XX

#else

typedef bool cra_atomic_flag_t;
#define CRA_ATOMIC_FLAG_INIT false

#define __CRA_ATOMIC_DO_WITHOUT_PTR(XX, x) \
    XX(int32_t, int32_t, x)                \
    XX(int64_t, int64_t, x)
#define __CRA_ATOMIC_DO(XX, x)         \
    XX(void *, ptr_t, x)               \
    __CRA_ATOMIC_DO_WITHOUT_PTR(XX, x)

#define CRA_ATOMIC_XX(type, name, x) typedef type cra_atomic_##name;
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, x)
#undef CRA_ATOMIC_XX

#endif

// ========================== memory order ==========================

#if defined(CRA_USE_STDATOMIC)

typedef enum
{
    cra_memory_order_relaxed = memory_order_relaxed,
    cra_memory_order_consume = memory_order_consume,
    cra_memory_order_acquire = memory_order_acquire,
    cra_memory_order_release = memory_order_release,
    cra_memory_order_acq_rel = memory_order_acq_rel,
    cra_memory_order_seq_cst = memory_order_seq_cst
} CraMemoryOrder_e;

#elif defined(CRA_USE___ATOMIC)

typedef enum
{
    cra_memory_order_relaxed = __ATOMIC_RELAXED,
    cra_memory_order_consume = __ATOMIC_CONSUME,
    cra_memory_order_acquire = __ATOMIC_ACQUIRE,
    cra_memory_order_release = __ATOMIC_RELEASE,
    cra_memory_order_acq_rel = __ATOMIC_ACQ_REL,
    cra_memory_order_seq_cst = __ATOMIC_SEQ_CST
} CraMemoryOrder_e;

#else

typedef enum
{
    cra_memory_order_relaxed,
    cra_memory_order_consume,
    cra_memory_order_acquire,
    cra_memory_order_release,
    cra_memory_order_acq_rel,
    cra_memory_order_seq_cst
} CraMemoryOrder_e;

#endif

#ifdef CRA_FORCE_SEQ_CST_ATOMICS
#define cra_memory_order_relaxed cra_memory_order_seq_cst
#define cra_memory_order_consume cra_memory_order_seq_cst
#define cra_memory_order_acquire cra_memory_order_seq_cst
#define cra_memory_order_release cra_memory_order_seq_cst
#define cra_memory_order_acq_rel cra_memory_order_seq_cst
#endif

// ========================== functions ==========================

#if defined(CRA_USE_STDATOMIC)

// bool flag_test_and_set(cra_atomic_flag_t *p, CraMemoryOrder_e order)
#define cra_atomic_flag_test_and_set(p, order) atomic_flag_test_and_set_explicit(p, order)
// void flag_clear(cra_atomic_flag_t *p, CraMemoryOrder_e order)
#define cra_atomic_flag_clear(p, order)        atomic_flag_clear_explicit(p, order)

#define __CRA_ATOMIC_XX_FUNC_DEF(type, name, fname) , cra_atomic_##name * : fname
#define __CRA_ATOMIC_FUNC_DEF(DO, fname, p, ...)    _Generic((p)DO(__CRA_ATOMIC_XX_FUNC_DEF, fname))(p, __VA_ARGS__)

// T load(Atomic(T) *p, CraMemoryOrder_e order)
#define cra_atomic_load(p, order)         __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, atomic_load_explicit, p, order)
// void store(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_store(p, value, order) __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, atomic_store_explicit, p, value, order)

// T fetch_add(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_add(p, value, order)                                                      \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, atomic_fetch_add_explicit, p, value, order)
// T fetch_sub(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_sub(p, value, order)                                                      \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, atomic_fetch_sub_explicit, p, value, order)
// T fetch_or(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_or(p, value, order)                                                      \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, atomic_fetch_or_explicit, p, value, order)
// T fetch_and(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_and(p, value, order)                                                      \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, atomic_fetch_and_explicit, p, value, order)
// T fetch_xor(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_xor(p, value, order)                                                      \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, atomic_fetch_xor_explicit, p, value, order)

// T exchange(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_exchange(p, value, order)                                          \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, atomic_exchange_explicit, p, value, order)

// bool compare_exchange_weak(Atomic(T) *p, T *expected, T value, CraMemoryOrder_e success, CraMemoryOrder_e failure)
#define cra_atomic_compare_exchange_weak(p, expected, value, success, failure) \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, atomic_compare_exchange_weak_explicit, p, expected, value, success, failure)
// bool compare_exchange_strong(Atomic(T) *p, T *expected, T value, CraMemoryOrder_e success, CraMemoryOrder_e
// failure)
#define cra_atomic_compare_exchange_strong(p, expected, value, success, failure)                                 \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, atomic_compare_exchange_strong_explicit, p, expected, value, success, \
                          failure)

#elif defined(CRA_USE___ATOMIC)

// bool flag_test_and_set(cra_atomic_flag_t *p, CraMemoryOrder_e order)
#define cra_atomic_flag_test_and_set(p, order) __atomic_test_and_set(p, order)
// void flag_clear(cra_atomic_flag_t *p, CraMemoryOrder_e order)
#define cra_atomic_flag_clear(p, order)        __atomic_clear(p, order)

#define __CRA_ATOMIC_XX_FUNC_DEF(type, name, fname) , cra_atomic_##name * : fname
#define __CRA_ATOMIC_FUNC_DEF(DO, fname, p, ...)    _Generic((p)DO(__CRA_ATOMIC_XX_FUNC_DEF, fname))(p, __VA_ARGS__)

// T load(Atomic(T) *p, CraMemoryOrder_e order)
#define cra_atomic_load(p, order)                   __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, __atomic_load_n, p, order)
// void store(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_store(p, value, order) __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, __atomic_store_n, p, value, order)

// T fetch_add(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_add(p, value, order)                                                \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, __atomic_fetch_add, p, value, order)
// T fetch_sub(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_sub(p, value, order)                                                \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, __atomic_fetch_sub, p, value, order)
// T fetch_or(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_or(p, value, order)                                                \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, __atomic_fetch_or, p, value, order)
// T fetch_and(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_and(p, value, order)                                                \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, __atomic_fetch_and, p, value, order)
// T fetch_xor(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_xor(p, value, order)                                                \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, __atomic_fetch_xor, p, value, order)

// T exchange(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_exchange(p, value, order)                                      \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, __atomic_exchange_n, p, value, order)

// bool compare_exchange_weak(Atomic(T) *p, T *expected, T value, CraMemoryOrder_e success, CraMemoryOrder_e failure)
#define cra_atomic_compare_exchange_weak(p, expected, value, success, failure)                                       \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, __atomic_compare_exchange_n, p, expected, value, true, success, failure)
// bool compare_exchange_strong(Atomic(T) *p, T *expected, T value, CraMemoryOrder_e success, CraMemoryOrder_e
// failure)
#define cra_atomic_compare_exchange_strong(p, expected, value, success, failure)                                     \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, __atomic_compare_exchange_n, p, expected, value, false, success, failure)

#elif defined(CRA_USE_INTERLOCKED)

// bool flag_test_and_set(cra_atomic_flag_t *p, CraMemoryOrder_e order)
#define cra_atomic_flag_test_and_set(p, order) (bool)InterlockedExchange8(p, 1)
// void flag_clear(cra_atomic_flag_t *p, CraMemoryOrder_e order)
#define cra_atomic_flag_clear(p, order)        (void)InterlockedExchange8(p, 0)

#define __CRA_INTERLOCKED_XX_FUNCptr_t(fname)   fname##Pointer
#define __CRA_INTERLOCKED_XX_FUNCint64_t(fname) fname##64
#define __CRA_INTERLOCKED_XX_FUNCint32_t(fname) fname

#define __CRA_ATOMIC_XX_FUNC_DEF(type, t, name, fname)                      \
    , cra_atomic_##name * : CRA_CAT(__CRA_INTERLOCKED_XX_FUNC, name)(fname)
#define __CRA_ATOMIC_FUNC_DEF(DO, fname, p, ...) _Generic((p)DO(__CRA_ATOMIC_XX_FUNC_DEF, fname))(p, __VA_ARGS__)

// T load(Atomic(T) *p, CraMemoryOrder_e order)
#define cra_atomic_load(p, order)         __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, InterlockedCompareExchange, p, 0, 0)
// void store(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_store(p, value, order) (void)__CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, InterlockedExchange, p, value)

// T fetch_add(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_add(p, value, order)                                             \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, InterlockedExchangeAdd, p, value)
// T fetch_sub(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_sub(p, value, order) cra_atomic_fetch_add(p, -(value), order)
// T fetch_or(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_or(p, value, order) __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, InterlockedOr, p, value)
// T fetch_and(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_and(p, value, order)                                     \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, InterlockedAnd, p, value)
// T fetch_xor(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_xor(p, value, order)                                     \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, InterlockedXor, p, value)

// T exchange(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_exchange(p, value, order) __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, InterlockedExchange, p, value)

// bool compare_exchange_weak(Atomic(T) *p, T *expected, T value, CraMemoryOrder_e success, CraMemoryOrder_e failure)
#define cra_atomic_compare_exchange_weak(p, expected, value, success, failure)                 \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, CraInterlockedCompareExchange, p, expected, value)
// bool compare_exchange_strong(Atomic(T) *p, T *expected, T value, CraMemoryOrder_e success, CraMemoryOrder_e
// failure)
#define cra_atomic_compare_exchange_strong(p, expected, value, success, failure)              \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, CraInterlockedCompareExchange, p, expected, value)

#define CRA_ATOMIC_XX(type, t, name, fname)                                                                          \
    static inline bool CRA_CAT(__CRA_INTERLOCKED_XX_FUNC, name)(fname)(cra_atomic_##name * p, t * expected, t value) \
    {                                                                                                                \
        type old = CRA_CAT(__CRA_INTERLOCKED_XX_FUNC, name)(InterlockedCompareExchange)(p, value, *expected);        \
        if (old != *expected)                                                                                        \
        {                                                                                                            \
            *expected = old;                                                                                         \
            return false;                                                                                            \
        }                                                                                                            \
        return true;                                                                                                 \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, CraInterlockedCompareExchange)
#undef CRA_ATOMIC_XX

#elif defined(CRA_USE___SYNC)

// bool flag_test_and_set(cra_atomic_flag_t *p, CraMemoryOrder_e order)
#define cra_atomic_flag_test_and_set(p, order) __sync_lock_test_and_set(p, 1)
// void flag_clear(cra_atomic_flag_t *p, CraMemoryOrder_e order)
#define cra_atomic_flag_clear(p, order)        __sync_lock_release(p)

#define __CRA_ATOMIC_XX_FUNC_DEF(type, name, fname) , cra_atomic_##name * : fname
#define __CRA_ATOMIC_FUNC_DEF(DO, fname, p, ...)    _Generic((p)DO(__CRA_ATOMIC_XX_FUNC_DEF, fname))(p, __VA_ARGS__)

// T load(Atomic(T) *p, CraMemoryOrder_e order)
#define cra_atomic_load(p, order)                   __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, __sync_fetch_and_or, p, 0)
// void store(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_store(p, value, order) __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, __sync_lock_test_and_set, p, value)

// T fetch_add(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_add(p, value, order)                                           \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, __sync_fetch_and_add, p, value)
// T fetch_sub(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_sub(p, value, order)                                           \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, __sync_fetch_and_sub, p, value)
// T fetch_or(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_or(p, value, order)                                           \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, __sync_fetch_and_or, p, value)
// T fetch_and(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_and(p, value, order)                                           \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, __sync_fetch_and_and, p, value)
// T fetch_xor(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_xor(p, value, order)                                           \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, __sync_fetch_and_xor, p, value)

// T exchange(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_exchange(p, value, order) __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, __sync_lock_test_and_set, p, value)

// bool compare_exchange_weak(Atomic(T) *p, T *expected, T value, CraMemoryOrder_e success, CraMemoryOrder_e failure)
#define cra_atomic_compare_exchange_weak(p, expected, value, success, failure)                  \
    __CRA_ATOMIC_FUNC_DEF2(__CRA_ATOMIC_DO, cra_sync_val_compare_and_swap, p, expected, value)
// bool compare_exchange_strong(Atomic(T) *p, T *expected, T value, CraMemoryOrder_e success, CraMemoryOrder_e
// failure)
#define cra_atomic_compare_exchange_strong(p, expected, value, success, failure)               \
    __CRA_ATOMIC_FUNC_DEF2(__CRA_ATOMIC_DO, cra_sync_val_compare_and_swap, p, expected, value)

#define __CRA_ATOMIC_XX_FUNC_DEF2(type, name, fname) , cra_atomic_##name * : _##fname##name
#define __CRA_ATOMIC_FUNC_DEF2(DO, fname, p, ...)    _Generic((p)DO(__CRA_ATOMIC_XX_FUNC_DEF2, fname))(p, __VA_ARGS__)

#define CRA_ATOMIC_XX(type, name, fname)                                                \
    static inline bool _##fname##name(cra_atomic_##name *p, type *expected, type value) \
    {                                                                                   \
        type old = __sync_val_compare_and_swap(p, *expected, value);                    \
        if (old != *expected)                                                           \
        {                                                                               \
            *expected = old;                                                            \
            return false;                                                               \
        }                                                                               \
        return true;                                                                    \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, cra_sync_val_compare_and_swap)
#undef CRA_ATOMIC_XX

#endif

#endif
