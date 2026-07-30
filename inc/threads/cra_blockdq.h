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

#if 0
#define CRA_BLK_DEQUE_INFINITE                              SIZE_MAX

typedef enum
{
    CRA_BLKDEQUE_STATE_NORMAL,
    CRA_BLKDEQUE_STATE_TERMINATING,
    CRA_BLKDEQUE_STATE_TERMINATED,
} CraBlkDequeState_e;

typedef struct CraBlkDeque
{
    CraDeque           deque;
    CraBlkDequeState_e state;
    size_t             que_max;
    cra_cond_t         not_full;
    cra_cond_t         not_empty;
    cra_mutex_t        mutex;
} CraBlkDeque;

CRA_API bool
cra_blkdeque_init(CraBlkDeque *que, size_t element_size, size_t que_max, bool zero_memory);
#define cra_blkdeque_init0(TVal, que, que_max, zero_memory) cra_blkdeque_init(que, sizeof(TVal), que_max, zero_memory)

CRA_API void
cra_blkdeque_uninit(CraBlkDeque *que);

static inline size_t
cra_blkdeque_get_count(CraBlkDeque *que)
{
    return que->deque.count;
}

static inline size_t
cra_blkdeque_get_que_max(CraBlkDeque *que)
{
    return que->que_max;
}

CRA_API void
cra_blkdeque_clear(CraBlkDeque *que);

CRA_API bool
cra_blkdeque_is_full(CraBlkDeque *que);

CRA_API bool
cra_blkdeque_is_empty(CraBlkDeque *que);

CRA_API void
cra_blkdeque_terminate(CraBlkDeque *que);

CRA_API void
cra_blkdeque_terminate_wait_empty(CraBlkDeque *que);

static inline bool
cra_blkdeque_is_terminated(CraBlkDeque *que)
{
    return que->state == CRA_BLKDEQUE_STATE_TERMINATED ||
           (que->state == CRA_BLKDEQUE_STATE_TERMINATING && cra_blkdeque_get_count(que) == 0);
}

static inline bool
cra_blkdeque_is_normal(CraBlkDeque *que)
{
    return que->state == CRA_BLKDEQUE_STATE_NORMAL;
}

CRA_API bool
cra_blkdeque_push_nonblocking(CraBlkDeque *que, void *val);

CRA_API bool
cra_blkdeque_push_left_nonblocking(CraBlkDeque *que, void *val);

CRA_API bool
cra_blkdeque_pop_nonblocking(CraBlkDeque *que, void *retval);

CRA_API bool
cra_blkdeque_pop_left_nonblocking(CraBlkDeque *que, void *retval);

CRA_API bool
cra_blkdeque_push(CraBlkDeque *que, void *val);

CRA_API bool
cra_blkdeque_push_left(CraBlkDeque *que, void *val);

CRA_API bool
cra_blkdeque_pop(CraBlkDeque *que, void *retval);

CRA_API bool
cra_blkdeque_pop_left(CraBlkDeque *que, void *retval);

CRA_API bool
cra_blkdeque_peek(CraBlkDeque *que, void *retval);

CRA_API bool
cra_blkdeque_peek_left(CraBlkDeque *que, void *retval);

CRA_API bool
cra_blkdeque_peek_ptr(CraBlkDeque *que, void **retvalptr);

CRA_API bool
cra_blkdeque_peek_left_ptr(CraBlkDeque *que, void **retvalptr);
#endif

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