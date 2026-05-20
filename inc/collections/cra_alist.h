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
#include "cra_assert.h"
#include "cra_collects.h"

#define CRA_ALIST_DEFAULT_CAPACITY 8

#define CRA_ALIST_CHECK_VAL(_list, _val) assert(sizeof(*(_val)) == (_list)->itemsize)
#define CRA_ALIST_PVAL(_list, _index)    ((_list)->array + (_index) * (_list)->itemsize)

typedef struct CraAList CraAList;
struct CraAList
{
    unsigned char *array;
    size_t         count;
    size_t         capacity;
    size_t         itemsize;
};

CRA_API bool
cra_alist_init_with_size(CraAList *list, size_t itemsize, size_t init_capacity);
// bool init_with_size<T>(CraAList *list, size_t init_capacity)
#define cra_alist_init_with_size(_T, _list, _init_capacity) cra_alist_init_with_size(_list, sizeof(_T), _init_capacity)
// bool init<T>(CraAList *list)
#define cra_alist_init(_T, _list) cra_alist_init_with_size(_T, _list, CRA_ALIST_DEFAULT_CAPACITY)

CRA_API void
cra_alist_uninit(CraAList *list);

static inline void
cra_alist_clear(CraAList *list)
{
    list->count = 0;
}

CRA_API bool
cra_alist_ensure(CraAList *list, size_t nspare, bool shrink2fit);

CRA_API bool
cra_alist_insert(CraAList *list, size_t index, void *val);
// bool insert(CraAList *list, size_t index, T *val)
#define cra_alist_insert(_list, _index, _val) (CRA_ALIST_CHECK_VAL(_list, _val), cra_alist_insert(_list, _index, _val))
// bool prepend(CraAList *list, T *val)
#define cra_alist_prepend(_list, _val)        cra_alist_insert(_list, 0, _val)
// bool append(CraAList *list, T *val)
#define cra_alist_append(_list, _val)         cra_alist_insert(_list, (_list)->count, _val)

CRA_API bool
cra_alist_pop_at(CraAList *list, size_t index, void *retval);
// bool pop_at(CraAList *list, size_t index, out T *retval)
#define cra_alist_pop_at(_list, _index, _retval)                                    \
    (CRA_ALIST_CHECK_VAL(_list, _retval), cra_alist_pop_at(_list, _index, _retval))
// bool pop_front(CraAList *list, out T *retval)
#define cra_alist_pop_front(_list, _retval) cra_alist_pop_at(_list, 0, _retval)
// bool pop_back(CraAList *list, out T *retval)
#define cra_alist_pop_back(_list, _retval)  cra_alist_pop_at(_list, (_list)->count - 1, _retval)

// bool remove_at(CraAList *list, size_t index)
#define cra_alist_remove_at(_list, _index) (cra_alist_pop_at)(_list, _index, NULL)
// bool remove_front(CraAList *list)
#define cra_alist_remove_front(_list)      cra_alist_remove_at(_list, 0)
// bool remove_back(CraAList *list)
#define cra_alist_remove_back(_list)       cra_alist_remove_at(_list, (_list)->count - 1)

static inline void *
cra_alist_get_ref(CraAList *list, size_t index)
{
    assert(list);
    assert(list->array);
    assert(list->itemsize > 0);

    if (index >= list->count)
        return NULL;
    return CRA_ALIST_PVAL(list, index);
}

static inline bool
cra_alist_get(CraAList *list, size_t index, void *retval)
{
    void *val = cra_alist_get_ref(list, index);
    if (val && retval)
        memcpy(retval, val, list->itemsize);
    return val != NULL;
}
// bool get(CraAList *list, size_t index, out T *retval)
#define cra_alist_get(_list, _index, _retval)                                    \
    (CRA_ALIST_CHECK_VAL(_list, _retval), cra_alist_get(_list, _index, _retval))

static inline bool
cra_alist_get_and_set(CraAList *list, size_t index, void *newval, void *retoldval)
{
    assert(newval);

    void *pval = cra_alist_get_ref(list, index);
    if (pval)
    {
        if (retoldval)
            memcpy(retoldval, pval, list->itemsize);
        memcpy(pval, newval, list->itemsize);
    }
    return pval != NULL;
}
// bool get_and_set(CraAList *list, size_t index, T *newval, out T *retoldval)
#define cra_alist_get_and_set(_list, _index, _newval, _retoldval) \
    (CRA_ALIST_CHECK_VAL(_list, _newval),                         \
     CRA_ALIST_CHECK_VAL(_list, _retoldval),                      \
     cra_alist_get_and_set(_list, _index, _newval, _retoldval))

// bool set(CraAList *list, size_t index, T *val)
#define cra_alist_set(_list, _index, _val)                                                 \
    (CRA_ALIST_CHECK_VAL(_list, _val), (cra_alist_get_and_set)(_list, _index, _val, NULL))

CRA_API bool
cra_alist_reverse(CraAList *list);

CRA_API bool
cra_alist_sort(struct CraAList *list, cra_cmp_fn compare);
// bool sort(CraAList *list, int (*compare)(const T *, const T *))
#define cra_alist_sort(_list, _compare) cra_alist_sort(_list, (cra_cmp_fn)(_compare))

CRA_API bool
cra_alist_add_sort(struct CraAList *list, cra_cmp_fn compare, void *val);
// bool add_sort(CraAList *list, int (*compare)(const T *, const T *), T *val)
#define cra_alist_add_sort(_list, _compare, _val)                                               \
    (CRA_ALIST_CHECK_VAL(_list, _val), cra_alist_add_sort(_list, (cra_cmp_fn)(_compare), _val))

// ====================================== interfaces ======================================

// initializable

typedef struct CraAListInitializableParam
{
    size_t itemsize;
    size_t init_capacity;
} CraAListInitializableParam;
#define CRA_ALIST_INITIALIZABLE_PARAM_INIT(_T, _init_capacity) { sizeof(_T), _init_capacity }
#define CRA_ALIST_INITIALIZABLE_PARAM_DECL(_var_name)          CraAListInitializableParam _var_name
#define CRA_ALIST_INITIALIZABLE_PARAM_DEF(_var_name, _T, _init_capacity)                                   \
    CRA_ALIST_INITIALIZABLE_PARAM_DECL(_var_name) = CRA_ALIST_INITIALIZABLE_PARAM_INIT(_T, _init_capacity)

CRA_API CRA_INITIALIZABLE_DEF(cra_g_alist_initializable_i);
#define CRA_ALIST_INITIALIZABLE_I (&cra_g_alist_initializable_i)

// appendable

CRA_API CRA_APPENDABLE_DEF(cra_g_alist_appendable_i);
#define CRA_ALIST_APPENDABLE_I (&cra_g_alist_appendable_i)

// iterable

CRA_API CRA_ITERABLE_DEF(cra_g_alist_iterable_i);
#define CRA_ALIST_ITERABLE_I (&cra_g_alist_iterable_i)

#endif