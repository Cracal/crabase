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
#include "cra_llist.h"

typedef struct _CraDeque
{
    bool zero_memory;
    size_t ele_size;
    size_t count;
    size_t que_max;
    size_t left_idx;
    size_t right_idx;
    CraLList list;
    cra_remove_val_fn remove_val;
} CraDeque;

typedef struct _CraDequeIter
{
    size_t index;
    CraLListNode *curr;
    CraDeque *deque;
} CraDequeIter;

#define CRA_DEQUE_INFINITE SIZE_MAX

CRA_API CraDequeIter cra_deque_iter_init(CraDeque *deque);
CRA_API bool cra_deque_iter_next(CraDequeIter *it, void **retvalptr);

CRA_API void cra_deque_init(CraDeque *deque, size_t element_size, size_t que_max, bool zero_memory, cra_remove_val_fn remove_val);
#define cra_deque_init0(_TVal, _deque, _que_max, _zero_memory, _remove_val_fn) \
    cra_deque_init(_deque, sizeof(_TVal), _que_max, _zero_memory, _remove_val_fn)
CRA_API void cra_deque_uninit(CraDeque *deque);

static inline size_t cra_deque_get_count(CraDeque *deque) { return deque->count; }

CRA_API void cra_deque_clear(CraDeque *deque);

CRA_API bool cra_deque_insert(CraDeque *deque, size_t index, void *val);
CRA_API bool cra_deque_push(CraDeque *deque, void *val);
CRA_API bool cra_deque_push_left(CraDeque *deque, void *val);

CRA_API bool cra_deque_remove_at(CraDeque *deque, size_t index);

CRA_API bool cra_deque_pop_at(CraDeque *deque, size_t index, void *retval);
CRA_API bool cra_deque_pop(CraDeque *deque, void *retval);
CRA_API bool cra_deque_pop_left(CraDeque *deque, void *retval);

CRA_API bool cra_deque_set(CraDeque *deque, size_t index, void *newval);
CRA_API bool cra_deque_set_and_pop_old(CraDeque *deque, size_t index, void *newval, void *retoldval);

CRA_API bool cra_deque_get(CraDeque *deque, size_t index, void *retval);
CRA_API bool cra_deque_get_ptr(CraDeque *deque, size_t index, void **retvalptr);

CRA_API bool cra_deque_peek(CraDeque *deque, void *retval);
CRA_API bool cra_deque_peek_ptr(CraDeque *deque, void **retvalptr);
CRA_API bool cra_deque_peek_left(CraDeque *deque, void *retval);
CRA_API bool cra_deque_peek_left_ptr(CraDeque *deque, void **retvalptr);

CRA_API void cra_deque_reverse(CraDeque *deque);

CRA_API CraDeque *cra_deque_clone(CraDeque *deque, cra_deep_copy_val_fn deep_copy_val);

// =================

typedef struct _CraDequeSerInitArgs
{
    bool zero_memory;
    size_t que_max;
    size_t element_size;
    cra_remove_val_fn remove_val_fn;
} CraDequeSerInitArgs;

CRA_API const CraTypeIter_i g_deque_ser_iter_i;
CRA_API const CraTypeInit_i g_deque_ser_init_i;

#define CRA_DEQUE_SER_ARGS(_name, _zero_memory, _que_max, _element_size, _remove_val_fn) \
    CraDequeSerInitArgs _name = {_zero_memory, _que_max, _element_size, _remove_val_fn}
#define CRA_DEQUE_SER_ARGS0(_name, _initialied_deque) \
    CRA_DEQUE_SER_ARGS(_name, (_initialied_deque)->zero_memory, (_initialied_deque)->que_max, (_initialied_deque)->ele_size, (_initialied_deque)->remove_val)

#endif