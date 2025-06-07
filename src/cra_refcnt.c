#include "cra_refcnt.h"
#include "cra_assert.h"
#include "cra_malloc.h"

void cra_refcnt_init(void *rc, bool is_obj_ptr, bool free_obj_ptr, bool free_rc, bool uninit_param_with_rc_head, cra_uninit_fn uninit)
{
    assert_always(rc != NULL);
    __CraRefcnt *ref = (__CraRefcnt *)rc;
    ref->isptr = is_obj_ptr;
    ref->freeptr = free_obj_ptr;
    ref->freeself = free_rc;
    ref->uninit_param_with_rc = uninit_param_with_rc_head;
    ref->cnt = 1;
    ref->uninit = uninit;
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