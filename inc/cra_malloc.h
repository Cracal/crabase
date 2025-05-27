/**
 * @file cra_malloc.h
 * @author Cracal
 * @brief malloc
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_MALLOC_H__
#define __CRA_MALLOC_H__
#include "cra_assert.h"

static inline void *__cra_malloc(size_t size)
{
    assert(size > 0);
    void *ptr = malloc(size);
    assert_always(ptr != NULL);
    return ptr;
}

static inline void *__cra_calloc(size_t num, size_t size)
{
    assert(num > 0 && size > 0);
    void *ptr = calloc(num, size);
    assert_always(ptr != NULL);
    return ptr;
}

static inline void *__cra_realloc(void *oldptr, size_t newsize)
{
    assert(newsize > 0);
    assert_always(oldptr != NULL);
    void *newptr = realloc(oldptr, newsize);
    assert_always(newptr != NULL);
    return newptr;
}

static inline void __cra_free(void *ptr)
{
    assert_always(ptr != NULL);
    free(ptr);
}

CRA_API void *__cra_malloc_dbg(size_t size, char *file, int line);
CRA_API void *__cra_calloc_dbg(size_t num, size_t size, char *file, int line);
CRA_API void *__cra_realloc_dbg(void *ptr, size_t newsize, char *file, int line);
CRA_API void __cra_free_dbg(void *ptr);

CRA_API void __cra_memory_leak_report(FILE *fp);

#ifndef CRA_MEMORY_LEAK_DETECTOR

#define cra_malloc __cra_malloc
#define cra_calloc __cra_calloc
#define cra_realloc __cra_realloc
#define cra_free __cra_free

#define cra_memory_leak_report(...)

#else

#define cra_malloc(_size) __cra_malloc_dbg(_size, __FILE__, __LINE__)
#define cra_calloc(_num, _size) __cra_calloc_dbg(_num, _size, __FILE__, __LINE__)
#define cra_realloc(_ptr, _newsize) __cra_realloc_dbg(_ptr, _newsize, __FILE__, __LINE__)
#define cra_free __cra_free_dbg

#define cra_memory_leak_report __cra_memory_leak_report

#endif

#define cra_alloc(_Type) (_Type *)cra_malloc(sizeof(_Type))
#define cra_dealloc cra_free

#endif