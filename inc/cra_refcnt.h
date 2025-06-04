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

typedef cra_atomic_int32_t cra_refcnt_t;
#define __CRA_REFCNT_INC cra_atomic_inc32
#define __CRA_REFCNT_DEC cra_atomic_dec32

typedef void (*cra_uninit_fn)(void *);

typedef struct
{
    bool isptr;
    bool freeptr;
    bool freeself;
    bool uninit_param_with_rc;
    cra_refcnt_t cnt;
    cra_uninit_fn uninit;
} __CraRefcnt;

#define CRA_REFCNT_NAME(_Type, _name) \
    struct _name                      \
    {                                 \
        __CraRefcnt rc;               \
        _Type o;                      \
    }

#define CRA_REFCNT_PTR_NAME(_Type, _name) \
    struct _name                          \
    {                                     \
        __CraRefcnt rc;                   \
        _Type *p;                         \
    }

#define CRA_REFCNT(_Type) \
    CRA_REFCNT_NAME(_Type, )
#define CRA_REFCNT_PTR(_Type) \
    CRA_REFCNT_PTR_NAME(_Type, )

CRA_API void cra_refcnt_init(void *rc, bool freeself, bool uninit_param_with_rc_head, cra_uninit_fn uninit);
CRA_API void cra_refcnt_init_ptr(void *rc, void *ptr, bool freeptr, bool freeself, bool uninit_param_with_rc_head, cra_uninit_fn uninit);

static inline void cra_refcnt_ref(void *rc)
{
    __CraRefcnt *ref = (__CraRefcnt *)rc;
    __CRA_REFCNT_INC(&ref->cnt);
}

CRA_API bool cra_refcnt_unref(void *rc);
static inline void cra_refcnt_unref0(void *rc) { cra_refcnt_unref(rc); }
CRA_API void cra_refcnt_unref_clear(void **prc);

#endif