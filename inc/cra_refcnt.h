/**
 * @file cra_refcnt.h
 * @author Cracal
 * @brief 引用计数
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_REFCNT_H__
#define __CRA_REFCNT_H__
#include "cra_atomic.h"

typedef cra_atomic_int64_t cra_refcnt_t;

typedef struct CraRefcnt CraRefcnt;

typedef void (*cra_refcnt_release_fn)(CraRefcnt *ref);

struct CraRefcnt
{
    cra_refcnt_t          cnt;     // 计数
    cra_refcnt_release_fn release; // 回调函数
};

#define CRA_REFCNT_NAME_DEF(_Type, _name) \
    struct _name                          \
    {                                     \
        CraRefcnt rc;                     \
        _Type     o;                      \
    }

#define CRA_REFCNT_PTR_NAME_DEF(_Type, _name) \
    struct _name                              \
    {                                         \
        CraRefcnt rc;                         \
        _Type    *p;                          \
    }

#define CRA_REFCNT_DEF(_Type)     CRA_REFCNT_NAME_DEF(_Type, )
#define CRA_REFCNT_PTR_DEF(_Type) CRA_REFCNT_PTR_NAME_DEF(_Type, )

#define CRA_REFCNT_RC(_rc)  (&(_rc)->rc)
#define CRA_REFCNT_OBJ(_rc) (&(_rc)->o)
#define CRA_REFCNT_PTR(_rc) ((_rc)->p)

static inline void
cra_refcnt_init(CraRefcnt *ref, cra_refcnt_release_fn func)
{
    ref->cnt = 1;
    ref->release = func;
}

static inline void
cra_refcnt_ref(CraRefcnt *ref)
{
    cra_atomic_inc(&ref->cnt, CRA_MO_RELAXED);
}

static inline bool
cra_refcnt_unref(CraRefcnt *ref)
{
    if (cra_atomic_dec(&ref->cnt, CRA_MO_ACQ_REL) == 1)
    {
        if (ref->release)
            ref->release(ref);
        return true;
    }
    return false;
}

static inline void
cra_refcnt_unref0(CraRefcnt *ref)
{
    cra_refcnt_unref(ref);
}

#endif