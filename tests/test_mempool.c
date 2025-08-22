#include "cra_assert.h"
#include "cra_malloc.h"
#include "cra_mempool.h"
#include "threads/cra_thread.h"
#include "time.h"

void
test_mempool(void)
{
    int       *pi1, *pi2, *pi3, *pi4, *pi5, *pi6, *pi7;
    CraMemPool pool;

    // cra_mempool_init(NULL, 1, 1);
    // cra_mempool_init(&pool, 0, 1);
    // cra_mempool_init(&pool, 1, 0);

    cra_mempool_init(&pool, sizeof(int), 4);

    pi1 = (int *)cra_mempool_alloc(&pool);
    assert_always(pi1 != NULL);
    *pi1 = 1;
    pi2 = (int *)cra_mempool_alloc(&pool);
    assert_always(pi2 != NULL);
    *pi2 = 2;
    pi3 = (int *)cra_mempool_alloc(&pool);
    assert_always(pi3 != NULL);
    *pi3 = 3;
    pi4 = (int *)cra_mempool_alloc(&pool);
    assert_always(pi4 != NULL);
    *pi4 = 4;
    pi5 = (int *)cra_mempool_alloc(&pool);
    assert_always(pi5 != NULL);
    *pi5 = 5;
    pi6 = (int *)cra_mempool_alloc(&pool);
    assert_always(pi6 != NULL);
    *pi6 = 6;
    pi7 = (int *)cra_mempool_alloc(&pool);
    assert_always(pi7 != NULL);
    *pi7 = 7;

    assert_always(*pi1 == 1);
    assert_always(*pi2 == 2);
    assert_always(*pi3 == 3);
    assert_always(*pi4 == 4);
    assert_always(*pi5 == 5);
    assert_always(*pi6 == 6);
    assert_always(*pi7 == 7);

    cra_mempool_dealloc(&pool, pi1);
    cra_mempool_dealloc(&pool, pi2);
    cra_mempool_dealloc(&pool, pi3);
    cra_mempool_dealloc(&pool, pi4);
    cra_mempool_dealloc(&pool, pi5);
    cra_mempool_dealloc(&pool, pi6);
    cra_mempool_dealloc(&pool, pi7);

    pi1 = (int *)cra_mempool_alloc(&pool);
    pi2 = (int *)cra_mempool_alloc(&pool);
    *pi1 = 100;
    *pi2 = 200;
    assert_always(*pi1 == 100);
    assert_always(*pi2 == 200);

    cra_mempool_dealloc(&pool, pi1);
    pi3 = (int *)cra_mempool_alloc(&pool);
    *pi3 = 300;
    assert_always(*pi3 == 300);

    cra_mempool_uninit(&pool);
}

CRA_THRD_FUNC(sub_thread)
{
    int        *pi;
    int         val;
    CraMemPool *pool;

    pool = (CraMemPool *)arg;
    srand((unsigned int)time(NULL));
    val = rand();
    for (int i = 0; i < 10; i++)
    {
        pi = (int *)cra_mempool_alloc(pool);
        assert_always(pi != NULL);
        *pi = val;
        cra_msleep((val % 300) + 50);
        assert_always(*pi == val);
        cra_mempool_dealloc(pool, pi);
        val = rand();
    }
    return (cra_thrd_ret_t){ 0 };
}

void
test_multi_threads(void)
{
    cra_thrd_t th1, th2;
    CraMemPool pool;
    cra_mempool_init(&pool, sizeof(int), 10);

    cra_thrd_create(&th1, sub_thread, &pool);
    cra_thrd_create(&th2, sub_thread, &pool);

    cra_thrd_join(th1);
    cra_thrd_join(th2);

    cra_mempool_uninit(&pool);
}

int
main(void)
{
    test_mempool();
    test_multi_threads();

    cra_memory_leak_report();
    return 0;
}
