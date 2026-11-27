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
#include "crabase/cra_assert.h"
#include "crabase/cra_atomic.h"
#include "crabase/threads/cra_thread.h"

void
test_load_store(void)
{
    printf("test load store.\n");

    cra_atomic_int32_t v1, v2;
    v1 = INT32_MAX;
    v2 = cra_atomic_load(&v1, cra_memory_order_acquire);
    assert_always(v1 == v2);
    cra_atomic_store(&v1, 200, cra_memory_order_release);
    assert_always(cra_atomic_load(&v1, cra_memory_order_relaxed) == 200);

    cra_atomic_int64_t vv1, vv2;
    vv1 = INT64_MAX;
    vv2 = cra_atomic_load(&vv1, cra_memory_order_acquire);
    assert_always(vv1 == vv2);
    cra_atomic_store(&vv1, 2300, cra_memory_order_release);
    assert_always(cra_atomic_load(&vv1, cra_memory_order_relaxed) == 2300);

    cra_atomic_ptr_t p1, p2;
    p1 = &v1;
    p2 = cra_atomic_load(&p1, cra_memory_order_acquire);
    assert_always(p1 == p2);
    cra_atomic_store(&p1, &vv2, cra_memory_order_release);
    assert_always(cra_atomic_load(&p1, cra_memory_order_relaxed) == (void *)&vv2);

    printf("test load store done.\n");
}

void
test_add_sub(void)
{
    printf("test add sub.\n");

    int32_t            i32;
    int64_t            i64;
    cra_atomic_int32_t ia32;
    cra_atomic_int64_t ia64;

    ia32 = 10;
    i32 = cra_atomic_fetch_add(&ia32, 1, cra_memory_order_acq_rel);
    assert_always(i32 == 10 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 11);
    i32 = cra_atomic_fetch_add(&ia32, 1, cra_memory_order_acq_rel);
    assert_always(i32 == 11 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 12);
    i32 = cra_atomic_fetch_add(&ia32, 10, cra_memory_order_acq_rel);
    assert_always(i32 == 12 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 22);

    ia64 = 10;
    i64 = cra_atomic_fetch_add(&ia64, 1, cra_memory_order_acq_rel);
    assert_always(i64 == 10 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 11);
    i64 = cra_atomic_fetch_add(&ia64, 1, cra_memory_order_acq_rel);
    assert_always(i64 == 11 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 12);
    i64 = cra_atomic_fetch_add(&ia64, 100, cra_memory_order_acq_rel);
    assert_always(i64 == 12 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 112);

    ia32 = 100;
    i32 = cra_atomic_fetch_sub(&ia32, 1, cra_memory_order_acq_rel);
    assert_always(i32 == 100 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 99);
    i32 = cra_atomic_fetch_sub(&ia32, 1, cra_memory_order_acq_rel);
    assert_always(i32 == 99 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 98);
    i32 = cra_atomic_fetch_sub(&ia32, 8, cra_memory_order_acq_rel);
    assert_always(i32 == 98 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 90);

    ia64 = 100;
    i64 = cra_atomic_fetch_sub(&ia64, 1, cra_memory_order_acq_rel);
    assert_always(i64 == 100 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 99);
    i64 = cra_atomic_fetch_sub(&ia64, 1, cra_memory_order_acq_rel);
    assert_always(i64 == 99 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 98);
    i64 = cra_atomic_fetch_sub(&ia64, 8, cra_memory_order_acq_rel);
    assert_always(i64 == 98 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 90);

    printf("test add sub done.\n");
}

void
test_or_and_xor(void)
{
    printf("test or and xor.\n");

    int32_t            i32;
    int64_t            i64;
    cra_atomic_int32_t ia32;
    cra_atomic_int64_t ia64;

    ia32 = 0;
    i32 = cra_atomic_fetch_or(&ia32, 1, cra_memory_order_acq_rel);
    assert_always(i32 == 0 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 1);
    i32 = cra_atomic_fetch_or(&ia32, 2, cra_memory_order_acq_rel);
    assert_always(i32 == 1 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 3);
    i32 = cra_atomic_fetch_or(&ia32, 4, cra_memory_order_acq_rel);
    assert_always(i32 == 3 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 7);

    i32 = cra_atomic_fetch_and(&ia32, 8, cra_memory_order_acq_rel);
    assert_always(i32 == 7 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 0);
    cra_atomic_store(&ia32, 7, cra_memory_order_release);
    i32 = cra_atomic_fetch_and(&ia32, 2, cra_memory_order_acq_rel);
    assert_always(i32 == 7 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 2);
    cra_atomic_store(&ia32, 7, cra_memory_order_release);
    i32 = cra_atomic_fetch_and(&ia32, 1, cra_memory_order_acq_rel);
    assert_always(i32 == 7 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 1);
    cra_atomic_store(&ia32, 7, cra_memory_order_release);
    i32 = cra_atomic_fetch_and(&ia32, 4, cra_memory_order_acq_rel);
    assert_always(i32 == 7 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 4);

    cra_atomic_store(&ia32, 15, cra_memory_order_release);
    i32 = cra_atomic_fetch_xor(&ia32, 15, cra_memory_order_acq_rel);
    assert_always(i32 == 15 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 0);
    cra_atomic_store(&ia32, 15, cra_memory_order_release);
    i32 = cra_atomic_fetch_xor(&ia32, 8, cra_memory_order_acq_rel);
    assert_always(i32 == 15 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 7);

    ia64 = 0;
    i64 = cra_atomic_fetch_or(&ia64, 1, cra_memory_order_acq_rel);
    assert_always(i64 == 0 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 1);
    i64 = cra_atomic_fetch_or(&ia64, 2, cra_memory_order_acq_rel);
    assert_always(i64 == 1 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 3);
    i64 = cra_atomic_fetch_or(&ia64, 4, cra_memory_order_acq_rel);
    assert_always(i64 == 3 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 7);

    i64 = cra_atomic_fetch_and(&ia64, 8, cra_memory_order_acq_rel);
    assert_always(i64 == 7 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 0);
    cra_atomic_store(&ia64, 7, cra_memory_order_release);
    i64 = cra_atomic_fetch_and(&ia64, 2, cra_memory_order_acq_rel);
    assert_always(i64 == 7 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 2);
    cra_atomic_store(&ia64, 7, cra_memory_order_release);
    i64 = cra_atomic_fetch_and(&ia64, 1, cra_memory_order_acq_rel);
    assert_always(i64 == 7 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 1);
    cra_atomic_store(&ia64, 7, cra_memory_order_release);
    i64 = cra_atomic_fetch_and(&ia64, 4, cra_memory_order_acq_rel);
    assert_always(i64 == 7 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 4);

    cra_atomic_store(&ia64, 15, cra_memory_order_release);
    i64 = cra_atomic_fetch_xor(&ia64, 15, cra_memory_order_acq_rel);
    assert_always(i64 == 15 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 0);
    cra_atomic_store(&ia64, 15, cra_memory_order_release);
    i64 = cra_atomic_fetch_xor(&ia64, 8, cra_memory_order_acq_rel);
    assert_always(i64 == 15 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 7);

    printf("test or and xor done.\n");
}

void
test_exchange(void)
{
    printf("test exchange.\n");

    int32_t            i32;
    int64_t            i64;
    void              *ptr;
    cra_atomic_ptr_t   pa1;
    cra_atomic_int32_t ia32;
    cra_atomic_int64_t ia64;

    cra_atomic_store(&ia32, 100, cra_memory_order_relaxed);
    i32 = cra_atomic_exchange(&ia32, -100, cra_memory_order_relaxed);
    assert_always(i32 == 100 && cra_atomic_load(&ia32, cra_memory_order_relaxed) == -100);

    cra_atomic_store(&ia64, 100000, cra_memory_order_relaxed);
    i64 = cra_atomic_exchange(&ia64, -2000, cra_memory_order_relaxed);
    assert_always(i64 == 100000 && cra_atomic_load(&ia64, cra_memory_order_relaxed) == -2000);

    cra_atomic_store(&pa1, &i32, cra_memory_order_relaxed);
    ptr = cra_atomic_exchange(&pa1, &i64, cra_memory_order_relaxed);
    assert_always(ptr == (void *)&i32 && cra_atomic_load(&pa1, cra_memory_order_relaxed) == (void *)&i64);

    printf("test exchange done.\n");
}

void
test_compare_exchange(void)
{
    printf("test compare_exchange.\n");

    bool               b;
    void              *pa;
    int32_t            i32;
    int64_t            i64;
    cra_atomic_ptr_t   pa1;
    cra_atomic_int32_t ia32;
    cra_atomic_int64_t ia64;

    for (int i = 0; i < 100000; i++)
    {
        i32 = 64, ia32 = 64;
        b = cra_atomic_compare_exchange_strong(&ia32, &i32, 1000, cra_memory_order_acq_rel, cra_memory_order_relaxed);
        assert_always(b && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 1000 && i32 == 64);
        i32 = 100, ia32 = 200;
        b = cra_atomic_compare_exchange_strong(&ia32, &i32, 1000, cra_memory_order_acq_rel, cra_memory_order_relaxed);
        assert_always(!b && cra_atomic_load(&ia32, cra_memory_order_relaxed) == 200 && i32 == 200);

        i64 = 64, ia64 = 64;
        b = cra_atomic_compare_exchange_strong(&ia64, &i64, 1000, cra_memory_order_acq_rel, cra_memory_order_relaxed);
        assert_always(b && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 1000 && i64 == 64);
        i64 = 100, ia64 = 200;
        b = cra_atomic_compare_exchange_strong(&ia64, &i64, 1000, cra_memory_order_acq_rel, cra_memory_order_relaxed);
        assert_always(!b && cra_atomic_load(&ia64, cra_memory_order_relaxed) == 200 && i64 == 200);
    }

    ia32 = 40;
    while (
      cra_atomic_compare_exchange_weak(&ia32, &(int32_t){ 40 }, 50, cra_memory_order_acq_rel, cra_memory_order_relaxed))
    {
    }
    assert_always(cra_atomic_load(&ia32, cra_memory_order_relaxed) == 50);

    pa1 = pa = &i64;
    b = cra_atomic_compare_exchange_strong(&pa1, &pa, &i32, cra_memory_order_acq_rel, cra_memory_order_relaxed);
    assert_always(b && cra_atomic_load(&pa1, cra_memory_order_relaxed) == (void *)&i32 && pa == (void *)&i64);
    b = cra_atomic_compare_exchange_strong(&pa1, &pa, &i64, cra_memory_order_acq_rel, cra_memory_order_relaxed);
    assert_always(!b && cra_atomic_load(&pa1, cra_memory_order_relaxed) == (void *)&i32 && pa == (void *)&i32);

    printf("test compare_exchange done.\n");
}

void
test_flag(void)
{
    printf("test cra_atomic_flag.\n");

    cra_atomic_flag_t flag = CRA_ATOMIC_FLAG_INIT;

    assert_always(!cra_atomic_flag_test_and_set(&flag, cra_memory_order_acq_rel));
    assert_always(cra_atomic_flag_test_and_set(&flag, cra_memory_order_acq_rel));
    assert_always(cra_atomic_flag_test_and_set(&flag, cra_memory_order_acq_rel));
    assert_always(cra_atomic_flag_test_and_set(&flag, cra_memory_order_acq_rel));
    cra_atomic_flag_clear(&flag, cra_memory_order_relaxed);
    assert_always(!cra_atomic_flag_test_and_set(&flag, cra_memory_order_acq_rel));

    printf("test cra_atomic_flag done.\n");
}

#define N_THREAD 12
#define N_TIMES  100000
cra_thrd_t         th[N_THREAD];
cra_atomic_int32_t ia32;
cra_atomic_int64_t ia64;
cra_atomic_flag_t  flag;
int64_t            i64;

CRA_THRD_FUNC(test_add_thr_fn)
{
    CRA_UNUSED(arg);
    for (int i = 0; i < N_TIMES; i++)
    {
        cra_atomic_fetch_add(&ia32, 10, cra_memory_order_acq_rel);
        cra_atomic_fetch_add(&ia64, 100, cra_memory_order_acq_rel);
    }
    return (cra_thrd_ret_t){ 0 };
}

CRA_THRD_FUNC(test_sub_thr_fn)
{
    CRA_UNUSED(arg);
    for (int i = 0; i < N_TIMES; i++)
    {
        cra_atomic_fetch_sub(&ia32, 10, cra_memory_order_acq_rel);
        cra_atomic_fetch_sub(&ia64, 100, cra_memory_order_acq_rel);
    }
    return (cra_thrd_ret_t){ 0 };
}

void
test_add_sub_thread(void)
{
    printf("test add sub(thread count: %d).\n", N_THREAD);

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

    for (int i = 0; i < N_THREAD; i += 2)
    {
        cra_thrd_create(&th[i], test_add_thr_fn, NULL);
        cra_thrd_create(&th[i + 1], test_sub_thr_fn, NULL);
    }

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_join(th[i]);

    assert_always(ia32 == 0);
    assert_always(ia64 == 0);

    printf("test add sub(thread count: %d) done.\n", N_THREAD);
}

CRA_THRD_FUNC(test_flag_thr_fn)
{
    CRA_UNUSED(arg);
    for (int i = 0; i < N_TIMES; i++)
    {
        while (cra_atomic_flag_test_and_set(&flag, cra_memory_order_acquire))
            ;
        i64++;
        cra_atomic_flag_clear(&flag, cra_memory_order_release);
    }
    return (cra_thrd_ret_t){ 0 };
}

void
test_flag_thr(void)
{
    printf("test flag(thread count: %d).\n", N_THREAD);

    i64 = 0;
    cra_atomic_flag_clear(&flag, cra_memory_order_release);

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_create(&th[i], test_flag_thr_fn, NULL);

    for (int i = 0; i < N_THREAD; i++)
        cra_thrd_join(th[i]);

    assert_always(i64 == N_THREAD * N_TIMES);

    printf("test flag(thread count: %d) done.\n", N_THREAD);
}

int
main(void)
{
    test_load_store();
    test_add_sub();
    test_or_and_xor();
    test_exchange();
    test_compare_exchange();
    test_flag();

    test_add_sub_thread();
    test_flag_thr();
    return 0;
}
