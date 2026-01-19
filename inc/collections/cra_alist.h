/**
 * @file cra_alist.h
 * @author Cracal
 * @brief 动态数组
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_ALIST_H__
#define __CRA_ALIST_H__
#include "cra_collects.h"

typedef struct _CraAList
{
    bool           zero_memory;
    size_t         ele_size;
    size_t         count;
    size_t         capacity;
    unsigned char *array;
} CraAList;

typedef struct _CraAListIter
{
    size_t    index;
    CraAList *list;
} CraAListIter;

#define CRA_ALIST_INIT_CAPACITY 8

CRA_API void
cra_alist_iter_init(CraAList *list, CraAListIter *it);

CRA_API bool
cra_alist_iter_next(CraAListIter *it, void **retvalptr);

CRA_API void
cra_alist_init_size(CraAList *list, size_t element_size, size_t init_capacity, bool zero_memory);

CRA_API void
cra_alist_init(CraAList *list, size_t element_size, bool zero_memory);

#define cra_alist_init_size0(_TVal, _list, _init_capacity, _zero_memory)    \
    cra_alist_init_size(_list, sizeof(_TVal), _init_capacity, _zero_memory)
#define cra_alist_init0(_TVal, _list, _zero_memory) cra_alist_init(_list, sizeof(_TVal), _zero_memory)

CRA_API void
cra_alist_uninit(CraAList *list);

static inline size_t
cra_alist_get_count(CraAList *list)
{
    return list->count;
}

CRA_API void
cra_alist_clear(CraAList *list);

CRA_API bool
cra_alist_resize(CraAList *list, size_t new_capacity);

CRA_API bool
cra_alist_insert(CraAList *list, size_t index, void *val);
#define cra_alist_prepend(_list, _val) cra_alist_insert(_list, 0, _val)
#define cra_alist_append(_list, _val)  cra_alist_insert(_list, (_list)->count, _val)

CRA_API bool
cra_alist_remove_at(CraAList *list, size_t index);
#define cra_alist_remove_front(_list) cra_alist_remove_at(_list, 0)
#define cra_alist_remove_back(_list)  cra_alist_remove_at(_list, (_list)->count - 1)

CRA_API bool
cra_alist_pop_at(CraAList *list, size_t index, void *retval);
#define cra_alist_pop_front(_list, _retval) cra_alist_pop_at(_list, 0, _retval)
#define cra_alist_pop_back(_list, _retval)  cra_alist_pop_at(_list, (_list)->count - 1, _retval)

CRA_API size_t
cra_alist_remove_match(CraAList *list, cra_match_fn match, void *arg);

CRA_API bool
cra_alist_set(CraAList *list, size_t index, void *newval);

CRA_API bool
cra_alist_set_and_pop_old(CraAList *list, size_t index, void *newval, void *retoldval);

CRA_API bool
cra_alist_get(CraAList *list, size_t index, void *retval);

CRA_API bool
cra_alist_get_ptr(CraAList *list, size_t index, void **retvalptr);

CRA_API void
cra_alist_reverse(CraAList *list);

CRA_API CraAList *
cra_alist_clone(CraAList *list, cra_deep_copy_val_fn deep_copy_val);

CRA_API void
cra_alist_sort(CraAList *list, cra_compare_fn compare);

CRA_API bool
cra_alist_add_sort(CraAList *list, cra_compare_fn compare, void *val);

#endif