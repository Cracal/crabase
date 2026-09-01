/**
 * @file cra_collections_i.h
 * @author Cracal
 * @brief xxx
 * @version 0.1
 * @date 2026-09-01
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __CRA_COLLECTIONS_I_H__
#define __CRA_COLLECTIONS_I_H__
#include "collections/cra_alist.h"
#include "collections/cra_llist.h"
#include "collections/cra_deque.h"
#include "collections/cra_dict.h"

typedef struct CraPair
{
    void *val_ref;
    void *key_ref;
} CraPair;

typedef struct CraIterator
{
    void *obj;
    union
    {
        size_t idx;
        void  *cur;
    } ic1, ic2;
} CraIterator;

// ========================== initializable ==========================

#define CRA_INITIALIZABLE_INIT_FN(name)   bool name(void *obj, size_t length, void *params)
#define CRA_INITIALIZABLE_UNINIT_FN(name) void name(void *obj)
#define CRA_INITIALIZABLE_DEF(name)       const CraInitializable_i name

typedef struct CraInitializable_i
{
    CRA_INITIALIZABLE_INIT_FN((*init));
    CRA_INITIALIZABLE_UNINIT_FN((*uninit));
} CraInitializable_i;

static inline bool
cra_initializable_init(const CraInitializable_i *i, void *obj, size_t length, void *params)
{
    return i->init(obj, length, params);
}

static inline void
cra_initializable_uninit(const CraInitializable_i *i, void *obj)
{
    i->uninit(obj);
}

// ========================== appendable ==========================

#define CRA_APPENDABLE_APPEND_FN(name) bool name(void *obj, CraPair *val)
#define CRA_APPENDABLE_DEF(name)       const CraAppendable_i name

typedef struct CraAppendable_i
{
    CRA_APPENDABLE_APPEND_FN((*append));
} CraAppendable_i;

static inline bool
cra_appendable_append(const CraAppendable_i *i, void *obj, CraPair *val)
{
    return i->append(obj, val);
}

// ========================== iterable ==========================

#define CRA_ITERABLE_INIT_FN(name) bool name(void *obj, CraIterator *it, size_t *retcnt, bool reverse)
#define CRA_ITERABLE_NEXT_FN(name) bool name(CraIterator *it, CraPair *val)
#define CRA_ITERABLE_PREV_FN(name) bool name(CraIterator *it, CraPair *val)
#define CRA_ITERABLE_DEF(name)     const CraIterable_i name

typedef struct CraIterable_i
{
    CRA_ITERABLE_INIT_FN((*init));
    CRA_ITERABLE_NEXT_FN((*next));
    CRA_ITERABLE_PREV_FN((*prev));
} CraIterable_i;

static inline bool
cra_iterable_init(const CraIterable_i *i, void *obj, CraIterator *it, size_t *retcnt, bool reverse)
{
    return i->init(obj, it, retcnt, reverse);
}

static inline bool
cra_iterable_next(const CraIterable_i *i, CraIterator *it, CraPair *val)
{
    return i->next(it, val);
}

static inline bool
cra_iterable_prev(const CraIterable_i *i, CraIterator *it, CraPair *val)
{
    return i->prev(it, val);
}

#define OLDCRA_FOREACH(iterable_i, obj, val_name)                                                       \
    for (CraIterator val_name##_it = { 0 };                                                             \
         val_name##_it.ic1.idx == 0 && cra_iterable_init(iterable_i, obj, &val_name##_it, NULL, false); \
         val_name##_it.ic1.idx = 1)                                                                     \
        for (CraPair val_name = { 0 }; cra_iterable_next(iterable_i, &val_name##_it, &val_name);)

#define OLDCRA_FOREACH_REVERSE(iterable_i, obj, val_name)                                              \
    for (CraIterator val_name##_it = { 0 };                                                            \
         val_name##_it.ic1.idx == 0 && cra_iterable_init(iterable_i, obj, &val_name##_it, NULL, true); \
         val_name##_it.ic1.idx = 1)                                                                    \
        for (CraPair val_name = { 0 }; cra_iterable_prev(iterable_i, &val_name##_it, &val_name);)

// ====================================== alist ======================================

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

// ====================================== llist ======================================

// initializable

typedef struct CraLListInitializableParam
{
    size_t itemsize;
} CraLListInitializableParam;
#define CRA_LLIST_INITIALIZABLE_PARAM_INIT(T)        { sizeof(T) }
#define CRA_LLIST_INITIALIZABLE_PARAM_DECL(var_name) CraLListInitializableParam var_name
#define CRA_LLIST_INITIALIZABLE_PARAM_DEF(var_name, T)                                   \
    CRA_LLIST_INITIALIZABLE_PARAM_DECL(var_name) = CRA_LLIST_INITIALIZABLE_PARAM_INIT(T)

CRA_API CRA_INITIALIZABLE_DEF(cra_g_llist_initializable_i);
#define CRA_LLIST_INITIALIZABLE_I (&cra_g_llist_initializable_i)

// appendable

CRA_API CRA_APPENDABLE_DEF(cra_g_llist_appendable_i);
#define CRA_LLIST_APPENDABLE_I (&cra_g_llist_appendable_i)

// iterable

CRA_API CRA_ITERABLE_DEF(cra_g_llist_iterable_i);
#define CRA_LLIST_ITERABLE_I (&cra_g_llist_iterable_i)

// ====================================== deque ======================================

// initializable

typedef struct CraDequeInitializableParam
{
    size_t itemsize;
} CraDequeInitializableParam;
#define CRA_DEQUE_INITIALIZABLE_PARAM_INIT(T)        { sizeof(T) }
#define CRA_DEQUE_INITIALIZABLE_PARAM_DECL(var_name) CraDequeInitializableParam var_name
#define CRA_DEQUE_INITIALIZABLE_PARAM_DEF(var_name, T)                                   \
    CRA_DEQUE_INITIALIZABLE_PARAM_DECL(var_name) = CRA_DEQUE_INITIALIZABLE_PARAM_INIT(T)

CRA_API CRA_INITIALIZABLE_DEF(cra_g_deque_initializable_i);
#define CRA_DEQUE_INITIALIZABLE_I (&cra_g_deque_initializable_i)

// appendable

CRA_API CRA_APPENDABLE_DEF(cra_g_deque_appendable_i);
#define CRA_DEQUE_APPENDABLE_I (&cra_g_deque_appendable_i)

// iterable

CRA_API CRA_ITERABLE_DEF(cra_g_deque_iterable_i);
#define CRA_DEQUE_ITERABLE_I (&cra_g_deque_iterable_i)

// ====================================== dict ======================================

// initializable

typedef struct CraDictInitializableParam
{
    size_t      key_size;
    size_t      val_size;
    size_t      key_align;
    size_t      val_align;
    cra_cmp_fn  compare_key;
    cra_hash_fn hash_key;
} CraDictInitializableParam;
#define CRA_DICT_INITIALIZABLE_PARAM_INIT(TKey, TVal, hash_key_fn, compare_key_fn) \
    {                                                                              \
        sizeof(TKey),                                                              \
        sizeof(TVal),                                                              \
        alignof(TKey),                                                             \
        alignof(TVal),                                                             \
        (cra_cmp_fn)(compare_key_fn),                                              \
        (cra_hash_fn)(hash_key_fn)                                                 \
    }
#define CRA_DICT_INITIALIZABLE_PARAM_DECL(var_name) CraDictInitializableParam var_name
#define CRA_DICT_INITIALIZABLE_PARAM_DEF(var_name, TKey, TVal, hash_key_fn, compare_key_fn) \
    CRA_DICT_INITIALIZABLE_PARAM_DECL(var_name) =                                           \
      CRA_DICT_INITIALIZABLE_PARAM_INIT(TKey, TVal, hash_key_fn, compare_key_fn)

CRA_API CRA_INITIALIZABLE_DEF(cra_g_dict_initializable_i);
#define CRA_DICT_INITIALIZABLE_I (&cra_g_dict_initializable_i)

// appendable

CRA_API CRA_APPENDABLE_DEF(cra_g_dict_appendable_i);
#define CRA_DICT_APPENDABLE_I (&cra_g_dict_appendable_i)

// iterable

CRA_API CRA_ITERABLE_DEF(cra_g_dict_iterable_i);
#define CRA_DICT_ITERABLE_I (&cra_g_dict_iterable_i)

#endif
