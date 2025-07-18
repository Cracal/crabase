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
    bool zero_memory;
    size_t key_size;
    size_t val_size;
    size_t entry_size;
    ssize_t count;
    ssize_t next;
    ssize_t capacity;
    ssize_t freelist;
    ssize_t *buckets;
    CraDictEntry *entries;
    cra_hash_fn hash_key;
    cra_compare_fn compare_key;
    cra_remove_val_fn remove_key;
    cra_remove_val_fn remove_val;
} CraDict;

typedef struct _CraDictIter
{
    ssize_t index;
    CraDict *dict;
} CraDictIter;

#define CRA_DICT_INIT_CAPACITY 7

CRA_API CraDictIter cra_dict_iter_init(CraDict *dict);
CRA_API bool cra_dict_iter_next(CraDictIter *it, void **retkeyptr, void **retvalptr);

CRA_API void cra_dict_init_size(CraDict *dict, size_t key_size, size_t val_size,
                                size_t init_capacity, bool zero_memory,
                                cra_hash_fn hash_key, cra_compare_fn compare_key,
                                cra_remove_val_fn remove_key, cra_remove_val_fn remove_val);
CRA_API void cra_dict_init(CraDict *dict, size_t key_size, size_t val_size, bool zero_memory,
                           cra_hash_fn hash_key, cra_compare_fn compare_key,
                           cra_remove_val_fn remove_key, cra_remove_val_fn remove_val);
#define cra_dict_init_size0(_TKey, _TVal, _dict, _init_capacity, _zero_memory, _hash_key_fn, _compare_key_fn, _remove_key_fn, _remove_val_fn) \
    cra_dict_init_size(_dict, sizeof(_TKey), sizeof(_TVal), _init_capacity, _zero_memory, _hash_key_fn, _compare_key_fn, _remove_key_fn, _remove_val_fn)
#define cra_dict_init0(_TKey, _TVal, _dict, _zero_memory, _hash_key_fn, _compare_key_fn, _remove_key_fn, _remove_val_fn) \
    cra_dict_init(_dict, sizeof(_TKey), sizeof(_TVal), _zero_memory, _hash_key_fn, _compare_key_fn, _remove_key_fn, _remove_val_fn)
CRA_API void cra_dict_uninit(CraDict *dict);

static inline size_t cra_dict_get_count(CraDict *dict) { return dict->count; }

CRA_API void cra_dict_clear(CraDict *dict);

CRA_API bool cra_dict_put(CraDict *dict, void *key, void *val);
CRA_API bool cra_dict_add(CraDict *dict, void *key, void *val);

CRA_API bool cra_dict_pop(CraDict *dict, void *key, void *retval);
CRA_API bool cra_dict_remove(CraDict *dict, void *key);

CRA_API bool cra_dict_get(CraDict *dict, void *key, void *retval);
CRA_API bool cra_dict_get_ptr(CraDict *dict, void *key, void **retvalptr);

CRA_API CraDict *cra_dict_clone(CraDict *dict, cra_deep_copy_val_fn deep_copy_key, cra_deep_copy_val_fn deep_copy_val);

// =========

typedef struct _CraDictSerInitArgs
{
    bool zero_memory;
    size_t key_size;
    size_t val_size;
    cra_hash_fn hash_key_fn;
    cra_compare_fn compare_key_fn;
    cra_remove_val_fn remove_key_fn;
    cra_remove_val_fn remove_val_fn;
} CraDictSerInitArgs;

CRA_API const CraTypeIter_i g_dict_ser_iter_i;
CRA_API const CraTypeInit_i g_dict_ser_init_i;

#define CRA_DICT_SER_ARGS(_name, _zero_memory, _key_size, _val_size, _hash_key_fn, _cmp_key_fn, _remove_key_fn, _remove_val_fn) \
    CraDictSerInitArgs _name = {_zero_memory, _key_size, _val_size, _hash_key_fn, _cmp_key_fn, _remove_key_fn, _remove_val_fn}
#define CRA_DICT_SER_ARGS0(_name, _initialied_dict)                                                                       \
    CRA_DICT_SER_ARGS(_name, (_initialied_dict)->zero_memory, (_initialied_dict)->key_size, (_initialied_dict)->val_size, \
                      (_initialied_dict)->hash_key, (_initialied_dict)->compare_key, (_initialied_dict)->remove_key,      \
                      (_initialied_dict)->remove_val)

#endif