#include "cra_refcnt.h"
#include "cra_assert.h"
#include "cra_malloc.h"

#if 1 // ref

typedef struct CraRef_head
{
    CraRef ref;
    bool   is_make;
} CraRef_head;

typedef struct CraRefMake
{
    CraRef_head   head;
    cra_ref_fn    uninit;
    unsigned char ptr[];
} CraRefMake;

typedef struct CraRefTake
{
    CraRef_head head;
    cra_ref_fn  uninit;
    cra_ref_fn  dealloc;
    void       *ptr;
} CraRefTake;

CraRef *
cra_ref_make(size_t size, cra_ref_fn uninit)
{
    CraRefMake *ref;

    assert(size > 0);

    if (!(ref = cra_malloc(sizeof(CraRefMake) + size)))
        return NULL;

    cra_ref_head_init(&ref->head.ref);
    ref->head.is_make = true;
    ref->uninit = uninit;

    return &ref->head.ref;
}

CraRef *
cra_ref_take(void *ptr, cra_ref_fn uninit, cra_ref_fn dealloc)
{
    CraRefTake *ref;

    assert(ptr);

    if (!(ref = cra_malloc(sizeof(CraRefTake))))
        return NULL;

    cra_ref_head_init(&ref->head.ref);
    ref->head.is_make = false;
    ref->dealloc = dealloc;
    ref->uninit = uninit;
    ref->ptr = ptr;

    return &ref->head.ref;
}

bool
cra_ref_unref(CraRef *ref)
{
    assert(ref);
    assert(ref->cnt > 0);

    if (cra_ref_head_unref(ref))
    {
        if (((CraRef_head *)ref)->is_make)
        {
            CraRefMake *make = (CraRefMake *)ref;
            if (make->uninit)
                make->uninit(make->ptr);
        }
        else
        {
            CraRefTake *take = (CraRefTake *)ref;
            if (take->uninit)
                take->uninit(take->ptr);
            if (take->dealloc)
                take->dealloc(take->ptr);
        }
        cra_free(ref);
        return true;
    }
    return false;
}

void *
cra_ref_get_ptr_uncheck(CraRef *ref)
{
    assert(ref);
    assert(ref->cnt > 0);

    if (((CraRef_head *)ref)->is_make)
        return ((CraRefMake *)ref)->ptr;
    return ((CraRefTake *)ref)->ptr;
}

#endif // end ref

#if 1 // weak ref

typedef struct CraWeakRef_head
{
    CraWeakRef ref;
    bool       is_make;
} CraWeakRef_head;

typedef struct CraWeakRefMake
{
    CraWeakRef_head head;
    cra_ref_fn      uninit;
    unsigned char   ptr[];
} CraWeakRefMake;

typedef struct CraWeakRefTake
{
    CraWeakRef_head head;
    cra_ref_fn      uninit;
    cra_ref_fn      dealloc;
    void           *ptr;
} CraWeakRefTake;

CraWeakRef *
cra_weak_ref_make(size_t size, cra_ref_fn uninit)
{
    CraWeakRefMake *ref;

    assert(size > 0);

    if (!(ref = cra_malloc(sizeof(CraWeakRefMake) + size)))
        return NULL;

    cra_weak_ref_head_init(&ref->head.ref);
    ref->head.is_make = true;
    ref->uninit = uninit;

    return &ref->head.ref;
}

CraWeakRef *
cra_weak_ref_take(void *ptr, cra_ref_fn uninit, cra_ref_fn dealloc)
{
    CraWeakRefTake *ref;

    assert(ptr);

    if (!(ref = cra_malloc(sizeof(CraWeakRefTake))))
        return NULL;

    cra_weak_ref_head_init(&ref->head.ref);
    ref->head.is_make = false;
    ref->dealloc = dealloc;
    ref->uninit = uninit;
    ref->ptr = ptr;

    return &ref->head.ref;
}

bool
cra_weak_ref_unref(CraWeakRef *ref)
{
    assert(ref);

    if (cra_weak_ref_head_unref(ref))
    {
        assert(ref->cnt <= 0);
        cra_free(ref);
        return true;
    }
    return false;
}

void *
cra_weak_ref_acquire(CraWeakRef *ref)
{
    assert(ref);

    if (cra_weak_ref_head_acquire(ref))
    {
        if (((CraWeakRef_head *)ref)->is_make)
            return ((CraWeakRefMake *)ref)->ptr;
        return ((CraWeakRefTake *)ref)->ptr;
    }
    return NULL;
}

void
cra_weak_ref_release(CraWeakRef *ref)
{
    assert(ref);

    if (cra_weak_ref_head_release(ref))
    {
        if (((CraWeakRef_head *)ref)->is_make)
        {
            CraWeakRefMake *make = (CraWeakRefMake *)ref;
            if (make->uninit)
                make->uninit(make->ptr);
        }
        else
        {
            CraWeakRefTake *take = (CraWeakRefTake *)ref;
            if (take->uninit)
                take->uninit(take->ptr);
            if (take->dealloc)
                take->dealloc(take->ptr);
        }

        assert(ref->weak_cnt > 0);
        cra_weak_ref_unref(ref);
    }
}

void *
cra_weak_ref_get_ptr_uncheck(CraWeakRef *ref)
{
    assert(ref);
    assert(ref->cnt > 0);

    if (((CraWeakRef_head *)ref)->is_make)
        return ((CraWeakRefMake *)ref)->ptr;
    return ((CraWeakRefTake *)ref)->ptr;
}

#endif // end weak ref
