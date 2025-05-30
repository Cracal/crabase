#include "threads/cra_thread.h"
#include "cra_refcnt.h"
#include "cra_malloc.h"
#include "cra_log.h"

#undef CRA_LOG_NAME
#define CRA_LOG_NAME "TestRefcnt"

typedef struct
{
    int i;
    float f;
} Stru;
typedef CRA_REFCNT(Stru) StruRc;
typedef CRA_REFCNT_PTR(Stru) StruRcPtr;

static void uninit_int100(void *pi)
{
    assert_always(*(int *)pi == 100);
    cra_log_info("uninit int: %d", *(int *)pi);
}

static void uninit_stru(void *ps)
{
    Stru *s = (Stru *)ps;
    cra_log_info("uninit Stru{i: %d, f: %f}", s->i, s->f);
}

void test_refcnt(void)
{
    CRA_REFCNT(int)
    ri;
    ri.o = 100;
    cra_refcnt_init(&ri, false, uninit_int100);
    assert_always(ri.rc.cnt == 1);
    assert_always(cra_refcnt_unref(&ri));
    assert_always(ri.rc.cnt == 0);

    cra_refcnt_init(&ri, false, uninit_int100);
    assert_always(ri.rc.cnt == 1);
    cra_refcnt_ref(&ri);
    assert_always(ri.rc.cnt == 2);
    assert_always(!cra_refcnt_unref(&ri));
    assert_always(ri.rc.cnt == 1);
    cra_refcnt_ref(&ri);
    assert_always(ri.rc.cnt == 2);
    assert_always(!cra_refcnt_unref(&ri));
    assert_always(ri.rc.cnt == 1);
    assert_always(cra_refcnt_unref(&ri));
    assert_always(ri.rc.cnt == 0);

    StruRc *rs = cra_alloc(StruRc);
    cra_refcnt_init(rs, true, uninit_stru);
    rs->o.i = 200;
    rs->o.f = 1.5f;

    cra_refcnt_ref(rs);
    assert_always(rs->rc.cnt == 2);

    cra_refcnt_unref0(rs);
    assert_always(rs->rc.cnt == 1);
    cra_refcnt_unref_clear((void **)&rs);
    assert_always(rs == NULL);
}

void test_refcnt_ptr(void)
{
    CRA_REFCNT_PTR(int)(ri);
    int i = 100;
    cra_refcnt_init_ptr(&ri, &i, false, false, uninit_int100);
    assert_always(ri.rc.cnt == 1);
    assert_always(cra_refcnt_unref(&ri));
    assert_always(ri.rc.cnt == 0);

    int *pi = cra_alloc(int);
    *pi = 100;
    cra_refcnt_init_ptr(&ri, pi, true, false, uninit_int100);
    assert_always(ri.rc.cnt == 1);
    cra_refcnt_ref(&ri);
    assert_always(ri.rc.cnt == 2);
    assert_always(!cra_refcnt_unref(&ri));
    assert_always(ri.rc.cnt == 1);
    cra_refcnt_ref(&ri);
    assert_always(ri.rc.cnt == 2);
    assert_always(!cra_refcnt_unref(&ri));
    assert_always(ri.rc.cnt == 1);
    assert_always(cra_refcnt_unref(&ri));
    assert_always(ri.rc.cnt == 0);

    StruRcPtr rs;
    Stru s = {.i = 100, .f = 2.5f};
    cra_refcnt_init_ptr(&rs, &s, false, false, uninit_stru);

    cra_refcnt_ref(&rs);
    assert_always(rs.rc.cnt == 2);

    cra_refcnt_unref0(&rs);
    assert_always(rs.rc.cnt == 1);
    cra_refcnt_unref0(&rs);
    assert_always(rs.rc.cnt == 0);

    // ====

    Stru *ps = cra_alloc(Stru);
    ps->i = 200;
    ps->f = 3.5f;
    cra_refcnt_init_ptr(&rs, ps, true, false, uninit_stru);

    cra_refcnt_ref(&rs);
    assert_always(rs.rc.cnt == 2);

    cra_refcnt_unref0(&rs);
    assert_always(rs.rc.cnt == 1);
    cra_refcnt_unref0(&rs);
    assert_always(rs.rc.cnt == 0);

    // ====================================

    StruRcPtr *prs = cra_alloc(StruRcPtr);
    s.i = 300;
    s.f = 4.5f;
    cra_refcnt_init_ptr(prs, &s, false, true, uninit_stru);

    cra_refcnt_ref(prs);
    assert_always(prs->rc.cnt == 2);

    cra_refcnt_unref0(prs);
    assert_always(prs->rc.cnt == 1);
    cra_refcnt_unref_clear((void **)&prs);
    assert_always(prs == NULL);

    // ====

    prs = cra_alloc(StruRcPtr);
    ps = cra_alloc(Stru);
    ps->i = 400;
    ps->f = 5.5f;
    cra_refcnt_init_ptr(prs, ps, true, true, uninit_stru);

    cra_refcnt_ref(prs);
    assert_always(prs->rc.cnt == 2);

    cra_refcnt_unref0(prs);
    assert_always(prs->rc.cnt == 1);
    cra_refcnt_unref_clear((void **)&prs);
    assert_always(prs == NULL);
}

static CRA_THRD_FUNC(thread_func)
{
    StruRc *rs = (StruRc *)arg;
    cra_log_info("thread: Stru{i: %d, f: %f}", rs->o.i, rs->o.f);
    cra_refcnt_unref0(rs);
    return (cra_thrd_ret_t){0};
}

void test_multithread(void)
{
    StruRc *rs = cra_alloc(StruRc);
    rs->o.i = 1000;
    rs->o.f = 1000.5f;
    cra_refcnt_init(rs, true, uninit_stru);

    cra_thrd_t th;
    cra_refcnt_ref(rs);
    cra_thrd_create(&th, thread_func, rs);

    // cra_msleep(50);
    cra_refcnt_unref_clear((void **)&rs);

    cra_thrd_join(th);
}

int main(void)
{
    cra_log_startup(CRA_LOG_LEVEL_TRACE, false, true);

    test_refcnt();
    test_refcnt_ptr();
    test_multithread();

    cra_log_cleanup();

    cra_memory_leak_report(stdout);
    return 0;
}
