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

#if __has_include(<stdatomic.h>) && !defined(__STDC_NO_ATOMICS__)

// stdatomic
#include <stdatomic.h>
#define CRA_USE_STDATOMIC

#elif defined(__ATOMIC_RELAXED) || defined(__ATOMIC_SEQ_CST)

// __atomic
#define CRA_USE___ATOMIC

#elif defined(CRA_COMPILER_MSVC) || defined(CRA_COMPILER_CLANG_CL)

// Interlocked
#define CRA_USE_INTERLOCKED

#elif defined(__GNUC__)

// __sync
#define CRA_USE___SYNC

#else

#error "No atomic implementation available"

#endif

// ========================== type ==========================

#if defined(CRA_USE_INTERLOCKED)

#define __CRA_ATOMIC_DO_WITHOUT_PTR(XX, ...)  \
    XX(LONG, int32_t, int32_t, __VA_ARGS__)   \
    XX(LONG64, int64_t, int64_t, __VA_ARGS__)
#define __CRA_ATOMIC_DO(XX, ...)                 \
    XX(PVOID, PVOID, ptr_t, __VA_ARGS__)         \
    __CRA_ATOMIC_DO_WITHOUT_PTR(XX, __VA_ARGS__)

#else

#define __CRA_ATOMIC_DO_WITHOUT_PTR(XX, ...) \
    XX(int32_t, int32_t, __VA_ARGS__)        \
    XX(int64_t, int64_t, __VA_ARGS__)
#define __CRA_ATOMIC_DO(XX, ...)                 \
    XX(void *, ptr_t, __VA_ARGS__)               \
    __CRA_ATOMIC_DO_WITHOUT_PTR(XX, __VA_ARGS__)

#endif

#if defined(CRA_USE_STDATOMIC)

typedef atomic_flag cra_atomic_flag_t;
#define CRA_ATOMIC_FLAG_INIT ATOMIC_FLAG_INIT

#define CRA_ATOMIC_XX(type, name, x) typedef _Atomic(type) cra_atomic_##name;
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, x)
#undef CRA_ATOMIC_XX

#elif defined(CRA_USE_INTERLOCKED)

typedef volatile CHAR cra_atomic_flag_t;
#define CRA_ATOMIC_FLAG_INIT 0

#define CRA_ATOMIC_XX(type, t, name, x) typedef type cra_atomic_##name;
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, x)
#undef CRA_ATOMIC_XX

#else

typedef bool cra_atomic_flag_t;
#define CRA_ATOMIC_FLAG_INIT false

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

static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p, CraMemoryOrder_e order)
{
    return atomic_flag_test_and_set_explicit(p, order);
}
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p, CraMemoryOrder_e order)
{
    atomic_flag_clear_explicit(p, order);
}

// load
#define CRA_ATOMIC_XX(type, name, x)                                                       \
    static inline type cra_atomic_load##name(cra_atomic_##name *p, CraMemoryOrder_e order) \
    {                                                                                      \
        return atomic_load_explicit(p, order);                                             \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// store
#define CRA_ATOMIC_XX(type, name, x)                                                                    \
    static inline void cra_atomic_store##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                   \
        atomic_store_explicit(p, value, order);                                                         \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// fetch_add
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_add##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        return atomic_fetch_add_explicit(p, value, order);                                                  \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_sub
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_sub##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        return atomic_fetch_sub_explicit(p, value, order);                                                  \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_or
#define CRA_ATOMIC_XX(type, name, x)                                                                       \
    static inline type cra_atomic_fetch_or##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                      \
        return atomic_fetch_or_explicit(p, value, order);                                                  \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_and
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_and##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        return atomic_fetch_and_explicit(p, value, order);                                                  \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_xor
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_xor##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        return atomic_fetch_xor_explicit(p, value, order);                                                  \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// exchange
#define CRA_ATOMIC_XX(type, name, x)                                                                       \
    static inline type cra_atomic_exchange##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                      \
        return atomic_exchange_explicit(p, value, order);                                                  \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// compare_exchange_weak
#define CRA_ATOMIC_XX(type, name, x)                                                                              \
    static inline bool cra_atomic_compare_exchange_weak##name(cra_atomic_##name *p, type *expected, type value,   \
                                                              CraMemoryOrder_e success, CraMemoryOrder_e failure) \
    {                                                                                                             \
        return atomic_compare_exchange_weak_explicit(p, expected, value, success, failure);                       \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// compare_exchange_strong
#define CRA_ATOMIC_XX(type, name, x)                                                                                \
    static inline bool cra_atomic_compare_exchange_strong##name(cra_atomic_##name *p, type *expected, type value,   \
                                                                CraMemoryOrder_e success, CraMemoryOrder_e failure) \
    {                                                                                                               \
        return atomic_compare_exchange_strong_explicit(p, expected, value, success, failure);                       \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

#elif defined(CRA_USE___ATOMIC)

static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p, CraMemoryOrder_e order)
{
    return __atomic_test_and_set(p, order);
}
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p, CraMemoryOrder_e order)
{
    __atomic_clear(p, order);
}

// load
#define CRA_ATOMIC_XX(type, name, x)                                                       \
    static inline type cra_atomic_load##name(cra_atomic_##name *p, CraMemoryOrder_e order) \
    {                                                                                      \
        return __atomic_load_n(p, order);                                                  \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// store
#define CRA_ATOMIC_XX(type, name, x)                                                                    \
    static inline void cra_atomic_store##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                   \
        __atomic_store_n(p, value, order);                                                              \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// fetch_add
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_add##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        return __atomic_fetch_add(p, value, order);                                                         \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_sub
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_sub##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        return __atomic_fetch_sub(p, value, order);                                                         \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_or
#define CRA_ATOMIC_XX(type, name, x)                                                                       \
    static inline type cra_atomic_fetch_or##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                      \
        return __atomic_fetch_or(p, value, order);                                                         \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_and
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_and##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        return __atomic_fetch_and(p, value, order);                                                         \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_xor
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_xor##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        return __atomic_fetch_xor(p, value, order);                                                         \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// exchange
#define CRA_ATOMIC_XX(type, name, x)                                                                       \
    static inline type cra_atomic_exchange##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                      \
        return __atomic_exchange_n(p, value, order);                                                       \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// compare_exchange_weak
#define CRA_ATOMIC_XX(type, name, x)                                                                              \
    static inline bool cra_atomic_compare_exchange_weak##name(cra_atomic_##name *p, type *expected, type value,   \
                                                              CraMemoryOrder_e success, CraMemoryOrder_e failure) \
    {                                                                                                             \
        return __atomic_compare_exchange_n(p, expected, value, true, success, failure);                           \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// compare_exchange_strong
#define CRA_ATOMIC_XX(type, name, x)                                                                                \
    static inline bool cra_atomic_compare_exchange_strong##name(cra_atomic_##name *p, type *expected, type value,   \
                                                                CraMemoryOrder_e success, CraMemoryOrder_e failure) \
    {                                                                                                               \
        return __atomic_compare_exchange_n(p, expected, value, false, success, failure);                            \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

#elif defined(CRA_USE_INTERLOCKED)

static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p, CraMemoryOrder_e order)
{
    CRA_UNUSED(order);
    return InterlockedExchange8(p, 1);
}
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p, CraMemoryOrder_e order)
{
    CRA_UNUSED(order);
    (void)InterlockedExchange8(p, 0);
}

#define __CRA_INTERLOCKED_XX_FUNCptr_t(fname)   fname##Pointer
#define __CRA_INTERLOCKED_XX_FUNCint64_t(fname) fname##64
#define __CRA_INTERLOCKED_XX_FUNCint32_t(fname) fname
#define __CRA_INTERLOCKED_XX_FUNC(name, fname)  CRA_CAT(__CRA_INTERLOCKED_XX_FUNC, name)(fname)

// load
#define CRA_ATOMIC_XX(type, t, name, x)                                                 \
    static inline t cra_atomic_load##name(cra_atomic_##name *p, CraMemoryOrder_e order) \
    {                                                                                   \
        CRA_UNUSED(order);                                                              \
        return __CRA_INTERLOCKED_XX_FUNC(name, InterlockedCompareExchange)(p, 0, 0);    \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// store
#define CRA_ATOMIC_XX(type, t, name, x)                                                              \
    static inline void cra_atomic_store##name(cra_atomic_##name *p, t value, CraMemoryOrder_e order) \
    {                                                                                                \
        CRA_UNUSED(order);                                                                           \
        (void)__CRA_INTERLOCKED_XX_FUNC(name, InterlockedExchange)(p, value);                        \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// fetch_add
#define CRA_ATOMIC_XX(type, t, name, x)                                                                  \
    static inline type cra_atomic_fetch_add##name(cra_atomic_##name *p, t value, CraMemoryOrder_e order) \
    {                                                                                                    \
        CRA_UNUSED(order);                                                                               \
        return __CRA_INTERLOCKED_XX_FUNC(name, InterlockedExchangeAdd)(p, value);                        \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_sub
#define CRA_ATOMIC_XX(type, t, name, x)                                                                  \
    static inline type cra_atomic_fetch_sub##name(cra_atomic_##name *p, t value, CraMemoryOrder_e order) \
    {                                                                                                    \
        CRA_UNUSED(order);                                                                               \
        return __CRA_INTERLOCKED_XX_FUNC(name, InterlockedExchangeAdd)(p, -value);                       \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_or
#define CRA_ATOMIC_XX(type, t, name, x)                                                                 \
    static inline type cra_atomic_fetch_or##name(cra_atomic_##name *p, t value, CraMemoryOrder_e order) \
    {                                                                                                   \
        CRA_UNUSED(order);                                                                              \
        return __CRA_INTERLOCKED_XX_FUNC(name, InterlockedOr)(p, value);                                \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_and
#define CRA_ATOMIC_XX(type, t, name, x)                                                                  \
    static inline type cra_atomic_fetch_and##name(cra_atomic_##name *p, t value, CraMemoryOrder_e order) \
    {                                                                                                    \
        CRA_UNUSED(order);                                                                               \
        return __CRA_INTERLOCKED_XX_FUNC(name, InterlockedAnd)(p, value);                                \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_xor
#define CRA_ATOMIC_XX(type, t, name, x)                                                                  \
    static inline type cra_atomic_fetch_xor##name(cra_atomic_##name *p, t value, CraMemoryOrder_e order) \
    {                                                                                                    \
        CRA_UNUSED(order);                                                                               \
        return __CRA_INTERLOCKED_XX_FUNC(name, InterlockedXor)(p, value);                                \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// exchange
#define CRA_ATOMIC_XX(type, t, name, x)                                                                 \
    static inline type cra_atomic_exchange##name(cra_atomic_##name *p, t value, CraMemoryOrder_e order) \
    {                                                                                                   \
        CRA_UNUSED(order);                                                                              \
        return __CRA_INTERLOCKED_XX_FUNC(name, InterlockedExchange)(p, value);                          \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// compare_exchange_weak
#define CRA_ATOMIC_XX(type, t, name, x)                                                                           \
    static inline bool cra_atomic_compare_exchange_weak##name(cra_atomic_##name *p, t *expected, t value,         \
                                                              CraMemoryOrder_e success, CraMemoryOrder_e failure) \
    {                                                                                                             \
        CRA_UNUSED(success);                                                                                      \
        CRA_UNUSED(failure);                                                                                      \
        t old = (t)__CRA_INTERLOCKED_XX_FUNC(name, InterlockedCompareExchange)(p, value, *expected);              \
        if (old != *expected)                                                                                     \
        {                                                                                                         \
            *expected = old;                                                                                      \
            return false;                                                                                         \
        }                                                                                                         \
        return true;                                                                                              \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// compare_exchange_strong
#define CRA_ATOMIC_XX(type, t, name, x)                                                                             \
    static inline bool cra_atomic_compare_exchange_strong##name(cra_atomic_##name *p, t *expected, t value,         \
                                                                CraMemoryOrder_e success, CraMemoryOrder_e failure) \
    {                                                                                                               \
        CRA_UNUSED(success);                                                                                        \
        CRA_UNUSED(failure);                                                                                        \
        t old = (t)__CRA_INTERLOCKED_XX_FUNC(name, InterlockedCompareExchange)(p, value, *expected);                \
        if (old != *expected)                                                                                       \
        {                                                                                                           \
            *expected = old;                                                                                        \
            return false;                                                                                           \
        }                                                                                                           \
        return true;                                                                                                \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

#elif defined(CRA_USE___SYNC)

static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p, CraMemoryOrder_e order)
{
    CRA_UNUSED(order);
    return __sync_lock_test_and_set(p, 1);
}
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p, CraMemoryOrder_e order)
{
    CRA_UNUSED(order);
    __sync_lock_release(p);
}

// load
#define CRA_ATOMIC_XX(type, name, x)                                                       \
    static inline type cra_atomic_load##name(cra_atomic_##name *p, CraMemoryOrder_e order) \
    {                                                                                      \
        CRA_UNUSED(order);                                                                 \
        return __sync_fetch_and_or(p, 0);                                                  \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// store
#define CRA_ATOMIC_XX(type, name, x)                                                                    \
    static inline void cra_atomic_store##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                   \
        CRA_UNUSED(order);                                                                              \
        (void)__sync_lock_test_and_set(p, value);                                                       \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// fetch_add
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_add##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        CRA_UNUSED(order);                                                                                  \
        return __sync_fetch_and_add(p, value);                                                              \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_sub
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_sub##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        CRA_UNUSED(order);                                                                                  \
        return __sync_fetch_and_sub(p, value);                                                              \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_or
#define CRA_ATOMIC_XX(type, name, x)                                                                       \
    static inline type cra_atomic_fetch_or##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                      \
        CRA_UNUSED(order);                                                                                 \
        return __sync_fetch_and_or(p, value);                                                              \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_and
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_and##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        CRA_UNUSED(order);                                                                                  \
        return __sync_fetch_and_and(p, value);                                                              \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// fetch_xor
#define CRA_ATOMIC_XX(type, name, x)                                                                        \
    static inline type cra_atomic_fetch_xor##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                       \
        CRA_UNUSED(order);                                                                                  \
        return __sync_fetch_and_xor(p, value);                                                              \
    }
__CRA_ATOMIC_DO_WITHOUT_PTR(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// exchange
#define CRA_ATOMIC_XX(type, name, x)                                                                       \
    static inline type cra_atomic_exchange##name(cra_atomic_##name *p, type value, CraMemoryOrder_e order) \
    {                                                                                                      \
        CRA_UNUSED(order);                                                                                 \
        return __sync_lock_test_and_set(p, value);                                                         \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

// compare_exchange_weak
#define CRA_ATOMIC_XX(type, name, x)                                                                              \
    static inline bool cra_atomic_compare_exchange_weak##name(cra_atomic_##name *p, type *expected, type value,   \
                                                              CraMemoryOrder_e success, CraMemoryOrder_e failure) \
    {                                                                                                             \
        CRA_UNUSED(success);                                                                                      \
        CRA_UNUSED(failure);                                                                                      \
        type old = __sync_val_compare_and_swap(p, *expected, value);                                              \
        if (old != *expected)                                                                                     \
        {                                                                                                         \
            *expected = old;                                                                                      \
            return false;                                                                                         \
        }                                                                                                         \
        return true;                                                                                              \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX
// compare_exchange_strong
#define CRA_ATOMIC_XX(type, name, x)                                                                                \
    static inline bool cra_atomic_compare_exchange_strong##name(cra_atomic_##name *p, type *expected, type value,   \
                                                                CraMemoryOrder_e success, CraMemoryOrder_e failure) \
    {                                                                                                               \
        CRA_UNUSED(success);                                                                                        \
        CRA_UNUSED(failure);                                                                                        \
        type old = __sync_val_compare_and_swap(p, *expected, value);                                                \
        if (old != *expected)                                                                                       \
        {                                                                                                           \
            *expected = old;                                                                                        \
            return false;                                                                                           \
        }                                                                                                           \
        return true;                                                                                                \
    }
__CRA_ATOMIC_DO(CRA_ATOMIC_XX, )
#undef CRA_ATOMIC_XX

#endif

#ifdef CRA_USE_INTERLOCKED
#define __CRA_ATOMIC_XX_FUNC_DEF(type, t, name, fname) , cra_atomic_##name * : cra_atomic_##fname##name
#else
#define __CRA_ATOMIC_XX_FUNC_DEF(type, name, fname) , cra_atomic_##name * : cra_atomic_##fname##name
#endif
#define __CRA_ATOMIC_FUNC_DEF(DO, fname, p, ...) _Generic((p)DO(__CRA_ATOMIC_XX_FUNC_DEF, fname))(p, __VA_ARGS__)

// T load(Atomic(T) *p, CraMemoryOrder_e order)
#define cra_atomic_load(p, order)         __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, load, p, order)
// void store(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_store(p, value, order) __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, store, p, value, order)

// T fetch_add(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_add(p, value, order)                                      \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, fetch_add, p, value, order)
// T fetch_sub(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_sub(p, value, order)                                      \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, fetch_sub, p, value, order)
// T fetch_or(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_or(p, value, order)                                      \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, fetch_or, p, value, order)
// T fetch_and(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_and(p, value, order)                                      \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, fetch_and, p, value, order)
// T fetch_xor(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_fetch_xor(p, value, order)                                      \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO_WITHOUT_PTR, fetch_xor, p, value, order)

// T exchange(Atomic(T) *p, T value, CraMemoryOrder_e order)
#define cra_atomic_exchange(p, value, order) __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, exchange, p, value, order)

// bool compare_exchange_weak(Atomic(T) *p, T *expected, T value, CraMemoryOrder_e success, CraMemoryOrder_e failure)
#define cra_atomic_compare_exchange_weak(p, expected, value, success, failure)                          \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, compare_exchange_weak, p, expected, value, success, failure)
// bool compare_exchange_strong(Atomic(T) *p, T *expected, T value, CraMemoryOrder_e success, CraMemoryOrder_e
// failure)
#define cra_atomic_compare_exchange_strong(p, expected, value, success, failure)                          \
    __CRA_ATOMIC_FUNC_DEF(__CRA_ATOMIC_DO, compare_exchange_strong, p, expected, value, success, failure)

#endif
