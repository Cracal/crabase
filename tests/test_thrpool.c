/**
 * @file test-thrpool.c
 * @author Cracal
 * @brief test thread pool
 * @version 0.1
 * @date 2024-09-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "cra_assert.h"
#include "cra_malloc.h"
#include "threads/cra_thrdpool.h"

#define PLUS 1000

static void
worker(void *arg)
{
    int *val = (int *)arg;
    int  old = *val;

    *val += PLUS;
    printf("Worker: tid=%lu, old=%d, val=%d\n", cra_get_current_tid(), old, *val);

    // if (*val % 2)
    //     cra_msleep(100);
}

static void
test_thread_pool(void)
{
    CraThrdPool *tp;
    int         *vals;
    int          i;
    int          num_threads = 10;
    int          num_items = 10000;

    tp = cra_alloc(CraThrdPool);
    assert_always(!!tp);
    cra_thrdpool_init(tp, num_threads, CRA_THRDPOOL_INFINITE_TASKS, CRA_THRDPOOL_FULL_WAIT);

    vals = (int *)cra_malloc(sizeof(*vals) * num_items);
    bzero(vals, sizeof(*vals) * num_items);

    for (i = 0; i < num_items; i++)
    {
        vals[i] = i;
        cra_thrdpool_add_task1(tp, worker, vals + i);
    }

    cra_thrdpool_uninit(tp, true);
    cra_dealloc(tp);

    for (i = 0; i < num_items; i++)
    {
        assert_always(vals[i] == i + PLUS);
        // printf("main: %d\n", vals[i]);
    }

    cra_free(vals);
}

static void
test_thread_pool2(void)
{
    CraThrdPool *tp;
    int         *vals;
    int          i;
    int          num_threads = 10;
    int          num_items = 10000;

    tp = cra_alloc(CraThrdPool);
    assert_always(!!tp);
    cra_thrdpool_init(tp, num_threads, CRA_THRDPOOL_INFINITE_TASKS, CRA_THRDPOOL_FULL_WAIT);

    vals = (int *)cra_malloc(sizeof(*vals) * num_items);
    bzero(vals, sizeof(*vals) * num_items);

    for (i = 0; i < num_items; i++)
    {
        vals[i] = i;
        cra_thrdpool_add_task1(tp, worker, vals + i);
    }

    cra_thrdpool_uninit(tp, false);
    cra_dealloc(tp);

    int ncompleted = 0;
    for (i = 0; i < num_items; i++)
    {
        // assert_always(vals[i] == i + PLUS);
        if (vals[i] == i + PLUS)
            ++ncompleted;
        // printf("main: %d\n", vals[i]);
    }
    printf("completed count: %d\n", ncompleted);

    cra_free(vals);
}

static void
test_thread_pool3(void)
{
    CraThrdPool tp;
    int        *vals;
    int         i;
    int         num_threads = 10;
    int         num_items = 100;

    cra_thrdpool_init(&tp, num_threads, 10, CRA_THRDPOOL_FULL_WAIT);

    vals = (int *)cra_malloc(sizeof(*vals) * num_items);
    bzero(vals, sizeof(*vals) * num_items);

    for (i = 0; i < num_items; i++)
    {
        vals[i] = i;
        cra_thrdpool_add_task1(&tp, worker, vals + i);
    }

    cra_thrdpool_uninit(&tp, true);

    int ncompleted = 0;
    for (i = 0; i < num_items; i++)
    {
        assert_always(vals[i] == i + PLUS);
        ++ncompleted;
        // printf("main: %d\n", vals[i]);
    }
    printf("completed count: %d\n", ncompleted);

    cra_free(vals);
}

int drop_cnt = 0;
static void
drop_fn(void *arg)
{
    CRA_UNUSED(arg);
    // printf("drop: %d\n", *(int *)arg);
    ++drop_cnt;
}

static void
test_thread_pool4(void)
{
    CraThrdPool tp;
    int        *vals;
    int         i;
    int         num_threads = 10;
    int         num_items = 100;

    cra_thrdpool_init(&tp, num_threads, 10, CRA_THRDPOOL_FULL_DROP_NEWEST);

    vals = (int *)cra_malloc(sizeof(*vals) * num_items);
    bzero(vals, sizeof(*vals) * num_items);
    drop_cnt = 0;

    for (i = 0; i < num_items; i++)
    {
        vals[i] = i;
        cra_thrdpool_add_task1_drop(&tp, drop_fn, worker, vals + i);
    }

    cra_thrdpool_uninit(&tp, true);

    int ncompleted = 0;
    for (i = 0; i < num_items; i++)
    {
        // assert_always(vals[i] == i + PLUS);
        if (vals[i] == i + PLUS)
            ++ncompleted;
        // printf("main: %d\n", vals[i]);
    }
    printf("completed count: %d, drop count: %d\n", ncompleted, drop_cnt);
    assert_always(ncompleted >= 10);
    assert_always(ncompleted + drop_cnt == num_items);

    cra_free(vals);
}

static void
test_thread_pool5(void)
{
    CraThrdPool tp;
    int        *vals;
    int         i;
    int         num_threads = 10;
    int         num_items = 100;

    cra_thrdpool_init(&tp, num_threads, 20, CRA_THRDPOOL_FULL_DROP_OLDEST);

    vals = (int *)cra_malloc(sizeof(*vals) * num_items);
    bzero(vals, sizeof(*vals) * num_items);
    drop_cnt = 0;

    for (i = 0; i < num_items; i++)
    {
        vals[i] = i;
        cra_thrdpool_add_task1_drop(&tp, drop_fn, worker, vals + i);
    }

    cra_thrdpool_uninit(&tp, true);

    int ncompleted = 0;
    for (i = 0; i < num_items; i++)
    {
        // assert_always(vals[i] == i + PLUS);
        if (vals[i] == i + PLUS)
            ++ncompleted;
        // printf("main: %d\n", vals[i]);
    }
    printf("completed count: %d, drop count: %d\n", ncompleted, drop_cnt);
    assert_always(ncompleted >= 20);
    assert_always(ncompleted + drop_cnt == num_items);

    cra_free(vals);
}
int
main(void)
{
    printf("## start test thpool...\n");
    test_thread_pool();
    printf("## end   test thpool...\n\n");
    printf("## start test thpool2...\n");
    test_thread_pool2();
    printf("## end   test thpool2...\n\n");
    printf("## start test thpool3...\n");
    test_thread_pool3();
    printf("## end   test thpool3...\n\n");
    printf("## start test thpool4...\n");
    test_thread_pool4();
    printf("## end   test thpool4...\n\n");
    printf("## start test thpool5...\n");
    test_thread_pool5();
    printf("## end   test thpool5...\n\n");

    cra_memory_leak_report();
    return 0;
}
