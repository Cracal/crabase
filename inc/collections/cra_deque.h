/**
 * @file cra_deque.h
 * @author Cracal
 * @brief 双端队列
 * @version 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_DEQUE_H__
#define __CRA_DEQUE_H__
#include "cra_collects.h"
#include "cra_ifs.h"

#define CRA_DEQUE_CHECK_VAL(deque, val) assert(sizeof(*(val)) == (deque)->itemsize)

typedef struct CraDeque CraDeque;
struct CraDeque
{
    // === ring queue ===
    unsigned char **array;
    size_t          front;
    size_t          rear;
    size_t          narray;
    // ====== deque =====
    size_t          count;
    size_t          lindex;
    size_t          rindex;
    size_t          itemsize;
};

CRA_API bool
cra_deque_init_with_size(CraDeque *deque, size_t itemsize, size_t init_capacity);
// bool init_with_size<T>(CraDeque *deque, size_t init_capacity)
#define cra_deque_init_with_size(T, deque, init_capacity) cra_deque_init_with_size(deque, sizeof(T), init_capacity)
// bool init<T>(CraDeque *deque)
#define cra_deque_init(T, deque)                          cra_deque_init_with_size(T, deque, 0)

CRA_API void
cra_deque_uninit(CraDeque *deque);

CRA_API void
cra_deque_clear(CraDeque *deque);

CRA_API bool
cra_deque_insert(CraDeque *deque, size_t index, void *val);
CRA_API bool
cra_deque_push_front(CraDeque *deque, void *val);
CRA_API bool
cra_deque_push_back(CraDeque *deque, void *val);
// bool insert(CraDeque *deque, size_t index, T *val)
#define cra_deque_insert(deque, index, val) (CRA_DEQUE_CHECK_VAL(deque, val), cra_deque_insert(deque, index, val))
// bool push_front(CraDeque *deque, T *val)
#define cra_deque_push_front(deque, val)    (CRA_DEQUE_CHECK_VAL(deque, val), cra_deque_push_front(deque, val))
// bool push_back(CraDeque *deque, T *val)
#define cra_deque_push_back(deque, val)     (CRA_DEQUE_CHECK_VAL(deque, val), cra_deque_push_back(deque, val))

CRA_API bool
cra_deque_pop_at(CraDeque *deque, size_t index, void *retval);
CRA_API bool
cra_deque_pop_front(CraDeque *deque, void *retval);
CRA_API bool
cra_deque_pop_back(CraDeque *deque, void *retval);
// bool pop_at(CraDeque *deque, size_t index, out T *retval)
#define cra_deque_pop_at(deque, index, retval)                                   \
    (CRA_DEQUE_CHECK_VAL(deque, retval), cra_deque_pop_at(deque, index, retval))
// bool pop_front(CraDeque *deque, out T *retval)
#define cra_deque_pop_front(deque, retval) (CRA_DEQUE_CHECK_VAL(deque, retval), cra_deque_pop_front(deque, retval))
// bool pop_back(CraDeque *deque, out T *retval)
#define cra_deque_pop_back(deque, retval)  (CRA_DEQUE_CHECK_VAL(deque, retval), cra_deque_pop_back(deque, retval))

// bool remove_at(CraDeque *deque, size_t index)
#define cra_deque_remove_at(deque, index) (cra_deque_pop_at)(deque, index, NULL)
// bool remove_left(CraDeque *deque)
#define cra_deque_remove_front(deque)     (cra_deque_pop_front)(deque, NULL)
// bool remove(CraDeque *deque)
#define cra_deque_remove_back(deque)      (cra_deque_pop_back)(deque, NULL)

CRA_API void *
cra_deque_get_ref(CraDeque *deque, size_t index);

static inline bool
cra_deque_get(CraDeque *deque, size_t index, void *retval)
{
    void *pval = cra_deque_get_ref(deque, index);
    if (pval && retval)
        memcpy(retval, pval, deque->itemsize);
    return pval != NULL;
}
// bool get(CraDeque *deque, size_t index, out T *retval)
#define cra_deque_get(deque, index, retval) (CRA_DEQUE_CHECK_VAL(deque, retval), cra_deque_get(deque, index, retval))

// bool peek_front(CraDeque *deque, out T *retval)
#define cra_deque_peek_front(deque, retval) cra_deque_get(deque, 0, retval)
// T *peek_front_ref(CraDeque *deque)
#define cra_deque_peek_front_ref(deque)     cra_deque_get_ref(deque, 0)
// bool peek_back(CraDeque *deque, out T *retval)
#define cra_deque_peek_back(deque, retval)  cra_deque_get(deque, (deque)->count - 1, retval)
// T *peek_back_ref(CraDeque *deque)
#define cra_deque_peek_back_ref(deque)      cra_deque_get_ref(deque, (deque)->count - 1)

static inline bool
cra_deque_get_and_set(CraDeque *deque, size_t index, void *newval, void *retoldval)
{
    assert(newval);

    void *pval = cra_deque_get_ref(deque, index);
    if (pval)
    {
        if (retoldval)
            memcpy(retoldval, pval, deque->itemsize);
        memcpy(pval, newval, deque->itemsize);
    }
    return pval != NULL;
}
// bool get_and_set(CraDeque *deque, size_t index, T *newval, out T *retoldval)
#define cra_deque_get_and_set(deque, index, newval, retoldval)                  \
    (CRA_DEQUE_CHECK_VAL(deque, newval), CRA_DEQUE_CHECK_VAL(deque, retoldval), \
     cra_deque_get_and_set(deque, index, newval, retoldval))

// bool set(CraDeque *deque, size_t index, T *val)
#define cra_deque_set(deque, index, val)                                                \
    (CRA_DEQUE_CHECK_VAL(deque, val), (cra_deque_get_and_set)(deque, index, val, NULL))

CRA_API bool
cra_deque_reverse(CraDeque *deque);

// ====================================== interfaces ======================================

// initializable

typedef struct CraDequeInitializableParam
{
    size_t itemsize;
} CraDequeInitializableParam;
#define CRA_DEQUE_INITIALIZABLE_PARAM_INIT(T)        { sizeof(T) }
#define CRA_DEQUE_INITIALIZABLE_PARAM_DECL(var_name) CraDequeInitializableParam var_name
#define CRA_DEQUE_INITIALIZABLE_PARAM_DEF(var_name, T)                                   \
    CRA_DEQUE_INITIALIZABLE_PARAM_DECL(var_name) = CRA_DEQUE_INITIALIZABLE_PARAM_INIT(T)

CRA_API CRA_INITIALIZABLE_DEF(cra_g_deque_initializable_i);
#define CRA_DEQUE_INITIALIZABLE_I (&cra_g_deque_initializable_i)

// appendable

CRA_API CRA_APPENDABLE_DEF(cra_g_deque_appendable_i);
#define CRA_DEQUE_APPENDABLE_I (&cra_g_deque_appendable_i)

// iterable

CRA_API CRA_ITERABLE_DEF(cra_g_deque_iterable_i);
#define CRA_DEQUE_ITERABLE_I (&cra_g_deque_iterable_i)

#endif