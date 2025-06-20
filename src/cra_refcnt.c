#include "cra_refcnt.h"
#include "cra_assert.h"

void cra_refcnt_init(void *rc, cra_refcnt_delete_fn on_delete)
{
    assert(rc != NULL);
    assert_always(on_delete != NULL);
    CraRefcnt *ref = (CraRefcnt *)rc;
    ref->cnt = 1;
    ref->on_delete = on_delete;
}

bool cra_refcnt_unref(void *rc)
{
    assert(rc != NULL);
    CraRefcnt *ref = (CraRefcnt *)rc;
    if (__CRA_REFCNT_DEC(&ref->cnt) == 1)
    {
        ref->on_delete(rc);
        return true;
    }
    return false;
}

void cra_refcnt_unref_clear(void **prc)
{
    assert(prc != NULL);
    cra_refcnt_unref(*prc);
    *prc = NULL;
}