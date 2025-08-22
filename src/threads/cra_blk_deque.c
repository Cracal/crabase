/**
 * @file cra_blk-deque.c
 * @author Cracal
 * @brief blocking deque
 * @version 0.2
 * @date 2024-09-25
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "threads/cra_blk_deque.h"

void
cra_blkdeque_init(CraBlkDeque *que, size_t element_size, size_t que_max, bool zero_memory, cra_remove_val_fn remove_val)
{
    cra_deque_init(&que->deque, element_size, CRA_DEQUE_INFINITE, zero_memory, remove_val);
    que->state = CRA_BLKDEQUE_STATE_NORMAL;
    que->que_max = que_max;
    cra_cond_init(&que->not_full);
    cra_cond_init(&que->not_empty);
    cra_mutex_init(&que->mutex);
}

void
cra_blkdeque_uninit(CraBlkDeque *que)
{
    cra_deque_uninit(&que->deque);
    cra_cond_destroy(&que->not_full);
    cra_cond_destroy(&que->not_empty);
    cra_mutex_destroy(&que->mutex);
}

void
cra_blkdeque_clear(CraBlkDeque *que)
{
    cra_mutex_lock(&que->mutex);
    cra_deque_clear(&que->deque);
    cra_mutex_unlock(&que->mutex);
}

bool
cra_blkdeque_is_full(CraBlkDeque *que)
{
    bool ret;
    // 是无限队列时直接返回false
    if (que->que_max == CRA_BLK_DEQUE_INFINITE)
        return false;
    cra_mutex_lock(&que->mutex);
    ret = (que->deque.count == que->que_max);
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_is_empty(CraBlkDeque *que)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    ret = (que->deque.count == 0);
    cra_mutex_unlock(&que->mutex);
    return ret;
}

void
cra_blkdeque_terminate_wait_empty(CraBlkDeque *que)
{
    cra_mutex_lock(&que->mutex);
    que->state = CRA_BLKDEQUE_STATE_TERMINATING;
    cra_cond_broadcast(&que->not_empty);
    cra_cond_broadcast(&que->not_full);
    cra_mutex_unlock(&que->mutex);
}

void
cra_blkdeque_terminate(CraBlkDeque *que)
{
    cra_mutex_lock(&que->mutex);
    que->state = CRA_BLKDEQUE_STATE_TERMINATED;
    cra_cond_broadcast(&que->not_empty);
    cra_cond_broadcast(&que->not_full);
    cra_mutex_unlock(&que->mutex);
}

bool
cra_blkdeque_push_nonblocking(CraBlkDeque *que, void *val)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    // 不是无限队列并且队列满或不是正常状态时不添加，返回false
    if ((que->que_max != CRA_BLK_DEQUE_INFINITE && que->deque.count >= que->que_max) || !cra_blkdeque_is_normal(que))
    {
        ret = false;
        goto end;
    }
    if ((ret = cra_deque_push(&que->deque, val)))
        cra_cond_signal(&que->not_empty);
end:
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_push_left_nonblocking(CraBlkDeque *que, void *val)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    // 不是无限队列并且队列满或不是正常状态时不添加，返回false
    if ((que->que_max != CRA_BLK_DEQUE_INFINITE && que->deque.count >= que->que_max) || !cra_blkdeque_is_normal(que))
    {
        ret = false;
        goto end;
    }
    if ((ret = cra_deque_push_left(&que->deque, val)))
        cra_cond_signal(&que->not_empty);
end:
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_pop_nonblocking(CraBlkDeque *que, void *retval)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    if (que->deque.count == 0 || cra_blkdeque_is_terminated(que))
    {
        ret = false;
        goto end;
    }
    if ((ret = cra_deque_pop(&que->deque, retval)))
        cra_cond_signal(&que->not_full);
end:
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_pop_left_nonblocking(CraBlkDeque *que, void *retval)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    if (que->deque.count == 0 || cra_blkdeque_is_terminated(que))
    {
        ret = false;
        goto end;
    }
    if ((ret = cra_deque_pop_left(&que->deque, retval)))
        cra_cond_signal(&que->not_full);
end:
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_push(CraBlkDeque *que, void *val)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    if (!cra_blkdeque_is_normal(que))
    {
        ret = false;
        goto end;
    }
    if (que->que_max != CRA_BLK_DEQUE_INFINITE)
    {
        while (que->deque.count >= que->que_max)
        {
            if (cra_blkdeque_is_normal(que))
            {
                cra_cond_wait(&que->not_full, &que->mutex);
            }
            else
            {
                ret = false;
                goto end;
            }
        }
    }
    if ((ret = cra_deque_push(&que->deque, val)))
        cra_cond_signal(&que->not_empty);
    else
        ret = true;
end:
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_push_left(CraBlkDeque *que, void *val)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    if (!cra_blkdeque_is_normal(que))
    {
        ret = false;
        goto end;
    }
    if (que->que_max != CRA_BLK_DEQUE_INFINITE)
    {
        while (que->deque.count >= que->que_max)
        {
            if (cra_blkdeque_is_normal(que))
            {
                cra_cond_wait(&que->not_full, &que->mutex);
            }
            else
            {
                ret = false;
                goto end;
            }
        }
    }
    if ((ret = cra_deque_push_left(&que->deque, val)))
        cra_cond_signal(&que->not_empty);
    else
        ret = true;
end:
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_pop(CraBlkDeque *que, void *retval)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    if (cra_blkdeque_is_terminated(que))
    {
        ret = false;
        goto end;
    }
    while (que->deque.count == 0)
    {
        if (!cra_blkdeque_is_terminated(que))
        {
            cra_cond_wait(&que->not_empty, &que->mutex);
        }
        else
        {
            ret = false;
            goto end;
        }
    }
    if ((ret = cra_deque_pop(&que->deque, retval)))
        cra_cond_signal(&que->not_full);
    else
        ret = true;
end:
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_pop_left(CraBlkDeque *que, void *retval)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    if (cra_blkdeque_is_terminated(que))
    {
        ret = false;
        goto end;
    }
    while (que->deque.count == 0)
    {
        if (!cra_blkdeque_is_terminated(que))
        {
            cra_cond_wait(&que->not_empty, &que->mutex);
        }
        else
        {
            ret = false;
            goto end;
        }
    }
    if ((ret = cra_deque_pop_left(&que->deque, retval)))
        cra_cond_signal(&que->not_full);
    else
        ret = true;
end:
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_peek(CraBlkDeque *que, void *retval)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    ret = cra_deque_peek(&que->deque, retval);
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_peek_left(CraBlkDeque *que, void *retval)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    ret = cra_deque_peek_left(&que->deque, retval);
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_peek_ptr(CraBlkDeque *que, void **retvalptr)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    ret = cra_deque_peek_ptr(&que->deque, retvalptr);
    cra_mutex_unlock(&que->mutex);
    return ret;
}

bool
cra_blkdeque_peek_left_ptr(CraBlkDeque *que, void **retvalptr)
{
    bool ret;
    cra_mutex_lock(&que->mutex);
    ret = cra_deque_peek_left_ptr(&que->deque, retvalptr);
    cra_mutex_unlock(&que->mutex);
    return ret;
}
