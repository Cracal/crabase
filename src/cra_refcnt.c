#include "cra_refcnt.h"
#include "cra_assert.h"

void
cra_refcnt_init(CraRefcnt *ref, cra_refcnt_release_fn func)
{
    assert(ref != NULL);
    ref->cnt = 1;
    ref->release = func;
}

bool
cra_refcnt_unref(CraRefcnt *ref)
{
    assert(ref != NULL);
    if (__CRA_REFCNT_DEC(&ref->cnt) == 1)
    {
        if (ref->release)
            ref->release(ref);
        return true;
    }
    return false;
}
