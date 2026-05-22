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
#include "cra_defs.h"

#if defined(CRA_COMPILER_GNUC)
#define ATTR_MALLOC __attribute__((malloc, alloc_size(1), warn_unused_result))
#define ATTR_CALLOC __attribute__((malloc, alloc_size(1, 2), warn_unused_result))
#define ATTR_REALLO __attribute__((nonnull(1), alloc_size(2), warn_unused_result))
#define ATTR_FREEEE __attribute__((nonnull(1)))
#elif defined(CRA_COMPILER_MSVC)
#define ATTR_MALLOC _Check_return_ _CRTALLOCATOR
#define ATTR_CALLOC _Check_return_ _CRTALLOCATOR
#define ATTR_REALLO _Check_return_ _CRTALLOCATOR
#define ATTR_FREEEE
#else
#define ATTR_MALLOC
#define ATTR_CALLOC
#define ATTR_REALLO
#define ATTR_FREEEE
#endif

CRA_API void *(*__cra_malloc_fn__)(size_t size);
CRA_API void *(*__cra_calloc_fn__)(size_t num, size_t size);
CRA_API void *(*__cra_reallo_fn__)(void *oldptr, size_t newsize);
CRA_API void  (*__cra_freeee_fn__)(void *ptr);

static inline ATTR_MALLOC void *
__cra_malloc(size_t size)
{
    assert(size > 0);
    if (__cra_malloc_fn__ == NULL)
        __cra_malloc_fn__ = malloc;
    return __cra_malloc_fn__(size);
}

static inline ATTR_CALLOC void *
__cra_calloc(size_t num, size_t size)
{
    assert(num > 0 && size > 0);
    if (__cra_calloc_fn__ == NULL)
        __cra_calloc_fn__ = calloc;
    return __cra_calloc_fn__(num, size);
}
static inline ATTR_REALLO void *
__cra_realloc(void *oldptr, size_t newsize)
{
    assert(newsize > 0);
#ifdef CRA_COMPILER_MSVC
    assert(oldptr != NULL);
#endif
    if (__cra_reallo_fn__ == NULL)
        __cra_reallo_fn__ = realloc;
    return __cra_reallo_fn__(oldptr, newsize);
}
static inline ATTR_FREEEE void
__cra_free(void *ptr)
{
#ifdef CRA_COMPILER_MSVC
    assert(ptr != NULL);
#endif
    if (__cra_freeee_fn__ == NULL)
        __cra_freeee_fn__ = free;
    __cra_freeee_fn__(ptr);
}

static inline void
cra_set_allocator(void *(*malloc_fn)(size_t),
                  void *(*calloc_fn)(size_t, size_t),
                  void *(*realloc_fn)(void *, size_t),
                  void  (*free_fn)(void *))
{
    assert(malloc_fn != NULL && calloc_fn != NULL && realloc_fn != NULL && free_fn != NULL);
    __cra_malloc_fn__ = malloc_fn;
    __cra_calloc_fn__ = calloc_fn;
    __cra_reallo_fn__ = realloc_fn;
    __cra_freeee_fn__ = free_fn;
}

#undef ATTR_MALLOC
#undef ATTR_CALLOC
#undef ATTR_REALLO
#undef ATTR_FREEEE

CRA_API void *
__cra_malloc_dbg(size_t size, char *file, int line);

CRA_API void *
__cra_calloc_dbg(size_t num, size_t size, char *file, int line);

CRA_API void *
__cra_realloc_dbg(void *ptr, size_t newsize, char *file, int line);

CRA_API void
__cra_free_dbg(void *ptr);

CRA_API void
__cra_memory_leak_report(void);

#ifndef CRA_MEMORY_LEAK_DETECTOR

#define cra_malloc  __cra_malloc
#define cra_calloc  __cra_calloc
#define cra_realloc __cra_realloc
#define cra_free    __cra_free

#define cra_memory_leak_report()

#else

#define cra_malloc(_size)           __cra_malloc_dbg(_size, __FILE__, __LINE__)
#define cra_calloc(_num, _size)     __cra_calloc_dbg(_num, _size, __FILE__, __LINE__)
#define cra_realloc(_ptr, _newsize) __cra_realloc_dbg(_ptr, _newsize, __FILE__, __LINE__)
#define cra_free                    __cra_free_dbg

#define cra_memory_leak_report __cra_memory_leak_report

#endif

#define cra_alloc(_Type) cra_malloc(sizeof(_Type))
#define cra_dealloc      cra_free

#define cra_new(_Type) (_Type *)cra_alloc(_Type)
#define cra_delete     cra_dealloc

#endif