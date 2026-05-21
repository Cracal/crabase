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

#define CRA_LLIST_CHECK_VAL(_list, _val) assert(sizeof(*(_val)) == (_list)->itemsize)

typedef struct CraLListNode CraLListNode;
typedef struct CraLList     CraLList;

struct CraLListNode
{
    CraLListNode *prev;
    CraLListNode *next;
    unsigned char val[];
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
#define cra_llist_init_with_size(_T, _list, _init_spare_node)     \
    cra_llist_init_with_size(_list, sizeof(_T), _init_spare_node)
// bool init<T>(CraLList *list)
#define cra_llist_init(_T, _list) cra_llist_init_with_size(_T, _list, 0)

CRA_API void
cra_llist_uninit(CraLList *list);

CRA_API void
cra_llist_clear(CraLList *list);

CRA_API bool
cra_llist_ensure(CraLList *list, size_t nspare, bool shrink2fit);

CRA_API bool
cra_llist_insert(CraLList *list, size_t index, void *val);
// bool insert(CraLList *list, size_t index, T *val)
#define cra_llist_insert(_list, _index, _val) (CRA_LLIST_CHECK_VAL(_list, _val), cra_llist_insert(_list, _index, _val))
// bool prepend(CraLList *list, T *val)
#define cra_llist_prepend(_list, _val)        cra_llist_insert(_list, 0, _val)
// bool append(CraLList *list, T *val)
#define cra_llist_append(_list, _val)         cra_llist_insert(_list, (_list)->count, _val)

CRA_API bool
cra_llist_pop_at(CraLList *list, size_t index, void *retval);
// bool pop_at(CraLList *list, size_t index, out T *retval)
#define cra_llist_pop_at(_list, _index, _retval)                                    \
    (CRA_LLIST_CHECK_VAL(_list, _retval), cra_llist_pop_at(_list, _index, _retval))
// bool pop_front(CraLList *list, out T *retval)
#define cra_llist_pop_front(_list, _retval) cra_llist_pop_at(_list, 0, _retval)
// bool pop_back(CraLList *list, out T *retval)
#define cra_llist_pop_back(_list, _retval)  cra_llist_pop_at(_list, (_list)->count - 1, _retval)

// bool remove_at(CraLList *list, size_t index)
#define cra_llist_remove_at(_list, _index) (cra_llist_pop_at)(_list, _index, NULL)
// bool remove_front(CraLList *list)
#define cra_llist_remove_front(_list)      cra_llist_remove_at(_list, 0)
// bool remove_back(CraLList *list)
#define cra_llist_remove_back(_list)       cra_llist_remove_at(_list, (_list)->count - 1)

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
#define cra_llist_get(_list, _index, _retval)                                    \
    (CRA_LLIST_CHECK_VAL(_list, _retval), cra_llist_get(_list, _index, _retval))

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
#define cra_llist_get_and_set(_list, _index, _newval, _retoldval) \
    (CRA_LLIST_CHECK_VAL(_list, _newval),                         \
     CRA_LLIST_CHECK_VAL(_list, _retoldval),                      \
     cra_llist_get_and_set(_list, _index, _newval, _retoldval))

// bool set(CraLList *list, size_t index, T *val)
#define cra_llist_set(_list, _index, _val)                                                 \
    (CRA_LLIST_CHECK_VAL(_list, _val), (cra_llist_get_and_set)(_list, _index, _val, NULL))

CRA_API void
cra_llist_reverse(CraLList *list);

CRA_API bool
cra_llist_sort(CraLList *list, cra_cmp_fn compare);
// bool sort(CraLList *list, int (*compare)(const T *, const T *))
#define cra_llist_sort(_list, _compare) cra_llist_sort(_list, (cra_cmp_fn)(_compare))

CRA_API bool
cra_llist_add_sort(CraLList *list, cra_cmp_fn compare, void *val);
// bool add_sort(CraLList *list, int (*compare)(const T *, const T *), T *val)
#define cra_llist_add_sort(_list, _compare, _val)                                               \
    (CRA_LLIST_CHECK_VAL(_list, _val), cra_llist_add_sort(_list, (cra_cmp_fn)(_compare), _val))

// ====================================== interfaces ======================================

// initializable

typedef struct CraLListInitializableParam
{
    size_t itemsize;
} CraLListInitializableParam;
#define CRA_LLIST_INITIALIZABLE_PARAM_INIT(_T)        { sizeof(_T) }
#define CRA_LLIST_INITIALIZABLE_PARAM_DECL(_var_name) CraLListInitializableParam _var_name
#define CRA_LLIST_INITIALIZABLE_PARAM_DEF(_var_name, _T)                                   \
    CRA_LLIST_INITIALIZABLE_PARAM_DECL(_var_name) = CRA_LLIST_INITIALIZABLE_PARAM_INIT(_T)

CRA_API CRA_INITIALIZABLE_DEF(cra_g_llist_initializable_i);
#define CRA_LLIST_INITIALIZABLE_I (&cra_g_llist_initializable_i)

// appendable

CRA_API CRA_APPENDABLE_DEF(cra_g_llist_appendable_i);
#define CRA_LLIST_APPENDABLE_I (&cra_g_llist_appendable_i)

// iterable

CRA_API CRA_ITERABLE_DEF(cra_g_llist_iterable_i);
#define CRA_LLIST_ITERABLE_I (&cra_g_llist_iterable_i)

#endif