/**
 * @file cra_dict.h
 * @author Cracal
 * @brief 字典
 * @version 0.1
 * @date 2024-10-16
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_DICT_H__
#define __CRA_DICT_H__
#include <stdalign.h>
#include "cra_collects.h"

#define CRA_DICT_DEAFULT_CAPACITY 11

#define CRA_DICT_CHECK_KEY(_dict, _key) assert((_dict)->key_size == sizeof(*(_key)))
#define CRA_DICT_CHECK_VAL(_dict, _val) assert((_dict)->val_size == sizeof(*(_val)))

typedef struct CraDictEntry CraDictEntry;
typedef struct CraDict      CraDict;

struct CraDict
{
    ssize_t      *buckets;
    CraDictEntry *entries;

    ssize_t next;
    ssize_t count;
    ssize_t capacity;
    ssize_t freelist;

    size_t key_size;
    size_t val_size;
    size_t key_offset;
    size_t val_offset;
    size_t entry_size;

    cra_hash_fn hash_key;
    cra_cmp_fn  compare_key;
};

CRA_API bool
cra_dict_init_with_size(CraDict    *dict,
                        size_t      key_size,
                        size_t      val_size,
                        size_t      key_offset,
                        size_t      val_offset,
                        size_t      init_capacity,
                        cra_hash_fn hash_key,
                        cra_cmp_fn  compare_key);
// bool init_with_size<TKey, TVal>(CraDict *dict, size_t init_capacity, cra_hash_t (*hash)(const TKey *key),
// int (*compare)(const TKey *a, const TKey *b))
#define cra_dict_init_with_size(_TKey, _TVal, _dict, _init_capacity, _hash_key_fn, _compare_key_fn) \
    cra_dict_init_with_size(_dict,                                                                  \
                            sizeof(_TKey),                                                          \
                            sizeof(_TVal),                                                          \
                            alignof(_TKey),                                                         \
                            alignof(_TVal),                                                         \
                            _init_capacity,                                                         \
                            (cra_hash_fn)(_hash_key_fn),                                            \
                            (cra_cmp_fn)(_compare_key_fn))
// bool init_with_size<TKey, TVal>(CraDict *dict, cra_hash_t (*hash)(const TKey *key),
// int (*compare)(const TKey *a, const TKey *b))
#define cra_dict_init(_TKey, _TVal, _dict, _hash_key_fn, _compare_key_fn)                                  \
    cra_dict_init_with_size(_TKey, _TVal, _dict, CRA_DICT_DEAFULT_CAPACITY, _hash_key_fn, _compare_key_fn)

CRA_API void
cra_dict_uninit(CraDict *dict);

static inline void
cra_dict_clear(CraDict *dict)
{
    assert(dict);
    assert(dict->buckets);
    assert(dict->entries);
    assert(dict->entry_size > 0 && dict->entry_size % 2 == 0);

    if (dict->next > 0)
        memset(dict->buckets, 0xff, dict->next * sizeof(ssize_t));
    dict->freelist = -1;
    dict->count = 0;
    dict->next = 0;
}

CRA_API bool
cra_dict_reserve(CraDict *dict, ssize_t new_capacity);

CRA_API bool
cra_dict_put_and_return_kv(CraDict *dict, void *key, void *val, void *retoldkey, void *retoldval);
CRA_API bool
cra_dict_put(CraDict *dict, void *key, void *val);
CRA_API bool
cra_dict_add(CraDict *dict, void *key, void *val);
// bool put_and_return_kv(CraDict *dict, TKey *key, TVal *val, out TKey *retoldkey, out TVal *retoldval)
#define cra_dict_put_and_return_kv(_dict, _key, _val, _retoldkey, _retoldval) \
    (CRA_DICT_CHECK_KEY(_dict, _key),                                         \
     CRA_DICT_CHECK_VAL(_dict, _val),                                         \
     CRA_DICT_CHECK_KEY(_dict, _retoldkey),                                   \
     CRA_DICT_CHECK_VAL(_dict, _retoldval),                                   \
     cra_dict_put_and_return_kv(_dict, _key, _val, _retoldkey, _retoldval))
// bool put_and_return_v(CraDict *dict, TKey *key, TVal *val, out TVal *retoldval)
#define cra_dict_put_and_return_v(_dict, _key, _val, _retoldval)        \
    (CRA_DICT_CHECK_KEY(_dict, _key),                                   \
     CRA_DICT_CHECK_VAL(_dict, _val),                                   \
     CRA_DICT_CHECK_VAL(_dict, _retoldval),                             \
     (cra_dict_put_and_return_kv)(_dict, _key, _val, NULL, _retoldval))
// bool put(CraDict *dict, TKey *key, TVal *val)
#define cra_dict_put(_dict, _key, _val)                                                                 \
    (CRA_DICT_CHECK_KEY(_dict, _key), CRA_DICT_CHECK_VAL(_dict, _val), cra_dict_put(_dict, _key, _val))
// bool add(CraDict *dict, TKey *key, TVal *val)
#define cra_dict_add(_dict, _key, _val)                                                                 \
    (CRA_DICT_CHECK_KEY(_dict, _key), CRA_DICT_CHECK_VAL(_dict, _val), cra_dict_add(_dict, _key, _val))

CRA_API bool
cra_dict_pop_kv(CraDict *dict, const void *key, void *retkey, void *retval);
// bool pop_kv(CraDict *dict, TKey *key, out TKey *retkey, out TVal *retval)
#define cra_dict_pop_kv(_dict, _key, _retkey, _retval) \
    (CRA_DICT_CHECK_KEY(_dict, _key),                  \
     CRA_DICT_CHECK_KEY(_dict, _retkey),               \
     CRA_DICT_CHECK_VAL(_dict, _retval),               \
     cra_dict_pop_kv(_dict, _key, _retkey, _retval))
// bool pop(CraDict *dict, TKey *key, out TVal *retval)
#define cra_dict_pop(_dict, _key, _retval) \
    (CRA_DICT_CHECK_KEY(_dict, _key), CRA_DICT_CHECK_VAL(_dict, _retval), (cra_dict_pop_kv)(_dict, _key, NULL, _retval))
// bool remove(CraDict *dict, TKey *key)
#define cra_dict_remove(_dict, _key) (CRA_DICT_CHECK_KEY(_dict, _key), (cra_dict_pop_kv)(_dict, _key, NULL, NULL))

CRA_API void *
cra_dict_get_ref(CraDict *dict, const void *key);
// TVal *get_ref(CraDict *dict, TKey *key)
#define cra_dict_get_ref(_dict, _key) (CRA_DICT_CHECK_KEY(_dict, _key), cra_dict_get_ref(_dict, _key))

static inline bool
cra_dict_get(CraDict *dict, const void *key, void *retval)
{
    void *pval = (cra_dict_get_ref)(dict, key);
    if (pval && retval)
        memcpy(retval, pval, dict->val_size);
    return pval != NULL;
}
// bool get(CraDict *dict, TKey *key, out TVal *retval)
#define cra_dict_get(_dict, _key, _retval)                                                                    \
    (CRA_DICT_CHECK_KEY(_dict, _key), CRA_DICT_CHECK_VAL(_dict, _retval), cra_dict_get(_dict, _key, _retval))

// ====================================== interfaces ======================================

// initializable

typedef struct CraDictInitializableParam
{
    size_t      key_size;
    size_t      val_size;
    size_t      key_align;
    size_t      val_align;
    size_t      init_capacity;
    cra_cmp_fn  compare_key;
    cra_hash_fn hash_key;
} CraDictInitializableParam;
#define CRA_DICT_INITIALIZABLE_PARAM_INIT(_TKey, _TVal, _init_capacity, _hash_key_fn, _compare_key_fn) \
    { sizeof(_TKey),                                                                                   \
      sizeof(_TVal),                                                                                   \
      alignof(_TKey),                                                                                  \
      alignof(_TVal),                                                                                  \
      _init_capacity,                                                                                  \
      (cra_cmp_fn)(_compare_key_fn),                                                                   \
      (cra_hash_fn)(_hash_key_fn) }
#define CRA_DICT_INITIALIZABLE_PARAM_DECL(_var_name) CraDictInitializableParam _var_name
#define CRA_DICT_INITIALIZABLE_PARAM_DEF(_var_name, _TKey, _TVal, _init_capacity, _hash_key_fn, _compare_key_fn) \
    CRA_DICT_INITIALIZABLE_PARAM_DECL(_var_name) =                                                               \
      CRA_DICT_INITIALIZABLE_PARAM_INIT(_TKey, _TVal, _init_capacity, _hash_key_fn, _compare_key_fn)

CRA_API CRA_INITIALIZABLE_DEF(cra_g_dict_initializable_i);
#define CRA_DICT_INITIALIZABLE_I (&cra_g_dict_initializable_i)

// appendable

CRA_API CRA_APPENDABLE_DEF(cra_g_dict_appendable_i);
#define CRA_DICT_APPENDABLE_I (&cra_g_dict_appendable_i)

// iterable

CRA_API CRA_ITERABLE_DEF(cra_g_dict_iterable_i);
#define CRA_DICT_ITERABLE_I (&cra_g_dict_iterable_i)

#endif