#include "cra_assert.h"
#include "cra_malloc.h"
#include "cra_mempool.h"
#include "time.h"

void
test_mempool(void)
{
    int       *pi1, *pi2, *pi3, *pi4, *pi5, *pi6, *pi7;
    CraMemPool pool;

    // cra_mempool_init(NULL, 1, 1);
    // cra_mempool_init(&pool, 0, 1);
    // cra_mempool_init(&pool, 1, 0);

    cra_mempool_init(&pool, sizeof(int), 4, 1);

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

    cra_mempool_dealloc(&pool, pi1);
    cra_mempool_dealloc(&pool, pi3);

    cra_mempool_uninit(&pool);
}

int
main(void)
{
    test_mempool();

    cra_memory_leak_report();
    return 0;
}
