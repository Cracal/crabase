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
#include "serialize/cra_serialize.h"

typedef struct _CraLListNode
{
    struct _CraLListNode *prev;
    struct _CraLListNode *next;
    char val[];
} CraLListNode;

typedef struct _CraLList
{
    bool zero_memory;
    size_t ele_size;
    size_t count;
    size_t freelist_count;
    CraLListNode *head;
    CraLListNode *freelist;
    cra_remove_val_fn remove_val;
} CraLList;

typedef struct _CraLListIter
{
    CraLListNode *curr;
    CraLListNode *head;
} CraLListIter;

CRA_API CraLListIter cra_llist_iter_init(CraLList *list);
CRA_API bool cra_llist_iter_next(CraLListIter *it, void **retvalptr);

CRA_API void cra_llist_init(CraLList *list, size_t element_size,
                            bool zero_memory, cra_remove_val_fn remove_val);
#define cra_llist_init0(_TVal, _list, _zero_memory, _remove_val_fn) \
    cra_llist_init(_list, sizeof(_TVal), _zero_memory, _remove_val_fn)
CRA_API void cra_llist_uninit(CraLList *list);

static inline size_t cra_llist_get_count(CraLList *list) { return list->count; }

CRA_API void cra_llist_clear(CraLList *list);

CRA_API bool cra_llist_insert(CraLList *list, size_t index, void *val);
#define cra_llist_prepend(_list, _val) cra_llist_insert(_list, 0, _val)
#define cra_llist_append(_list, _val) cra_llist_insert(_list, (_list)->count, _val)

CRA_API bool cra_llist_remove_at(CraLList *list, size_t index);
#define cra_llist_remove_front(_list) cra_llist_remove_at(_list, 0)
#define cra_llist_remove_back(_list) cra_llist_remove_at(_list, (_list)->count - 1)

CRA_API bool cra_llist_pop(CraLList *list, size_t index, void *retval);
#define cra_llist_pop_front(_list, _retval) cra_llist_pop(_list, 0, _retval)
#define cra_llist_pop_back(_list, _retval) cra_llist_pop(_list, (_list)->count - 1, _retval)

CRA_API size_t cra_llist_remove_match(CraLList *list, cra_match_fn match, void *arg);

CRA_API bool cra_llist_set(CraLList *list, size_t index, void *newval);
CRA_API bool cra_llist_set_and_pop_old(CraLList *list, size_t index, void *newval, void *retoldval);

CRA_API bool cra_llist_get(CraLList *list, size_t index, void *retval);
CRA_API bool cra_llist_get_ptr(CraLList *list, size_t index, void **retvalptr);

CRA_API void cra_llist_reverse(CraLList *list);

CRA_API CraLList *cra_llist_clone(CraLList *list, cra_deep_copy_val_fn deep_copy_val);

CRA_API void cra_llist_sort(CraLList *list, cra_compare_fn compare);
CRA_API bool cra_llist_add_sort(CraLList *list, cra_compare_fn compare, void *val);

// linked node

CRA_API CraLListNode *cra_llist_create_node(size_t element_size);
CRA_API void cra_llist_destroy_node(CraLListNode **node);
CRA_API CraLListNode *cra_llist_get_free_node(CraLList *list);
CRA_API void cra_llist_put_free_node(CraLList *list, CraLListNode *node);
CRA_API bool cra_llist_insert_node(CraLList *list, size_t index, CraLListNode *node);
CRA_API void cra_llist_unlink_node(CraLList *list, CraLListNode *node);
CRA_API CraLListNode *cra_llist_get_node(CraLList *list, size_t index);

// =================

typedef struct _CraLListSerInitArgs
{
    bool zero_memory;
    size_t element_size;
    cra_remove_val_fn remove_val_fn;
} CraLListSerInitArgs;

CRA_API const CraTypeIter_i g_llist_ser_iter_i;
CRA_API const CraTypeInit_i g_llist_ser_init_i;

#define CRA_LLIST_SER_ARGS(_name, _zero_memory, _element_size, _remove_val_fn) \
    CraLListSerInitArgs _name = {_zero_memory, _element_size, _remove_val_fn}
#define CRA_LLIST_SER_ARGS0(_name, _initialied_list) \
    CRA_LLIST_SER_ARGS(_name, (_initialied_list)->zero_memory, (_initialied_list)->ele_size, (_initialied_list)->remove_val)

#endif