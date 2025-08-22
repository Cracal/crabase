#include "cra_mempool.h"
#include "cra_assert.h"
#include "cra_malloc.h"

#define CRA_MEMPOOL_LOCK   while (cra_atomic_flag_test_and_set(&pool->locker))
#define CRA_MEMPOOL_UNLOCK cra_atomic_flag_clear(&pool->locker)

void
cra_mempool_init(CraMemPool *pool, size_t itemsize, unsigned int count)
{
    void *ptr;

    assert(pool != NULL);
    assert(itemsize > 0);
    assert(count > 0);

    CRA_MEMPOOL_UNLOCK; // init locker
    pool->count = count;
    pool->itemsize = itemsize;
    pool->memory = cra_malloc(itemsize * count);
    cra_deque_init0(void *, &pool->stack, CRA_DEQUE_INFINITE, false, NULL);
    for (unsigned int i = 0; i < count; ++i)
    {
        ptr = pool->memory + i * itemsize;
        cra_deque_push(&pool->stack, &ptr);
    }
}

void
cra_mempool_uninit(CraMemPool *pool)
{
    CRA_MEMPOOL_LOCK;
    cra_free(pool->memory);
    cra_deque_uninit(&pool->stack);
}

void *
cra_mempool_alloc(CraMemPool *pool)
{
    void *ret;
    CRA_MEMPOOL_LOCK;
    if (!cra_deque_pop(&pool->stack, &ret))
        ret = cra_malloc(pool->itemsize);
    CRA_MEMPOOL_UNLOCK;
    return ret;
}

void
cra_mempool_dealloc(CraMemPool *pool, void *ptr)
{
    if (ptr >= (void *)pool->memory && ptr < (void *)(pool->memory + pool->itemsize * pool->count))
    {
        CRA_MEMPOOL_LOCK;
        cra_deque_push(&pool->stack, &ptr);
        CRA_MEMPOOL_UNLOCK;
    }
    else
    {
        cra_free(ptr);
    }
}
