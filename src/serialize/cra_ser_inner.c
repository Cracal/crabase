/**
 * @file cra_ser-inner.c
 * @author Cracal
 * @brief inner
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "serialize/cra_ser_inner.h"
#include "cra_assert.h"
#include "cra_malloc.h"

void
cra_serializer_extend_buf(CraSerializer *ser, size_t needed)
{
    assert(ser);
    assert(needed > 0);
    assert(ser->buffer);
    assert(!ser->noalloc);

    size_t newsize = ser->length << 1;
    if (newsize - ser->index < needed)
        newsize += needed;
    ser->buffer = (unsigned char *)cra_realloc(ser->buffer, newsize);
    ser->length = newsize;
}

#define CRA_SER_RELEASE_NODES1_MAX (sizeof(release->nodes1) / sizeof(release->nodes1[0]))

void
cra_ser_release_init(CraSerRelease *release)
{
    // release->current = 0;
    release->n_nodes = CRA_SER_RELEASE_NODES1_MAX;
    // bzero(release->nodes1, sizeof(release->nodes1));
    // release->nodes2 = NULL;
}

void
cra_ser_release_uninit(CraSerRelease *release, bool free_ptr)
{
    if (release->current > 0)
    {
        if (free_ptr)
        {
            CraSerReleaseNode *node;
            for (size_t i = 0; i < release->current; ++i)
            {
                if (i < CRA_SER_RELEASE_NODES1_MAX)
                    node = &release->nodes1[i];
                else
                    node = &release->nodes2[i];

                if (node->uninit)
                    node->uninit(node->ptr);
                if (node->dealloc)
                    node->dealloc(node->ptr);
            }
        }
        if (release->nodes2)
            cra_free(release->nodes2);
        // bzero(release, sizeof(CraSerRelease));
    }
}

void
cra_ser_release_add(CraSerRelease *release, void *ptr, void (*uninit_fn)(void *), void (*dealloc_fn)(void *))
{
    CraSerReleaseNode *node;
    if (release->current == release->n_nodes)
    {
        release->n_nodes <<= 1; // * 2
        if (release->nodes2 == NULL)
        {
            release->nodes2 = (CraSerReleaseNode *)cra_malloc(sizeof(CraSerReleaseNode) *
                                                              (release->n_nodes - CRA_SER_RELEASE_NODES1_MAX));
        }
        else
        {
            release->nodes2 = (CraSerReleaseNode *)cra_realloc(
              release->nodes2, sizeof(CraSerReleaseNode) * (release->n_nodes - CRA_SER_RELEASE_NODES1_MAX));
        }
    }
    if (release->n_nodes <= CRA_SER_RELEASE_NODES1_MAX)
        node = &release->nodes1[release->current++];
    else
        node = &release->nodes2[release->current++ - CRA_SER_RELEASE_NODES1_MAX];
    node->ptr = ptr;
    node->uninit = uninit_fn;
    node->dealloc = dealloc_fn;
}
