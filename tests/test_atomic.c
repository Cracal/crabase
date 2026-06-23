/**
 * @file test-atomic.c
 * @author Cracal
 * @brief test atomic
 * @version 0.1
 * @date 2024-09-17
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "cra_assert.h"
#include "cra_atomic.h"
#include "threads/cra_thread.h"

void
test_load_and_store(void)
{
    printf("test load and store.\n");

    cra_atomic_int32_t v1, v2;
    v1 = INT32_MAX;
    v2 = cra_atomic_load(&v1, CRA_MO_ACQUIRE);
    assert_always(v1 == v2);
    cra_atomic_store(&v1, 200, CRA_MO_RELEASE);
    assert_always(v1 == 200);

    cra_atomic_int64_t vv1, vv2;
    vv1 = INT64_MAX;
    vv2 = cra_atomic_load(&vv1, CRA_MO_ACQUIRE);
    assert_always(vv1 == vv2);
    cra_atomic_store(&vv1, 2300, CRA_MO_RELEASE);
    assert_always(vv1 == 2300);

    cra_atomic_ptr_t p1, p2;
    p1 = &v1;
    p2 = cra_atomic_load(&p1, CRA_MO_ACQUIRE);
    assert_always(p1 == p2);
    cra_atomic_store(&p1, &vv2, CRA_MO_RELEASE);
    assert_always(p1 == &vv2);

    printf("test load and store done.\n");
}

void
test_add_and_sub(void)
{
    printf("test add and sub.\n");

    int32_t            i32;
    int64_t            i64;
    cra_atomic_int32_t ia32;
    cra_atomic_int64_t ia64;

    ia32 = 10;
    i32 = cra_atomic_add(&ia32, 1, CRA_MO_ACQ_REL);
    assert_always(i32 == 10 && ia32 == 11);
    i32 = cra_atomic_add(&ia32, 1, CRA_MO_ACQ_REL);
    assert_always(i32 == 11 && ia32 == 12);
    i32 = cra_atomic_add(&ia32, 10, CRA_MO_ACQ_REL);
    assert_always(i32 == 12 && ia32 == 22);

    ia64 = 10;
    i64 = cra_atomic_add(&ia64, 1, CRA_MO_ACQ_REL);
    assert_always(i64 == 10 && ia64 == 11);
    i64 = cra_atomic_add(&ia64, 1, CRA_MO_ACQ_REL);
    assert_always(i64 == 11 && ia64 == 12);
    i64 = cra_atomic_add(&ia64, 100, CRA_MO_ACQ_REL);
    assert_always(i64 == 12 && ia64 == 112);

    ia32 = 100;
    i32 = cra_atomic_sub(&ia32, 1, CRA_MO_ACQ_REL);
    assert_always(i32 == 100 && ia32 == 99);
    i32 = cra_atomic_sub(&ia32, 1, CRA_MO_ACQ_REL);
    assert_always(i32 == 99 && ia32 == 98);
    i32 = cra_atomic_sub(&ia32, 8, CRA_MO_ACQ_REL);
    assert_always(i32 == 98 && ia32 == 90);

    ia64 = 100;
    i64 = cra_atomic_sub(&ia64, 1, CRA_MO_ACQ_REL);
    assert_always(i64 == 100 && ia64 == 99);
    i64 = cra_atomic_sub(&ia64, 1, CRA_MO_ACQ_REL);
    assert_always(i64 == 99 && ia64 == 98);
    i64 = cra_atomic_sub(&ia64, 8, CRA_MO_ACQ_REL);
    assert_always(i64 == 98 && ia64 == 90);

    printf("test add and sub done.\n");
}

void
test_inc_and_dec(void)
{
    printf("test inc and dec.\n");

    int32_t            i32;
    int64_t            i64;
    cra_atomic_int32_t ia32;
    cra_atomic_int64_t ia64;

    ia32 = 8;
    i32 = cra_atomic_inc(&ia32, CRA_MO_ACQ_REL);
    assert_always(i32 == 8 && ia32 == 9);
    i32 = cra_atomic_inc(&ia32, CRA_MO_ACQ_REL);
    assert_always(i32 == 9 && ia32 == 10);

    ia32 = 8;
    i32 = cra_atomic_dec(&ia32, CRA_MO_ACQ_REL);
    assert_always(i32 == 8 && ia32 == 7);
    i32 = cra_atomic_dec(&ia32, CRA_MO_ACQ_REL);
    assert_always(i32 == 7 && ia32 == 6);

    ia64 = 8;
    i64 = cra_atomic_inc(&ia64, CRA_MO_ACQ_REL);
    assert_always(i64 == 8 && ia64 == 9);
    i64 = cra_atomic_inc(&ia64, CRA_MO_ACQ_REL);
    assert_always(i64 == 9 && ia64 == 10);

    ia64 = 8;
    i64 = cra_atomic_dec(&ia64, CRA_MO_ACQ_REL);
    assert_always(i64 == 8 && ia64 == 7);
    i64 = cra_atomic_dec(&ia64, CRA_MO_ACQ_REL);
    assert_always(i64 == 7 && ia64 == 6);

    printf("test inc and dec done.\n");
}

void
test_cas(void)
{
    printf("test cas.\n");

    bool               b;
    int32_t            i32;
    int64_t            i64;
    cra_atomic_ptr_t   pa1;
    cra_atomic_int32_t ia32;
    cra_atomic_int64_t ia64;

    for (int i = 0; i < 100000; i++)
    {
        i32 = 64, ia32 = 64;
        b = cra_atomic_cas_strong(&ia32, i32, 1000, CRA_MO_ACQ_REL, CRA_MO_RELAXED);
        assert_always(b && ia32 == 1000);
        i32 = 100, ia32 = 200;
        b = cra_atomic_cas_strong(&ia32, i32, 1000, CRA_MO_ACQ_REL, CRA_MO_RELAXED);
        assert_always(!b && ia32 == 200);

        i64 = 64, ia64 = 64;
        b = cra_atomic_cas_strong(&ia64, i64, 1000, CRA_MO_ACQ_REL, CRA_MO_RELAXED);
        assert_always(b && ia64 == 1000);
        i64 = 100, ia64 = 200;
        b = cra_atomic_cas_strong(&ia64, i64, 1000, CRA_MO_ACQ_REL, CRA_MO_RELAXED);
        assert_always(!b && ia64 == 200);
    }

    ia32 = 40;
    while (cra_atomic_cas_weak(&ia32, 40, 50, CRA_MO_ACQ_REL, CRA_MO_RELAXED))
        ;
    assert_always(ia32 == 50);

    pa1 = &i64;
    b = cra_atomic_cas_strong(&pa1, &i64, &i32, CRA_MO_ACQ_REL, CRA_MO_RELAXED);
    assert_always(b && pa1 == &i32);
    b = cra_atomic_cas_strong(&pa1, &i64, &i32, CRA_MO_ACQ_REL, CRA_MO_RELAXED);
    assert_always(!b && pa1 == &i32);

    printf("test cas done.\n");
}

void
test_flag(void)
{
    printf("test cra_atomic_flag.\n");

    cra_atomic_flag_t flag = CRA_ATOMIC_FLAG_INIT;

    assert_always(!cra_atomic_flag_test_and_set(&flag, CRA_MO_ACQ_REL));
    assert_always(cra_atomic_flag_test_and_set(&flag, CRA_MO_ACQ_REL));
    assert_always(cra_atomic_flag_test_and_set(&flag, CRA_MO_ACQ_REL));
    assert_always(cra_atomic_flag_test_and_set(&flag, CRA_MO_ACQ_REL));
    cra_atomic_flag_clear(&flag, CRA_MO_RELAXED);
    assert_always(!cra_atomic_flag_test_and_set(&flag, CRA_MO_ACQ_REL));

    printf("test cra_atomic_flag done.\n");
}

#define N_THREAD 10
#define N_TIMES  10000
cra_thrd_t         th[N_THREAD];
cra_atomic_int32_t ia32;
cra_atomic_int64_t ia64;
cra_atomic_flag_t  flag;
int64_t            i64;

CRA_THRD_FUNC(test_add_thr_fn)
{
    CRA_UNUSED_VALUE(arg);
    for (int i = 0; i < N_TIMES; i++)
    {
        cra_atomic_add(&ia32, 10, CRA_MO_ACQ_REL);
        cra_atomic_add(&ia64, 100, CRA_MO_ACQ_REL);
    }
    return (cra_thrd_ret_t){ 0 };
}

CRA_THRD_FUNC(test_sub_thr_fn)
{
    CRA_UNUSED_VALUE(arg);
    for (int i = 0; i < N_TIMES; i++)
    {
        cra_atomic_sub(&ia32, 10, CRA_MO_ACQ_REL);
        cra_atomic_sub(&ia64, 100, CRA_MO_ACQ_REL);
    }
    return (cra_thrd_ret_t){ 0 };
}

void
test_add_and_sub_thr(void)
{
    ia32 = 0;
    ia64 = 0;

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_create(&th[i], test_add_thr_fn, NULL);

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_join(th[i]);

    assert_always(ia32 == 10 * N_THREAD * N_TIMES);
    assert_always(ia64 == 100 * N_THREAD * N_TIMES);

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_create(&th[i], test_sub_thr_fn, NULL);

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_join(th[i]);

    assert_always(ia32 == 0);
    assert_always(ia64 == 0);
}

CRA_THRD_FUNC(test_inc_thr_fn)
{
    CRA_UNUSED_VALUE(arg);
    for (int i = 0; i < N_TIMES; i++)
    {
        cra_atomic_inc(&ia32, CRA_MO_ACQ_REL);
        cra_atomic_inc(&ia64, CRA_MO_ACQ_REL);
    }
    return (cra_thrd_ret_t){ 0 };
}

CRA_THRD_FUNC(test_dec_thr_fn)
{
    CRA_UNUSED_VALUE(arg);
    for (int i = 0; i < N_TIMES; i++)
    {
        cra_atomic_dec(&ia32, CRA_MO_ACQ_REL);
        cra_atomic_dec(&ia64, CRA_MO_ACQ_REL);
    }
    return (cra_thrd_ret_t){ 0 };
}

void
test_inc_and_dec_thr(void)
{
    ia32 = 10;
    ia64 = 10000;

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_create(&th[i], test_inc_thr_fn, NULL);

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_join(th[i]);

    assert_always(ia32 == 10 + N_THREAD * N_TIMES);
    assert_always(ia64 == 10000 + N_THREAD * N_TIMES);

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_create(&th[i], test_dec_thr_fn, NULL);

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_join(th[i]);

    assert_always(ia32 == 10);
    assert_always(ia64 == 10000);
}

CRA_THRD_FUNC(test_flag_thr_fn)
{
    CRA_UNUSED_VALUE(arg);
    for (int i = 0; i < N_TIMES; i++)
    {
        while (cra_atomic_flag_test_and_set(&flag, CRA_MO_ACQUIRE))
            ;
        i64++;
        cra_atomic_flag_clear(&flag, CRA_MO_RELEASE);
    }
    return (cra_thrd_ret_t){ 0 };
}

void
test_flag_thr(void)
{
    i64 = 0;
    cra_atomic_flag_clear(&flag, CRA_MO_RELEASE);

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_create(&th[i], test_flag_thr_fn, NULL);

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_join(th[i]);

    assert_always(i64 == N_THREAD * N_TIMES);
}

int
main(void)
{
    test_load_and_store();
    test_add_and_sub();
    test_inc_and_dec();
    test_cas();
    test_flag();

    test_add_and_sub_thr();
    test_inc_and_dec_thr();
    test_flag_thr();
    return 0;
}
