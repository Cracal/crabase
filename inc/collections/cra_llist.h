/**
 * @file cra_llist.h
 * @author Cracal
 * @brief 双向循环链表
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_LLIST_H__
#define __CRA_LLIST_H__
#include "cra_collects.h"

#define CRA_LLIST_CHECK_VAL(list, val) assert(sizeof(*(val)) == (list)->itemsize)

typedef struct CraLListNode CraLListNode;
typedef struct CraLListIter CraLListIter;
typedef struct CraLList     CraLList;

struct CraLListNode
{
    CraLListNode *prev;
    CraLListNode *next;
    unsigned char val[];
};

struct CraLListIter
{
    bool          initialized;
    CraLListNode *current;
};

struct CraLList
{
    CraLListNode *head;
    size_t        count;
    CraLListNode *free_list;
    size_t        nfreelist;
    size_t        itemsize;
};

#if 1 // node operation

CRA_API CraLListNode *
cra_llist_create_node(size_t itemsize);

CRA_API void
cra_llist_destroy_node(CraLListNode *node);

static inline void
cra_llist_link_node(CraLListNode *node, CraLListNode *prev)
{
    assert(node);
    assert(prev);
    assert(prev->prev);
    assert(prev->next);

    node->prev = prev;
    node->next = prev->next;
    prev->next->prev = node;
    prev->next = node;
}

static inline void
cra_llist_unlink_node(CraLListNode *node)
{
    assert(node);
    assert(node->prev);
    assert(node->next);

    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = node->next = NULL;
}

static inline CraLListNode *
cra_llist_get_free_node(CraLList *list)
{
    CraLListNode *node;

    assert(list);

    if (list->free_list)
    {
        node = list->free_list;
        list->free_list = node->next;
        node->prev = NULL;
        node->next = NULL;
        --list->nfreelist;
        return node;
    }
    else
    {
        return cra_llist_create_node(list->itemsize);
    }
}

static inline void
cra_llist_put_free_node(CraLList *list, CraLListNode *node)
{
    assert(list);
    assert(node);

    node->next = list->free_list;
    list->free_list = node;
    ++list->nfreelist;
}

static inline CraLListNode *
cra_llist_get_node(CraLList *list, size_t index)
{
    assert(list);

    if (index >= list->count)
        return NULL;

    CraLListNode *node;
    if (index <= list->count >> 1)
    {
        node = list->head;
        for (size_t i = 0; i < index; i++)
            node = node->next;
    }
    else
    {
        node = list->head->prev;
        for (size_t i = list->count - 1; i > index; i--)
            node = node->prev;
    }
    return node;
}

static inline bool
cra_llist_insert_node(CraLList *list, size_t index, CraLListNode *node)
{
    assert(list);
    assert(node);

    if (index > list->count)
        return false;

    if (!list->head)
    {
        assert(list->count == 0);

        list->head = node;
        node->prev = node->next = node;
    }
    else
    {
        assert(list->head);
        assert(list->head->next);
        assert(list->head->prev);
        assert(list->head->prev->next == list->head);
        assert(list->count > 0);

        CraLListNode *slot = cra_llist_get_node(list, index % list->count);
        assert(slot && slot->prev && slot->next);
        cra_llist_link_node(node, slot->prev);
        if (index == 0)
            list->head = node;
    }
    ++list->count;
    return true;
}

static inline void
cra_llist_remove_node(CraLList *list, CraLListNode *node, bool put_to_free_list)
{
    assert(list);
    assert(node);

    if (!list->head)
        return;

    if (list->head == node)
        list->head = node->next;

    cra_llist_unlink_node(node);
    if (put_to_free_list)
        cra_llist_put_free_node(list, node);

    if (--list->count == 0)
        list->head = NULL;
}

#endif // end node operation

CRA_API bool
cra_llist_init_with_size(CraLList *list, size_t itemsize, size_t init_spare_node);
// bool init_with_size<T>(CraLList *list, size_t init_spare_node)
#define cra_llist_init_with_size(T, list, init_spare_node) cra_llist_init_with_size(list, sizeof(T), init_spare_node)
// bool init<T>(CraLList *list)
#define cra_llist_init(T, list)                            cra_llist_init_with_size(T, list, 0)

CRA_API void
cra_llist_uninit(CraLList *list);

CRA_API void
cra_llist_clear(CraLList *list);

CRA_API bool
cra_llist_reserve(CraLList *list, size_t nspare);

CRA_API bool
cra_llist_insert(CraLList *list, size_t index, void *val);
// bool insert(CraLList *list, size_t index, T *val)
#define cra_llist_insert(list, index, val) (CRA_LLIST_CHECK_VAL(list, val), cra_llist_insert(list, index, val))
// bool prepend(CraLList *list, T *val)
#define cra_llist_prepend(list, val)       cra_llist_insert(list, 0, val)
// bool append(CraLList *list, T *val)
#define cra_llist_append(list, val)        cra_llist_insert(list, (list)->count, val)

CRA_API bool
cra_llist_pop_at(CraLList *list, size_t index, void *retval);
// bool pop_at(CraLList *list, size_t index, out T *retval)
#define cra_llist_pop_at(list, index, retval) (CRA_LLIST_CHECK_VAL(list, retval), cra_llist_pop_at(list, index, retval))
// bool pop_front(CraLList *list, out T *retval)
#define cra_llist_pop_front(list, retval)     cra_llist_pop_at(list, 0, retval)
// bool pop_back(CraLList *list, out T *retval)
#define cra_llist_pop_back(list, retval)      cra_llist_pop_at(list, (list)->count - 1, retval)

// bool remove_at(CraLList *list, size_t index)
#define cra_llist_remove_at(list, index) (cra_llist_pop_at)(list, index, NULL)
// bool remove_front(CraLList *list)
#define cra_llist_remove_front(list)     cra_llist_remove_at(list, 0)
// bool remove_back(CraLList *list)
#define cra_llist_remove_back(list)      cra_llist_remove_at(list, (list)->count - 1)

static inline void *
cra_llist_get_ref(CraLList *list, size_t index)
{
    CraLListNode *node;

    assert(list);
    assert(list->itemsize > 0);

    node = cra_llist_get_node(list, index);
    if (!node)
        return NULL;
    return node->val;
}

static inline bool
cra_llist_get(CraLList *list, size_t index, void *retval)
{
    void *pval = cra_llist_get_ref(list, index);
    if (pval && retval)
        memcpy(retval, pval, list->itemsize);
    return pval != NULL;
}
// bool get(CraLList *list, size_t index, out T *retval)
#define cra_llist_get(list, index, retval) (CRA_LLIST_CHECK_VAL(list, retval), cra_llist_get(list, index, retval))

static inline bool
cra_llist_get_and_set(CraLList *list, size_t index, void *newval, void *retoldval)
{
    assert(newval);

    void *pval = cra_llist_get_ref(list, index);
    if (pval)
    {
        if (retoldval)
            memcpy(retoldval, pval, list->itemsize);
        memcpy(pval, newval, list->itemsize);
    }
    return pval != NULL;
}
// bool get_and_set(CraLList *list, size_t index, T *newval, out T *retoldval)
#define cra_llist_get_and_set(list, index, newval, retoldval)                 \
    (CRA_LLIST_CHECK_VAL(list, newval), CRA_LLIST_CHECK_VAL(list, retoldval), \
     cra_llist_get_and_set(list, index, newval, retoldval))

// bool set(CraLList *list, size_t index, T *val)
#define cra_llist_set(list, index, val)                                               \
    (CRA_LLIST_CHECK_VAL(list, val), (cra_llist_get_and_set)(list, index, val, NULL))

CRA_API void
cra_llist_reverse(CraLList *list);

CRA_API bool
cra_llist_sort(CraLList *list, cra_cmp_fn compare);
// bool sort(CraLList *list, int (*compare)(const T *, const T *))
#define cra_llist_sort(list, compare) cra_llist_sort(list, (cra_cmp_fn)(compare))

CRA_API bool
cra_llist_add_sort(CraLList *list, cra_cmp_fn compare, void *val);
// bool add_sort(CraLList *list, int (*compare)(const T *, const T *), T *val)
#define cra_llist_add_sort(list, compare, val)                                             \
    (CRA_LLIST_CHECK_VAL(list, val), cra_llist_add_sort(list, (cra_cmp_fn)(compare), val))

CRA_API CRA_FOREACH_NEXT_DEF(CraLList);
CRA_API CRA_FOREACH_PREV_DEF(CraLList);

#endif