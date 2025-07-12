#include "threads/cra_thread.h"
#include "cra_refcnt.h"
#include "cra_malloc.h"
#include "cra_assert.h"
#include "cra_log.h"

#undef CRA_LOG_NAME
#define CRA_LOG_NAME "TestRefcnt"

typedef struct
{
    int i;
    float f;
} Stru;
typedef CRA_REFCNT_DEF(Stru) Stru_rc;
typedef CRA_REFCNT_PTR_DEF(Stru) Stru_rc_p;

static void test_int100(void *rc)
{
    CRA_REFCNT_DEF(int) *ref = rc;
    assert_always(*CRA_REFCNT_OBJ(ref) == 100);
    cra_log_info("test int: %d", *CRA_REFCNT_OBJ(ref));
}

static void test_and_delete_int100_p(void *rc)
{
    CRA_REFCNT_PTR_DEF(int) *ref = rc;
    assert_always(*CRA_REFCNT_PTR(ref) == 100);
    cra_log_info("delete int: %d", *CRA_REFCNT_PTR(ref));
    cra_dealloc(CRA_REFCNT_PTR(ref));
}

static void delete_stru(Stru_rc *s)
{
    cra_log_info("delete Stru{i: %d, f: %f}", CRA_REFCNT_OBJ(s)->i, CRA_REFCNT_OBJ(s)->f);
    cra_dealloc(s);
}

static void delete_stru_p(Stru_rc_p *s)
{
    cra_log_info("delete Stru{i: %d, f: %f}", CRA_REFCNT_PTR(s)->i, CRA_REFCNT_PTR(s)->f);
    cra_dealloc(CRA_REFCNT_PTR(s));
}

static void delete_stru_p2(Stru_rc_p *s)
{
    delete_stru_p(s);
    cra_dealloc(s);
}

void test_refcnt(void)
{
    CRA_REFCNT_DEF(int)
    ri;
    *CRA_REFCNT_OBJ(&ri) = 100;
    cra_refcnt_init(CRA_REFCNT_RC(&ri), (cra_refcnt_uninit_fn)test_int100);
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 1);
    assert_always(cra_refcnt_unref(CRA_REFCNT_RC(&ri)));
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 0);

    cra_refcnt_init(CRA_REFCNT_RC(&ri), (cra_refcnt_uninit_fn)test_int100);
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 1);
    cra_refcnt_ref(CRA_REFCNT_RC(&ri));
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 2);
    assert_always(!cra_refcnt_unref(CRA_REFCNT_RC(&ri)));
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 1);
    cra_refcnt_ref(CRA_REFCNT_RC(&ri));
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 2);
    assert_always(!cra_refcnt_unref(CRA_REFCNT_RC(&ri)));
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 1);
    assert_always(cra_refcnt_unref(CRA_REFCNT_RC(&ri)));
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 0);

    Stru_rc *rs = cra_alloc(Stru_rc);
    cra_refcnt_init(CRA_REFCNT_RC(rs), (cra_refcnt_uninit_fn)delete_stru);
    CRA_REFCNT_OBJ(rs)->i = 200;
    CRA_REFCNT_OBJ(rs)->f = 1.5f;

    cra_refcnt_ref(CRA_REFCNT_RC(rs));
    assert_always(CRA_REFCNT_RC(rs)->cnt == 2);

    cra_refcnt_unref0(CRA_REFCNT_RC(rs));
    assert_always(CRA_REFCNT_RC(rs)->cnt == 1);
    cra_refcnt_unref_clear((CraRefcnt **)&rs);
    assert_always(rs == NULL);
}

void test_refcnt_ptr(void)
{
    CRA_REFCNT_PTR_DEF(int)(ri);
    int *pi = cra_alloc(int);
    *pi = 100;
    cra_refcnt_init(CRA_REFCNT_RC(&ri), (cra_refcnt_uninit_fn)test_and_delete_int100_p);
    CRA_REFCNT_PTR(&ri) = pi;
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 1);
    cra_refcnt_ref(CRA_REFCNT_RC(&ri));
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 2);
    assert_always(!cra_refcnt_unref(CRA_REFCNT_RC(&ri)));
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 1);
    cra_refcnt_ref(CRA_REFCNT_RC(&ri));
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 2);
    assert_always(!cra_refcnt_unref(CRA_REFCNT_RC(&ri)));
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 1);
    assert_always(cra_refcnt_unref(CRA_REFCNT_RC(&ri)));
    assert_always(CRA_REFCNT_RC(&ri)->cnt == 0);

    Stru_rc_p rs;
    Stru *ps = cra_alloc(Stru);
    ps->i = 200;
    ps->f = 3.5f;
    cra_refcnt_init(CRA_REFCNT_RC(&rs), (cra_refcnt_uninit_fn)delete_stru_p);
    CRA_REFCNT_PTR(&rs) = ps;

    cra_refcnt_ref(CRA_REFCNT_RC(&rs));
    assert_always(CRA_REFCNT_RC(&rs)->cnt == 2);

    cra_refcnt_unref0(CRA_REFCNT_RC(&rs));
    assert_always(CRA_REFCNT_RC(&rs)->cnt == 1);
    cra_refcnt_unref0(CRA_REFCNT_RC(&rs));
    assert_always(CRA_REFCNT_RC(&rs)->cnt == 0);

    // ====================================

    Stru_rc_p *prs = cra_alloc(Stru_rc_p);
    ps = cra_alloc(Stru);
    ps->i = 400;
    ps->f = 5.5f;
    cra_refcnt_init(CRA_REFCNT_RC(prs), (cra_refcnt_uninit_fn)delete_stru_p2);
    CRA_REFCNT_PTR(prs) = ps;

    cra_refcnt_ref(CRA_REFCNT_RC(prs));
    assert_always(CRA_REFCNT_RC(prs)->cnt == 2);

    cra_refcnt_unref0(CRA_REFCNT_RC(prs));
    assert_always(CRA_REFCNT_RC(prs)->cnt == 1);
    cra_refcnt_unref_clear((CraRefcnt **)&prs);
    assert_always(prs == NULL);
}

static CRA_THRD_FUNC(thread_func)
{
    Stru_rc *rs = (Stru_rc *)arg;
    cra_log_info("thread: Stru{i: %d, f: %f}", CRA_REFCNT_OBJ(rs)->i, CRA_REFCNT_OBJ(rs)->f);
    cra_refcnt_unref0(CRA_REFCNT_RC(rs));
    return (cra_thrd_ret_t){0};
}

void test_multithread(void)
{
    Stru_rc *rs = cra_alloc(Stru_rc);
    CRA_REFCNT_OBJ(rs)->i = 1000;
    CRA_REFCNT_OBJ(rs)->f = 1000.5f;
    cra_refcnt_init(CRA_REFCNT_RC(rs), (cra_refcnt_uninit_fn)delete_stru);

    cra_thrd_t th;
    cra_refcnt_ref(CRA_REFCNT_RC(rs));
    cra_thrd_create(&th, thread_func, rs);

    // cra_msleep(50);
    cra_refcnt_unref_clear((CraRefcnt **)&rs);

    cra_thrd_join(th);
}

struct StruIn
{
    int i;
    CraRefcnt ref;
    float f;
};

static void cra_struin_print(CraRefcnt *ref)
{
    struct StruIn *si = container_of(ref, struct StruIn, ref);
    cra_log_info("StruIn{i: %d, f: %f}\n", si->i, si->f);
}

static void cra_struin_delete(CraRefcnt *ref)
{
    cra_struin_print(ref);
    struct StruIn *si = container_of(ref, struct StruIn, ref);
    cra_dealloc(si);
}

void test_ref_inner(void)
{
    struct StruIn si;
    cra_refcnt_init(&si.ref, cra_struin_print);
    si.i = 100;
    si.f = 2.5f;
    assert_always(si.ref.cnt == 1);

    cra_refcnt_ref(&si.ref);
    assert_always(si.ref.cnt == 2);

    assert_always(!cra_refcnt_unref(&si.ref));
    assert_always(si.ref.cnt == 1);
    assert_always(cra_refcnt_unref(&si.ref));
    assert_always(si.ref.cnt == 0);

    struct StruIn *psi = cra_alloc(struct StruIn);
    cra_refcnt_init(&psi->ref, cra_struin_delete);
    psi->i = 200;
    psi->f = 4.8f;
    assert_always(psi->ref.cnt == 1);

    cra_refcnt_ref(&psi->ref);
    assert_always(psi->ref.cnt == 2);

    assert_always(!cra_refcnt_unref(&psi->ref));
    assert_always(psi->ref.cnt == 1);
    assert_always(cra_refcnt_unref(&psi->ref));
    psi = NULL;
}

int main(void)
{
    cra_log_startup(CRA_LOG_LEVEL_TRACE, false, true);

    test_refcnt();
    test_refcnt_ptr();
    test_multithread();
    test_ref_inner();

    cra_log_cleanup();

    cra_memory_leak_report();
    return 0;
}
