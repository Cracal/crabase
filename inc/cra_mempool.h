/**
 * @file cra_mempool.h
 * @author Cracal
 * @brief a simple memory pool
 * @version 0.1
 * @date 2025-02-07
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __CRA_MEMPOOL_H__
#define __CRA_MEMPOOL_H__
#include "collections/cra_alist.h"

typedef struct _CraMemPool
{
    size_t   item_size;
    size_t   items_per_block;
    CraAList blocks; // AList<BLOCK *>
    CraAList stack;  // AList<ITEM *>
} CraMemPool;

CRA_API void
cra_mempool_init(CraMemPool *pool, size_t item_size, size_t items_per_block, size_t init_block);

CRA_API void
cra_mempool_uninit(CraMemPool *pool);

// no check for in-use items is performed
CRA_API void
cra_mempool_uninit_no_check(CraMemPool *pool);

CRA_API void *
cra_mempool_alloc(CraMemPool *pool);

CRA_API void
cra_mempool_dealloc(CraMemPool *pool, void *ptr);

#endif