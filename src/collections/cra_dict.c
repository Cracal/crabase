/**
 * @file cra_dict.c
 * @author Cracal
 * @brief 字典
 * @version 0.1
 * @date 2024-10-16
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "collections/cra_dict.h"
#include "cra_assert.h"
#include "cra_malloc.h"

struct _CraDictEntry
{
    // `hashcode == -1`表示该位置的entry是未使用的entry
    // 虽然hashcode会用来判断是否使用了该entry，
    // 但因为有dict.next，所以不用特别对hashcode
    // 初始化为-1。只要在移除entry时对hashcode设为-1就行
    cra_hash_t hashcode;
    ssize_t    next;
    char       kv[];
};

#define CRA_DICT_COLLISION_MAX                         64
#define CRA_DICT_USABLE_FRACTION(n)                    (((n) << 1) / 3) // n * (2 / 3)
#define _CRA_DICT_ENTRY(_entries, _entry_size, _index) ((CraDictEntry *)((char *)(_entries) + (_index) * (_entry_size)))
#define CRA_DICT_ENTRY(_dict, _index)                  _CRA_DICT_ENTRY((_dict)->entries, (_dict)->entry_size, _index)
#define CRA_DICT_BUCKET(_hashcode, _capacity)          ((_hashcode) & CRA_HASH_MAX) % (_capacity)

static bool
__cra_is_prime(ssize_t n)
{
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;
    if (n % 2 == 0 || n % 3 == 0)
        return false;
    for (ssize_t i = 5; i * i <= n;)
    {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
        i += 6;
    }
    return true;
}

static ssize_t
__cra_next_prime(ssize_t n)
{
    if (n <= 1)
        return 2;
    while (true)
    {
        if (__cra_is_prime(n))
            return n;
        ++n;
    }
}

static inline void
__cra_dict_init_size(CraDict          *dict,
                     size_t            key_size,
                     size_t            val_size,
                     size_t            init_capacity,
                     bool              zero_memory,
                     cra_hash_fn       hash_key,
                     cra_compare_fn    compare_key,
                     cra_remove_val_fn remove_key,
                     cra_remove_val_fn remove_val)
{
    assert(!!dict && key_size > 0 && val_size > 0 && !!hash_key && !!compare_key);

    dict->zero_memory = zero_memory;
    dict->key_size = key_size;
    dict->val_size = val_size;
    dict->entry_size = sizeof(CraDictEntry) + key_size + val_size;
    dict->count = 0;
    dict->next = 0;
    dict->capacity = __cra_next_prime(init_capacity);
    dict->freelist = -1;
    dict->hash_key = hash_key;
    dict->compare_key = compare_key;
    dict->remove_key = remove_key;
    dict->remove_val = remove_val;

    dict->buckets = cra_malloc(dict->capacity * sizeof(ssize_t));
    dict->entries = cra_malloc(CRA_DICT_USABLE_FRACTION(dict->capacity) * dict->entry_size);

    // set to [-1, -1, ...]
    memset(dict->buckets, 0xff, dict->capacity * sizeof(ssize_t));
    // zero key-val pair.
    if (zero_memory)
        bzero(dict->entries, CRA_DICT_USABLE_FRACTION(dict->capacity) * dict->entry_size);
}

void
cra_dict_iter_init(CraDict *dict, CraDictIter *it)
{
    it->index = 0;
    it->dict = dict;
}

bool
cra_dict_iter_next(CraDictIter *it, void **retkeyptr, void **retvalptr)
{
    CraDictEntry *entry;
    while (it->index < it->dict->next)
    {
        entry = CRA_DICT_ENTRY(it->dict, it->index++);
        if (entry->hashcode == -1)
            continue;

        *retkeyptr = entry->kv;
        *retvalptr = entry->kv + it->dict->key_size;
        return true;
    }
    return false;
}

void
cra_dict_init_size(CraDict          *dict,
                   size_t            key_size,
                   size_t            val_size,
                   size_t            init_capacity,
                   bool              zero_memory,
                   cra_hash_fn       hash_key,
                   cra_compare_fn    compare_key,
                   cra_remove_val_fn remove_key,
                   cra_remove_val_fn remove_val)
{
    __cra_dict_init_size(
      dict, key_size, val_size, init_capacity, zero_memory, hash_key, compare_key, remove_key, remove_val);
}

void
cra_dict_init(CraDict          *dict,
              size_t            key_size,
              size_t            val_size,
              bool              zero_memory,
              cra_hash_fn       hash_key,
              cra_compare_fn    compare_key,
              cra_remove_val_fn remove_key,
              cra_remove_val_fn remove_val)
{
    __cra_dict_init_size(
      dict, key_size, val_size, CRA_DICT_INIT_CAPACITY, zero_memory, hash_key, compare_key, remove_key, remove_val);
}

void
cra_dict_uninit(CraDict *dict)
{
    cra_dict_clear(dict);
    cra_free(dict->buckets);
    cra_free(dict->entries);
}

void
cra_dict_clear(CraDict *dict)
{
    if (dict->remove_key || dict->remove_val)
    {
        CraDictEntry *entry;
        for (ssize_t i = 0; i < dict->next; i++)
        {
            entry = CRA_DICT_ENTRY(dict, i);
            if (entry->hashcode != -1)
            {
                if (dict->remove_key)
                    dict->remove_key(entry->kv);
                if (dict->remove_val)
                    dict->remove_val(entry->kv + dict->key_size);
            }
        }
    }
    // set to [-1, -1, ...]
    memset(dict->buckets, 0xff, dict->capacity * sizeof(ssize_t));
    // zero key-val pair
    if (dict->zero_memory)
        bzero(dict->entries, dict->next * dict->entry_size);
    dict->freelist = -1;
    dict->count = 0;
    dict->next = 0;
}

static void
__cra_dict_resize(CraDict *dict, ssize_t newcapacity)
{
    ssize_t      *newbuckets, bucket;
    ssize_t       new_entries_capacity;
    CraDictEntry *newentries, *entry;

    newcapacity = __cra_next_prime(newcapacity);
    new_entries_capacity = CRA_DICT_USABLE_FRACTION(newcapacity);

    newbuckets = cra_realloc(dict->buckets, newcapacity * sizeof(ssize_t));
    newentries = cra_realloc(dict->entries, new_entries_capacity * dict->entry_size);

    // set to [-1, -1, ...]
    memset(newbuckets, 0xff, newcapacity * sizeof(ssize_t));
    // zero key-val pair
    if (dict->zero_memory)
    {
        ssize_t old_entries_capacity = CRA_DICT_USABLE_FRACTION(dict->capacity);
        bzero((char *)newentries + old_entries_capacity * dict->entry_size,
              (new_entries_capacity - old_entries_capacity) * dict->entry_size);
    }

    for (ssize_t i = 0; i < dict->next; i++)
    {
        entry = _CRA_DICT_ENTRY(newentries, dict->entry_size, i);
        if (entry->hashcode != -1)
        {
            bucket = CRA_DICT_BUCKET(entry->hashcode, newcapacity);
            entry->next = newbuckets[bucket];
            newbuckets[bucket] = i;
        }
    }

    dict->buckets = newbuckets;
    dict->entries = newentries;
    dict->capacity = newcapacity;
}

static inline bool
__cra_dict_put(CraDict *dict, void *key, void *val, bool add)
{
    CraDictEntry *entry;
    unsigned int  collision_count = 0;
    cra_hash_t    hashcode = dict->hash_key(key);
    ssize_t       index, bucket = CRA_DICT_BUCKET(hashcode, dict->capacity);

    for (ssize_t i = dict->buckets[bucket]; i >= 0;)
    {
        entry = CRA_DICT_ENTRY(dict, i);
        if (entry->hashcode == hashcode && dict->compare_key(key, entry->kv) == 0)
        {
            if (add)
                return false;

            if (dict->remove_key)
                dict->remove_key(entry->kv);
            if (dict->remove_val)
                dict->remove_val(entry->kv + dict->key_size);

            memcpy(entry->kv, key, dict->key_size);
            memcpy(entry->kv + dict->key_size, val, dict->val_size);
            return true;
        }
        i = entry->next;
        collision_count++;
    }

    if (collision_count >= CRA_DICT_COLLISION_MAX)
    {
        __cra_dict_resize(dict, dict->capacity + (dict->capacity << 1)); // n * 1.5
    }

    if (dict->freelist != -1)
    {
        index = dict->freelist;
        entry = CRA_DICT_ENTRY(dict, index);
        dict->freelist = entry->next;
    }
    else
    {
        if (dict->next >= CRA_DICT_USABLE_FRACTION(dict->capacity))
        {
            __cra_dict_resize(dict, dict->capacity + (dict->capacity << 1)); // n * 1.5
            bucket = CRA_DICT_BUCKET(hashcode, dict->capacity);
        }
        index = dict->next++;
        entry = CRA_DICT_ENTRY(dict, index);
    }

    entry->hashcode = hashcode;
    entry->next = dict->buckets[bucket];
    dict->buckets[bucket] = index;
    memcpy(entry->kv, key, dict->key_size);
    memcpy(entry->kv + dict->key_size, val, dict->val_size);

    dict->count++;
    return true;
}

bool
cra_dict_put(CraDict *dict, void *key, void *val)
{
    return __cra_dict_put(dict, key, val, false);
}

bool
cra_dict_add(CraDict *dict, void *key, void *val)
{
    return __cra_dict_put(dict, key, val, true);
}

static inline bool
__cra_dict_pop(CraDict *dict, void *key, void *retval)
{
    CraDictEntry *entry;
    cra_hash_t    hashcode = dict->hash_key(key);
    ssize_t       bucket = CRA_DICT_BUCKET(hashcode, dict->capacity);

    for (ssize_t last = -1, i = dict->buckets[bucket]; i >= 0;)
    {
        entry = CRA_DICT_ENTRY(dict, i);
        if (entry->hashcode == hashcode && dict->compare_key(key, entry->kv) == 0)
        {
            if (dict->remove_key)
                dict->remove_key(entry->kv);
            if (retval)
                memcpy(retval, entry->kv + dict->key_size, dict->val_size);
            else if (dict->remove_val)
                dict->remove_val(entry->kv + dict->key_size);

            if (last < 0)
                dict->buckets[bucket] = entry->next;
            else
                CRA_DICT_ENTRY(dict, last)->next = entry->next;

            if (dict->zero_memory)
                bzero(entry->kv, dict->entry_size - sizeof(CraDictEntry)); // zero key-val pair
            entry->hashcode = -1;
            entry->next = dict->freelist;
            dict->freelist = i;

            dict->count--;
            return true;
        }
        last = i;
        i = entry->next;
    }
    return false;
}

bool
cra_dict_pop(CraDict *dict, void *key, void *retval)
{
    return __cra_dict_pop(dict, key, retval);
}

bool
cra_dict_remove(CraDict *dict, void *key)
{
    return __cra_dict_pop(dict, key, NULL);
}

static inline bool
__cra_dict_get_ptr(CraDict *dict, void *key, void **retvalptr)
{
    CraDictEntry *entry;
    cra_hash_t    hashcode = dict->hash_key(key);
    ssize_t       bucket = CRA_DICT_BUCKET(hashcode, dict->capacity);

    for (ssize_t i = dict->buckets[bucket]; i >= 0;)
    {
        entry = CRA_DICT_ENTRY(dict, i);
        if (entry->hashcode == hashcode && dict->compare_key(key, entry->kv) == 0)
        {
            *retvalptr = entry->kv + dict->key_size;
            return true;
        }
        i = entry->next;
    }
    return false;
}

bool
cra_dict_get(CraDict *dict, void *key, void *retval)
{
    void *valptr;
    if (__cra_dict_get_ptr(dict, key, &valptr))
    {
        memcpy(retval, valptr, dict->val_size);
        return true;
    }
    return false;
}

bool
cra_dict_get_ptr(CraDict *dict, void *key, void **retvalptr)
{
    return __cra_dict_get_ptr(dict, key, retvalptr);
}

CraDict *
cra_dict_clone(CraDict *dict, cra_deep_copy_val_fn deep_copy_key, cra_deep_copy_val_fn deep_copy_val)
{
    CraDictIter it;
    CraDict    *ret;
    void       *keyptr1, *keyptr2;
    void       *valptr1, *valptr2;

    ret = cra_alloc(CraDict);
    cra_dict_init_size(ret,
                       dict->key_size,
                       dict->val_size,
                       dict->capacity,
                       dict->zero_memory,
                       dict->hash_key,
                       dict->compare_key,
                       dict->remove_key,
                       dict->remove_val);
    for (cra_dict_iter_init(dict, &it); cra_dict_iter_next(&it, &keyptr1, &valptr1);)
    {
        if (deep_copy_key)
        {
            deep_copy_key(keyptr1, &keyptr2);
            keyptr1 = &keyptr2;
        }
        if (deep_copy_val)
        {
            deep_copy_val(valptr1, &valptr2);
            valptr1 = &valptr2;
        }

        if (!cra_dict_add(ret, keyptr1, valptr1))
            goto fail;
    }
    return ret;

fail:
    cra_dict_uninit(ret);
    cra_free(ret);
    return NULL;
}

// =============

static void
cra_dict_ser_iter_init(void *obj, void *const it, size_t itbufsize)
{
    CRA_UNUSED_VALUE(itbufsize);
    assert(sizeof(CraDictIter) <= itbufsize);
    cra_dict_iter_init((CraDict *)obj, (CraDictIter *)it);
}

static void
cra_dict_ser_init(void *obj, void *args)
{
    assert_always(args != NULL);

    CraDict            *dict = (CraDict *)obj;
    CraDictSerInitArgs *params = (CraDictSerInitArgs *)args;

    cra_dict_init(dict,
                  params->key_size,
                  params->val_size,
                  params->zero_memory,
                  params->hash_key_fn,
                  params->compare_key_fn,
                  params->remove_key_fn,
                  params->remove_val_fn);
}

const CraTypeIter_i g_dict_ser_iter_i = {
    .dict.init = cra_dict_ser_iter_init,
    .dict.next = (bool (*)(void *, void **, void **))cra_dict_iter_next,
    .dict.append = (bool (*)(void *, void *, void *))cra_dict_add,
};

const CraTypeInit_i g_dict_ser_init_i = {
    .alloc = NULL,
    .dealloc = NULL,
    .init = cra_dict_ser_init,
    .uinit = (void (*)(void *))cra_dict_uninit,
};
