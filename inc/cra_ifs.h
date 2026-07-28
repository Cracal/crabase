/**
 * @file cra_ifs.h
 * @author Cracal
 * @brief interfaces defination
 * @version 0.1
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __CRA_IFS_H__
#define __CRA_IFS_H__
#include <stdint.h>
#include <stdbool.h>

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

#define CRA_FOREACH(iterable_i, obj, val_name)                                                          \
    for (CraIterator val_name##_it = { 0 };                                                             \
         val_name##_it.ic1.idx == 0 && cra_iterable_init(iterable_i, obj, &val_name##_it, NULL, false); \
         val_name##_it.ic1.idx = 1)                                                                     \
        for (CraPair val_name = { 0 }; cra_iterable_next(iterable_i, &val_name##_it, &val_name);)

#define CRA_FOREACH_REVERSE(iterable_i, obj, val_name)                                                 \
    for (CraIterator val_name##_it = { 0 };                                                            \
         val_name##_it.ic1.idx == 0 && cra_iterable_init(iterable_i, obj, &val_name##_it, NULL, true); \
         val_name##_it.ic1.idx = 1)                                                                    \
        for (CraPair val_name = { 0 }; cra_iterable_prev(iterable_i, &val_name##_it, &val_name);)

#endif