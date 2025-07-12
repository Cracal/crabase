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
#include "cra_assert.h"

void *__cra_malloc(size_t size)
{
    assert(size > 0);
    void *ptr = malloc(size);
    assert_always(ptr != NULL);
    return ptr;
}

void *__cra_calloc(size_t num, size_t size)
{
    assert(num > 0 && size > 0);
    void *ptr = calloc(num, size);
    assert_always(ptr != NULL);
    return ptr;
}

void *__cra_realloc(void *oldptr, size_t newsize)
{
    assert(oldptr != NULL && newsize > 0);
    void *newptr = realloc(oldptr, newsize);
    assert_always(newptr != NULL);
    return newptr;
}

void __cra_free(void *ptr)
{
    assert(ptr != NULL);
    free(ptr);
}

typedef struct _CraMallocBlkNode
{
    int line;
    char *file;
    size_t size;
    void *block;
    struct _CraMallocBlkNode *next;
} CraMallocBlkNode;

static CraMallocBlkNode *__s_malloc_memblk_head = NULL;
static cra_atomic_flag __s_malloc_memblk_lock = CRA_ATOMIC_FLAG_INIT;
#define CRA_MALLOC_LOCK() \
    while (cra_atomic_flag_test_and_set(&__s_malloc_memblk_lock))
#define CRA_MALLOC_UNLOCK() \
    cra_atomic_flag_clear(&__s_malloc_memblk_lock)

static inline void __cra_malloc_set_block(void *ptr, size_t size, char *file, int line)
{
    CraMallocBlkNode *node = (CraMallocBlkNode *)__cra_malloc(sizeof(CraMallocBlkNode));
    node->line = line;
    node->file = file;
    node->size = size;
    node->block = ptr;

    CRA_MALLOC_LOCK();
    node->next = __s_malloc_memblk_head;
    __s_malloc_memblk_head = node;
    CRA_MALLOC_UNLOCK();
}

void *__cra_malloc_dbg(size_t size, char *file, int line)
{
    void *ptr = __cra_malloc(size);
    __cra_malloc_set_block(ptr, size, file, line);
    return ptr;
}

void *__cra_calloc_dbg(size_t num, size_t size, char *file, int line)
{
    void *ptr = __cra_calloc(num, size);
    __cra_malloc_set_block(ptr, num * size, file, line);
    return ptr;
}

void *__cra_realloc_dbg(void *ptr, size_t newsize, char *file, int line)
{
    void *newptr = __cra_realloc(ptr, newsize);

    CRA_MALLOC_LOCK();
    CraMallocBlkNode *curr = __s_malloc_memblk_head;
    for (; curr != NULL; curr = curr->next)
    {
        if (curr->block == ptr)
        {
            curr->size = newsize;
            curr->block = newptr;
            break;
        }
    }
    CRA_MALLOC_UNLOCK();
    if (curr == NULL)
        __cra_malloc_set_block(newptr, newsize, file, line);
    return newptr;
}

void __cra_free_dbg(void *ptr)
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

            __cra_free(curr->block);
            __cra_free(curr);
            break;
        }
        last = curr;
        curr = curr->next;
    }
    CRA_MALLOC_UNLOCK();

    // ptr不是通过`cra_malloc()`和`cra_realloc()`申请的内存
    if (curr == NULL)
        __cra_free(ptr);
}

void __cra_memory_leak_report(void)
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
            fprintf(stderr, "memory leak (0x%zx, size: %zu) in %s:%d.\n",
                    (size_t)curr->block, curr->size, curr->file, curr->line);
        }
        fprintf(stderr, "leak memory count: %d.\n", count);
    }
    CRA_MALLOC_UNLOCK();
}
