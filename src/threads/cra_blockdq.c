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

bool(cra_blockdq_init_with_size)(CraBlockdq *deque, size_t itemsize, size_t init_capacity)
{
    assert(deque);
    assert(itemsize > 0);

    if (!(cra_deque_init_with_size)(&deque->deque, itemsize, init_capacity))
        return false;

    cra_mutex_init(&deque->mutex);
    cra_cond_init(&deque->condi);
    deque->colsed = false;

    return true;
}

void
cra_blockdq_uninit(CraBlockdq *deque)
{
    assert(deque);
    assert(deque->colsed);

    cra_deque_uninit(&deque->deque);
    cra_cond_destroy(&deque->condi);
    cra_mutex_destroy(&deque->mutex);
}

void
cra_blockdq_shutdown(CraBlockdq *deque)
{
    assert(deque);
    assert(!deque->colsed);

    cra_mutex_lock(&deque->mutex);
    deque->colsed = true;
    cra_cond_broadcast(&deque->condi);
    cra_mutex_unlock(&deque->mutex);
}

bool(cra_blockdq_push_back)(CraBlockdq *deque, void *val)
{
    bool ret = false;

    assert(val);
    assert(deque);

    cra_mutex_lock(&deque->mutex);
    if (!deque->colsed)
    {
        if ((ret = (cra_deque_push_back)(&deque->deque, val)))
            cra_cond_signal(&deque->condi);
    }
    cra_mutex_unlock(&deque->mutex);

    return ret;
}

bool(cra_blockdq_push_front)(CraBlockdq *deque, void *val)
{
    bool ret = false;

    assert(val);
    assert(deque);

    cra_mutex_lock(&deque->mutex);
    if (!deque->colsed)
    {
        if ((ret = (cra_deque_push_front)(&deque->deque, val)))
            cra_cond_signal(&deque->condi);
    }
    cra_mutex_unlock(&deque->mutex);

    return ret;
}

bool(cra_blockdq_pop_back)(CraBlockdq *deque, void *retval)
{
    bool ret;

    assert(retval);
    assert(deque);

    cra_mutex_lock(&deque->mutex);
    while (!deque->colsed && deque->deque.count == 0)
        cra_cond_wait(&deque->condi, &deque->mutex);
    // assert(deque->deque.count > 0);
    ret = (cra_deque_pop_back)(&deque->deque, retval);
    cra_mutex_unlock(&deque->mutex);

    return ret;
}

bool(cra_blockdq_pop_front)(CraBlockdq *deque, void *retval)
{
    bool ret;

    assert(retval);
    assert(deque);

    cra_mutex_lock(&deque->mutex);
    while (!deque->colsed && deque->deque.count == 0)
        cra_cond_wait(&deque->condi, &deque->mutex);
    // assert(deque->deque.count > 0);
    ret = (cra_deque_pop_front)(&deque->deque, retval);
    cra_mutex_unlock(&deque->mutex);

    return ret;
}