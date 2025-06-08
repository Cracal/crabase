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
#include "threads/cra_locker.h"
#include "collections/cra_deque.h"

#define CRA_BLK_DEQUE_INFINITE CRA_DEQUE_INFINITE

typedef struct _CraBlkDeque
{
    CraDeque deque;
    bool blocking_in;  // 阻塞入队
    bool blocking_out; // 阻塞出队
    size_t que_max;
    cra_cond_t not_full;
    cra_cond_t not_empty;
    cra_mutex_t mutex;
} CraBlkDeque;

CRA_API void cra_blkdeque_init(CraBlkDeque *que, size_t element_size, size_t que_max,
                               bool zero_memory, cra_remove_val_fn remove_val);
#define cra_blkdeque_init0(_TVal, _que, _que_max, _zero_memory, _remove_val_fn) \
    cra_blkdeque_init(_que, sizeof(_TVal), _que_max, _zero_memory, _remove_val_fn)
CRA_API void cra_blkdeque_uninit(CraBlkDeque *que);

static inline size_t cra_blkdeque_get_count(CraBlkDeque *que)
{
    return que->deque.count;
}

static inline size_t cra_blkdeque_get_que_max(CraBlkDeque *que)
{
    return que->que_max;
}

CRA_API void cra_blkdeque_clear(CraBlkDeque *que);

CRA_API bool cra_blkdeque_is_full(CraBlkDeque *que);
CRA_API bool cra_blkdeque_is_empty(CraBlkDeque *que);

CRA_API void cra_blkdeque_dont_block_in(CraBlkDeque *que);
CRA_API void cra_blkdeque_dont_block_out(CraBlkDeque *que);

CRA_API bool cra_blkdeque_push_nonblocking(CraBlkDeque *que, void *val);
CRA_API bool cra_blkdeque_push_left_nonblocking(CraBlkDeque *que, void *val);
CRA_API bool cra_blkdeque_pop_nonblocking(CraBlkDeque *que, void *retval);
CRA_API bool cra_blkdeque_pop_left_nonblocking(CraBlkDeque *que, void *retval);

CRA_API bool cra_blkdeque_push(CraBlkDeque *que, void *val);
CRA_API bool cra_blkdeque_push_left(CraBlkDeque *que, void *val);
CRA_API bool cra_blkdeque_pop(CraBlkDeque *que, void *retval);
CRA_API bool cra_blkdeque_pop_left(CraBlkDeque *que, void *retval);

CRA_API bool cra_blkdeque_peek(CraBlkDeque *que, void *retval);
CRA_API bool cra_blkdeque_peek_left(CraBlkDeque *que, void *retval);
CRA_API bool cra_blkdeque_peek_ptr(CraBlkDeque *que, void **retvalptr);
CRA_API bool cra_blkdeque_peek_left_ptr(CraBlkDeque *que, void **retvalptr);

#endif