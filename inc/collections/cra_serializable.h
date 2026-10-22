/**
 * @file cra_serializable.h
 * @author Cracal
 * @brief collections serializable interface
 * @version 0.1
 * @date 2026-10-05
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef CRA_COLLECTIONS_SERIALIZABLE_H
#define CRA_COLLECTIONS_SERIALIZABLE_H
#include "cra_alist.h"
#include "cra_llist.h"
#include "cra_deque.h"
#include "cra_dict.h"

#define CRA_LIST_DEF_SRLZ_UNINIT_FN(name, fn, list_type, item_type, item_name, release_item_code) \
    static void name(void *obj)                                                                   \
    {                                                                                             \
        assert(obj);                                                                              \
        item_type  item_name;                                                                     \
        list_type *list = (list_type *)obj;                                                       \
                                                                                                  \
        if (list->count > 0)                                                                      \
        {                                                                                         \
            CRA_FOREACH(list_type, list, &item_name)                                              \
            {                                                                                     \
                release_item_code                                                                 \
            }                                                                                     \
        }                                                                                         \
                                                                                                  \
        fn(list);                                                                                 \
    }

#if 1 // for alist

CRA_API void
cra_alist_srlz_iter_init(void *iter, size_t itersize, void *list, size_t *retcount);

CRA_API bool
cra_alist_srlz_iter_next(void *iter, void *retitem);

CRA_API bool
cra_alist_srlz_append(void *list, void *item);

CRA_API bool
cra_alist_srlz_init(void *list, size_t itemsize, size_t count);

#define CRA_ALIST_DEF_SRLZ_UNINIT_FN(name, item_type, item_name, release_item_code)                        \
    CRA_LIST_DEF_SRLZ_UNINIT_FN(name, cra_alist_uninit, CraAList, item_type, item_name, release_item_code)

#ifdef CRA_COMPILER_MSVC

static inline void
cra_alist_srlz_iter_int_wrap(void *iter, size_t itersize, void *list, size_t *retcount)
{
    cra_alist_srlz_iter_init(iter, itersize, list, retcount);
}
static inline bool
cra_alist_srlz_iter_next_wrap(void *iter, void *retitem)
{
    return cra_alist_srlz_iter_next(iter, retitem);
}
static inline bool
cra_alist_srlz_append_wrap(void *list, void *item)
{
    return cra_alist_srlz_append(list, item);
}
static inline bool
cra_alist_srlz_init_wrap(void *list, size_t itemsize, size_t count)
{
    return cra_alist_srlz_init(list, itemsize, count);
}
static inline void
cra_alist_srlz_uninit_wrap(void *list)
{
    cra_alist_uninit((CraAList *)list);
}

#define CRA_ALIST_SRLZ_WITH_UNINIT(uninit_fn)                                                      \
    cra_alist_srlz_init_wrap, uninit_fn, cra_alist_srlz_append_wrap, cra_alist_srlz_iter_int_wrap, \
      cra_alist_srlz_iter_next_wrap
#define CRA_ALIST_SRLZ CRA_ALIST_SRLZ_WITH_UNINIT(cra_alist_srlz_uninit_wrap)

#else

#define CRA_ALIST_SRLZ_WITH_UNINIT(uninit_fn)                                                                 \
    cra_alist_srlz_init, uninit_fn, cra_alist_srlz_append, cra_alist_srlz_iter_init, cra_alist_srlz_iter_next
#define CRA_ALIST_SRLZ CRA_ALIST_SRLZ_WITH_UNINIT((void (*)(void *))cra_alist_uninit)

#endif

#endif // end for alist

#if 1 // for llist

CRA_API void
cra_llist_srlz_iter_init(void *iter, size_t itersize, void *list, size_t *retcount);

CRA_API bool
cra_llist_srlz_iter_next(void *iter, void *retitem);

CRA_API bool
cra_llist_srlz_append(void *list, void *item);

CRA_API bool
cra_llist_srlz_init(void *list, size_t itemsize, size_t count);

#define CRA_LLIST_DEF_SRLZ_UNINIT_FN(name, item_type, item_name, release_item_code)                        \
    CRA_LIST_DEF_SRLZ_UNINIT_FN(name, cra_llist_uninit, CraLList, item_type, item_name, release_item_code)

#ifdef CRA_COMPILER_MSVC

static inline void
cra_llist_srlz_iter_int_wrap(void *iter, size_t itersize, void *list, size_t *retcount)
{
    cra_llist_srlz_iter_init(iter, itersize, list, retcount);
}
static inline bool
cra_llist_srlz_iter_next_wrap(void *iter, void *retitem)
{
    return cra_llist_srlz_iter_next(iter, retitem);
}
static inline bool
cra_llist_srlz_append_wrap(void *list, void *item)
{
    return cra_llist_srlz_append(list, item);
}
static inline bool
cra_llist_srlz_init_wrap(void *list, size_t itemsize, size_t count)
{
    return cra_llist_srlz_init(list, itemsize, count);
}
static inline void
cra_llist_srlz_uninit_wrap(void *list)
{
    cra_llist_uninit((CraLList *)list);
}

#define CRA_LLIST_SRLZ_WITH_UNINIT(uninit_fn)                                                      \
    cra_llist_srlz_init_wrap, uninit_fn, cra_llist_srlz_append_wrap, cra_llist_srlz_iter_int_wrap, \
      cra_llist_srlz_iter_next_wrap
#define CRA_LLIST_SRLZ CRA_LLIST_SRLZ_WITH_UNINIT(cra_llist_srlz_uninit_wrap)

#else

#define CRA_LLIST_SRLZ_WITH_UNINIT(uninit_fn)                                                                 \
    cra_llist_srlz_init, uninit_fn, cra_llist_srlz_append, cra_llist_srlz_iter_init, cra_llist_srlz_iter_next
#define CRA_LLIST_SRLZ CRA_LLIST_SRLZ_WITH_UNINIT((void (*)(void *))cra_llist_uninit)

#endif

#endif // end for llist

#if 1 // for deque

CRA_API void
cra_deque_srlz_iter_init(void *iter, size_t itersize, void *list, size_t *retcount);

CRA_API bool
cra_deque_srlz_iter_next(void *iter, void *retitem);

CRA_API bool
cra_deque_srlz_append(void *list, void *item);

CRA_API bool
cra_deque_srlz_init(void *list, size_t itemsize, size_t count);

#define CRA_DEQUE_DEF_SRLZ_UNINIT_FN(name, item_type, item_name, release_item_code)                        \
    CRA_LIST_DEF_SRLZ_UNINIT_FN(name, cra_deque_uninit, CraDeque, item_type, item_name, release_item_code)

#ifdef CRA_COMPILER_MSVC

static inline void
cra_deque_srlz_iter_int_wrap(void *iter, size_t itersize, void *list, size_t *retcount)
{
    cra_deque_srlz_iter_init(iter, itersize, list, retcount);
}
static inline bool
cra_deque_srlz_iter_next_wrap(void *iter, void *retitem)
{
    return cra_deque_srlz_iter_next(iter, retitem);
}
static inline bool
cra_deque_srlz_append_wrap(void *list, void *item)
{
    return cra_deque_srlz_append(list, item);
}
static inline bool
cra_deque_srlz_init_wrap(void *list, size_t itemsize, size_t count)
{
    return cra_deque_srlz_init(list, itemsize, count);
}
static inline void
cra_deque_srlz_uninit_wrap(void *list)
{
    cra_deque_uninit((CraDeque *)list);
}

#define CRA_DEQUE_SRLZ_WITH_UNINIT(uninit_fn)                                                      \
    cra_deque_srlz_init_wrap, uninit_fn, cra_deque_srlz_append_wrap, cra_deque_srlz_iter_int_wrap, \
      cra_deque_srlz_iter_next_wrap
#define CRA_DEQUE_SRLZ CRA_DEQUE_SRLZ_WITH_UNINIT(cra_deque_srlz_uninit_wrap)

#else

#define CRA_DEQUE_SRLZ_WITH_UNINIT(uninit_fn)                                          \
    cra_deque_srlz_init, uninit_fn, cra_deque_srlz_append, cra_deque_srlz_iter_init, cra_deque_srlz_iter_next
#define CRA_DEQUE_SRLZ CRA_DEQUE_SRLZ_WITH_UNINIT((void (*)(void *))cra_deque_uninit)

#endif

#endif // end for deque

#if 1 // for dict

CRA_API void
cra_dict_srlz_iter_init(void *iter, size_t itersize, void *dict, size_t *retcount);

CRA_API bool
cra_dict_srlz_iter_next(void *iter, void *retkey, void *retval);

CRA_API bool
cra_dict_srlz_append(void *dict, void *key, void *val);

#define CRA_DICT_DEF_SRLZ_INIT_FN(name, TKey, TVal, hash_fn, cmp_fn)                         \
    static bool name(void *dict, size_t keysize, size_t valsize, size_t count)               \
    {                                                                                        \
        CRA_UNUSED(keysize);                                                                 \
        CRA_UNUSED(valsize);                                                                 \
        assert(keysize == sizeof(TKey));                                                     \
        assert(valsize == sizeof(TVal));                                                     \
        return cra_dict_init_with_size(TKey, TVal, (CraDict *)dict, count, hash_fn, cmp_fn); \
    }
#define CRA_DICT_DEF_SRLZ_UNINIT_FN(name, key_type, key_name, val_type, val_name, release_item_code) \
    static void name(void *obj)                                                                      \
    {                                                                                                \
        assert(obj);                                                                                 \
        key_type key_name;                                                                           \
        val_type val_name;                                                                           \
        CraDict *dict = (CraDict *)obj;                                                              \
                                                                                                     \
        if (dict->count > 0)                                                                         \
        {                                                                                            \
            CRA_FOREACH(CraDict, dict, &key_name, &val_name)                                         \
            {                                                                                        \
                release_item_code                                                                    \
            }                                                                                        \
        }                                                                                            \
                                                                                                     \
        cra_dict_uninit(dict);                                                                       \
    }

#ifdef CRA_COMPILER_MSVC

static inline void
cra_dict_srlz_iter_init_wrap(void *iter, size_t itersize, void *dict, size_t *retcount)
{
    cra_dict_srlz_iter_init(iter, itersize, dict, retcount);
}
static inline bool
cra_dict_srlz_iter_next_wrap(void *iter, void *retkey, void *retval)
{
    return cra_dict_srlz_iter_next(iter, retkey, retval);
}
static inline bool
cra_dict_srlz_append_wrap(void *dict, void *key, void *val)
{
    return cra_dict_srlz_append(dict, key, val);
}
static inline void
cra_dict_srlz_uninit_wrap(void *dict)
{
    cra_dict_uninit((CraDict *)dict);
}

#define CRA_DICT_SRLZ_WITH_UNINIT(init_fn, uninit_fn)                                                         \
    init_fn, uninit_fn, cra_dict_srlz_append_wrap, cra_dict_srlz_iter_init_wrap, cra_dict_srlz_iter_next_wrap
#define CRA_DICT_SRLZ(init_fn) CRA_DICT_SRLZ_WITH_UNINIT(init_fn, cra_dict_srlz_uninit_wrap)

#else

#define CRA_DICT_SRLZ_WITH_UNINIT(init_fn, uninit_fn)                                          \
    init_fn, uninit_fn, cra_dict_srlz_append, cra_dict_srlz_iter_init, cra_dict_srlz_iter_next
#define CRA_DICT_SRLZ(init_fn) CRA_DICT_SRLZ_WITH_UNINIT(init_fn, (void (*)(void *))cra_dict_uninit)

#endif

#endif // end for dict

#endif
