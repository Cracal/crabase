/**
 * @file cra_blk_deque.h
 * @author Cracal
 * @brief blocking deque
 * @version 0.2
 * @date 2024-09-25
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_BLK_DEQUE_H__
#define __CRA_BLK_DEQUE_H__
#include "collections/cra_deque.h"
#include "threads/cra_locker.h"

#define CRA_BLK_DEQUE_INFINITE CRA_DEQUE_INFINITE

typedef enum
{
    CRA_BLKDEQUE_STATE_NORMAL,
    CRA_BLKDEQUE_STATE_TERMINATING,
    CRA_BLKDEQUE_STATE_TERMINATED,
} CraBlkDequeState_e;

typedef struct _CraBlkDeque
{
    CraDeque           deque;
    CraBlkDequeState_e state;
    size_t             que_max;
    cra_cond_t         not_full;
    cra_cond_t         not_empty;
    cra_mutex_t        mutex;
} CraBlkDeque;

CRA_API void
cra_blkdeque_init(CraBlkDeque *que, size_t element_size, size_t que_max, bool zero_memory);
#define cra_blkdeque_init0(_TVal, _que, _que_max, _zero_memory)    \
    cra_blkdeque_init(_que, sizeof(_TVal), _que_max, _zero_memory)

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