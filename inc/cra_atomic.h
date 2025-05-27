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
#include <stdatomic.h>

typedef _Atomic int32_t cra_atomic_int32_t;
typedef _Atomic int64_t cra_atomic_int64_t;

static inline int32_t cra_atomic_load32(cra_atomic_int32_t *p) { return atomic_load(p); }
static inline int64_t cra_atomic_load64(cra_atomic_int64_t *p) { return atomic_load(p); }

static inline void cra_atomic_store32(cra_atomic_int32_t *p, int32_t v) { atomic_store(p, v); }
static inline void cra_atomic_store64(cra_atomic_int64_t *p, int64_t v) { atomic_store(p, v); }

/**
 * @brief *p += n
 * @return 原来的值（没有加上n的旧值）
 */
static inline int32_t cra_atomic_add32(cra_atomic_int32_t *p, int32_t v) { return atomic_fetch_add(p, v); }
/**
 * @brief *p += n
 * @return 原来的值（没有加上n的旧值）
 */
static inline int64_t cra_atomic_add64(cra_atomic_int64_t *p, int64_t v) { return atomic_fetch_add(p, v); }

/**
 * @brief *p -= n
 *
 * @return 原来的值（没有减去n的旧值）
 */
static inline int32_t cra_atomic_sub32(cra_atomic_int32_t *p, int32_t v) { return atomic_fetch_sub(p, v); }
/**
 * @brief *p -= n
 *
 * @return 原来的值（没有减去n的旧值）
 */
static inline int64_t cra_atomic_sub64(cra_atomic_int64_t *p, int64_t v) { return atomic_fetch_sub(p, v); }

/**
 * @brief (*p)++
 *
 * @return 原来的值（没有加1的旧值）
 */
static inline int32_t cra_atomic_inc32(cra_atomic_int32_t *p) { return atomic_fetch_add(p, 1); }
/**
 * @brief (*p)++
 *
 * @return 原来的值（没有加1的旧值）
 */
static inline int64_t cra_atomic_inc64(cra_atomic_int64_t *p) { return atomic_fetch_add(p, 1); }

/**
 * @brief (*p)--
 *
 * @return 原来的值（没有减1的旧值）
 */
static inline int32_t cra_atomic_dec32(cra_atomic_int32_t *p) { return atomic_fetch_sub(p, 1); }
/**
 * @brief (*p)--
 *
 * @return 原来的值（没有减1的旧值）
 */
static inline int64_t cra_atomic_dec64(cra_atomic_int64_t *p) { return atomic_fetch_sub(p, 1); }

/**
 * @brief (*p == cmp_val) ? (*p = set_val, true) : false
 *
 * @return true:  更新*p的值成功
 * @return false: 更新*p的值失败
 */
static inline bool cra_atomic_compare_and_set32(cra_atomic_int32_t *p, int32_t cmp_val, int32_t set_val)
{
    return atomic_compare_exchange_strong(p, &(typeof(cmp_val)){cmp_val}, set_val);
}
/**
 * @brief (*p == cmp_val) ? (*p = set_val, true) : false
 *
 * @return true:  更新*p的值成功
 * @return false: 更新*p的值失败
 */
static inline bool cra_atomic_compare_and_set64(cra_atomic_int64_t *p, int64_t cmp_val, int64_t set_val)
{
    return atomic_compare_exchange_strong(p, &(typeof(cmp_val)){cmp_val}, set_val);
}

typedef atomic_flag cra_atomic_flag;

// cra_atomic_flag flag = {false}
#define CRA_ATOMIC_FLAG_INIT ATOMIC_FLAG_INIT

/**
 * @brief (*p == false) ? (*p = true, false) : true
 *
 * @return 旧值
 * @return true:  flag已经是true了
 * @return false: flag原来是false，但现在已经更新为true
 */
static inline bool cra_atomic_flag_test_and_set(cra_atomic_flag *p) { return atomic_flag_test_and_set(p); }

// *p = false
static inline void cra_atomic_flag_clear(cra_atomic_flag *p) { atomic_flag_clear(p); }

#else

#ifdef CRA_COMPILER_MSVC

typedef volatile LONG cra_atomic_int32_t;
typedef volatile LONG64 cra_atomic_int64_t;

static inline void cra_atomic_store32(cra_atomic_int32_t *p, int32_t v) { InterlockedExchange(p, v); }
static inline void cra_atomic_store64(cra_atomic_int64_t *p, int64_t v) { InterlockedExchange64(p, v); }

/**
 * @brief *p += n
 *
 * @return 原来的值（没有加上n的旧值）
 */
static inline int32_t cra_atomic_add32(cra_atomic_int32_t *p, int32_t v)
{
    int32_t ret = *p;
    InterlockedAdd(p, v);
    return ret;
}
/**
 * @brief *p += n
 *
 * @return 原来的值（没有加上n的旧值）
 */
static inline int64_t cra_atomic_add64(cra_atomic_int64_t *p, int64_t v)
{
    int64_t ret = *p;
    InterlockedAdd64(p, v);
    return ret;
}

/**
 * @brief *p -= n
 *
 * @return 原来的值（没有减去n的旧值）
 */
static inline int32_t cra_atomic_sub32(cra_atomic_int32_t *p, int32_t v)
{
    int32_t ret = *p;
    InterlockedAdd(p, -v);
    return ret;
}
/**
 * @brief *p -= n
 *
 * @return 原来的值（没有减去n的旧值）
 */
static inline int64_t cra_atomic_sub64(cra_atomic_int64_t *p, int64_t v)
{
    int64_t ret = *p;
    InterlockedAdd64(p, -v);
    return ret;
}

/**
 * @brief (*p)++
 *
 * @return 原来的值（没有加1的旧值）
 */
static inline int32_t cra_atomic_inc32(cra_atomic_int32_t *p)
{
    int32_t ret = *p;
    InterlockedIncrement(p);
    return ret;
}
/**
 * @brief (*p)++
 *
 * @return 原来的值（没有加1的旧值）
 */
static inline int64_t cra_atomic_inc64(cra_atomic_int64_t *p)
{
    int64_t ret = *p;
    InterlockedIncrement64(p);
    return ret;
}

/**
 * @brief (*p)--
 *
 * @return 原来的值（没有减1的旧值）
 */
static inline int32_t cra_atomic_dec32(cra_atomic_int32_t *p)
{
    int32_t ret = *p;
    InterlockedDecrement(p);
    return ret;
}
/**
 * @brief (*p)--
 *
 * @return 原来的值（没有减1的旧值）
 */
static inline int64_t cra_atomic_dec64(cra_atomic_int64_t *p)
{
    int64_t ret = *p;
    InterlockedDecrement64(p);
    return ret;
}

/**
 * @brief (*p == cmp_val) ? (*p = set_val, true) : false
 *
 * @return true:  更新*p的值成功
 * @return false: 更新*p的值失败
 */
static inline bool cra_atomic_compare_and_set32(cra_atomic_int32_t *p, int32_t cmp_val, int32_t set_val)
{
    return InterlockedCompareExchange(p, set_val, cmp_val) == cmp_val;
}
/**
 * @brief (*p == cmp_val) ? (*p = set_val, true) : false
 *
 * @return true:  更新*p的值成功
 * @return false: 更新*p的值失败
 */
static inline bool cra_atomic_compare_and_set64(cra_atomic_int64_t *p, int64_t cmp_val, int64_t set_val)
{
    return InterlockedCompareExchange64(p, set_val, cmp_val) == cmp_val;
}

typedef struct
{
    volatile short _val;
} cra_atomic_flag;

/**
 * @brief (*p == false) ? (*p = true, false) : true
 *
 * @return true:  flag已经是true了
 * @return false: flag原来是false，但现在已经更新为true
 */
static inline bool cra_atomic_flag_test_and_set(cra_atomic_flag *p) { return InterlockedCompareExchange16(&p->_val, 1, 0); }

// *p = false
static inline void cra_atomic_flag_clear(cra_atomic_flag *p) { InterlockedExchange16(&p->_val, 0); }

#elif defined(CRA_COMPILER_GNUC)

typedef volatile int32_t cra_atomic_int32_t;
typedef volatile int64_t cra_atomic_int64_t;

static inline void cra_atomic_store32(cra_atomic_int32_t *p, int32_t v) { *p = v; }
static inline void cra_atomic_store64(cra_atomic_int64_t *p, int64_t v) { *p = v; }

/**
 * @brief *p += n
 *
 * @return 原来的值（没有加上n的旧值）
 */
static inline int32_t cra_atomic_add32(cra_atomic_int32_t *p, int32_t v) { return __sync_fetch_and_add(p, v); }
/**
 * @brief *p += n
 *
 * @return 原来的值（没有加上n的旧值）
 */
static inline int64_t cra_atomic_add64(cra_atomic_int64_t *p, int64_t v) { return __sync_fetch_and_add(p, v); }

/**
 * @brief *p -= n
 *
 * @return 原来的值（没有减去n的旧值）
 */
static inline int32_t cra_atomic_sub32(cra_atomic_int32_t *p, int32_t v) { return __sync_fetch_and_sub(p, v); }
/**
 * @brief *p -= n
 *
 * @return 原来的值（没有减去n的旧值）
 */
static inline int64_t cra_atomic_sub64(cra_atomic_int64_t *p, int64_t v) { return __sync_fetch_and_sub(p, v); }

/**
 * @brief (*p)++
 *
 * @return 原来的值（没有加1的旧值）
 */
static inline int32_t cra_atomic_inc32(cra_atomic_int32_t *p) { return cra_atomic_add32(p, 1); }
/**
 * @brief (*p)++
 *
 * @return 原来的值（没有加1的旧值）
 */
static inline int64_t cra_atomic_inc64(cra_atomic_int64_t *p) { return cra_atomic_add64(p, 1); }

/**
 * @brief (*p)--
 *
 * @return 原来的值（没有减1的旧值）
 */
static inline int32_t cra_atomic_dec32(cra_atomic_int32_t *p) { return cra_atomic_sub32(p, 1); }
/**
 * @brief (*p)--
 *
 * @return 原来的值（没有减1的旧值）
 */
static inline int64_t cra_atomic_dec64(cra_atomic_int64_t *p) { return cra_atomic_sub64(p, 1); }

/**
 * @brief (*p == cmp_val) ? (*p = set_val, true) : false
 *
 * @return true:  更新*p的值成功
 * @return false: 更新*p的值失败
 */
static inline bool cra_atomic_compare_and_set32(cra_atomic_int32_t *p, int32_t cmp_val, int32_t set_val)
{
    return __sync_val_compare_and_swap(p, cmp_val, set_val) == cmp_val;
}
/**
 * @brief (*p == cmp_val) ? (*p = set_val, true) : false
 *
 * @return true:  更新*p的值成功
 * @return false: 更新*p的值失败
 */
static inline bool cra_atomic_compare_and_set64(cra_atomic_int64_t *p, int64_t cmp_val, int64_t set_val)
{
    return __sync_val_compare_and_swap(p, cmp_val, set_val) == cmp_val;
}

typedef struct
{
    volatile bool _val;
} cra_atomic_flag;

/**
 * @brief (*p == false) ? (*p = true, false) : true
 *
 * @return true:  flag已经是true了
 * @return false: flag原来是false，但现在已经更新为true
 */
static inline bool cra_atomic_flag_test_and_set(cra_atomic_flag *p) { return !__sync_bool_compare_and_swap(&p->_val, 0, 1); }

// *p = false
static inline void cra_atomic_flag_clear(cra_atomic_flag *p) { __sync_bool_compare_and_swap(&p->_val, p->_val, 0); }

#endif

static inline int32_t cra_atomic_load32(cra_atomic_int32_t *p) { return *p; }
static inline int64_t cra_atomic_load64(cra_atomic_int64_t *p) { return *p; }

// cra_atomic_flag flag = {false}
#define CRA_ATOMIC_FLAG_INIT {0}

#endif

#endif