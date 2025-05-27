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
#include "cra_atomic.h"
#include "collections/cra_deque.h"

typedef struct _CraMemPool
{
    cra_atomic_flag locker;
    unsigned int count;
    size_t itemsize;
    unsigned char *memory;
    CraDeque stack; // CraDeque<void *>
} CraMemPool;

CRA_API void cra_mempool_init(CraMemPool *pool, size_t itemsize, unsigned int count);
CRA_API void cra_mempool_uninit(CraMemPool *pool);

CRA_API void *cra_mempool_alloc(CraMemPool *pool);
CRA_API void cra_mempool_dealloc(CraMemPool *pool, void *ptr);

#endif