#include "cra_refcnt.h"
#include "cra_assert.h"
#include "cra_malloc.h"

void cra_refcnt_init(void *rc, bool free_rc, bool free_obj, cra_uninit_fn uninit)
{
    assert_always(rc != NULL);
    __CraRefcnt *ref = (__CraRefcnt *)rc;
    ref->freeptr = free_obj;
    ref->freeself = free_rc;
    ref->cnt = 1;
    ref->uninit = uninit;
}

bool cra_refcnt_unref(void *rc)
{
    assert_always(rc != NULL);
    __CraRefcnt *ref = (__CraRefcnt *)rc;
    if (__CRA_REFCNT_DEC(&ref->cnt) == 1)
    {
        void *ptr = ref->freeptr ? ((CRA_REFCNT_PTR(void) *)rc)->p : (void *)&((CRA_REFCNT(int) *)rc)->o;
        if (ref->uninit)
            ref->uninit(ptr);
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