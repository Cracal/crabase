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

void *(*__cra_malloc_fn__)(size_t size) = NULL;
void *(*__cra_calloc_fn__)(size_t num, size_t size) = NULL;
void *(*__cra_reallo_fn__)(void *oldptr, size_t newsize) = NULL;
void  (*__cra_freeee_fn__)(void *ptr) = NULL;

// ===========================================

typedef struct CraMallocBlkNode
{
    int                      line;
    char                    *file;
    size_t                   size;
    struct CraMallocBlkNode *next;
    char                     block[];
} CraMallocBlkNode;

static CraMallocBlkNode *__s_malloc_memblk_head = NULL;
static cra_atomic_flag_t __s_malloc_memblk_lock = CRA_ATOMIC_FLAG_INIT;
#define CRA_MALLOC_LOCK()   while (cra_atomic_flag_test_and_set(&__s_malloc_memblk_lock))
#define CRA_MALLOC_UNLOCK() cra_atomic_flag_clear(&__s_malloc_memblk_lock)

static inline void
__cra_malloc_set_block(CraMallocBlkNode *node, size_t size, char *file, int line)
{
    node->line = line;
    node->file = file;
    node->size = size;

    CRA_MALLOC_LOCK();
    node->next = __s_malloc_memblk_head;
    __s_malloc_memblk_head = node;
    CRA_MALLOC_UNLOCK();
}

void *
__cra_malloc_dbg(size_t size, char *file, int line)
{
    CraMallocBlkNode *node = (CraMallocBlkNode *)__cra_malloc(sizeof(CraMallocBlkNode) + size);
    if (node != NULL)
    {
        __cra_malloc_set_block(node, size, file, line);
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
    void             *ret;
    CraMallocBlkNode *curr;
    CraMallocBlkNode *last;
    CraMallocBlkNode *node;

    CRA_UNUSED_VALUE(file);
    CRA_UNUSED_VALUE(line);

    ret = NULL;
    CRA_MALLOC_LOCK();
    for (last = NULL, curr = __s_malloc_memblk_head; curr != NULL; last = curr, curr = curr->next)
    {
        if (curr->block == ptr)
            break;
    }

    if (curr != NULL)
    {
        node = (CraMallocBlkNode *)__cra_realloc(curr, sizeof(CraMallocBlkNode) + newsize);
        if (node != NULL)
        {
            node->size = newsize;
            if (last != NULL)
                last->next = node;
            else
                __s_malloc_memblk_head = node;
            ret = node->block;
        }
    }
    CRA_MALLOC_UNLOCK();

    return ret;
}

void
__cra_free_dbg(void *ptr)
{
    CraMallocBlkNode *last, *curr;

    last = NULL;
    CRA_MALLOC_LOCK();
    curr = __s_malloc_memblk_head;
    while (curr != NULL)
    {
        if (curr->block == ptr)
        {
            if (last == NULL)
                __s_malloc_memblk_head = curr->next;
            else
                last->next = curr->next;

            __cra_free(curr);

            CRA_MALLOC_UNLOCK();
            return;
        }
        last = curr;
        curr = curr->next;
    }
    CRA_MALLOC_UNLOCK();

    fprintf(stderr, "__cra_free_dbg() failed. ptr: 0x%zx is not malloced.\n", (size_t)ptr);
    exit(EXIT_FAILURE);
}

void
__cra_memory_leak_report(void)
{
    int count = 0;
    CRA_MALLOC_LOCK();
    if (!__s_malloc_memblk_head)
    {
        fprintf(stderr, "no memory leak.\n");
    }
    else
    {
        CraMallocBlkNode *curr = __s_malloc_memblk_head;
        for (; curr != NULL; curr = curr->next)
        {
            ++count;
            fprintf(stderr,
                    "memory leak (0x%zx, size: %zu) in %s:%d.\n",
                    (size_t)curr->block,
                    curr->size,
                    curr->file,
                    curr->line);
        }
        fprintf(stderr, "leak memory count: %d.\n", count);
    }
    CRA_MALLOC_UNLOCK();
}
