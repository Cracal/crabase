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
#include "cra_assert.h"
#include "cra_malloc.h"
#include "collections/cra_llist.h"

#define _CRA_LLIST_DEAFULT_FREE_COUNT 32

static inline CraLListNode *__cra_llist_create_node(size_t element_size)
{
    return cra_malloc(sizeof(CraLListNode) + element_size);
}

static inline void __cra_llist_destroy_node(CraLListNode *node)
{
    cra_free(node);
}

static inline CraLListNode *__cra_llist_get_free_node(CraLList *list)
{
    CraLListNode *node;
    if (list->freelist)
    {
        node = list->freelist;
        list->freelist = node->next;
        list->freelist_count--;
    }
    else
    {
        node = __cra_llist_create_node(list->ele_size);
        if (list->zero_memory)
            bzero(node->val, list->ele_size);
    }
    return node;
}

static inline void __cra_llist_put_free_node(CraLList *list, CraLListNode *node, bool move_to_free_list)
{
    if (move_to_free_list &&
        list->freelist_count < _CRA_LLIST_DEAFULT_FREE_COUNT)
    {
        if (list->zero_memory)
            bzero(node->val, list->ele_size);
        node->next = list->freelist;
        list->freelist = node;
        list->freelist_count++;
    }
    else
    {
        __cra_llist_destroy_node(node);
    }
}

static inline void __cra_llist_insert_node(CraLList *list, size_t index, CraLListNode *node)
{
    CraLListNode *curr;

    // 没有元素时
    if (list->head == NULL)
    {
        node->prev = node;
        node->next = node;
        list->head = node;
    }
    else
    { // 尾插
        if (index == list->count)
        {
            curr = list->head;
        }
        // 头插和中插
        else
        {
            curr = list->head;
            for (size_t i = 0; i < list->count - 1; i++)
            {
                if (i == index)
                    break;
                curr = curr->next;
            }
            if (index == 0)
                list->head = node;
        }
        node->next = curr;
        node->prev = curr->prev;
        curr->prev->next = node;
        curr->prev = node;
    }
    ++list->count;
}

static inline void __cra_llist_unlink_node(CraLList *list, CraLListNode *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    if (node == list->head)
        list->head = node->next == node ? NULL : node->next;
    --list->count;
}

static inline CraLListNode *__cra_llist_get_node(CraLList *list, size_t index)
{
    size_t i;
    CraLListNode *curr;
    if (index < (list->count >> 1))
    {
        curr = list->head;
        for (i = 0; i < index; i++)
            curr = curr->next;
    }
    else
    {
        curr = list->head->prev;
        for (i = list->count - 1; i > index; i--)
            curr = curr->prev;
    }
    return curr;
}

static inline void __cra_llist_clear(CraLList *list, bool move_to_free_list)
{
    CraLListNode *cur;
    while (list->head)
    {
        cur = list->head;
        __cra_llist_unlink_node(list, cur);
        if (list->remove_val)
            list->remove_val(cur->val);
        __cra_llist_put_free_node(list, cur, move_to_free_list);
    }
    list->count = 0;
}

#define _CRA_LLIST_REMOVE_NODE(_list, _node, _retval)     \
    if (_retval)                                          \
        memcpy(_retval, (_node)->val, (_list)->ele_size); \
    else if ((_list)->remove_val)                         \
        (_list)->remove_val((_node)->val);                \
    __cra_llist_unlink_node(_list, _node);                \
    __cra_llist_put_free_node(_list, (_node), true)

static inline bool __cra_llist_pop__(CraLList *list, size_t index, void *retval)
{
    CraLListNode *curr;
    if (index >= list->count)
        return false;
    curr = __cra_llist_get_node(list, index);
    _CRA_LLIST_REMOVE_NODE(list, curr, retval);
    return true;
}

CraLListIter cra_llist_iter_init(CraLList *list)
{
    CraLListIter it;
    it.curr = list->head;
    it.head = list->head;
    return it;
}

bool cra_llist_iter_next(CraLListIter *it, void **retvalptr)
{
    if (it->curr)
    {
        *retvalptr = it->curr->val;
        it->curr = it->curr->next;
        if (it->curr == it->head)
            it->curr = NULL;
        return true;
    }
    return false;
}

void cra_llist_init(CraLList *list, size_t element_size,
                    bool zero_memory, cra_remove_val_fn remove_val)
{
    assert(!!list && element_size > 0);
    list->zero_memory = zero_memory;
    list->ele_size = element_size;
    list->count = 0;
    list->freelist_count = 0;
    list->remove_val = remove_val;
    list->head = NULL;
    list->freelist = NULL;
}

void cra_llist_uninit(CraLList *list)
{
    CraLListNode *node, *temp;
    __cra_llist_clear(list, false);
    node = list->freelist;
    while (node)
    {
        temp = node;
        node = node->next;
        cra_free(temp);
    }
}

void cra_llist_clear(CraLList *list)
{
    __cra_llist_clear(list, true);
}

bool cra_llist_insert(CraLList *list, size_t index, void *val)
{
    CraLListNode *node;

    if (index > list->count)
        return false;

    node = __cra_llist_get_free_node(list);
    memcpy(node->val, val, list->ele_size);
    __cra_llist_insert_node(list, index, node);
    return true;
}

bool cra_llist_remove_at(CraLList *list, size_t index)
{
    return __cra_llist_pop__(list, index, NULL);
}

bool cra_llist_pop(CraLList *list, size_t index, void *retval)
{
    return __cra_llist_pop__(list, index, retval);
}

size_t cra_llist_remove_match(CraLList *list, cra_match_fn match, void *arg)
{
    void *_null = NULL;
    size_t remove_count = 0;
    CraLListNode *temp, *curr = list->head;
    for (size_t i = 0; i < list->count;)
    {
        if (match(curr->val, arg))
        {
            temp = curr;
            curr = curr->next;
            _CRA_LLIST_REMOVE_NODE(list, temp, _null);
            remove_count++;
            continue;
        }
        i++;
        curr = curr->next;
    }
    return remove_count;
}

bool cra_llist_set(CraLList *list, size_t index, void *newval)
{
    CraLListNode *node;
    if (index >= list->count)
        return false;
    node = __cra_llist_get_node(list, index);
    if (list->remove_val)
        list->remove_val(node->val);
    memcpy(node->val, newval, list->ele_size);
    return true;
}

bool cra_llist_set_and_pop_old(CraLList *list, size_t index, void *newval, void *retoldval)
{
    CraLListNode *node;
    if (index >= list->count)
        return false;
    node = __cra_llist_get_node(list, index);
    if (retoldval)
        memcpy(retoldval, node->val, list->ele_size);
    else if (list->remove_val)
        list->remove_val(node->val);
    memcpy(node->val, newval, list->ele_size);
    return true;
}

bool cra_llist_get(CraLList *list, size_t index, void *retval)
{
    CraLListNode *node;
    if (index >= list->count)
        return false;
    node = __cra_llist_get_node(list, index);
    memcpy(retval, node->val, list->ele_size);
    return true;
}

bool cra_llist_get_ptr(CraLList *list, size_t index, void **retvalptr)
{
    CraLListNode *node;
    if (index >= list->count)
        return false;
    node = __cra_llist_get_node(list, index);
    *retvalptr = node->val;
    return true;
}

void cra_llist_reverse(CraLList *list)
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

CraLList *cra_llist_clone(CraLList *list, cra_deep_copy_val_fn deep_copy_val)
{
    CraLList *ret;
    CraLListNode *curr, *node;

    ret = cra_alloc(CraLList);
    cra_llist_init(ret, list->ele_size, list->zero_memory, list->remove_val);

    if (!list->head)
        return ret;

    curr = list->head;
    do
    {
        node = __cra_llist_get_free_node(ret);
        // link node
        __cra_llist_insert_node(ret, ret->count, node);

        // copy val
        if (deep_copy_val)
            deep_copy_val(curr->val, node->val);
        else
            memcpy(node->val, curr->val, ret->ele_size);

        curr = curr->next;
    } while (curr != list->head);

    return ret;
}

static CraLListNode *cra_llist_partition(CraLList *list,
                                         cra_compare_fn compare,
                                         CraLListNode *begin, CraLListNode *end)
{
    CraLListNode *left, *right, *temp;
    left = begin;
    right = end;
    // temp.val = list[left].val
    temp = cra_llist_get_free_node(list);
    memcpy(temp->val, right->val, list->ele_size);

    while (left != right)
    {
        //                      list[left].val <= temp.val;
        while (left != right && compare(left->val, temp->val) <= 0)
            left = left->next;
        // list[right].val = list[left].val
        memcpy(right->val, left->val, list->ele_size);

        //                      list[right].val >= temp.val;
        while (left != right && compare(right->val, temp->val) >= 0)
            right = right->prev;
        // list[left].val = list[right].val
        memcpy(left->val, right->val, list->ele_size);
    }
    // list[left].val = temp.val
    memcpy(left->val, temp->val, list->ele_size);

    cra_llist_put_free_node(list, temp);
    return left;
}

static void cra_llist_quick_sort(CraLList *list,
                                 cra_compare_fn compare,
                                 CraLListNode *begin, CraLListNode *end)
{
    CraLListNode *middle = cra_llist_partition(list, compare, begin, end);
    if (begin != middle && begin != middle->prev)
        cra_llist_quick_sort(list, compare, begin, middle->prev);
    if (middle != end && middle->next != end)
        cra_llist_quick_sort(list, compare, middle->next, end);
}

void cra_llist_sort(CraLList *list, cra_compare_fn compare)
{
    assert(compare != NULL);
    if (list->count > 1) // count(list) >= 2
        cra_llist_quick_sort(list, compare, list->head, list->head->prev);
}

bool cra_llist_add_sort(CraLList *list, cra_compare_fn compare, void *val)
{
    size_t index;
    CraLListNode *curr;

    assert(compare != NULL);

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
    return cra_llist_insert(list, index, val);
}

CraLListNode *cra_llist_create_node(size_t element_size)
{
    return __cra_llist_create_node(element_size);
}

void cra_llist_destroy_node(CraLListNode **node)
{
    __cra_llist_destroy_node(*node);
    *node = NULL;
}

CraLListNode *cra_llist_get_free_node(CraLList *list)
{
    return __cra_llist_get_free_node(list);
}

void cra_llist_put_free_node(CraLList *list, CraLListNode *node)
{
    __cra_llist_put_free_node(list, node, true);
}

bool cra_llist_insert_node(CraLList *list, size_t index, CraLListNode *node)
{
    if (index > list->count)
        return false;
    __cra_llist_insert_node(list, index, node);
    return true;
}

void cra_llist_unlink_node(CraLList *list, CraLListNode *node)
{
    assert(node && node->prev && node->next);
    __cra_llist_unlink_node(list, node);
    node->prev = node->next = NULL;
}

CraLListNode *cra_llist_get_node(CraLList *list, size_t index)
{
    if (index >= list->count)
        return NULL;
    return __cra_llist_get_node(list, index);
}

// ===============

static void cra_llist_ser_iter_init(void *obj, void *const it, size_t itbufsize)
{
    CRA_UNUSED_VALUE(itbufsize);
    assert(sizeof(CraLListIter) <= itbufsize);
    CraLList *list = (CraLList *)obj;
    CraLListIter _it = cra_llist_iter_init(list);
    memcpy(it, &_it, sizeof(CraLListIter));
}

static bool cra_llist_ser_iter_append(void *obj, void *val)
{
    return cra_llist_append((CraLList *)obj, val);
}

static void cra_llist_ser_init(void *obj, void *args)
{
    assert_always(args != NULL);

    CraLList *list = (CraLList *)obj;
    CraLListSerInitArgs *params = (CraLListSerInitArgs *)args;

    cra_llist_init(list, params->element_size, params->zero_memory, params->remove_val_fn);
}

const CraTypeIter_i g_llist_ser_iter_i = {
    .list.init = cra_llist_ser_iter_init,
    .list.next = (bool (*)(void *, void **))cra_llist_iter_next,
    .list.append = cra_llist_ser_iter_append,
};

const CraTypeInit_i g_llist_ser_init_i = {
    .alloc = NULL,
    .dealloc = NULL,
    .init = cra_llist_ser_init,
    .uinit = (void (*)(void *))cra_llist_uninit,
};
