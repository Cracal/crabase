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
#include "cra_malloc.h"

#define CRA_DICT_USABLE_FRACTION(n) (((n) * 72) / 100) // n * 0.72

#define CRA_DICT_ALIGN_UP(n, a) (((n) + ((a) - 1)) & ~((a) - 1))

#define CRA_DICT_PENTRY0(_dict, _entries, _index)                                    \
    ((CraDictEntry *)((unsigned char *)(_entries) + (_index) * (_dict)->entry_size))
#define CRA_DICT_PENTRY(_dict, _index) CRA_DICT_PENTRY0(_dict, (_dict)->entries, _index)
#define CRA_DICT_PKEY(_dict, _entry)   ((void *)((unsigned char *)(_entry) + (_dict)->key_offset))
#define CRA_DICT_PVAL(_dict, _entry)   ((void *)((unsigned char *)(_entry) + (_dict)->val_offset))

#define CRA_DICT_BUCKET(_hash, _capacity) (((_hash) & CRA_HASH_MAX) % (_capacity))

struct CraDictEntry
{
    cra_hash_t    hash;
    ssize_t       next;
    unsigned char kv[];
};

static inline ssize_t
cra_dict_next_prime(ssize_t n)
{
    static ssize_t prime_table[] = {
        // 5,
        11,
        23,
        53,
        97,
        193,
        389,
        769,
        1543,
        3079,
        6151,
        12289,
        24593,
        49157,
        98317,
        196613,
        393241,
        786433,
        1572869,
        3145739,
        6291469,
        12582917,
        25165843,
        50331653,
        100663319,
        201326611,
        402653189,
        805306457,
        1610612741,
        3221225473,
        4294967291,
        8589934583,
        17179869143,
        34359738337,
        68719476731,
        137438953447,
        274877906899,
        549755813881,
        1099511627689,
        2199023255531,
        4398046511093,
        8796093022151,
        17592186044399,
        35184372088777,
        70368744177643,
        140737488355213,
        281474976710597,
        562949953421231,
        1125899906842597,
        2251799813685119,
        4503599627370449,
        9007199254740881,
        18014398509481951,
        36028797018963913,
        72057594037927931,
        144115188075855859,
        288230376151711717,
        576460752303423433,
        1152921504606846883,
        2305843009213693951,
    };

    size_t mid;
    size_t l = 0;
    size_t r = CRA_NARRAY(prime_table);

    while (l < r)
    {
        mid = l + ((r - l) >> 1);
        if (prime_table[mid] >= n)
            r = mid;
        else
            l = mid + 1;
    }
    return prime_table[l];
}

bool(cra_dict_init_with_size)(CraDict    *dict,
                              size_t      key_size,
                              size_t      val_size,
                              size_t      key_align,
                              size_t      val_align,
                              size_t      init_capacity,
                              cra_hash_fn hash_key,
                              cra_cmp_fn  compare_key)
{
    size_t offset;
    size_t buckets_size;
    size_t entries_size;

    assert(dict);
    assert(key_size > 0);
    assert(val_size > 0);
    assert(key_align > 0);
    assert(val_align > 0);
    assert(key_size % key_align == 0);
    assert(val_size % val_align == 0);
    assert(compare_key);
    assert(hash_key);

    offset = sizeof(CraDictEntry);

    offset = CRA_DICT_ALIGN_UP(offset, key_align);
    dict->key_offset = offset;
    offset += key_size;

    offset = CRA_DICT_ALIGN_UP(offset, val_align);
    dict->val_offset = offset;
    offset += val_size;

    dict->entry_size = CRA_DICT_ALIGN_UP(offset, alignof(CraDictEntry));
    dict->key_size = key_size;
    dict->val_size = val_size;

    dict->hash_key = hash_key;
    dict->compare_key = compare_key;

    dict->next = 0;
    dict->count = 0;
    dict->freelist = -1;
    dict->capacity = cra_dict_next_prime(init_capacity);

    buckets_size = dict->capacity * sizeof(ssize_t);
    entries_size = CRA_DICT_USABLE_FRACTION(dict->capacity) * dict->entry_size;

    dict->buckets = cra_malloc(buckets_size);
    if (!dict->buckets)
        return false;

    dict->entries = cra_malloc(entries_size);
    if (!dict->entries)
    {
        cra_free(dict->buckets);
        return false;
    }

    memset(dict->buckets, 0xff, buckets_size);

    return true;
}

void
cra_dict_uninit(CraDict *dict)
{
    assert(dict);

    cra_free(dict->buckets);
    cra_free(dict->entries);
    bzero(dict, sizeof(*dict));
}

bool
cra_dict_reserve(CraDict *dict, ssize_t new_capacity)
{
    ssize_t       new_bucket;
    ssize_t      *new_buckets;
    CraDictEntry *new_entries;
    CraDictEntry *entry1, *entry2;
    size_t        new_buckets_size;
    size_t        new_entries_size;

    assert(dict);
    assert(dict->buckets);
    assert(dict->entries);
    assert(dict->entry_size > 0 && dict->entry_size % 2 == 0);

    if (new_capacity < dict->next)
        new_capacity = dict->next;
    new_capacity = cra_dict_next_prime(new_capacity);
    new_buckets_size = new_capacity * sizeof(ssize_t);
    new_entries_size = CRA_DICT_USABLE_FRACTION(new_capacity) * dict->entry_size;

    new_buckets = cra_malloc(new_buckets_size);
    if (!new_buckets)
        return false;
    new_entries = cra_malloc(new_entries_size);
    if (!new_entries)
    {
        cra_free(new_entries);
        return false;
    }

    memset(new_buckets, 0xff, new_buckets_size);

    for (ssize_t i = 0, j = 0; i < dict->next; ++i)
    {
        entry1 = CRA_DICT_PENTRY(dict, i);
        if (entry1->hash != -1)
        {
            entry2 = CRA_DICT_PENTRY0(dict, new_entries, j);
            memcpy(entry2, entry1, dict->entry_size);

            new_bucket = CRA_DICT_BUCKET(entry1->hash, new_capacity);
            entry2->next = new_buckets[new_bucket];
            new_buckets[new_bucket] = j;

            ++j;
        }
    }

    cra_free(dict->buckets);
    cra_free(dict->entries);
    dict->buckets = new_buckets;
    dict->entries = new_entries;
    dict->capacity = new_capacity;
    return true;
}

static inline CraDictEntry *
cra_dict_find_entry(CraDict *dict, const void *key, cra_hash_t *phash, ssize_t *pbucket, ssize_t *plast)
{
    CraDictEntry *entry;
    cra_hash_t    hash;
    ssize_t       bucket;

    hash = *phash = dict->hash_key(key);
    bucket = *pbucket = CRA_DICT_BUCKET(hash, dict->capacity);

    assert(hash != -1); // hash must be valid

    if (plast)
        *plast = -1;

    for (ssize_t i = dict->buckets[bucket]; i >= 0;)
    {
        entry = CRA_DICT_PENTRY(dict, i);
        if (entry->hash == hash && dict->compare_key(key, CRA_DICT_PKEY(dict, entry)) == 0)
            return entry;
        if (plast)
            *plast = i;
        i = entry->next;
    }

    return NULL;
}

static inline bool
cra_dict_add_inner(CraDict *dict, cra_hash_t hash, ssize_t bucket, void *key, void *val)
{
    ssize_t       index;
    CraDictEntry *entry;

    if (dict->freelist >= 0)
    {
        index = dict->freelist;
        entry = CRA_DICT_PENTRY(dict, index);
        dict->freelist = entry->next;
    }
    else
    {
        if (dict->next >= CRA_DICT_USABLE_FRACTION(dict->capacity))
        {
            if (!cra_dict_reserve(dict, dict->capacity << 1))
                return false;
            bucket = CRA_DICT_BUCKET(hash, dict->capacity);
        }
        index = dict->next++;
        entry = CRA_DICT_PENTRY(dict, index);
    }

    entry->hash = hash;
    entry->next = dict->buckets[bucket];
    dict->buckets[bucket] = index;
    memcpy(CRA_DICT_PKEY(dict, entry), key, dict->key_size);
    memcpy(CRA_DICT_PVAL(dict, entry), val, dict->val_size);

    ++dict->count;

    return true;
}

bool(cra_dict_put_and_return_kv)(CraDict *dict, void *key, void *val, void *retoldkey, void *retoldval)
{
    cra_hash_t    hash;
    CraDictEntry *entry;
    ssize_t       bucket;

    assert(key);
    assert(val);
    assert(dict);
    assert(dict->buckets);
    assert(dict->entries);

    entry = cra_dict_find_entry(dict, key, &hash, &bucket, NULL);
    if (entry)
    {
        if (retoldkey)
            memcpy(retoldkey, CRA_DICT_PKEY(dict, entry), dict->key_size);
        if (retoldval)
            memcpy(retoldval, CRA_DICT_PVAL(dict, entry), dict->val_size);

        memcpy(CRA_DICT_PKEY(dict, entry), key, dict->key_size);
        memcpy(CRA_DICT_PVAL(dict, entry), val, dict->val_size);

        return true;
    }

    return cra_dict_add_inner(dict, hash, bucket, key, val);
}

bool(cra_dict_put)(CraDict *dict, void *key, void *val)
{
    cra_hash_t    hash;
    CraDictEntry *entry;
    ssize_t       bucket;

    assert(key);
    assert(val);
    assert(dict);
    assert(dict->buckets);
    assert(dict->entries);

    entry = cra_dict_find_entry(dict, key, &hash, &bucket, NULL);
    if (entry)
    {
        memcpy(CRA_DICT_PKEY(dict, entry), key, dict->key_size);
        memcpy(CRA_DICT_PVAL(dict, entry), val, dict->val_size);
        return true;
    }
    return cra_dict_add_inner(dict, hash, bucket, key, val);
}

bool(cra_dict_add)(CraDict *dict, void *key, void *val)
{
    cra_hash_t    hash;
    CraDictEntry *entry;
    ssize_t       bucket;

    assert(key);
    assert(val);
    assert(dict);
    assert(dict->buckets);
    assert(dict->entries);

    entry = cra_dict_find_entry(dict, key, &hash, &bucket, NULL);
    if (entry)
        return false;
    return cra_dict_add_inner(dict, hash, bucket, key, val);
}

bool(cra_dict_pop_kv)(CraDict *dict, const void *key, void *retkey, void *retval)
{
    cra_hash_t    hash;
    CraDictEntry *entry;
    ssize_t       bucket, last, index;

    assert(key);
    assert(dict);
    assert(dict->buckets);
    assert(dict->entries);

    entry = cra_dict_find_entry(dict, key, &hash, &bucket, &last);
    if (entry)
    {
        if (retkey)
            memcpy(retkey, CRA_DICT_PKEY(dict, entry), dict->key_size);
        if (retval)
            memcpy(retval, CRA_DICT_PVAL(dict, entry), dict->val_size);

        index = dict->buckets[bucket];

        if (last >= 0)
            CRA_DICT_PENTRY(dict, last)->next = entry->next;
        else
            dict->buckets[bucket] = entry->next;

        entry->hash = -1;
        entry->next = dict->freelist;
        dict->freelist = index;

        --dict->count;
        return true;
    }
    return false;
}

void *(cra_dict_get_ref)(CraDict * dict, const void *key)
{
    cra_hash_t    hash;
    CraDictEntry *entry;
    ssize_t       bucket;

    assert(key);
    assert(dict);
    assert(dict->buckets);
    assert(dict->entries);

    entry = cra_dict_find_entry(dict, key, &hash, &bucket, NULL);
    if (entry)
        return CRA_DICT_PVAL(dict, entry);
    return NULL;
}

// ====================================== interfaces ======================================

// initializable

static CRA_INITIALIZABLE_INIT_FN(cra_dict_initializable_init)
{
    CraDict                   *dict = (CraDict *)obj;
    CraDictInitializableParam *param = (CraDictInitializableParam *)params;

    assert(dict);
    assert(param);

    return (cra_dict_init_with_size)(dict,
                                     param->key_size,
                                     param->val_size,
                                     param->key_align,
                                     param->val_align,
                                     length,
                                     param->hash_key,
                                     param->compare_key);
}

CRA_INITIALIZABLE_DEF(cra_g_dict_initializable_i) = {
    .init = cra_dict_initializable_init,
    .uninit = (CRA_INITIALIZABLE_UNINIT_FN((*)))cra_dict_uninit,
};

// appendable

static CRA_APPENDABLE_APPEND_FN(cra_dict_appendable_append)
{
    assert(obj);
    assert(vals);
    assert(vals->val1_ref);
    assert(vals->val2_ref);

    CraDict *dict = (CraDict *)obj;
    return (cra_dict_add)(dict, vals->val1_ref, vals->val2_ref);
}

CRA_APPENDABLE_DEF(cra_g_dict_appendable_i) = {
    .append = cra_dict_appendable_append,
};

// iterable

static CRA_ITERABLE_INIT_FN(cra_dict_iterable_init)
{
    CraDict *dict = (CraDict *)obj;

    assert(it);
    assert(dict);
    assert(dict->buckets);
    assert(dict->entries);

    if (retcnt)
        *retcnt = (size_t)dict->count;

    it->obj = obj;
    it->ic1.idx = reverse ? dict->next - 1 : 0;

    return dict->count > 0;
}

static CRA_ITERABLE_NEXT_FN(cra_dict_iterable_next)
{
    CraDict      *dict;
    CraDictEntry *entry;

    assert(it);
    assert(vals);
    assert(it->obj);

    dict = (CraDict *)it->obj;

    while ((ssize_t)it->ic1.idx < dict->next)
    {
        entry = CRA_DICT_PENTRY(dict, it->ic1.idx);
        ++it->ic1.idx;

        if (entry->hash != -1)
        {
            vals->val1_ref = CRA_DICT_PKEY(dict, entry);
            vals->val2_ref = CRA_DICT_PVAL(dict, entry);
            return true;
        }
    }
    return false;
}

static CRA_ITERABLE_PREV_FN(cra_dict_iterable_prev)
{
    CraDict      *dict;
    CraDictEntry *entry;

    assert(it);
    assert(vals);
    assert(it->obj);

    dict = (CraDict *)it->obj;

    while ((ssize_t)it->ic1.idx >= 0)
    {
        entry = CRA_DICT_PENTRY(dict, it->ic1.idx);
        --it->ic1.idx;

        if (entry->hash != -1)
        {
            vals->val1_ref = CRA_DICT_PKEY(dict, entry);
            vals->val2_ref = CRA_DICT_PVAL(dict, entry);
            return true;
        }
    }
    return false;
}

CRA_ITERABLE_DEF(cra_g_dict_iterable_i) = {
    .init = cra_dict_iterable_init,
    .next = cra_dict_iterable_next,
    .prev = cra_dict_iterable_prev,
};
