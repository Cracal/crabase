#include "cra_assert.h"
#include "cra_malloc.h"

void
test_error(void)
{
    void *p;
    CRA_UNUSED_VALUE(p);

    // p = __cra_malloc(0);

    // p = __cra_calloc(0, 1);
    // p = __cra_calloc(1, 0);
    // p = __cra_calloc(0, 0);

    // p = __cra_realloc(NULL, 1);
    // void *o = __cra_malloc(10);
    // p = __cra_realloc(o, 0);

    // __cra_free(NULL);

    // p = __cra_malloc(100);
    // __cra_free_dbg(p);
    // p = __cra_calloc(1, 100);
    // __cra_free_dbg(p);

    // p = __cra_malloc_dbg(100, __FILE__, __LINE__);
    // __cra_free(p); // !!ERROR
}

void
test_normal(void)
{
    void *p;

    p = __cra_malloc(1);
    assert_always(p != NULL);
    __cra_free(p);
    p = __cra_malloc_dbg(1, __FILE__, __LINE__);
    assert_always(p != NULL);
    __cra_free_dbg(p);

    p = __cra_calloc(1, 4);
    assert_always(p != NULL);
    assert_always(memcmp(p, "\0\0\0\0", 4) == 0);
    __cra_free(p);
    p = __cra_calloc_dbg(1, 1, __FILE__, __LINE__);
    assert_always(p != NULL);
    assert_always(memcmp(p, "\0", 1) == 0);
    __cra_free_dbg(p);

    p = __cra_malloc(10);
    assert_always(p != NULL);
    p = __cra_realloc(p, 20);
    assert_always(p != NULL);
    __cra_free(p);
    p = __cra_malloc_dbg(40, __FILE__, __LINE__);
    assert_always(p != NULL);
    p = __cra_realloc_dbg(p, 10, __FILE__, __LINE__);
    assert_always(p != NULL);
    __cra_free_dbg(p);

    __cra_memory_leak_report();
}

void
test_malloc_free_dbg1(bool report)
{
    void *p = __cra_malloc_dbg(10, __FILE__, __LINE__);
    __cra_free_dbg(p);
    p = __cra_calloc_dbg(1, 10, __FILE__, __LINE__);
    __cra_free_dbg(p);
    void *o = __cra_malloc_dbg(10, __FILE__, __LINE__);
    p = __cra_realloc_dbg(o, 20, __FILE__, __LINE__);
    __cra_free_dbg(p);

    void *p1 = __cra_malloc_dbg(10, __FILE__, __LINE__);
    void *p2 = __cra_calloc_dbg(1, 10, __FILE__, __LINE__);
    o = __cra_malloc_dbg(10, __FILE__, __LINE__);
    void *p3 = __cra_realloc_dbg(o, 40, __FILE__, __LINE__);
    __cra_free_dbg(p1);
    __cra_free_dbg(p2);
    __cra_free_dbg(p3);

    if (report)
        __cra_memory_leak_report();
}

void
test_malloc_free_dbg2(void)
{
    void *n = __cra_malloc_dbg(100, __FILE__, __LINE__);

    test_malloc_free_dbg1(false);

    __cra_free_dbg(n);

    __cra_memory_leak_report();
}

#if 1 // custom

static void *
my_malloc(size_t size)
{
    printf("custom malloc\n");
    return malloc(size);
}

static void *
my_calloc(size_t num, size_t size)
{
    printf("custom calloc\n");
    return calloc(num, size);
}

static void *
my_realloc(void *oldptr, size_t newsize)
{
    printf("custom realloc\n");
    return realloc(oldptr, newsize);
}

static void
my_free(void *ptr)
{
    printf("custom free\n");
    free(ptr);
}

void
test_custom(void)
{
    cra_set_allocator(my_malloc, my_calloc, my_realloc, my_free);

    void *p = cra_malloc(10);
    assert_always(p != NULL);
    cra_free(p);
    p = cra_calloc(1, 10);
    assert_always(p != NULL);
    assert_always(memcmp(p, "\0\0\0\0\0\0\0\0\0\0", 10) == 0);
    cra_free(p);
    p = cra_malloc(10);
    assert_always(p != NULL);
    p = cra_realloc(p, 20);
    assert_always(p != NULL);
    cra_free(p);
}

#endif

int
main(void)
{
    test_error();
    test_normal();
    test_malloc_free_dbg1(true);
    test_malloc_free_dbg2();
    test_custom();

    return 0;
}
