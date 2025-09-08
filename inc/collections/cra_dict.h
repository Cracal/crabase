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
#include "cra_collects.h"
#include "serialize/cra_serialize.h"

typedef struct _CraDictEntry CraDictEntry;

typedef struct _CraDict
{
    bool           zero_memory;
    size_t         key_size;
    size_t         val_size;
    size_t         entry_size;
    ssize_t        count;
    ssize_t        next;
    ssize_t        capacity;
    ssize_t        freelist;
    ssize_t       *buckets;
    CraDictEntry  *entries;
    cra_hash_fn    hash_key;
    cra_compare_fn compare_key;
} CraDict;

typedef struct _CraDictIter
{
    ssize_t  index;
    CraDict *dict;
} CraDictIter;

#define CRA_DICT_INIT_CAPACITY 7

CRA_API void
cra_dict_iter_init(CraDict *dict, CraDictIter *it);

CRA_API bool
cra_dict_iter_next(CraDictIter *it, void **retkeyptr, void **retvalptr);

CRA_API void
cra_dict_init_size(CraDict       *dict,
                   size_t         key_size,
                   size_t         val_size,
                   size_t         init_capacity,
                   bool           zero_memory,
                   cra_hash_fn    hash_key,
                   cra_compare_fn compare_key);

CRA_API void
cra_dict_init(CraDict       *dict,
              size_t         key_size,
              size_t         val_size,
              bool           zero_memory,
              cra_hash_fn    hash_key,
              cra_compare_fn compare_key);

#define cra_dict_init_size0(_TKey, _TVal, _dict, _init_capacity, _zero_memory, _hash_key_fn, _compare_key_fn) \
    cra_dict_init_size(_dict, sizeof(_TKey), sizeof(_TVal), _init_capacity, _zero_memory, _hash_key_fn, _compare_key_fn)
#define cra_dict_init0(_TKey, _TVal, _dict, _zero_memory, _hash_key_fn, _compare_key_fn)            \
    cra_dict_init(_dict, sizeof(_TKey), sizeof(_TVal), _zero_memory, _hash_key_fn, _compare_key_fn)

CRA_API void
cra_dict_uninit(CraDict *dict);

static inline size_t
cra_dict_get_count(CraDict *dict)
{
    return dict->count;
}

CRA_API void
cra_dict_clear(CraDict *dict);

CRA_API bool
cra_dict_put(CraDict *dict, void *key, void *val, void *retoldkey, void *retoldval);
#define cra_dict_put0(_dict, _key, _val) cra_dict_put(_dict, _key, _val, NULL, NULL)

CRA_API bool
cra_dict_add(CraDict *dict, void *key, void *val);

CRA_API bool
cra_dict_pop(CraDict *dict, void *key, void *retkey, void *retval);

CRA_API bool
cra_dict_remove(CraDict *dict, void *key);

CRA_API bool
cra_dict_get(CraDict *dict, void *key, void *retval);

CRA_API bool
cra_dict_get_ptr(CraDict *dict, void *key, void **retvalptr);

CRA_API CraDict *
cra_dict_clone(CraDict *dict, cra_deep_copy_val_fn deep_copy_key, cra_deep_copy_val_fn deep_copy_val);

// =========

typedef struct
{
    cra_hash_fn    hash;
    cra_compare_fn compare;
} CraDictDzerArg;

CRA_API const CraSzer_i __g_cra_dict_szer_i;
CRA_API const CraDzer_i __g_cra_dict_dzer_i;
#define CRA_DICT_SZER_I (&__g_cra_dict_szer_i)
#define CRA_DICT_DZER_I (&__g_cra_dict_dzer_i)

#endif