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
#include "cra_malloc.h"
#include "threads/cra_thrdpool.h"

#define PLUS 1000

static void worker(const CraThrPoolArgs1 *args)
{
    int *val = (int *)args->arg1;
    int old = *val;

    *val += PLUS;
    printf("Worker: tid=%lu, old=%d, val=%d\n", args->tid, old, *val);

    // if (*val % 2)
    //     cra_msleep(100);
}

static void test_thread_pool(void)
{
    CraThrPool *tp;
    int *vals;
    int i;
    int num_threads = 10;
    int num_items = 1000;

    tp = cra_alloc(CraThrPool);
    assert_always(!!tp);
    cra_thrdpool_init(tp, num_threads, num_items);

    vals = cra_malloc(sizeof(*vals) * num_items);
    bzero(vals, sizeof(*vals) * num_items);

    for (i = 0; i < num_items; i++)
    {
        vals[i] = i;
        cra_thrdpool_add_task1(tp, worker, vals + i);
    }

    cra_thrdpool_wait(tp);

    cra_thrdpool_uninit(tp);
    cra_dealloc(tp);

    for (i = 0; i < num_items; i++)
    {
        assert_always(vals[i] == i + PLUS);
        // printf("main: %d\n", vals[i]);
    }

    cra_free(vals);
}

static void test_thread_pool2(void)
{
    CraThrPool *tp;
    int *vals;
    int i;
    int num_threads = 10;
    int num_items = 100;

    tp = cra_alloc(CraThrPool);
    cra_thrdpool_init(tp, num_threads, 10);
    // cra_thrdpool_set_discard_policy(tp, CRA_TPTASK_DISCARD_LAST);
    cra_thrdpool_set_discard_policy(tp, CRA_TPTASK_DISCARD_FIRST);
    // cra_thrdpool_set_discard_policy(tp, CRA_TPTASK_DISCARD_SELF);
    vals = cra_malloc(sizeof(*vals) * num_items);
    bzero(vals, sizeof(*vals) * num_items);

    for (i = 0; i < num_items; i++)
    {
        vals[i] = i;
        cra_thrdpool_add_task1(tp, worker, vals + i);
    }

    cra_thrdpool_wait(tp);

    cra_thrdpool_uninit(tp);
    cra_dealloc(tp);

    for (i = 0; i < num_items; i++)
    {
        printf("main: %d\n", vals[i]);
    }

    cra_free(vals);
}

static void test_thread_pool3(void)
{
    CraThrPool *tp;
    int *vals;
    int i;
    int num_threads = 10;
    int num_items = 100;

    tp = cra_alloc(CraThrPool);
    cra_thrdpool_init(tp, num_threads, num_items);
    cra_thrdpool_set_discard_policy(tp, CRA_TPTASK_DISCARD_FIRST);
    vals = cra_malloc(sizeof(*vals) * num_items);
    bzero(vals, sizeof(*vals) * num_items);

    for (i = 0; i < num_items; i++)
    {
        vals[i] = i;
        cra_thrdpool_add_task1(tp, worker, vals + i);
    }

    // cra_thrdpool_wait(tp);

    cra_thrdpool_uninit(tp);
    cra_dealloc(tp);

    for (i = 0; i < num_items; i++)
    {
        printf("main: %d\n", vals[i]);
    }

    cra_free(vals);
}

int main(void)
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

    cra_memory_leak_report(stdout);
    return 0;
}
