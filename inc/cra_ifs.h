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

// ========================== initializable ==========================

#define CRA_INITIALIZABLE_INIT_FN(_name)   bool _name(void *obj, size_t length, void *params)
#define CRA_INITIALIZABLE_UNINIT_FN(_name) void _name(void *obj)
#define CRA_INITIALIZABLE_DEF(_name)       const CraInitializable_i _name

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

#define CRA_APPENDABLE_APPEND_FN(_name) bool _name(void *obj, CraTwoVals *vals)
#define CRA_APPENDABLE_DEF(_name)       const CraAppendable_i _name

typedef struct CraTwoVals
{
    void *val1_ref;
    void *val2_ref;
} CraTwoVals;

typedef struct CraAppendable_i
{
    CRA_APPENDABLE_APPEND_FN((*append));
} CraAppendable_i;

static inline bool
cra_appendable_append(const CraAppendable_i *i, void *obj, CraTwoVals *vals)
{
    return i->append(obj, vals);
}

// ========================== iterable ==========================

#define CRA_ITERABLE_INIT_FN(_name) bool _name(void *obj, CraIterator *it, size_t *retcnt, bool reverse)
#define CRA_ITERABLE_NEXT_FN(_name) bool _name(CraIterator *it, CraTwoVals *vals)
#define CRA_ITERABLE_PREV_FN(_name) bool _name(CraIterator *it, CraTwoVals *vals)
#define CRA_ITERABLE_DEF(_name)     const CraIterable_i _name

union CraIteratorVal_u
{
    size_t idx;
    void  *cur;
};

typedef struct CraIterator
{
    void                  *obj;
    union CraIteratorVal_u ic1;
    union CraIteratorVal_u ic2;
} CraIterator;

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
cra_iterable_next(const CraIterable_i *i, CraIterator *it, CraTwoVals *vals)
{
    return i->next(it, vals);
}

static inline bool
cra_iterable_prev(const CraIterable_i *i, CraIterator *it, CraTwoVals *vals)
{
    return i->prev(it, vals);
}

#define CRA_FOREACH(_iterable_i, _obj, _val_name)                                                           \
    for (CraIterator _val_name##_it = { 0 };                                                                \
         _val_name##_it.ic1.idx == 0 && cra_iterable_init(_iterable_i, _obj, &_val_name##_it, NULL, false); \
         _val_name##_it.ic1.idx = 1)                                                                        \
        for (CraTwoVals _val_name = { 0 }; cra_iterable_next(_iterable_i, &_val_name##_it, &_val_name);)

#define CRA_FOREACH_REVERSE(_iterable_i, _obj, _val_name)                                                  \
    for (CraIterator _val_name##_it = { 0 };                                                               \
         _val_name##_it.ic1.idx == 0 && cra_iterable_init(_iterable_i, _obj, &_val_name##_it, NULL, true); \
         _val_name##_it.ic1.idx = 1)                                                                       \
        for (CraTwoVals _val_name = { 0 }; cra_iterable_prev(_iterable_i, &_val_name##_it, &_val_name);)

#endif