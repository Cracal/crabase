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
    v2 = cra_atomic_load32(&v1);
    assert_always(v1 == v2);
    cra_atomic_store32(&v1, 200);
    assert_always(v1 == 200);

    cra_atomic_int64_t vv1, vv2;
    vv1 = INT64_MAX;
    vv2 = cra_atomic_load64(&vv1);
    assert_always(vv1 == vv2);
    cra_atomic_store64(&vv1, 2300);
    assert_always(vv1 == 2300);

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
    i32 = cra_atomic_add32(&ia32, 1);
    assert_always(i32 == 10 && ia32 == 11);
    i32 = cra_atomic_add32(&ia32, 1);
    assert_always(i32 == 11 && ia32 == 12);
    i32 = cra_atomic_add32(&ia32, 10);
    assert_always(i32 == 12 && ia32 == 22);

    ia64 = 10;
    i64 = cra_atomic_add64(&ia64, 1);
    assert_always(i64 == 10 && ia64 == 11);
    i64 = cra_atomic_add64(&ia64, 1);
    assert_always(i64 == 11 && ia64 == 12);
    i64 = cra_atomic_add64(&ia64, 100);
    assert_always(i64 == 12 && ia64 == 112);

    ia32 = 100;
    i32 = cra_atomic_sub32(&ia32, 1);
    assert_always(i32 == 100 && ia32 == 99);
    i32 = cra_atomic_sub32(&ia32, 1);
    assert_always(i32 == 99 && ia32 == 98);
    i32 = cra_atomic_sub32(&ia32, 8);
    assert_always(i32 == 98 && ia32 == 90);

    ia64 = 100;
    i64 = cra_atomic_sub64(&ia64, 1);
    assert_always(i64 == 100 && ia64 == 99);
    i64 = cra_atomic_sub64(&ia64, 1);
    assert_always(i64 == 99 && ia64 == 98);
    i64 = cra_atomic_sub64(&ia64, 8);
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
    i32 = cra_atomic_inc32(&ia32);
    assert_always(i32 == 8 && ia32 == 9);
    i32 = cra_atomic_inc32(&ia32);
    assert_always(i32 == 9 && ia32 == 10);

    ia32 = 8;
    i32 = cra_atomic_dec32(&ia32);
    assert_always(i32 == 8 && ia32 == 7);
    i32 = cra_atomic_dec32(&ia32);
    assert_always(i32 == 7 && ia32 == 6);

    ia64 = 8;
    i64 = cra_atomic_inc64(&ia64);
    assert_always(i64 == 8 && ia64 == 9);
    i64 = cra_atomic_inc64(&ia64);
    assert_always(i64 == 9 && ia64 == 10);

    ia64 = 8;
    i64 = cra_atomic_dec64(&ia64);
    assert_always(i64 == 8 && ia64 == 7);
    i64 = cra_atomic_dec64(&ia64);
    assert_always(i64 == 7 && ia64 == 6);

    printf("test inc and dec done.\n");
}

void
test_compare_and_set(void)
{
    printf("test compare_and_set.\n");

    bool               b;
    int32_t            i32;
    int64_t            i64;
    cra_atomic_int32_t ia32;
    cra_atomic_int64_t ia64;

    i32 = 64, ia32 = 64;
    b = cra_atomic_compare_and_set32(&ia32, i32, 1000);
    assert_always(b && ia32 == 1000);
    i32 = 100, ia32 = 200;
    b = cra_atomic_compare_and_set32(&ia32, i32, 1000);
    assert_always(!b && ia32 == 200);

    i64 = 64, ia64 = 64;
    b = cra_atomic_compare_and_set64(&ia64, i64, 1000);
    assert_always(b && ia64 == 1000);
    i64 = 100, ia64 = 200;
    b = cra_atomic_compare_and_set64(&ia64, i64, 1000);
    assert_always(!b && ia64 == 200);

    printf("test compare_and_set done.\n");
}

void
test_flag(void)
{
    bool b;

    printf("test cra_atomic_flag.\n");

    cra_atomic_flag_t flag = CRA_ATOMIC_FLAG_INIT;
    assert_always(*(bool *)&flag == false);

    b = cra_atomic_flag_test_and_set(&flag);
    assert_always(!b && *(bool *)&flag == true);

    b = cra_atomic_flag_test_and_set(&flag);
    assert_always(b && *(bool *)&flag == true);

    cra_atomic_flag_clear(&flag);
    assert_always(!(*(bool *)&flag));

    printf("test cra_atomic_flag done.\n");
}

#define NUM 100
cra_thrd_t         th[NUM];
cra_atomic_int32_t ia32;
cra_atomic_int64_t ia64;

CRA_THRD_FUNC(test_add_thr_fn)
{
    CRA_UNUSED_VALUE(arg);
    cra_atomic_add32(&ia32, 10);
    cra_atomic_add64(&ia64, 100);
    return (cra_thrd_ret_t){ 0 };
}

CRA_THRD_FUNC(test_sub_thr_fn)
{
    CRA_UNUSED_VALUE(arg);
    cra_atomic_sub32(&ia32, 10);
    cra_atomic_sub64(&ia64, 100);
    return (cra_thrd_ret_t){ 0 };
}

void
test_add_and_sub_thr(void)
{
    ia32 = 0;
    ia64 = 0;

    for (int i = 0; i < NUM; i++)
        cra_thrd_create(&th[i], test_add_thr_fn, NULL);

    for (int i = 0; i < NUM; i++)
        cra_thrd_join(th[i]);

    assert_always(ia32 == 1000);
    assert_always(ia64 == 10000);

    for (int i = 0; i < NUM; i++)
        cra_thrd_create(&th[i], test_sub_thr_fn, NULL);

    for (int i = 0; i < NUM; i++)
        cra_thrd_join(th[i]);

    assert_always(ia32 == 0);
    assert_always(ia64 == 0);
}

CRA_THRD_FUNC(test_inc_thr_fn)
{
    CRA_UNUSED_VALUE(arg);
    cra_atomic_inc32(&ia32);
    cra_atomic_inc64(&ia64);
    return (cra_thrd_ret_t){ 0 };
}

CRA_THRD_FUNC(test_dec_thr_fn)
{
    CRA_UNUSED_VALUE(arg);
    cra_atomic_dec32(&ia32);
    cra_atomic_dec64(&ia64);
    return (cra_thrd_ret_t){ 0 };
}

void
test_inc_and_dec_thr(void)
{
    ia32 = 10;
    ia64 = 10000;

    for (int i = 0; i < 100; i++)
        cra_thrd_create(&th[i], test_inc_thr_fn, NULL);

    for (int i = 0; i < NUM; i++)
        cra_thrd_join(th[i]);

    assert_always(ia32 == 110);
    assert_always(ia64 == 10100);

    for (int i = 0; i < 100; i++)
        cra_thrd_create(&th[i], test_dec_thr_fn, NULL);

    for (int i = 0; i < NUM; i++)
        cra_thrd_join(th[i]);

    assert_always(ia32 == 10);
    assert_always(ia64 == 10000);
}

int
main(void)
{
    test_load_and_store();
    test_add_and_sub();
    test_inc_and_dec();
    test_compare_and_set();
    test_flag();

    test_add_and_sub_thr();
    test_inc_and_dec_thr();
    return 0;
}
