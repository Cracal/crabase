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

typedef void (*cra_ref_fn)(void *ptr);

typedef cra_atomic_int32_t    cra_refcnt_t;
typedef struct CraRefHead     CraRefHead, CraRef;
typedef struct CraWeakRefHead CraWeakRefHead, CraWeakRef;

struct CraRefHead
{
    cra_refcnt_t cnt;
};

struct CraWeakRefHead
{
    cra_refcnt_t cnt;
    cra_refcnt_t weak_cnt;
};

#if 1 // ref head

// init: reference count = 1
static inline void
cra_ref_head_init(CraRefHead *head)
{
    head->cnt = 1;
}

// reference count +1
static inline void
cra_ref_head_ref(CraRefHead *head)
{
    cra_atomic_inc(&head->cnt, CRA_MO_RELAXED);
}

// reference count -1
static inline bool
cra_ref_head_unref(CraRefHead *head)
{
    if (cra_atomic_dec(&head->cnt, CRA_MO_ACQ_REL) == 1)
        return true;
    return false;
}

#endif // end ref head

#if 1 // weak ref head

// init: strong reference count = 1, weak reference count = 1
static inline void
cra_weak_ref_head_init(CraWeakRefHead *head)
{
    head->cnt = 1;
    head->weak_cnt = 1;
}

// weak reference count +1
static inline void
cra_weak_ref_head_ref(CraWeakRefHead *head)
{
    cra_atomic_inc(&head->weak_cnt, CRA_MO_RELAXED);
}

// weak reference count -1
static inline bool
cra_weak_ref_head_unref(CraWeakRefHead *head)
{
    if (cra_atomic_dec(&head->weak_cnt, CRA_MO_ACQ_REL) == 1)
        return true;
    return false;
}

// weak reference holder tries to acquire a strong reference
static inline bool
cra_weak_ref_head_acquire(CraWeakRefHead *head)
{
    int32_t old = cra_atomic_load(&head->cnt, CRA_MO_RELAXED);

    while (old > 0)
    {
        // try +1
        if (cra_atomic_cas_weak(&head->cnt, &old, old + 1, CRA_MO_ACQUIRE, CRA_MO_RELAXED))
            return true;

        // CAS failed, try again
    }

    // dead
    return false;
}

// weak reference holder releases the strong reference
static inline bool
cra_weak_ref_head_release(CraWeakRefHead *head)
{
    if (cra_atomic_dec(&head->cnt, CRA_MO_ACQ_REL) == 1)
        return true;
    return false;
}

#endif // end weak ref head

#if 1 // ref

// memory layout:
// | ........ |
// | head     |
// | obj(ptr) |
// | ........ |
CRA_API CraRef *
cra_ref_make(size_t size, cra_ref_fn uninit);

// memory layout:
// | ......... |     | ........ |
// | head      |     | ........ |
// | ptr       | --> | obj(ptr) |
// | ......... |     | ........ |
CRA_API CraRef *
cra_ref_take(void *ptr, cra_ref_fn uninit, cra_ref_fn dealloc);

// reference count +1
#define cra_ref_ref cra_ref_head_ref

// reference count -1
// return true and release the object if reference count reaches 0
// return false otherwise
CRA_API bool
cra_ref_unref(CraRef *ref);

// reference count -1
// *ref = NULL after this call
static inline void
cra_ref_unref_clear(CraRef **ref)
{
    cra_ref_unref(*ref);
    *ref = NULL;
}

// get pointer to the object
// you must ensure that the reference count is not 0 before calling this function
CRA_API void *
cra_ref_get_ptr_uncheck(CraRef *ref);

#endif // end ref

#if 1 // weak ref

// memory layout:
// | ........ |
// | head     |
// | obj(ptr) |
// | ........ |
CRA_API CraWeakRef *
cra_weak_ref_make(size_t size, cra_ref_fn uninit);

// memory layout:
// | ......... |     | ........ |
// | head      |     | ........ |
// | ptr       | --> | obj(ptr) |
// | ......... |     | ........ |
CRA_API CraWeakRef *
cra_weak_ref_take(void *ptr, cra_ref_fn uninit, cra_ref_fn dealloc);

// weak reference count +1
#define cra_weak_ref_ref cra_weak_ref_head_ref

// weak reference count -1
// return true and release the object if weak reference count reaches 0
// return false otherwise
CRA_API bool
cra_weak_ref_unref(CraWeakRef *ref);

// weak reference count -1
// *ref = NULL after this call
static inline void
cra_weak_ref_unref_clear(CraWeakRef **ref)
{
    cra_weak_ref_unref(*ref);
    *ref = NULL;
}

// weak reference holder tries to acquire a strong reference
// return the pointer to the object if success(strong reference count +1)
// return NULL otherwise
CRA_API void *
cra_weak_ref_acquire(CraWeakRef *ref);

// weak reference holder releases the strong reference(strong reference count -1)
CRA_API void
cra_weak_ref_release(CraWeakRef *ref);

static inline void
cra_weak_ref_release_clear(CraWeakRef **ref)
{
    cra_weak_ref_release(*ref);
    *ref = NULL;
}

static inline void
cra_weak_ref_release_clear_ptr(CraWeakRef *ref, void **pp)
{
    cra_weak_ref_release(ref);
    *pp = NULL;
}

static inline void
cra_weak_ref_release_clear_both(CraWeakRef **ref, void **pp)
{
    cra_weak_ref_release(*ref);
    *ref = NULL;
    *pp = NULL;
}

// get pointer to the object
// only use this function if you are the strong reference holder of the object
// you must ensure that the strong reference count is not 0 before calling this function
CRA_API void *
cra_weak_ref_get_ptr_uncheck(CraWeakRef *ref);

#endif // end weak ref

#endif