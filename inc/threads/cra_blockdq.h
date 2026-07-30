/**
 * @file cra_blockdq.h
 * @author Cracal
 * @brief blocking double-ended queue
 * @version 0.2
 * @date 2024-09-25
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_BLK_DEQUE_H__
#define __CRA_BLK_DEQUE_H__
#include "threads/cra_lock.h"
#include "collections/cra_deque.h"

#define CRA_BLOCKDQ_CHECK_VAL(deque_, val) assert(sizeof(*(val)) == (deque_)->deque.itemsize)

typedef struct CraBlockdq CraBlockdq;

struct CraBlockdq
{
    CraDeque    deque;
    cra_mutex_t mutex;
    cra_cond_t  condi;
    bool        colsed;
};

CRA_API bool
cra_blockdq_init_with_size(CraBlockdq *deque, size_t itemsize, size_t init_capacity);
// bool init_with_size<T>(CraBlockdq *deque, size_t init_capacity)
#define cra_blockdq_init_with_size(T, deque, init_capacity) cra_blockdq_init_with_size(deque, sizeof(T), init_capacity)
// bool init<T>(CraBlockdq *deque)
#define cra_blockdq_init(T, deque)                          cra_blockdq_init_with_size(T, deque, 0)

CRA_API void
cra_blockdq_uninit(CraBlockdq *deque);

CRA_API void
cra_blockdq_shutdown(CraBlockdq *deque);

CRA_API bool
cra_blockdq_push_back(CraBlockdq *deque, void *val);
// bool push_back(CraBlockdq *deque, T *val)
#define cra_blockdq_push_back(deque, val) (CRA_BLOCKDQ_CHECK_VAL(deque, val), cra_blockdq_push_back(deque, val))

CRA_API bool
cra_blockdq_push_front(CraBlockdq *deque, void *val);
// bool push_front(CraBlockdq *deque, T *val)
#define cra_blockdq_push_front(deque, val) (CRA_BLOCKDQ_CHECK_VAL(deque, val), cra_blockdq_push_front(deque, val))

CRA_API bool
cra_blockdq_pop_back(CraBlockdq *deque, void *retval);
// bool pop_back(CraBlockdq *deque, out T *retval)
#define cra_blockdq_pop_back(deque, retval) (CRA_BLOCKDQ_CHECK_VAL(deque, retval), cra_blockdq_pop_back(deque, retval))

CRA_API bool
cra_blockdq_pop_front(CraBlockdq *deque, void *retval);
// bool pop_front(CraBlockdq *deque, out T *retval)
#define cra_blockdq_pop_front(deque, retval)                                     \
    (CRA_BLOCKDQ_CHECK_VAL(deque, retval), cra_blockdq_pop_front(deque, retval))

#endif