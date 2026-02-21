#include "cra_mempool.h"
#include "cra_assert.h"
#include "cra_malloc.h"

static bool
cra_mempool_make_block(CraMemPool *pool)
{
    char *block;
    char *item;

    // alloc & zero memory
    block = (char *)cra_calloc(pool->items_per_block, pool->item_size);
    if (block == NULL)
        return false;
    if (!cra_alist_append(&pool->blocks, &block))
        return false;
    for (size_t j = 0; j < pool->items_per_block; ++j)
    {
        item = block + (j * pool->item_size);
        if (!cra_alist_append(&pool->stack, &item))
            return false;
    }
    return true;
}

bool
cra_mempool_init(CraMemPool *pool, size_t item_size, size_t items_per_block, size_t init_block)
{
    assert(pool != NULL);
    assert(item_size > 0);
    assert(init_block > 0);
    assert(items_per_block > 0);

    pool->item_size = item_size;
    pool->items_per_block = items_per_block;
    if (!cra_alist_init_size0(void *, &pool->blocks, init_block, false))
        return false;
    if (!cra_alist_init_size0(void *, &pool->stack, init_block *items_per_block, false))
        return false;

    for (size_t i = 0; i < init_block; ++i)
        if (!cra_mempool_make_block(pool))
            return false;

    return true;
}

void
cra_mempool_uninit_no_check(CraMemPool *pool)
{
    CraAListIter it;
    void       **pblock;

    assert(pool);

    for (cra_alist_iter_init(&pool->blocks, &it); cra_alist_iter_next(&it, (void **)&pblock);)
        cra_free(*pblock);
    cra_alist_uninit(&pool->blocks);
    cra_alist_uninit(&pool->stack);
}

void
cra_mempool_uninit(CraMemPool *pool)
{
    size_t nmax;
    size_t nstack;

    assert(pool);

    nstack = cra_alist_get_count(&pool->stack);
    nmax = cra_alist_get_count(&pool->blocks) * pool->items_per_block;
    if (nstack != nmax)
    {
        fprintf(stderr, "mempool_uninit() error: %zu items are actively being used.", nmax - nstack);
        exit(EXIT_FAILURE);
    }

    cra_mempool_uninit_no_check(pool);
}

void *
cra_mempool_alloc(CraMemPool *pool)
{
    assert(pool);
    void *ret = NULL;
    if (!cra_alist_pop_back(&pool->stack, &ret))
    {
        if (cra_mempool_make_block(pool))
            cra_alist_pop_back(&pool->stack, &ret);
    }
    return ret;
}

void
cra_mempool_dealloc(CraMemPool *pool, void *ptr)
{
    assert(pool);
    assert(ptr);
    cra_alist_append(&pool->stack, &ptr);
}
