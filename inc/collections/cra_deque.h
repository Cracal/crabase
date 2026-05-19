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

#define CRA_DEQUE_CHECK_VAL(_deque, _val) assert(sizeof(*(_val)) == (_deque)->itemsize)

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
#define cra_deque_init_with_size(_T, _deque, _init_capacity)     \
    cra_deque_init_with_size(_deque, sizeof(_T), _init_capacity)
// bool init<T>(CraDeque *deque)
#define cra_deque_init(_T, _deque) cra_deque_init_with_size(_T, _deque, 0)

CRA_API void
cra_deque_uninit(CraDeque *deque);

CRA_API void
cra_deque_clear(CraDeque *deque);

CRA_API bool
cra_deque_insert(CraDeque *deque, size_t index, void *val);
CRA_API bool
cra_deque_prepend(CraDeque *deque, void *val);
CRA_API bool
cra_deque_append(CraDeque *deque, void *val);
// bool insert(CraDeque *deque, size_t index, T *val)
#define cra_deque_insert(_deque, _index, _val)                                  \
    (CRA_DEQUE_CHECK_VAL(_deque, _val), cra_deque_insert(_deque, _index, _val))
// bool push_front(CraDeque *deque, T *val)
#define cra_deque_prepend(_deque, _val) (CRA_DEQUE_CHECK_VAL(_deque, _val), cra_deque_prepend(_deque, _val))
// bool push_back(CraDeque *deque, T *val)
#define cra_deque_append(_deque, _val)  (CRA_DEQUE_CHECK_VAL(_deque, _val), cra_deque_append(_deque, _val))

CRA_API bool
cra_deque_pop_at(CraDeque *deque, size_t index, void *retval);
CRA_API bool
cra_deque_pop_front(CraDeque *deque, void *retval);
CRA_API bool
cra_deque_pop_back(CraDeque *deque, void *retval);
// bool pop_at(CraDeque *deque, size_t index, out T *retval)
#define cra_deque_pop_at(_deque, _index, _retval)                                     \
    (CRA_DEQUE_CHECK_VAL(_deque, _retval), cra_deque_pop_at(_deque, _index, _retval))
// bool pop_front(CraDeque *deque, out T *retval)
#define cra_deque_pop_front(_deque, _retval)                                     \
    (CRA_DEQUE_CHECK_VAL(_deque, _retval), cra_deque_pop_front(_deque, _retval))
// bool pop_back(CraDeque *deque, out T *retval)
#define cra_deque_pop_back(_deque, _retval) (CRA_DEQUE_CHECK_VAL(_deque, _retval), cra_deque_pop_back(_deque, _retval))

// bool remove_at(CraDeque *deque, size_t index)
#define cra_deque_remove_at(_deque, _index) (cra_deque_pop_at)(_deque, _index, NULL)
// bool remove_left(CraDeque *deque)
#define cra_deque_remove_front(_deque)      (cra_deque_pop_front)(_deque, NULL)
// bool remove(CraDeque *deque)
#define cra_deque_remove_back(_deque)       (cra_deque_pop_back)(_deque, NULL)

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
#define cra_deque_get(_deque, _index, _retval)                                     \
    (CRA_DEQUE_CHECK_VAL(_deque, _retval), cra_deque_get(_deque, _index, _retval))

// bool peek_front(CraDeque *deque, out T *retval)
#define cra_deque_peek_front(_deque, _retval) cra_deque_get(_deque, 0, _retval)
// T *peek_front_ref(CraDeque *deque)
#define cra_deque_peek_front_ref(_deque)      cra_deque_get_ref(_deque, 0)
// bool peek_back(CraDeque *deque, out T *retval)
#define cra_deque_peek_back(_deque, _retval)  cra_deque_get(_deque, (_deque)->count - 1, _retval)
// T *peek_back_ref(CraDeque *deque)
#define cra_deque_peek_back_ref(_deque)       cra_deque_get_ref(_deque, (_deque)->count - 1)

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
#define cra_deque_get_and_set(_deque, _index, _newval, _retoldval) \
    (CRA_DEQUE_CHECK_VAL(_deque, _newval),                         \
     CRA_DEQUE_CHECK_VAL(_deque, _retoldval),                      \
     cra_deque_get_and_set(_deque, _index, _newval, _retoldval))

// bool set(CraDeque *deque, size_t index, T *val)
#define cra_deque_set(_deque, _index, _val)                                                  \
    (CRA_DEQUE_CHECK_VAL(_deque, _val), (cra_deque_get_and_set)(_deque, _index, _val, NULL))

CRA_API bool
cra_deque_reverse(CraDeque *deque);

// ====================================== interfaces ======================================

// initializable

typedef struct CraDequeInitializableParam
{
    size_t itemsize;
    size_t init_capacity;
} CraDequeInitializableParam;
#define CRA_DEQUE_INITIALIZABLE_PARAM_INIT(_T, _init_capacity) { sizeof(_T), _init_capacity }
#define CRA_DEQUE_INITIALIZABLE_PARAM_DECL(_var_name)          CraDequeInitializableParam _var_name
#define CRA_DEQUE_INITIALIZABLE_PARAM_DEF(_var_name, _T, _init_capacity)                                   \
    CRA_DEQUE_INITIALIZABLE_PARAM_DECL(_var_name) = CRA_DEQUE_INITIALIZABLE_PARAM_INIT(_T, _init_capacity)

CRA_API CRA_INITIALIZABLE_DEF(cra_g_deque_initializable_i);
#define CRA_DEQUE_INITIALIZABLE_I (&cra_g_deque_initializable_i)

// appendable

CRA_API CRA_APPENDABLE_DEF(cra_g_deque_appendable_i);
#define CRA_DEQUE_APPENDABLE_I (&cra_g_deque_appendable_i)

// iterable

CRA_API CRA_ITERABLE_DEF(cra_g_deque_iterable_i);
#define CRA_DEQUE_ITERABLE_I (&cra_g_deque_iterable_i)

#endif