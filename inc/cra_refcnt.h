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
    bool isptr;                // 要用引用计数的对象是否是指针（CRA_REFCNT[_NAME]() -> false; CRA_REFCNT_PTR[_NAME]() -> true）
    bool freeptr;              // 计数为0时，是否释放对象占用的内存。对象是堆上的内存时应设为true
    bool freeself;             // 计数为0时，是否释放引用计数占用的内存。引用计数是堆上的内存时应设为true
    bool uninit_param_with_rc; // 为true时，传给`uninit`回调函数的参数是引用计数，引用计数->o/p才能得到对象；为false时，传入的直接是对象
    cra_refcnt_t cnt;          // 计数
    cra_uninit_fn uninit;      // 反初始化回调函数
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

/**
 * @brief init refcnt
 *
 * @param rc refcnt obj
 * @param is_obj_ptr CRA_REFCNT[_NAME]\() -> false; CRA_REFCNT_PTR[_NAME]\() -> true
 * @param free_obj_ptr `true`: on_uninit(){... free(rc->p);...}
 * @param free_rc `true`: on_uninit(){...free(rc);...}
 * @param uninit_param_with_rc_head `true`: on_uninit(){...user_uninit(rc);...}; `flase`: on_uninit(){...user_uniit(rc->p/&rc->o);...}
 * @param uninit user_uninit
 */
CRA_API void cra_refcnt_init(void *rc, bool is_obj_ptr, bool free_obj_ptr, bool free_rc, bool uninit_param_with_rc_head, cra_uninit_fn uninit);

static inline void cra_refcnt_ref(void *rc)
{
    __CraRefcnt *ref = (__CraRefcnt *)rc;
    __CRA_REFCNT_INC(&ref->cnt);
}

CRA_API bool cra_refcnt_unref(void *rc);
static inline void cra_refcnt_unref0(void *rc) { cra_refcnt_unref(rc); }
CRA_API void cra_refcnt_unref_clear(void **prc);

#endif