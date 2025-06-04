#include "cra_refcnt.h"
#include "cra_assert.h"
#include "cra_malloc.h"

static inline void _cra_refcnt_init(void *rc, void *ptr, bool freeptr, bool freeself, bool uninit_param_with_rc_head, cra_uninit_fn uninit)
{
    assert_always(rc != NULL);
    assert_always(uninit != NULL);
    __CraRefcnt *ref = (__CraRefcnt *)rc;
    ref->isptr = !!ptr;
    ref->freeptr = freeptr;
    ref->freeself = freeself;
    ref->uninit_param_with_rc = uninit_param_with_rc_head;
    ref->cnt = 1;
    ref->uninit = uninit;
    if (!!ptr)
        ((CRA_REFCNT_PTR(void *) *)rc)->p = ptr;
}

void cra_refcnt_init(void *rc, bool freeself, bool uninit_param_with_rc_head, cra_uninit_fn uninit)
{
    _cra_refcnt_init(rc, NULL, false, freeself, uninit_param_with_rc_head, uninit);
}

void cra_refcnt_init_ptr(void *rc, void *ptr, bool freeptr, bool freeself, bool uninit_param_with_rc_head, cra_uninit_fn uninit)
{
    assert_always(ptr != NULL);
    _cra_refcnt_init(rc, ptr, freeptr, freeself, uninit_param_with_rc_head, uninit);
}

bool cra_refcnt_unref(void *rc)
{
    assert_always(rc != NULL);
    __CraRefcnt *ref = (__CraRefcnt *)rc;
    if (__CRA_REFCNT_DEC(&ref->cnt) == 1)
    {
        void *ptr = ref->isptr ? ((CRA_REFCNT_PTR(void) *)rc)->p : (void *)&((CRA_REFCNT(int) *)rc)->o;
        if (ref->uninit)
        {
            if (ref->uninit_param_with_rc)
                ref->uninit(ref);
            else
                ref->uninit(ptr);
        }
        if (ref->freeptr)
            cra_free(ptr);
        if (ref->freeself)
            cra_free(ref);
        return true;
    }
    return false;
}

void cra_refcnt_unref_clear(void **prc)
{
    assert_always(prc != NULL);
    cra_refcnt_unref(*prc);
    *prc = NULL;
}