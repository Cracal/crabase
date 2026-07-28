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
#include "cra_ifs.h"

#define CRA_ALIST_DEFAULT_CAPACITY 8

#define CRA_ALIST_CHECK_VAL(list, val) assert(sizeof(*(val)) == (list)->itemsize)
#define CRA_ALIST_PVAL(list, index)    ((list)->array + (index) * (list)->itemsize)

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
#define cra_alist_init_with_size(T, list, init_capacity) cra_alist_init_with_size(list, sizeof(T), init_capacity)
// bool init<T>(CraAList *list)
#define cra_alist_init(T, list)                          cra_alist_init_with_size(T, list, CRA_ALIST_DEFAULT_CAPACITY)

CRA_API void
cra_alist_uninit(CraAList *list);

static inline void
cra_alist_clear(CraAList *list)
{
    list->count = 0;
}

CRA_API bool
cra_alist_reserve(CraAList *list, size_t new_capacity);

CRA_API bool
cra_alist_insert(CraAList *list, size_t index, void *val);
// bool insert(CraAList *list, size_t index, T *val)
#define cra_alist_insert(list, index, val) (CRA_ALIST_CHECK_VAL(list, val), cra_alist_insert(list, index, val))
// bool prepend(CraAList *list, T *val)
#define cra_alist_prepend(list, val)       cra_alist_insert(list, 0, val)
// bool append(CraAList *list, T *val)
#define cra_alist_append(list, val)        cra_alist_insert(list, (list)->count, val)

CRA_API bool
cra_alist_pop_at(CraAList *list, size_t index, void *retval);
// bool pop_at(CraAList *list, size_t index, out T *retval)
#define cra_alist_pop_at(list, index, retval) (CRA_ALIST_CHECK_VAL(list, retval), cra_alist_pop_at(list, index, retval))
// bool pop_front(CraAList *list, out T *retval)
#define cra_alist_pop_front(list, retval)     cra_alist_pop_at(list, 0, retval)
// bool pop_back(CraAList *list, out T *retval)
#define cra_alist_pop_back(list, retval)      cra_alist_pop_at(list, (list)->count - 1, retval)

// bool remove_at(CraAList *list, size_t index)
#define cra_alist_remove_at(list, index) (cra_alist_pop_at)(list, index, NULL)
// bool remove_front(CraAList *list)
#define cra_alist_remove_front(list)     cra_alist_remove_at(list, 0)
// bool remove_back(CraAList *list)
#define cra_alist_remove_back(list)      cra_alist_remove_at(list, (list)->count - 1)

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
#define cra_alist_get(list, index, retval) (CRA_ALIST_CHECK_VAL(list, retval), cra_alist_get(list, index, retval))

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
#define cra_alist_get_and_set(list, index, newval, retoldval)                 \
    (CRA_ALIST_CHECK_VAL(list, newval), CRA_ALIST_CHECK_VAL(list, retoldval), \
     cra_alist_get_and_set(list, index, newval, retoldval))

// bool set(CraAList *list, size_t index, T *newval)
#define cra_alist_set(list, index, newval)                                                  \
    (CRA_ALIST_CHECK_VAL(list, newval), (cra_alist_get_and_set)(list, index, newval, NULL))

CRA_API bool
cra_alist_reverse(CraAList *list);

CRA_API bool
cra_alist_sort(struct CraAList *list, cra_cmp_fn compare);
// bool sort(CraAList *list, int (*compare)(const T *, const T *))
#define cra_alist_sort(list, compare) cra_alist_sort(list, (cra_cmp_fn)(compare))

CRA_API bool
cra_alist_add_sort(struct CraAList *list, cra_cmp_fn compare, void *val);
// bool add_sort(CraAList *list, int (*compare)(const T *, const T *), T *val)
#define cra_alist_add_sort(list, compare, val)                                             \
    (CRA_ALIST_CHECK_VAL(list, val), cra_alist_add_sort(list, (cra_cmp_fn)(compare), val))

// ====================================== interfaces ======================================

// initializable

typedef struct CraAListInitializableParam
{
    size_t itemsize;
} CraAListInitializableParam;
#define CRA_ALIST_INITIALIZABLE_PARAM_INIT(T)        { sizeof(T) }
#define CRA_ALIST_INITIALIZABLE_PARAM_DECL(var_name) CraAListInitializableParam var_name
#define CRA_ALIST_INITIALIZABLE_PARAM_DEF(var_name, T)                                   \
    CRA_ALIST_INITIALIZABLE_PARAM_DECL(var_name) = CRA_ALIST_INITIALIZABLE_PARAM_INIT(T)

CRA_API CRA_INITIALIZABLE_DEF(cra_g_alist_initializable_i);
#define CRA_ALIST_INITIALIZABLE_I (&cra_g_alist_initializable_i)

// appendable

CRA_API CRA_APPENDABLE_DEF(cra_g_alist_appendable_i);
#define CRA_ALIST_APPENDABLE_I (&cra_g_alist_appendable_i)

// iterable

CRA_API CRA_ITERABLE_DEF(cra_g_alist_iterable_i);
#define CRA_ALIST_ITERABLE_I (&cra_g_alist_iterable_i)

#endif