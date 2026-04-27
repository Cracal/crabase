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

#if 1
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

CRA_API bool
cra_alist_init_size(CraAList *list, size_t element_size, size_t init_capacity, bool zero_memory);

CRA_API bool
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

CRA_API bool
cra_alist_reverse(CraAList *list);

CRA_API CraAList *
cra_alist_clone(CraAList *list, cra_deep_copy_val_fn deep_copy_val);

CRA_API bool
cra_alist_sort(CraAList *list, cra_compare_fn compare);

CRA_API bool
cra_alist_add_sort(CraAList *list, cra_compare_fn compare, void *val);
#endif

#define CRA_ALIST_DEFAULT_CAPACITY 8

#define CRA_ALIST_STRUCT(_Name, _TVal) \
    struct _Name                       \
    {                                  \
        _TVal *array;                  \
        size_t count;                  \
        size_t capacity;               \
        size_t itemsize;               \
    }

CRA_ALIST_STRUCT(NewCraAList, char);

#define CRA_ALIST_DEF_SHORT(_TVal, _TVal_short)                                                                    \
    union CraAList_##_TVal_short                                                                                   \
    {                                                                                                              \
        struct NewCraAList list;                                                                                   \
        CRA_ALIST_STRUCT(, _TVal);                                                                                 \
    };                                                                                                             \
    _Static_assert(sizeof(((union CraAList_##_TVal_short *)0)->array) == sizeof(((struct NewCraAList *)0)->array), \
                   "array pointer size mismatch");                                                                 \
    _Static_assert(offsetof(union CraAList_##_TVal_short, array) == offsetof(struct NewCraAList, array),           \
                   "array pointer offset mismatch")
#define CRA_ALIST_DECL(_TVal) union CraAList_##_TVal
#define CRA_ALIST_DEF(_TVal)  CRA_ALIST_DEF_SHORT(_TVal, _TVal)
#define CRA_ALIST(_TVal)      CRA_ALIST_DECL(_TVal)

#define CRA_ALIST_ITEM_T(_list) __typeof__((_list)->array[0])

CRA_API bool
newcra_alist_init_with_size(struct NewCraAList *list, size_t itemsize, size_t init_capacity);
// bool init_with_size(CraAList<T> *list, size_t init_capacity)
#define newcra_alist_init_with_size(_list, _init_capacity)                                 \
    newcra_alist_init_with_size(&(_list)->list, sizeof((_list)->array[0]), _init_capacity)
// bool init(CraAList<T> *list)
#define newcra_alist_init(_list) newcra_alist_init_with_size(_list, CRA_ALIST_DEFAULT_CAPACITY)

CRA_API void
newcra_alist_uninit(struct NewCraAList *list);
// void uninit(CraAList<T> *list)
#define newcra_alist_uninit(_list) newcra_alist_uninit(&(_list)->list)

#define newcra_alist_clear(_list) (void)((_list)->count = 0)

CRA_API bool
newcra_alist_ensure(struct NewCraAList *list, size_t nspare, bool shrink2fit);
// bool ensure(CraAList<T> *list, size_t nspare, bool shrink2fit)
#define newcra_alist_ensure(_list, _nspare, _shrink2fit) newcra_alist_ensure(&(_list)->list, _nspare, _shrink2fit)

static inline bool
newcra_alist_insert(struct NewCraAList *list, size_t index, void *val)
{
    assert(val);
    assert(list);
    assert(list->array);

    if (index > list->count)
        return false;

    if (!(newcra_alist_ensure)(list, 1, false))
        return false;

    char  *array = (char *)list->array;
    size_t nmoving = list->count - index;
    if (nmoving > 0)
        memmove(array + (index + 1) * list->itemsize, array + index * list->itemsize, nmoving * list->itemsize);
    memcpy(array + index * list->itemsize, val, list->itemsize);
    ++list->count;
    return true;
}
// bool insert(CraAList<T> *list, size_t index, T val)
#define newcra_alist_insert(_list, _index, _val)                                      \
    newcra_alist_insert(&(_list)->list, _index, &(CRA_ALIST_ITEM_T(_list)){ (_val) })
// bool prepend(CraAList<T> *list, T val)
#define newcra_alist_prepend(_list, _val) newcra_alist_insert(_list, 0, _val)
// bool append(CraAList<T> *list, T val)
#define newcra_alist_append(_list, _val)  newcra_alist_insert(_list, (_list)->count, _val)

// bool has(CraAList<T> *list, size_t index)
#define newcra_alist_has(_list, _index) ((size_t)(_index) < (_list)->count)

static inline bool
newcra_alist_pop_at(struct NewCraAList *list, size_t index, void *retval)
{
    if (!newcra_alist_has(list, index))
        return false;

    char *array = (char *)list->array;
    if (retval)
        memcpy(retval, array + index * list->itemsize, list->itemsize);
    size_t nmoving = list->count - index - 1;
    if (nmoving > 0)
        memmove(array + index * list->itemsize, array + (index + 1) * list->itemsize, nmoving * list->itemsize);
    --list->count;
    return true;
}
// bool pop_at(CraAList<T> *list, size_t index, T *retval)
#define newcra_alist_pop_at(_list, _index, _retval)                                                          \
    ((void)((CRA_ALIST_ITEM_T(_list) *){ (_retval) }), newcra_alist_pop_at(&(_list)->list, _index, _retval))
// bool pop_front(CraAList<T> *list, T *retval)
#define newcra_alist_pop_front(_list, _retval) newcra_alist_pop_at(_list, 0, _retval)
// bool pop_back(CraAList<T> *list, T *retval)
#define newcra_alist_pop_back(_list, _retval)  newcra_alist_pop_at(_list, (_list)->count - 1, _retval)
// bool remove_at(CraAList<T> *list, size_t index)
#define newcra_alist_remove_at(_list, _index)  newcra_alist_pop_at(_list, _index, NULL)
// bool remove_front(CraAList<T> *list)
#define newcra_alist_remove_front(_list)       newcra_alist_pop_front(_list, NULL)
// bool remove_back(CraAList<T> *list)
#define newcra_alist_remove_back(_list)        newcra_alist_pop_back(_list, NULL)

// T get(CraAList<T> *list, size_t index, T default_val)
#define newcra_alist_get(_list, _index, _default_val)                           \
    (newcra_alist_has(_list, _index) ? (_list)->array[_index] : (_default_val))
// T *get_ref(CraAList<T> *list, size_t index)
#define newcra_alist_get_ref(_list, _index) (newcra_alist_has(_list, _index) ? &(_list)->array[_index] : NULL)

// bool set(CraAList<T> *list, size_t index, T val)
#define newcra_alist_set(_list, _index, _val)                                    \
    (newcra_alist_has(_list, _index) && ((_list)->array[_index] = (_val), true))

// bool get_and_set(CraAList<T> *list, size_t index, T val, T *retval)
#define newcra_alist_get_and_set(_list, _index, _val, _retval)                                                        \
    (newcra_alist_has(_list, _index) && (*(_retval) = (_list)->array[_index], (_list)->array[_index] = (_val), true))

CRA_API bool
newcra_alist_sort(struct NewCraAList *list, cra_compare_fn compare);
// bool sort(CraAList<T> *list, int (*compare)(const T *, const T *))
#define newcra_alist_sort(_list, _compare)                                \
    ((void)((CRA_COMPARE_FN(, CRA_ALIST_ITEM_T(_list) *)){ (_compare) }), \
     newcra_alist_sort(&(_list)->list, (cra_compare_fn)(_compare)))

CRA_API bool
newcra_alist_add_sort(struct NewCraAList *list, cra_compare_fn compare, void *val);
// bool add_sort(CraAList<T> *list, int (*compare)(const T *, const T *), T val)
#define newcra_alist_add_sort(_list, _compare, _val)                                                          \
    ((void)((CRA_COMPARE_FN(, CRA_ALIST_ITEM_T(_list) *)){ (_compare) }),                                     \
     newcra_alist_add_sort(&(_list)->list, (cra_compare_fn)(_compare), &(CRA_ALIST_ITEM_T(_list)){ (_val) }))

// ====================================== interfaces ======================================

// initializable

typedef struct CraAListInitializableParam
{
    size_t itemsize;
    size_t init_capacity;
} CraAListInitializableParam;
#define CRA_ALIST_INITIALIZABLE_PARAM_INIT(_TList, _init_capacity) { sizeof(((_TList *)0)->array[0]), _init_capacity }

CRA_API CRA_INITIALIZABLE_DEF(cra_g_alist_initializable_i);
#define CRA_ALIST_INITIALIZABLE_I (&cra_g_alist_initializable_i)

// appendable

CRA_API CRA_APPENDABLE_DEF(cra_g_alist_appendable_i);
#define CRA_ALIST_APPENDABLE_I (&cra_g_alist_appendable_i)

// iterable

CRA_API CRA_ITERABLE_DEF(cra_g_alist_iterable_i);
#define CRA_ALIST_ITERABLE_I (&cra_g_alist_iterable_i)

#endif