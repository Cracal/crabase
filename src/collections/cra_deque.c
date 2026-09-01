/**
 * @file cra_deque.c
 * @author Cracal
 * @brief 双端队列
 * @version 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "collections/cra_deque.h"
#include "cra_malloc.h"

#define CRA_DEQUE_ITEM_SHIFT 6
#define CRA_DEQUE_ITEM_COUNT (1ULL << CRA_DEQUE_ITEM_SHIFT)
#define CRA_DEQUE_ITEM_MASK  (CRA_DEQUE_ITEM_COUNT - 1)
#define CRA_DEQUE_CENTER     ((CRA_DEQUE_ITEM_COUNT - 1) >> 1)
#define CRA_DEQUE_EMPTY_INDEX(deque)            \
    do                                          \
    {                                           \
        (deque)->lindex = CRA_DEQUE_CENTER + 1; \
        (deque)->rindex = CRA_DEQUE_CENTER;     \
        (deque)->front = 0;                     \
        (deque)->rear = 0;                      \
    } while (0)

#define CRA_DEQUE_ARRAY_PVAL(deque, array, index) ((array) + (index) * (deque)->itemsize)

static inline bool
cra_deque_is_pow2(size_t n)
{
    return n > 0 && (n & (n - 1)) == 0;
}

static inline size_t
cra_deque_get_next_pow2(size_t n)
{
    if (n <= 8)
        return 8;

    --n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
#if SIZE_MAX > UINT32_MAX
    n |= n >> 32;
#endif
    return n + 1;
}

static inline bool
cra_deque_expand_array(CraDeque *deque, size_t new_capacity)
{
    unsigned char **new_array;

    new_array = cra_malloc(new_capacity * sizeof(deque->array[0]));
    if (!new_array)
        return false;
    bzero(new_array, new_capacity * sizeof(deque->array[0]));

    // move blocks to new array
    for (size_t i = 0, j = deque->front; i < deque->narray; ++i)
    {
        assert(deque->array[j]);
        new_array[i] = deque->array[j];
        j = (j + 1) & (deque->narray - 1);
    }
    // update front & rear
    deque->front = 0;
    deque->rear = deque->narray - 1;

    cra_free(deque->array);
    deque->array = new_array;
    deque->narray = new_capacity;
    return true;
}

static inline bool
cra_deque_copy_value(CraDeque *deque, size_t iblock, size_t iitem, void *val)
{
    if (!deque->array[iblock])
    {
        deque->array[iblock] = cra_malloc(deque->itemsize * CRA_DEQUE_ITEM_COUNT);
        if (!deque->array[iblock])
            return false;
    }
    memcpy(CRA_DEQUE_ARRAY_PVAL(deque, deque->array[iblock], iitem), val, deque->itemsize);
    return true;
}

static inline bool
cra_deque_ensure_and_increment_left(CraDeque *deque)
{
    if (deque->lindex == 0)
    {
        deque->lindex = CRA_DEQUE_ITEM_COUNT - 1;
        // front = (front - 1) % narray
        size_t front = (deque->front - 1) & (deque->narray - 1);
        if (front == deque->rear)
        {
            if (!cra_deque_expand_array(deque, (deque)->narray << 1))
                return false;
            front = (deque->front - 1) & (deque->narray - 1);
        }
        deque->front = front;

        // create block
        if (!deque->array[deque->front])
        {
            deque->array[deque->front] = cra_malloc(deque->itemsize * CRA_DEQUE_ITEM_COUNT);
            if (!deque->array[deque->front])
                return false;
        }
    }
    else
    {
        --deque->lindex;
    }
    return true;
}

static inline void
cra_deque_decrement_left(CraDeque *deque)
{
    if (deque->lindex == CRA_DEQUE_ITEM_COUNT - 1)
    {
        // front = (front + 1) % narray
        deque->front = (deque->front + 1) & (deque->narray - 1);
        deque->lindex = 0;
    }
    else
    {
        ++deque->lindex;
    }
}

static inline bool
cra_deque_ensure_and_increment_right(CraDeque *deque)
{
    if (deque->rindex == CRA_DEQUE_ITEM_COUNT - 1)
    {
        deque->rindex = 0;
        // rear = (rear + 1) % narray
        size_t rear = (deque->rear + 1) & (deque->narray - 1);
        if (rear == deque->front)
        {
            if (!cra_deque_expand_array(deque, (deque)->narray << 1))
                return false;
            rear = (deque->rear + 1) & (deque->narray - 1);
        }
        deque->rear = rear;

        // create block
        if (!deque->array[deque->rear])
        {
            deque->array[deque->rear] = cra_malloc(deque->itemsize * CRA_DEQUE_ITEM_COUNT);
            if (!deque->array[deque->rear])
                return false;
        }
    }
    else
    {
        ++deque->rindex;
    }
    return true;
}

static inline void
cra_deque_decrement_right(CraDeque *deque)
{
    if (deque->rindex == 0)
    {
        // rear = (rear - 1) % narray
        deque->rear = (deque->rear - 1) & (deque->narray - 1);
        deque->rindex = CRA_DEQUE_ITEM_COUNT - 1;
    }
    else
    {
        --deque->rindex;
    }
}

static inline void
cra_deque_move_items_r2l(CraDeque *deque, size_t start_block, size_t start_item, size_t nitems)
{
    size_t rest = nitems;
    size_t j = start_item;
    size_t k = start_block;
    while (rest > 0)
    {
        if (j == CRA_DEQUE_ITEM_COUNT - 1)
        {
            // update indexes
            size_t last = k;
            if (++k == deque->narray)
                k = 0;
            --rest;
            j = 0;

            assert(deque->array[k]);
            assert(deque->array[last]);
            memcpy(CRA_DEQUE_ARRAY_PVAL(deque, deque->array[last], CRA_DEQUE_ITEM_COUNT - 1),
                   CRA_DEQUE_ARRAY_PVAL(deque, deque->array[k], 0), deque->itemsize);
        }

        size_t n = CRA_DEQUE_ITEM_COUNT - 1 - j;
        if (n > rest)
            n = rest;
        if (n > 0)
        {
            assert(deque->array[k]);
            memmove(CRA_DEQUE_ARRAY_PVAL(deque, deque->array[k], j),
                    CRA_DEQUE_ARRAY_PVAL(deque, deque->array[k], j + 1), n * deque->itemsize);

            rest -= n;
            j += n;
        }
    }
}

static inline void
cra_deque_move_items_l2r(CraDeque *deque, size_t start_block, size_t start_item, size_t nitems)
{
    size_t rest = nitems;
    size_t j = start_item;
    size_t k = start_block;
    while (rest > 0)
    {
        if (j == 0)
        {
            // update indexes
            size_t last = k;
            if (k-- == 0)
                k = deque->narray - 1;
            j = CRA_DEQUE_ITEM_COUNT - 1;
            --rest;

            assert(deque->array[k]);
            assert(deque->array[last]);
            memcpy(CRA_DEQUE_ARRAY_PVAL(deque, deque->array[last], 0),
                   CRA_DEQUE_ARRAY_PVAL(deque, deque->array[k], CRA_DEQUE_ITEM_COUNT - 1), deque->itemsize);
        }

        size_t l = rest < j ? j - rest : 0;
        size_t n = j - l;
        if (n > 0)
        {
            assert(deque->array[k]);
            assert(n < CRA_DEQUE_ITEM_COUNT);
            assert(l + n < CRA_DEQUE_ITEM_COUNT);
            memmove(CRA_DEQUE_ARRAY_PVAL(deque, deque->array[k], l + 1),
                    CRA_DEQUE_ARRAY_PVAL(deque, deque->array[k], l), n * deque->itemsize);

            rest -= n;
            j -= n;
        }
    }
}

// =================

bool(cra_deque_init_with_size)(CraDeque *deque, size_t itemsize, size_t init_capacity)
{
    size_t n;
    size_t size;

    assert(deque);
    assert(itemsize > 0);
    assert(cra_deque_is_pow2(CRA_DEQUE_ITEM_COUNT));

    n = (size_t)ceil(init_capacity / (double)CRA_DEQUE_ITEM_COUNT);
    deque->narray = cra_deque_get_next_pow2(n);
    size = deque->narray * sizeof(deque->array[0]);
    deque->array = cra_malloc(size);
    if (!deque->array)
        return false;
    bzero(deque->array, size);

    CRA_DEQUE_EMPTY_INDEX(deque);
    deque->itemsize = itemsize;
    deque->count = 0;
    return true;
}

void
cra_deque_uninit(CraDeque *deque)
{
    assert(deque);
    assert(deque->array);

    for (size_t i = 0; i < deque->narray; ++i)
    {
        if (deque->array[i])
            cra_free(deque->array[i]);
    }
    cra_free(deque->array);
    bzero(deque, sizeof(*deque));
}

void
cra_deque_clear(CraDeque *deque)
{
    assert(deque);
    assert(deque->array);

    CRA_DEQUE_EMPTY_INDEX(deque);
    deque->count = 0;
}

bool(cra_deque_insert)(CraDeque *deque, size_t index, void *val)
{
    size_t iblock, iitem;

    assert(val);
    assert(deque);
    assert(deque->array);
    assert(cra_deque_is_pow2(deque->narray));

    if (index > deque->count)
        return false;

    if (index < deque->count >> 1)
    {
        if (!cra_deque_ensure_and_increment_left(deque))
            return false;
        // no need to move items if index is 0 (prepend)
        if (index > 0)
            cra_deque_move_items_r2l(deque, deque->front, deque->lindex, index + 1);
    }
    else
    {
        if (!cra_deque_ensure_and_increment_right(deque))
            return false;
        // no need to move items if index is deque->count (append)
        if (index < deque->count)
            cra_deque_move_items_l2r(deque, deque->rear, deque->rindex, deque->count - index);
    }

    index += deque->lindex;
    iitem = index & CRA_DEQUE_ITEM_MASK;
    iblock = (deque->front + (index >> CRA_DEQUE_ITEM_SHIFT)) & (deque->narray - 1);
    if (cra_deque_copy_value(deque, iblock, iitem, val))
    {
        ++deque->count;
        return true;
    }
    return false;
}

bool(cra_deque_push_front)(CraDeque *deque, void *val)
{
    assert(val);
    assert(deque);
    assert(deque->array);

    if (!cra_deque_ensure_and_increment_left(deque))
        return false;
    if (!cra_deque_copy_value(deque, deque->front, deque->lindex, val))
        return false;
    ++deque->count;
    return true;
}

bool(cra_deque_push_back)(CraDeque *deque, void *val)
{
    assert(val);
    assert(deque);
    assert(deque->array);

    if (!cra_deque_ensure_and_increment_right(deque))
        return false;
    if (!cra_deque_copy_value(deque, deque->rear, deque->rindex, val))
        return false;
    ++deque->count;
    return true;
}

bool(cra_deque_pop_at)(CraDeque *deque, size_t index, void *retval)
{
    size_t idx;
    size_t iblock, iitem;

    assert(deque);
    assert(deque->array);
    assert(cra_deque_is_pow2(deque->narray));

    if (index >= deque->count)
        return false;

    idx = index + deque->lindex;
    iitem = idx & CRA_DEQUE_ITEM_MASK;
    iblock = (deque->front + (idx >> CRA_DEQUE_ITEM_SHIFT)) & (deque->narray - 1);

    if (retval)
    {
        memcpy(retval, CRA_DEQUE_ARRAY_PVAL(deque, deque->array[iblock], iitem), deque->itemsize);
    }

    if (index < deque->count >> 1)
    {
        cra_deque_decrement_left(deque);
        if (index > 0)
            cra_deque_move_items_l2r(deque, iblock, iitem, index);
    }
    else
    {
        cra_deque_decrement_right(deque);
        if (index < deque->count - 1)
            cra_deque_move_items_r2l(deque, iblock, iitem, deque->count - 1 - index);
    }

    if (--deque->count == 0)
        CRA_DEQUE_EMPTY_INDEX(deque);

    return true;
}

bool(cra_deque_pop_front)(CraDeque *deque, void *retval)
{
    assert(deque);
    assert(deque->array);

    if (deque->count == 0)
        return false;

    if (retval)
    {
        memcpy(retval, CRA_DEQUE_ARRAY_PVAL(deque, deque->array[deque->front], deque->lindex), deque->itemsize);
    }
    cra_deque_decrement_left(deque);

    if (--deque->count == 0)
        CRA_DEQUE_EMPTY_INDEX(deque);

    return true;
}

bool(cra_deque_pop_back)(CraDeque *deque, void *retval)
{
    assert(deque);
    assert(deque->array);

    if (deque->count == 0)
        return false;

    if (retval)
    {
        memcpy(retval, CRA_DEQUE_ARRAY_PVAL(deque, deque->array[deque->rear], deque->rindex), deque->itemsize);
    }
    cra_deque_decrement_right(deque);

    if (--deque->count == 0)
        CRA_DEQUE_EMPTY_INDEX(deque);

    return true;
}

void *(cra_deque_get_ref)(CraDeque * deque, size_t index)
{
    size_t iblock, iitem;

    assert(deque);
    assert(deque->array);

    if (index >= deque->count)
        return NULL;

    index += deque->lindex;
    iitem = index & CRA_DEQUE_ITEM_MASK;
    iblock = (deque->front + (index >> CRA_DEQUE_ITEM_SHIFT)) & (deque->narray - 1);

    assert(deque->array[iblock]);
    return CRA_DEQUE_ARRAY_PVAL(deque, deque->array[iblock], iitem);
}

bool
cra_deque_reverse(CraDeque *deque)
{
    size_t         i;
    size_t         l, r;
    size_t         f, b;
    unsigned char *temp;
    unsigned char *front, *back;

    assert(deque);
    assert(deque->array);

    if (deque->count < 2)
        return true;

    temp = cra_malloc(deque->itemsize);
    if (!temp)
        return false;

    l = deque->lindex;
    r = deque->rindex;
    f = deque->front;
    b = deque->rear;

    i = 0;
    while (i < deque->count)
    {
        front = CRA_DEQUE_ARRAY_PVAL(deque, deque->array[f], l);
        back = CRA_DEQUE_ARRAY_PVAL(deque, deque->array[b], r);

        // temp = deque[f][l]
        memcpy(temp, front, deque->itemsize);
        // deque[f][l] = deque[b][r]
        memcpy(front, back, deque->itemsize);
        // deque[b][r] = temp
        memcpy(back, temp, deque->itemsize);

        if (l == CRA_DEQUE_ITEM_COUNT - 1)
        {
            l = 0;
            f = (f + 1) & (deque->narray - 1);
        }
        else
        {
            ++l;
        }

        if (r == 0)
        {
            r = CRA_DEQUE_ITEM_COUNT - 1;
            b = (b - 1) & (deque->narray - 1);
        }
        else
        {
            --r;
        }

        i += 2;
    }

    cra_free(temp);
    return true;
}

CRA_FOREACH_NEXT_DEF(CraDeque)
{
    assert(it);
    assert(obj);

    if (!it->initialized)
    {
        it->initialized = true;
        it->block_index = obj->front;
        it->item_index = obj->lindex;
        it->index = 0;
    }

    if (it->index++ >= obj->count)
        return false;

    if (retval)
        memcpy(retval, CRA_DEQUE_ARRAY_PVAL(obj, obj->array[it->block_index], it->item_index), obj->itemsize);

    if (it->item_index++ == CRA_DEQUE_ITEM_COUNT - 1)
    {
        it->item_index = 0;
        if (++it->block_index == obj->narray)
            it->block_index = 0;
    }

    return true;
}

CRA_FOREACH_PREV_DEF(CraDeque)
{
    assert(it);
    assert(obj);

    if (!it->initialized)
    {
        it->initialized = true;
        it->block_index = obj->rear;
        it->item_index = obj->rindex;
        it->index = obj->count;
    }

    if (it->index-- == 0)
        return false;

    if (retval)
        memcpy(retval, CRA_DEQUE_ARRAY_PVAL(obj, obj->array[it->block_index], it->item_index), obj->itemsize);

    if (it->item_index-- == 0)
    {
        it->item_index = CRA_DEQUE_ITEM_COUNT - 1;
        if (it->block_index-- == 0)
            it->block_index = obj->narray - 1;
    }

    return true;
}
