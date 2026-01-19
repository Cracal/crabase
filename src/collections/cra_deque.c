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
#include "cra_assert.h"
#include "cra_malloc.h"

#define CRA_DEQUE_ELE_COUNT 32
#define CRA_DEQUE_CENTER    ((CRA_DEQUE_ELE_COUNT - 1) >> 1)
#define CRA_DEQUE_EMPTY_INDEX               \
    deque->left_idx = CRA_DEQUE_CENTER + 1; \
    deque->right_idx = CRA_DEQUE_CENTER

void
cra_deque_iter_init(CraDeque *deque, CraDequeIter *it)
{
    it->index = deque->left_idx;
    it->curr = deque->count > 0 ? deque->list.head : NULL;
    it->deque = deque;
}

bool
cra_deque_iter_next(CraDequeIter *it, void **retvalptr)
{
    size_t end;
    if (it->curr)
    {
        if (retvalptr)
            *retvalptr = it->curr->val + it->index++ * it->deque->ele_size;

        end = it->curr->next == it->deque->list.head ? it->deque->right_idx + 1 : CRA_DEQUE_ELE_COUNT;

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

void
cra_deque_init(CraDeque *deque, size_t element_size, size_t que_max, bool zero_memory)
{
    assert(!!deque && element_size > 0);

    cra_llist_init(&deque->list, element_size * CRA_DEQUE_ELE_COUNT, zero_memory);
    CraLListNode *node = cra_llist_get_free_node(&deque->list);
    cra_llist_insert_node(&deque->list, 0, node);

    deque->ele_size = element_size;
    deque->que_max = que_max;
    deque->zero_memory = zero_memory;
    deque->count = 0;
    CRA_DEQUE_EMPTY_INDEX;
}

void
cra_deque_uninit(CraDeque *deque)
{
    cra_deque_clear(deque);
    cra_llist_uninit(&deque->list);
}

void
cra_deque_clear(CraDeque *deque)
{
    deque->count = 0;
    CRA_DEQUE_EMPTY_INDEX;
    if (deque->zero_memory)
        bzero(deque->list.head->val, deque->list.ele_size);

    while (deque->list.count > 1)
        cra_llist_remove_back(&deque->list);
}

bool
cra_deque_insert(CraDeque *deque, size_t index, void *val)
{
    if (index > deque->count)
        return false;

    if (index == 0)
        return cra_deque_push_left(deque, val);
    else if (index == deque->count)
        return cra_deque_push(deque, val);

    bool      ret = true;
    CraLList *list = &deque->list;
    // 计算目标链表结点下标
    size_t    node_idx = (index + deque->left_idx) / CRA_DEQUE_ELE_COUNT;
    // 计算目标在结点中的下标
    size_t    real_idx = (index + deque->left_idx) % CRA_DEQUE_ELE_COUNT;

    size_t        move_count = 0;
    CraLListNode *curr = list->head->prev, *next = NULL;
    for (size_t i = list->count - 1;; --i)
    {
        // 是最后面的结点
        if (curr->next == list->head)
        {
            if (deque->right_idx == CRA_DEQUE_ELE_COUNT - 1)
            {
                move_count = CRA_DEQUE_ELE_COUNT - 1;
                next = cra_llist_get_free_node(list);
                cra_llist_insert_node(list, list->count, next);
                deque->right_idx = 0;
            }
            else
            {
                next = NULL;
                ++deque->right_idx;
                // 在++后，因为没有next，所以单独move的那一个元素要一起移动
                move_count = deque->right_idx;
            }
        }

        // 将本结点的最后一个元素移动到下一个结点的第一个位置
        if (!!next)
            memmove(next->val, curr->val + (CRA_DEQUE_ELE_COUNT - 1) * deque->ele_size, deque->ele_size);

        // 非目标结点时
        if (i > node_idx)
        {
            // 移动本结点元素，空出要插入的位置
            memmove(curr->val + deque->ele_size, curr->val, move_count * deque->ele_size);
        }
        // 在目标结点时
        else
        {
            move_count -= real_idx;
            // 移动本结点元素，空出要插入的位置
            memmove(curr->val + (real_idx + 1) * deque->ele_size,
                    curr->val + real_idx * deque->ele_size,
                    move_count * deque->ele_size);

            // 插入
            memcpy(curr->val + real_idx * deque->ele_size, val, deque->ele_size);
            break;
        }

        next = curr;
        curr = curr->prev;

        move_count = CRA_DEQUE_ELE_COUNT - 1;
    }
    if (deque->count == deque->que_max)
        ret = cra_deque_remove_at(deque, deque->count - 1);
    ++deque->count;
    return ret;
}

bool
cra_deque_push(CraDeque *deque, void *val)
{
    bool          ret = true;
    CraLListNode *curr;
    CraLList     *list = &deque->list;
    assert(deque->right_idx < CRA_DEQUE_ELE_COUNT);
    if (deque->right_idx == CRA_DEQUE_ELE_COUNT - 1)
    {
        curr = cra_llist_get_free_node(list);
        cra_llist_insert_node(list, list->count, curr);
        deque->right_idx = 0;
    }
    else
    {
        curr = list->head->prev;
        ++deque->right_idx;
    }
    memcpy(curr->val + deque->right_idx * deque->ele_size, val, deque->ele_size);
    if (deque->count == deque->que_max)
        ret = cra_deque_remove_at(deque, 0);
    ++deque->count;
    return ret;
}

bool
cra_deque_push_left(CraDeque *deque, void *val)
{
    bool          ret = true;
    CraLListNode *curr;
    CraLList     *list = &deque->list;
    if (deque->left_idx == 0)
    {
        curr = cra_llist_get_free_node(list);
        cra_llist_insert_node(list, 0, curr);
        deque->left_idx = CRA_DEQUE_ELE_COUNT - 1;
    }
    else
    {
        curr = list->head;
        --deque->left_idx;
    }
    memcpy(curr->val + deque->left_idx * deque->ele_size, val, deque->ele_size);
    if (deque->count == deque->que_max)
        ret = cra_deque_remove_at(deque, deque->count - 1);
    ++deque->count;
    return ret;
}

static inline bool
__cra_deque_pop_at(CraDeque *deque, size_t index, void *retval)
{
    if (index >= deque->count)
        return false;

    if (index == 0)
        return cra_deque_pop_left(deque, retval);
    else if (index == deque->count - 1)
        return cra_deque_pop(deque, retval);

    CraLList *list = &deque->list;
    // 计算目标链表结点下标
    size_t    node_idx = (index + deque->left_idx) / CRA_DEQUE_ELE_COUNT;
    // 计算目标在结点中的下标
    size_t    real_idx = (index + deque->left_idx) % CRA_DEQUE_ELE_COUNT;

    size_t        move_count = 0;
    CraLListNode *curr = list->head, *next, *node;
    // 找到目标结点
    for (size_t i = 0; i < node_idx; ++i)
        curr = curr->next;
    next = curr->next;
    node = curr;

    // copy value
    if (retval)
        memcpy(retval, curr->val + real_idx * deque->ele_size, deque->ele_size);

    for (;;)
    {
        if (next == list->head)
            move_count = deque->right_idx; // is last node
        else
            move_count = CRA_DEQUE_ELE_COUNT - 1;

        // 在目标结点时
        if (curr == node)
        {
            // 移动本结点的元素并覆盖目标位置
            memmove(curr->val + real_idx * deque->ele_size,
                    curr->val + (real_idx + 1) * deque->ele_size,
                    (move_count - real_idx) * deque->ele_size);
        }
        // 非目标结点时
        else
        {
            // 移动本结点的元素并覆盖目标位置
            memmove(curr->val, curr->val + deque->ele_size, move_count * deque->ele_size);
        }
        // 不是最后一个结点时，移动下一个结点的第一个元素到本结点末尾
        if (next != list->head)
            memmove(curr->val + (CRA_DEQUE_ELE_COUNT - 1) * deque->ele_size, next->val, deque->ele_size);
        else
        {
            if (deque->zero_memory)
                bzero(curr->val + deque->right_idx * deque->ele_size, deque->ele_size);
            break; // done
        }

        curr = curr->next;
        next = curr->next;
    }

    if (deque->right_idx == 0)
    {
        if (list->count > 1)
        {
            cra_llist_remove_back(list);
            deque->right_idx = CRA_DEQUE_ELE_COUNT - 1;
        }
        else
        {
            CRA_DEQUE_EMPTY_INDEX;
        }
    }
    else
    {
        --deque->right_idx;
    }
    --deque->count;
    return true;
}

bool
cra_deque_remove_at(CraDeque *deque, size_t index)
{
    return __cra_deque_pop_at(deque, index, NULL);
}

bool
cra_deque_pop_at(CraDeque *deque, size_t index, void *retval)
{
    return __cra_deque_pop_at(deque, index, retval);
}

bool
cra_deque_pop(CraDeque *deque, void *retval)
{
    CraLListNode *curr;
    CraLList     *list;
    if (deque->count > 0)
    {
        list = &deque->list;
        curr = list->head->prev;
        // copy value
        if (retval)
            memcpy(retval, curr->val + deque->right_idx * deque->ele_size, deque->ele_size);
        if (deque->zero_memory)
            bzero(curr->val + deque->right_idx * deque->ele_size, deque->ele_size);

        if (deque->right_idx == 0)
        {
            if (list->count > 1)
            {
                cra_llist_remove_back(list);
                deque->right_idx = CRA_DEQUE_ELE_COUNT - 1;
            }
            else
            {
                CRA_DEQUE_EMPTY_INDEX;
            }
        }
        else
        {
            --deque->right_idx;
        }
        --deque->count;
        return true;
    }
    return false;
}

bool
cra_deque_pop_left(CraDeque *deque, void *retval)
{
    CraLListNode *curr;
    CraLList     *list;
    if (deque->count > 0)
    {
        list = &deque->list;
        curr = list->head;
        // copy value
        if (retval)
            memcpy(retval, curr->val + deque->left_idx * deque->ele_size, deque->ele_size);
        if (deque->zero_memory)
            bzero(curr->val + deque->left_idx * deque->ele_size, deque->ele_size);

        if (deque->left_idx == CRA_DEQUE_ELE_COUNT - 1)
        {
            if (list->count > 1)
            {
                cra_llist_remove_front(list);
                deque->left_idx = 0;
            }
            else
            {
                CRA_DEQUE_EMPTY_INDEX;
            }
        }
        else
        {
            ++deque->left_idx;
        }
        --deque->count;
        return true;
    }
    return false;
}

static inline bool
__cra_deque_set_and_pop_old(CraDeque *deque, size_t index, void *newval, void *retoldval)
{
    if (index >= deque->count)
        return false;

    CraLList *list = &deque->list;
    // 计算目标链表结点下标
    size_t    node_idx = (index + deque->left_idx) / CRA_DEQUE_ELE_COUNT;
    // 计算目标在结点中的下标
    size_t    real_idx = (index + deque->left_idx) % CRA_DEQUE_ELE_COUNT;

    CraLListNode *curr = list->head;
    // 找到目标结点
    for (size_t i = 0; i < node_idx; ++i)
        curr = curr->next;

    if (retoldval)
        memcpy(retoldval, curr->val + real_idx * deque->ele_size, deque->ele_size);
    memcpy(curr->val + real_idx * deque->ele_size, newval, deque->ele_size);
    return true;
}

bool
cra_deque_set(CraDeque *deque, size_t index, void *newval)
{
    return __cra_deque_set_and_pop_old(deque, index, newval, NULL);
}

bool
cra_deque_set_and_pop_old(CraDeque *deque, size_t index, void *newval, void *retoldval)
{
    return __cra_deque_set_and_pop_old(deque, index, newval, retoldval);
}

static inline bool
__cra_deque_get_ptr(CraDeque *deque, size_t index, void **retvalptr)
{
    if (index >= deque->count)
        return false;

    CraLList *list = &deque->list;
    // 计算目标链表结点下标
    size_t    node_idx = (index + deque->left_idx) / CRA_DEQUE_ELE_COUNT;
    // 计算目标在结点中的下标
    size_t    real_idx = (index + deque->left_idx) % CRA_DEQUE_ELE_COUNT;

    CraLListNode *curr = list->head;
    // 找到目标结点
    for (size_t i = 0; i < node_idx; ++i)
        curr = curr->next;

    *retvalptr = curr->val + real_idx * deque->ele_size;
    return true;
}

bool
cra_deque_get(CraDeque *deque, size_t index, void *retval)
{
    void *valptr;
    if (__cra_deque_get_ptr(deque, index, &valptr))
    {
        memcpy(retval, valptr, deque->ele_size);
        return true;
    }
    return false;
}

bool
cra_deque_get_ptr(CraDeque *deque, size_t index, void **retvalptr)
{
    return __cra_deque_get_ptr(deque, index, retvalptr);
}

static inline bool
__cra_deque_peek_ptr(CraDeque *deque, void **retvalptr)
{
    CraLListNode *curr;
    CraLList     *list;
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

bool
cra_deque_peek(CraDeque *deque, void *retval)
{
    void *valptr;
    if (__cra_deque_peek_ptr(deque, &valptr))
    {
        memcpy(retval, valptr, deque->ele_size);
        return true;
    }
    return false;
}

bool
cra_deque_peek_ptr(CraDeque *deque, void **retvalptr)
{
    return __cra_deque_peek_ptr(deque, retvalptr);
}

static inline bool
__cra_deque_peek_left_ptr(CraDeque *deque, void **retvalptr)
{
    CraLListNode *curr;
    CraLList     *list;
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

bool
cra_deque_peek_left(CraDeque *deque, void *retval)
{
    void *valptr;
    if (__cra_deque_peek_left_ptr(deque, &valptr))
    {
        memcpy(retval, valptr, deque->ele_size);
        return true;
    }
    return false;
}

bool
cra_deque_peek_left_ptr(CraDeque *deque, void **retvalptr)
{
    return __cra_deque_peek_left_ptr(deque, retvalptr);
}

void
cra_deque_reverse(CraDeque *deque)
{
    size_t        left, right;
    void         *leftval, *rightval;
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

CraDeque *
cra_deque_clone(CraDeque *deque, cra_deep_copy_val_fn deep_copy_val)
{
    CraDeque    *ret;
    CraDequeIter it;
    void        *valptr, *val;

    ret = cra_alloc(CraDeque);
    cra_deque_init(ret, deque->ele_size, deque->que_max, deque->zero_memory);

    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, &valptr);)
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
