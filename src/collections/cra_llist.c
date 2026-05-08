/**
 * @file cra_llist.c
 * @author Cracal
 * @brief 双向循环链表
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "collections/cra_llist.h"
#include "cra_malloc.h"

#define CRA_LLIST_CREATE_NODE(_itemsize) (CraLListNode *)cra_malloc(sizeof(CraLListNode) + (_itemsize))
#define CRA_LLIST_DESTROY_NODE(_node)    cra_free(_node)

CraLListNode *
cra_llist_create_node(size_t itemsize)
{
    assert(itemsize > 0);
    return CRA_LLIST_CREATE_NODE(itemsize);
}

void
cra_llist_destroy_node(CraLListNode *node)
{
    assert(node);
    CRA_LLIST_DESTROY_NODE(node);
}

bool(cra_llist_init_with_size)(CraLList *list, size_t itemsize, size_t init_spare_node)
{
    assert(list);
    assert(itemsize > 0);

    bzero(list, sizeof(*list));
    list->itemsize = itemsize;

    for (size_t i = 0; i < init_spare_node; ++i)
    {
        CraLListNode *node = CRA_LLIST_CREATE_NODE(itemsize);
        if (!node)
        {
            cra_llist_uninit(list);
            return false;
        }
        cra_llist_put_free_node(list, node);
    }
    return true;
}

void
cra_llist_uninit(CraLList *list)
{
    CraLListNode *curr, *temp;

    assert(list);
    assert(list->itemsize > 0);

    // destroy list nodes
    curr = list->head;
    if (curr)
    {
        assert(list->count > 0);
        assert(list->head->prev);
        assert(list->head->next);
        assert(list->head->prev->next == list->head);

        curr->prev->next = NULL;
        while (curr)
        {
            temp = curr;
            curr = temp->next;
            CRA_LLIST_DESTROY_NODE(temp);
        }
    }

    // destroy free list nodes
    curr = list->free_list;
    while (curr)
    {
        temp = curr;
        curr = temp->next;
        CRA_LLIST_DESTROY_NODE(temp);
    }

    bzero(list, sizeof(*list));
}

void
cra_llist_clear(CraLList *list)
{
    assert(list);
    assert(list->itemsize > 0);

    if (list->head)
    {
        assert(list->count > 0);
        assert(list->head->prev);
        assert(list->head->next);
        assert(list->head->prev->next == list->head);

        list->head->prev->next = list->free_list;
        list->nfreelist += list->count;
        list->free_list = list->head;
        list->head = NULL;
    }
    list->count = 0;
}

bool
cra_llist_ensure(CraLList *list, size_t nspare, bool shrink2fit)
{
    size_t        needed;
    CraLListNode *curr, *temp;

    assert(list);
    assert(list->itemsize > 0);

    if (list->nfreelist < nspare)
    {
        needed = nspare - list->nfreelist;
        while (needed--)
        {
            CraLListNode *node = CRA_LLIST_CREATE_NODE(list->itemsize);
            if (!node)
            {
                return false;
            }
            cra_llist_put_free_node(list, node);
        }
    }
    else if (shrink2fit)
    {
        curr = list->free_list;
        needed = list->nfreelist - nspare;
        while (needed--)
        {
            temp = curr;
            curr = temp->next;
            CRA_LLIST_DESTROY_NODE(temp);
        }
        list->free_list = curr;
    }
    return true;
}

bool(cra_llist_insert)(CraLList *list, size_t index, void *val)
{
    CraLListNode *node;

    node = cra_llist_get_free_node(list);
    if (!node)
        return false;

    if (cra_llist_insert_node(list, index, node))
    {
        memcpy(node->val, val, list->itemsize);
        return true;
    }
    cra_llist_put_free_node(list, node);
    return false;
}

bool(cra_llist_pop_at)(CraLList *list, size_t index, void *retval)
{
    CraLListNode *node;

    node = cra_llist_get_node(list, index);
    if (!node)
        return false;

    if (retval)
        memcpy(retval, node->val, list->itemsize);

    cra_llist_remove_node(list, node, true);
    return true;
}

void
cra_llist_reverse(CraLList *list)
{
    CraLListNode *curr, *temp;

    if (list->count < 2)
        return;

    curr = list->head;

    do
    {
        temp = curr->prev;
        curr->prev = curr->next;
        curr->next = temp;

        curr = curr->prev;
    } while (curr != list->head);

    list->head = curr->next;
}

static CraLListNode *
cra_llist_partition(CraLList *list, cra_compare_fn compare, CraLListNode *begin, CraLListNode *end)
{
    CraLListNode *left, *right, *temp;
    left = begin;
    right = end;
    // temp.val = list[left].val
    temp = cra_llist_get_free_node(list);
    if (!temp)
        return NULL;
    memcpy(temp->val, right->val, list->itemsize);

    while (left != right)
    {
        //                      list[left].val <= temp.val;
        while (left != right && compare(left->val, temp->val) <= 0)
            left = left->next;
        // list[right].val = list[left].val
        memcpy(right->val, left->val, list->itemsize);

        //                      list[right].val >= temp.val;
        while (left != right && compare(right->val, temp->val) >= 0)
            right = right->prev;
        // list[left].val = list[right].val
        memcpy(left->val, right->val, list->itemsize);
    }
    // list[left].val = temp.val
    memcpy(left->val, temp->val, list->itemsize);

    cra_llist_put_free_node(list, temp);
    return left;
}

static bool
cra_llist_quick_sort(CraLList *list, cra_compare_fn compare, CraLListNode *begin, CraLListNode *end)
{
    CraLListNode *middle = cra_llist_partition(list, compare, begin, end);
    if (!middle)
        return false;
    if (begin != middle && begin != middle->prev)
        cra_llist_quick_sort(list, compare, begin, middle->prev);
    if (middle != end && middle->next != end)
        cra_llist_quick_sort(list, compare, middle->next, end);
    return true;
}

bool(cra_llist_sort)(CraLList *list, cra_compare_fn compare)
{
    assert(list);
    assert(compare);
    assert(list->itemsize > 0);

    if (list->count > 1) // count(list) >= 2
        return cra_llist_quick_sort(list, compare, list->head, list->head->prev);
    return true;
}

bool(cra_llist_add_sort)(CraLList *list, cra_compare_fn compare, void *val)
{
    size_t        index;
    CraLListNode *curr;

    assert(val);
    assert(list);
    assert(compare);
    assert(list->itemsize > 0);

    index = 0;
    if (list->head)
    {
        curr = list->head;
        do
        {
            if (compare(curr->val, val) > 0)
                break;
            index++;
            curr = curr->next;
        } while (curr != list->head);
    }
    return (cra_llist_insert)(list, index, val);
}
// ====================================== interfaces ======================================

// initializable

static CRA_INITIALIZABLE_INIT_FN(cra_llist_init_init)
{
    CraLList                   *list = (CraLList *)obj;
    CraLListInitializableParam *param = (CraLListInitializableParam *)params;

    assert(list);
    assert(param);
    assert(param->itemsize > 0);

    return (cra_llist_init_with_size)(list, param->itemsize, param->init_spare_node);
}

CRA_INITIALIZABLE_DEF(cra_g_llist_initializable_i) = {
    .init = cra_llist_init_init,
    .uninit = (CRA_INITIALIZABLE_UNINIT_FN((*)))cra_llist_uninit,
};

// appendable

static CRA_APPENDABLE_APPEND_FN(cra_llist_append_append)
{
    CraLList *list;

    assert(obj);
    assert(vals);
    assert(vals->val1_ref);

    list = (CraLList *)obj;
    return (cra_llist_insert)(list, list->count, vals->val1_ref);
}

CRA_APPENDABLE_DEF(cra_g_llist_appendable_i) = {
    .append = cra_llist_append_append,
};

// iterable

static CRA_ITERABLE_INIT_FN(cra_llist_iter_init)
{
    CraLList *list = (CraLList *)obj;

    assert(it);
    assert(list);

    if (!list->head)
        return false;

    assert(list->head);
    assert(list->head->next);
    assert(list->head->prev);

    it->obj = list;
    it->cur = reverse ? list->head->prev : list->head;
    return true;
}

static CRA_ITERABLE_NEXT_FN(cra_llist_iter_next)
{
    CraLList     *list;
    CraLListNode *curr, *next;

    assert(it);
    assert(vals);
    assert(it->obj);

    list = (CraLList *)it->obj;
    curr = (CraLListNode *)it->cur;

    if (!curr)
        return false;

    assert(curr->next);
    assert(curr->prev);

    next = curr->next;
    vals->val1_ref = curr->val;
    it->cur = next != list->head ? next : NULL;
    return true;
}

static CRA_ITERABLE_PREV_FN(cra_llist_iter_prev)
{
    CraLList     *list;
    CraLListNode *curr, *prev;

    assert(it);
    assert(vals);
    assert(it->obj);

    list = (CraLList *)it->obj;
    curr = (CraLListNode *)it->cur;

    if (!curr)
        return false;

    assert(curr->next);
    assert(curr->prev);

    prev = curr->prev;
    vals->val1_ref = curr->val;
    it->cur = prev != list->head->prev ? prev : NULL;
    return true;
}

CRA_ITERABLE_DEF(cra_g_llist_iterable_i) = {
    .init = cra_llist_iter_init,
    .next = cra_llist_iter_next,
    .prev = cra_llist_iter_prev,
};
