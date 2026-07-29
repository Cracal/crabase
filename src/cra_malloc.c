/**
 * @file cra_malloc.c
 * @author Cracal
 * @brief malloc
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "cra_malloc.h"
#include "cra_atomic.h"

void *(*__cra_malloc_fn__)(size_t) = NULL;
void *(*__cra_calloc_fn__)(size_t, size_t) = NULL;
void *(*__cra_reallo_fn__)(void *, size_t) = NULL;
void  (*__cra_freeee_fn__)(void *) = NULL;

// ===========================================

typedef struct CraMallocData CraMallocData;
typedef struct CraMallocCB   CraMallocCB;

struct CraMallocData
{
    CraMallocCB       *mem_list;
    cra_atomic_flag_t  mem_lock;
    cra_atomic_int32_t nalloc;
    cra_atomic_int32_t nfree;
    cra_atomic_int64_t alloc_bytes;
    cra_atomic_int64_t free_bytes;
};

struct CraMallocCB
{
    CraMallocCB *next;
    size_t       size;
    char        *file;
    int          line;
    char         block[];
};

static CraMallocData s_mdata = { 0 };

#define CRA_MALLOC_LOCK()   while (cra_atomic_flag_test_and_set(&s_mdata.mem_lock, CRA_MO_ACQUIRE))
#define CRA_MALLOC_UNLOCK() cra_atomic_flag_clear(&s_mdata.mem_lock, CRA_MO_RELEASE)

static inline void
__cra_malloc_set_block(CraMallocCB *node, size_t size, char *file, int line)
{
    node->size = size;
    node->line = line;
    node->file = file;

    CRA_MALLOC_LOCK();
    node->next = s_mdata.mem_list;
    s_mdata.mem_list = node;
    CRA_MALLOC_UNLOCK();
}

void *
__cra_malloc_dbg(size_t size, char *file, int line)
{
    CraMallocCB *node = (CraMallocCB *)__cra_malloc(sizeof(CraMallocCB) + size);
    if (node != NULL)
    {
        __cra_malloc_set_block(node, size, file, line);
        cra_atomic_inc(&s_mdata.nalloc, CRA_MO_RELAXED);
        cra_atomic_add(&s_mdata.alloc_bytes, size, CRA_MO_RELAXED);
        return node->block;
    }
    return NULL;
}

void *
__cra_calloc_dbg(size_t num, size_t size, char *file, int line)
{
    void *ptr = __cra_malloc_dbg(num * size, file, line);
    if (ptr != NULL)
        bzero(ptr, num * size);
    return ptr;
}

void *
__cra_realloc_dbg(void *ptr, size_t newsize, char *file, int line)
{
    void        *ret;
    size_t       diff;
    CraMallocCB *curr;
    CraMallocCB *last;
    CraMallocCB *node;

    CRA_UNUSED(file);
    CRA_UNUSED(line);

    ret = NULL;

    CRA_MALLOC_LOCK();

    for (last = NULL, curr = s_mdata.mem_list; curr != NULL; last = curr, curr = curr->next)
    {
        if (curr->block == ptr)
            break;
    }

    assert_always(curr != NULL);

    diff = newsize - curr->size;

    node = (CraMallocCB *)__cra_realloc(curr, sizeof(CraMallocCB) + newsize);
    if (node != NULL)
    {
        node->size = newsize;
        if (last != NULL)
            last->next = node;
        else
            s_mdata.mem_list = node;
        ret = node->block;
    }

    CRA_MALLOC_UNLOCK();

    cra_atomic_add(&s_mdata.alloc_bytes, diff, CRA_MO_RELAXED);

    return ret;
}

void
__cra_free_dbg(void *ptr)
{
    size_t       size;
    CraMallocCB *last, *curr;

    last = NULL;
    CRA_MALLOC_LOCK();
    curr = s_mdata.mem_list;
    while (curr != NULL)
    {
        if (curr->block == ptr)
        {
            if (last == NULL)
                s_mdata.mem_list = curr->next;
            else
                last->next = curr->next;

            size = curr->size;
            __cra_free(curr);

            CRA_MALLOC_UNLOCK();

            cra_atomic_inc(&s_mdata.nfree, CRA_MO_RELAXED);
            cra_atomic_add(&s_mdata.free_bytes, size, CRA_MO_RELAXED);
            return;
        }
        last = curr;
        curr = curr->next;
    }
    CRA_MALLOC_UNLOCK();

    assert_always(false && "double free");
}

void
__cra_memory_leak_report(void)
{
    int count = 0;
    CRA_MALLOC_LOCK();
    if (!s_mdata.mem_list)
    {
        printf("no memory leak.\n\n");
    }
    else
    {
        CraMallocCB *curr = s_mdata.mem_list;
        for (; curr != NULL; curr = curr->next)
        {
            ++count;
            printf("memory leak (0x%zx, size: %zu) at %s:%d.\n", (size_t)curr->block, curr->size, curr->file,
                   curr->line);
        }
        printf("leak memory count: %d.\n\n", count);
    }
    CRA_MALLOC_UNLOCK();

    printf("%" PRId32 " allocs, %" PRId32 " frees\n", s_mdata.nalloc, s_mdata.nfree);
    printf("%" PRId64 " bytes allocated, %" PRId64 " bytes freed\n\n", s_mdata.alloc_bytes, s_mdata.free_bytes);
}
