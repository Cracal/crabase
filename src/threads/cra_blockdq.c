/**
 * @file cra_blockdq.c
 * @author Cracal
 * @brief blocking double-ended queue
 * @version 0.2
 * @date 2024-09-25
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "threads/cra_blockdq.h"

bool(cra_blockdq_init_with_size)(CraBlockdq      *deque,
                                 size_t           itemsize,
                                 size_t           init_capacity,
                                 size_t           max_capacity,
                                 CraBlockdqFull_e full_policy)
{
    assert(deque);
    assert(itemsize > 0);
    assert(max_capacity > 0);
    assert(max_capacity >= init_capacity);

    if (!(cra_deque_init_with_size)(&deque->deque, itemsize, init_capacity))
        return false;

    cra_mutex_init(&deque->mutex);
    cra_cond_init(&deque->not_full);
    cra_cond_init(&deque->not_empty);
    deque->max_capacity = max_capacity;
    deque->full_policy = full_policy;
    deque->en_colsed = false;
    deque->de_colsed = false;

    return true;
}

void
cra_blockdq_uninit(CraBlockdq *deque)
{
    assert(deque);
    assert(deque->en_colsed && deque->de_colsed);

    cra_deque_uninit(&deque->deque);
    cra_cond_destroy(&deque->not_full);
    cra_cond_destroy(&deque->not_empty);
    cra_mutex_destroy(&deque->mutex);

    // bzero(deque, sizeof(*deque));
}

void
cra_blockdq_shutdown(CraBlockdq *deque, int how)
{
    assert(deque);
    assert(!deque->en_colsed || !deque->de_colsed);

    cra_mutex_lock(&deque->mutex);
    if (!deque->en_colsed && (how & CRA_BLOCKDQ_CLOSE_ENQUEUE))
    {
        deque->en_colsed = true;
        cra_cond_broadcast(&deque->not_full);
    }
    if (!deque->de_colsed && (how & CRA_BLOCKDQ_CLOSE_DEQUEUE))
    {
        deque->de_colsed = true;
        cra_cond_broadcast(&deque->not_empty);
    }
    cra_mutex_unlock(&deque->mutex);
}

bool(cra_blockdq_push_back)(CraBlockdq *deque, void *val, void *retdrop)
{
    bool ret = false;

    assert(val);
    assert(deque);

    cra_mutex_lock(&deque->mutex);
    while (!deque->en_colsed && deque->deque.count == deque->max_capacity)
    {
        switch (deque->full_policy)
        {
            case CRA_BLOCKDQ_FULL_WAIT:
                cra_cond_wait(&deque->not_full, &deque->mutex);
                break;
            case CRA_BLOCKDQ_FULL_DROP_NEWEST:
                (cra_deque_pop_back)(&deque->deque, retdrop);
                goto enque;
            case CRA_BLOCKDQ_FULL_DROP_OLDEST:
                (cra_deque_pop_front)(&deque->deque, retdrop);
                goto enque;
            case CRA_BLOCKDQ_FULL_RETURN_FALSE:
                assert(ret == false);
                goto end;
            default:
                assert_always(false && "Invalid full policy");
        }
    }
enque:
    if (!deque->en_colsed && (ret = (cra_deque_push_back)(&deque->deque, val)))
        cra_cond_signal(&deque->not_empty);
end:
    cra_mutex_unlock(&deque->mutex);

    return ret;
}

bool(cra_blockdq_push_front)(CraBlockdq *deque, void *val, void *retdrop)
{
    bool ret = false;

    assert(val);
    assert(deque);

    cra_mutex_lock(&deque->mutex);
    while (!deque->en_colsed && deque->deque.count == deque->max_capacity)
    {
        switch (deque->full_policy)
        {
            case CRA_BLOCKDQ_FULL_WAIT:
                cra_cond_wait(&deque->not_full, &deque->mutex);
                break;
            case CRA_BLOCKDQ_FULL_DROP_NEWEST:
                (cra_deque_pop_front)(&deque->deque, retdrop);
                goto enque;
            case CRA_BLOCKDQ_FULL_DROP_OLDEST:
                (cra_deque_pop_back)(&deque->deque, retdrop);
                goto enque;
            case CRA_BLOCKDQ_FULL_RETURN_FALSE:
                assert(ret == false);
                goto end;
            default:
                assert_always(false && "Invalid full policy");
        }
    }
enque:
    if (!deque->en_colsed && (ret = (cra_deque_push_front)(&deque->deque, val)))
        cra_cond_signal(&deque->not_empty);
end:
    cra_mutex_unlock(&deque->mutex);

    return ret;
}

bool(cra_blockdq_pop_back)(CraBlockdq *deque, void *retval)
{
    bool ret;

    assert(retval);
    assert(deque);

    cra_mutex_lock(&deque->mutex);
    while (!deque->de_colsed && deque->deque.count == 0)
        cra_cond_wait(&deque->not_empty, &deque->mutex);
    // assert(deque->deque.count > 0);
    if ((ret = (cra_deque_pop_back)(&deque->deque, retval)))
        cra_cond_signal(&deque->not_full);
    cra_mutex_unlock(&deque->mutex);

    return ret;
}

bool(cra_blockdq_pop_front)(CraBlockdq *deque, void *retval)
{
    bool ret;

    assert(retval);
    assert(deque);

    cra_mutex_lock(&deque->mutex);
    while (!deque->de_colsed && deque->deque.count == 0)
        cra_cond_wait(&deque->not_empty, &deque->mutex);
    // assert(deque->deque.count > 0);
    if ((ret = (cra_deque_pop_front)(&deque->deque, retval)))
        cra_cond_signal(&deque->not_full);
    cra_mutex_unlock(&deque->mutex);

    return ret;
}