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
    bool freeptr;         // 计数为0时，是否释放对象占用的内存。对象是堆上的内存时应设为true。（CRA_REFCNT[_NAME]() -> false; CRA_REFCNT_PTR[_NAME]() -> true）
    bool freeself;        // 计数为0时，是否释放引用计数占用的内存。引用计数是堆上的内存时应设为true
    cra_refcnt_t cnt;     // 计数
    cra_uninit_fn uninit; // 反初始化回调函数
} CraRefcnt;

#define CRA_REFCNT_NAME_DEF(_Type, _name) \
    struct _name                          \
    {                                     \
        CraRefcnt rc;                     \
        _Type o;                          \
    }

#define CRA_REFCNT_PTR_NAME_DEF(_Type, _name) \
    struct _name                              \
    {                                         \
        CraRefcnt rc;                         \
        _Type *p;                             \
    }

#define CRA_REFCNT_DEF(_Type) \
    CRA_REFCNT_NAME_DEF(_Type, )
#define CRA_REFCNT_PTR_DEF(_Type) \
    CRA_REFCNT_PTR_NAME_DEF(_Type, )

#define CRA_REFCNT_RC(_rc) (&(_rc)->rc)
#define CRA_REFCNT_OBJ(_rc) (&(_rc)->o)
#define CRA_REFCNT_PTR(_rc) ((_rc)->p)

CRA_API void cra_refcnt_init(void *rc, bool free_rc, bool free_obj, cra_uninit_fn uninit);

static inline void cra_refcnt_ref(void *rc)
{
    CraRefcnt *ref = (CraRefcnt *)rc;
    __CRA_REFCNT_INC(&ref->cnt);
}

CRA_API bool cra_refcnt_unref(void *rc);
static inline void cra_refcnt_unref0(void *rc) { cra_refcnt_unref(rc); }
CRA_API void cra_refcnt_unref_clear(void **prc);

#endif