/**
 * @file cra_serialize.c
 * @author Cracal
 * @brief serializer
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#define __CRA_SER_INNER
#include "serialize/cra_serialize.h"
#include "cra_assert.h"
#include "cra_malloc.h"

void
cra_release_mgr_uninit(CraReleaseMgr *mgr, bool free_ptr)
{
    CraReleaseBlk *blk;
    size_t         nnodes1;
    if (free_ptr && mgr->count > 0)
    {
        nnodes1 = CRA_NARRAY(mgr->nodes1);
        for (size_t i = 0; i < mgr->count; ++i)
        {
            blk = i < nnodes1 ? mgr->nodes1 + i : mgr->nodes2 + (i - nnodes1);
            if (blk->meta->init_i && blk->meta->init_i->uninit)
                blk->meta->init_i->uninit(blk->ptr, blk->meta);
            if (blk->meta->is_ptr)
                cra_free(blk->ptr);
        }
    }
    if (mgr->nodes2)
        cra_free(mgr->nodes2);
}

void
cra_release_mgr_add(CraReleaseMgr *mgr, void *ptr, const CraTypeMeta *meta)
{
    CraReleaseBlk *blk;
    size_t         nnodes1;

    assert(ptr);
    assert(meta);

    nnodes1 = CRA_NARRAY(mgr->nodes1);
    if (mgr->count == mgr->size)
    {
        if (mgr->nodes2)
            mgr->nodes2 = cra_realloc(mgr->nodes2, sizeof(CraReleaseBlk) * (mgr->size)); // X: mgr->size + nndoes1
        else
            mgr->nodes2 = cra_malloc(sizeof(CraReleaseBlk) * nnodes1);
        mgr->size += nnodes1;
    }

    blk = mgr->count < nnodes1 ? mgr->nodes1 + mgr->count : mgr->nodes2 + (mgr->count - nnodes1);
    blk->meta = meta;
    blk->ptr = ptr;

    ++mgr->count;
}
