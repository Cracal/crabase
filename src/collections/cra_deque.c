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
#include "cra_malloc.h"
#include "collections/cra_deque.h"

#define CRA_DEQUE_ELE_COUNT 32
#define CRA_DEQUE_CENTER ((CRA_DEQUE_ELE_COUNT - 1) >> 1)

CraDequeIter cra_deque_iter_init(CraDeque *deque)
{
    CraDequeIter it;
    it.index = deque->left_idx;
    it.curr = deque->count > 0 ? deque->list.head : NULL;
    it.deque = deque;
    return it;
}

bool cra_deque_iter_next(CraDequeIter *it, void **retvalptr)
{
    size_t end;
    if (it->curr)
    {
        *retvalptr = it->curr->val + it->index++ * it->deque->ele_size;

        end = it->curr->next == it->deque->list.head
                  ? it->deque->right_idx + 1
                  : CRA_DEQUE_ELE_COUNT;

        if (it->index == end)
        {
            it->index = 0;
            it->curr = it->curr->next;
            if (it->curr == it->deque->list.head)
                it->curr = NULL;
        }
        return true;
    }
    return false;
}

void cra_deque_init(CraDeque *deque, size_t element_size, size_t que_max,
                    bool zero_memory, cra_remove_val_fn remove_val)
{
    assert(!!deque && element_size > 0);

    cra_llist_init(&deque->list, element_size * CRA_DEQUE_ELE_COUNT, zero_memory, NULL);
    CraLListNode *node = cra_llist_get_free_node(&deque->list);
    cra_llist_insert_node(&deque->list, 0, node);

    deque->ele_size = element_size;
    deque->que_max = que_max;
    deque->zero_memory = zero_memory;
    deque->remove_val = remove_val;
    deque->count = 0;
    deque->left_idx = CRA_DEQUE_CENTER + 1;
    deque->right_idx = CRA_DEQUE_CENTER;
}

void cra_deque_uninit(CraDeque *deque)
{
    cra_deque_clear(deque);
    cra_llist_uninit(&deque->list);
}

void cra_deque_clear(CraDeque *deque)
{
    if (deque->count > 0)
    {
        if (deque->remove_val)
        {
            void *valptr;
            for (CraDequeIter it = cra_deque_iter_init(deque); cra_deque_iter_next(&it, (void **)&valptr);)
                deque->remove_val(valptr);
        }
        deque->count = 0;
    }

    while (deque->list.count > 1)
        cra_llist_remove_back(&deque->list);
    deque->left_idx = CRA_DEQUE_CENTER + 1;
    deque->right_idx = CRA_DEQUE_CENTER;
}

bool cra_deque_insert(CraDeque *deque, size_t index, void *val)
{
    if (index > deque->count)
        return false;

    if (index == 0)
        return cra_deque_push_left(deque, val);
    else if (index == deque->count)
        return cra_deque_push(deque, val);

    CraLList *list = &deque->list;
    size_t list_idx = index / CRA_DEQUE_ELE_COUNT;
    size_t idx = (list_idx == 0 ? deque->left_idx + index : index) % CRA_DEQUE_ELE_COUNT;
    CraLListNode *next = NULL, *curr = list->head->prev;

    for (size_t temp_idx, temp_end, i = list->count;;)
    {
        if (!next)
        {
            temp_end = deque->right_idx;
            if (temp_end == CRA_DEQUE_ELE_COUNT - 1)
            {
                next = cra_llist_get_free_node(list);
                deque->right_idx = 0;
                cra_llist_insert_node(list, list->count, next);
            }
            else
            {
                temp_end = ++deque->right_idx;
            }
        }
        else
        {
            temp_end = CRA_DEQUE_ELE_COUNT - 1;
        }
        if (!!next)
            memcpy(next->val, curr->val + (CRA_DEQUE_ELE_COUNT - 1) * deque->ele_size, deque->ele_size);
        temp_idx = (--i == list_idx) ? idx : 0;
        memmove(curr->val + (temp_idx + 1) * deque->ele_size, curr->val + temp_idx * deque->ele_size, (temp_end - temp_idx) * deque->ele_size);

        if (i == list_idx)
            break;

        next = curr;
        curr = curr->prev;
    }
    memcpy(curr->val + idx * deque->ele_size, val, deque->ele_size);
    if (deque->count++ == deque->que_max)
        cra_deque_pop(deque, NULL);
    return true;
}

bool cra_deque_push(CraDeque *deque, void *val)
{
    bool ret = true;
    CraLListNode *curr;
    CraLList *list = &deque->list;
    if (deque->right_idx >= CRA_DEQUE_ELE_COUNT - 1)
    {
        curr = cra_llist_get_free_node(list);
        cra_llist_insert_node(list, list->count, curr);
        deque->right_idx = 0;
    }
    else
    {
        curr = list->head->prev;
        deque->right_idx++;
    }
    memcpy(curr->val + deque->right_idx * deque->ele_size, val, deque->ele_size);
    if (deque->count++ == deque->que_max)
        ret = cra_deque_pop_left(deque, NULL);
    return ret;
}

bool cra_deque_push_left(CraDeque *deque, void *val)
{
    bool ret = true;
    CraLListNode *curr;
    CraLList *list = &deque->list;
    if (deque->left_idx == 0)
    {
        curr = cra_llist_get_free_node(list);
        cra_llist_insert_node(list, 0, curr);
        deque->left_idx = CRA_DEQUE_ELE_COUNT - 1;
    }
    else
    {
        curr = list->head;
        deque->left_idx--;
    }
    memcpy(curr->val + deque->left_idx * deque->ele_size, val, deque->ele_size);
    if (deque->count++ == deque->que_max)
        ret = cra_deque_pop(deque, NULL);
    return ret;
}

static inline bool __cra_deque_pop_at(CraDeque *deque, size_t index, void *retval)
{
    if (index >= deque->count)
        return false;

    if (index == 0)
        return cra_deque_pop_left(deque, retval);
    else if (index == deque->count - 1)
        return cra_deque_pop(deque, retval);

    CraLList *list = &deque->list;
    size_t list_idx = index / CRA_DEQUE_ELE_COUNT;
    size_t idx = (list_idx == 0 ? deque->left_idx + index : index) % CRA_DEQUE_ELE_COUNT;
    CraLListNode *curr = list->head;

    for (size_t i = 0; i < list_idx; i++)
        curr = curr->next;
    if (retval)
        memcpy(retval, curr->val + idx * deque->ele_size, deque->ele_size);
    else if (deque->remove_val)
        deque->remove_val(curr->val + idx * deque->ele_size);

    for (size_t temp_idx, temp_end, i = list_idx; i < list->count;)
    {
        temp_idx = i == list_idx ? idx : 0;
        temp_end = ++i == list->count ? deque->right_idx : CRA_DEQUE_ELE_COUNT - 1;
        if (temp_end == 0)
            goto exit_loop;
        memmove(curr->val + temp_idx * deque->ele_size, curr->val + (temp_idx + 1) * deque->ele_size, (temp_end - temp_idx) * deque->ele_size);
        if (i < list->count)
        {
            memcpy(curr->val + temp_end * deque->ele_size, curr->next->val, deque->ele_size);
        }
        else
        {
        exit_loop:
            if (deque->zero_memory)
                bzero(curr->val + temp_end * deque->ele_size, deque->ele_size);
            break;
        }

        curr = curr->next;
    }
    if (deque->right_idx-- == 0)
    {
        if (list->count > 1)
            cra_llist_remove_back(list);
        deque->right_idx = CRA_DEQUE_ELE_COUNT - 1;
    }
    deque->count--;
    return true;
}

bool cra_deque_remove_at(CraDeque *deque, size_t index)
{
    return __cra_deque_pop_at(deque, index, NULL);
}

bool cra_deque_pop_at(CraDeque *deque, size_t index, void *retval)
{
    return __cra_deque_pop_at(deque, index, retval);
}

bool cra_deque_pop(CraDeque *deque, void *retval)
{
    CraLListNode *curr;
    CraLList *list;
    if (deque->count > 0)
    {
        list = &deque->list;
        curr = list->head->prev;
        // copy value
        if (retval)
            memcpy(retval, curr->val + deque->right_idx * deque->ele_size, deque->ele_size);
        // on remove val
        else if (deque->remove_val)
            deque->remove_val(curr->val + deque->right_idx * deque->ele_size);
        if (deque->zero_memory)
            bzero(curr->val + deque->right_idx * deque->ele_size, deque->ele_size);

        if (deque->right_idx == 0)
        {
            if (list->count > 1)
                cra_llist_remove_back(list);
            deque->right_idx = CRA_DEQUE_ELE_COUNT - 1;
        }
        else
        {
            deque->right_idx--;
        }
        deque->count--;
        return true;
    }
    return false;
}

bool cra_deque_pop_left(CraDeque *deque, void *retval)
{
    CraLListNode *curr;
    CraLList *list;
    if (deque->count > 0)
    {
        list = &deque->list;
        curr = list->head;
        // copy value
        if (retval)
            memcpy(retval, curr->val + deque->left_idx * deque->ele_size, deque->ele_size);
        // on remove val
        else if (deque->remove_val)
            deque->remove_val(curr->val + deque->left_idx * deque->ele_size);
        if (deque->zero_memory)
            bzero(curr->val + deque->left_idx * deque->ele_size, deque->ele_size);

        if (deque->left_idx == CRA_DEQUE_ELE_COUNT - 1)
        {
            deque->left_idx = 0;
            if (list->count > 1)
                cra_llist_remove_front(list);
        }
        else
        {
            deque->left_idx++;
        }
        deque->count--;
        return true;
    }
    return false;
}

static inline bool __cra_deque_peek_ptr(CraDeque *deque, void **retvalptr)
{
    CraLListNode *curr;
    CraLList *list;
    if (deque->count > 0)
    {
        list = &deque->list;
        curr = list->head->prev;
        // copy value pointer
        *retvalptr = curr->val + deque->right_idx * deque->ele_size;
        return true;
    }
    return false;
}

bool cra_deque_peek(CraDeque *deque, void *retval)
{
    void *valptr;
    if (__cra_deque_peek_ptr(deque, &valptr))
    {
        memcpy(retval, valptr, deque->ele_size);
        return true;
    }
    return false;
}

bool cra_deque_peek_ptr(CraDeque *deque, void **retvalptr)
{
    return __cra_deque_peek_ptr(deque, retvalptr);
}

static inline bool __cra_deque_peek_left_ptr(CraDeque *deque, void **retvalptr)
{
    CraLListNode *curr;
    CraLList *list;
    if (deque->count > 0)
    {
        list = &deque->list;
        curr = list->head;
        // copy value pointer
        *retvalptr = curr->val + deque->left_idx * deque->ele_size;
        return true;
    }
    return false;
}

bool cra_deque_peek_left(CraDeque *deque, void *retval)
{
    void *valptr;
    if (__cra_deque_peek_left_ptr(deque, &valptr))
    {
        memcpy(retval, valptr, deque->ele_size);
        return true;
    }
    return false;
}

bool cra_deque_peek_left_ptr(CraDeque *deque, void **retvalptr)
{
    return __cra_deque_peek_left_ptr(deque, retvalptr);
}

void cra_deque_reverse(CraDeque *deque)
{
    size_t left, right;
    void *leftval, *rightval;
    CraLListNode *front, *back, *temp;

    if (deque->count < 2)
        return;

    temp = cra_llist_get_free_node(&deque->list);

    left = deque->left_idx;
    right = deque->right_idx;
    front = deque->list.head;
    back = deque->list.head->prev;
    while (front != back || left < right)
    {
        // swap value
        leftval = front->val + left * deque->ele_size;
        rightval = back->val + right * deque->ele_size;
        memcpy(temp->val, leftval, deque->ele_size);
        memcpy(leftval, rightval, deque->ele_size);
        memcpy(rightval, temp->val, deque->ele_size);

        if (left++ == CRA_DEQUE_ELE_COUNT - 1)
        {
            front = front->next;
            left = 0;
        }
        if (right-- == 0)
        {
            back = back->prev;
            right = CRA_DEQUE_ELE_COUNT - 1;
        }
    }
    cra_llist_put_free_node(&deque->list, temp);
}

CraDeque *cra_deque_clone(CraDeque *deque, cra_deep_copy_val_fn deep_copy_val)
{
    CraDeque *ret;
    void *valptr, *val;

    ret = cra_alloc(CraDeque);
    cra_deque_init(ret, deque->ele_size, deque->que_max, deque->zero_memory, deque->remove_val);

    for (CraDequeIter it = cra_deque_iter_init(deque); cra_deque_iter_next(&it, &valptr);)
    {
        if (deep_copy_val)
        {
            deep_copy_val(valptr, &val);
            valptr = &val;
        }
        cra_deque_push(ret, valptr);
    }
    return ret;
}

// ===============

static void cra_deque_ser_iter_init(void *obj, void *const it, size_t itbufsize)
{
    CRA_UNUSED_VALUE(itbufsize);
    assert(sizeof(CraDequeIter) <= itbufsize);
    CraDeque *list = (CraDeque *)obj;
    CraDequeIter _it = cra_deque_iter_init(list);
    memcpy(it, &_it, sizeof(CraDequeIter));
}

static void cra_deque_ser_init(void *obj, void *args)
{
    assert_always(args != NULL);

    CraDeque *list = (CraDeque *)obj;
    CraDequeSerInitArgs *params = (CraDequeSerInitArgs *)args;

    cra_deque_init(list, params->element_size, params->que_max, params->zero_memory, params->remove_val_fn);
}

const CraTypeIter_i g_deque_ser_iter_i = {
    .list.init = cra_deque_ser_iter_init,
    .list.next = (bool (*)(void *, void **))cra_deque_iter_next,
    .list.append = (bool (*)(void *, void *))cra_deque_push,
};

const CraTypeInit_i g_deque_ser_init_i = {
    .alloc = NULL,
    .dealloc = NULL,
    .init = cra_deque_ser_init,
    .uinit = (void (*)(void *))cra_deque_uninit,
};
