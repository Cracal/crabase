#include "cra_refcnt.h"
#include "cra_assert.h"

void cra_refcnt_init(CraRefcnt *ref, cra_refcnt_uninit_fn uninit)
{
    assert(ref != NULL);
    ref->cnt = 1;
    ref->uninit = uninit;
}

bool cra_refcnt_unref(CraRefcnt *ref)
{
    assert(ref != NULL);
    if (__CRA_REFCNT_DEC(&ref->cnt) == 1)
    {
        if (ref->uninit)
            ref->uninit(ref);
        return true;
    }
    return false;
}

void cra_refcnt_unref_clear(CraRefcnt **refptr)
{
    assert(refptr != NULL);
    cra_refcnt_unref(*refptr);
    *refptr = NULL;
}