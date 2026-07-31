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

typedef enum CraBlockdqFull_e
{
    CRA_BLOCKDQ_FULL_WAIT,
    CRA_BLOCKDQ_FULL_DROP_NEWEST,
    CRA_BLOCKDQ_FULL_DROP_OLDEST,
    CRA_BLOCKDQ_FULL_RETURN_FALSE
} CraBlockdqFull_e;

struct CraBlockdq
{
    CraDeque         deque;
    cra_mutex_t      mutex;
    cra_cond_t       not_full;
    cra_cond_t       not_empty;
    size_t           max_capacity;
    CraBlockdqFull_e full_policy;
    bool             en_colsed;
    bool             de_colsed;
};

#define CRA_BLOCKDQ_INFINITE SIZE_MAX

CRA_API bool
cra_blockdq_init_with_size(CraBlockdq      *deque,
                           size_t           itemsize,
                           size_t           init_capacity,
                           size_t           max_capacity,
                           CraBlockdqFull_e full_policy);
// bool init_with_size<T>(CraBlockdq *deque, size_t init_capacity, size_t max_capacity, CraBlockdqFull_e full_policy)
#define cra_blockdq_init_with_size(T, deque, init_capacity, max_capacity, full_policy)     \
    cra_blockdq_init_with_size(deque, sizeof(T), init_capacity, max_capacity, full_policy)
// bool init<T>(CraBlockdq *deque, size_t max_capacity, CraBlockdqFull_e full_policy)
#define cra_blockdq_init(T, deque, max_capacity, full_policy)          \
    cra_blockdq_init_with_size(T, deque, 0, max_capacity, full_policy)

CRA_API void
cra_blockdq_uninit(CraBlockdq *deque);

#define CRA_BLOCKDQ_CLOSE_ENQUEUE 1
#define CRA_BLOCKDQ_CLOSE_DEQUEUE 2
#define CRA_BLOCKDQ_CLOSE_ALL     (CRA_BLOCKDQ_CLOSE_ENQUEUE | CRA_BLOCKDQ_CLOSE_DEQUEUE)
CRA_API void
cra_blockdq_shutdown(CraBlockdq *deque, int how);

CRA_API bool
cra_blockdq_push_back(CraBlockdq *deque, void *val, void *retdrop);
// bool push_back(CraBlockdq *deque, T *val, out T *retdrop)
//
// retdrop:
//      value to drop if deque is full and full policy is CRA_BLOCKDQ_FULL_DROP_NEWEST or CRA_BLOCKDQ_FULL_DROP_OLDEST.
// returns:
//      true:  val is pushed
//      false: 1. failed to push val to deque
//             2. deque is closed for enqueue
//             3. deque is full and full policy is CRA_BLOCKDQ_FULL_RETURN_FALSE
#define cra_blockdq_push_back(deque, val, retdrop)                                  \
    (CRA_BLOCKDQ_CHECK_VAL(deque, val), cra_blockdq_push_back(deque, val, retdrop))

CRA_API bool
cra_blockdq_push_front(CraBlockdq *deque, void *val, void *retdrop);
// bool push_front(CraBlockdq *deque, T *val, out T *retdrop)
//
// retdrop:
//      value to drop if deque is full and full policy is CRA_BLOCKDQ_FULL_DROP_NEWEST or CRA_BLOCKDQ_FULL_DROP_OLDEST.
// returns:
//      true:  val is pushed
//      false: 1. failed to push val to deque
//             2. deque is closed for enqueue
//             3. deque is full and full policy is CRA_BLOCKDQ_FULL_RETURN_FALSE
#define cra_blockdq_push_front(deque, val, retdrop)                                  \
    (CRA_BLOCKDQ_CHECK_VAL(deque, val), cra_blockdq_push_front(deque, val, retdrop))

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