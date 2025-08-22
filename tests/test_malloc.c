#include "cra_assert.h"
#include "cra_malloc.h"

void
test_error(void)
{
    void *p;
    CRA_UNUSED_VALUE(p);

    p = __cra_malloc(0);

    p = __cra_calloc(0, 1);
    p = __cra_calloc(1, 0);
    p = __cra_calloc(0, 0);

    // p = __cra_realloc(NULL, 1);
    void *o = __cra_malloc(10);
    p = __cra_realloc(o, 0);

    // __cra_free(NULL);
}

void
test_normal(void)
{
    void *p;

    p = __cra_malloc(1);
    __cra_free(p);
    p = __cra_malloc_dbg(1, __FILE__, __LINE__);
    __cra_free_dbg(p);

    p = __cra_calloc(1, 1);
    __cra_free(p);
    p = __cra_calloc_dbg(1, 1, __FILE__, __LINE__);
    __cra_free_dbg(p);

    void *o = __cra_malloc(10);
    p = __cra_realloc(o, 20);
    p = __cra_realloc_dbg(p, 10, __FILE__, __LINE__);
    __cra_free_dbg(p);

    __cra_memory_leak_report();
}

void
test_malloc_free_dbg1(bool report)
{
    void *p = __cra_malloc(10);
    __cra_free_dbg(p);
    p = __cra_calloc(1, 10);
    __cra_free_dbg(p);
    void *o = __cra_malloc(10);
    p = __cra_realloc(o, 20);
    __cra_free_dbg(p);

    void *p1 = __cra_malloc(10);
    void *p2 = __cra_calloc(1, 10);
    o = __cra_malloc(10);
    void *p3 = __cra_realloc(o, 40);
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

/**
 * 严禁通过__cra_[malloc|calloc|realloc]_dbg()申请内存，让__cra_free()或free()释放。
 */
void
test_malloc_dbg_free(void)
{
    void *p = __cra_malloc_dbg(10, __FILE__, __LINE__);
    __cra_free(p);
    p = __cra_calloc_dbg(1, 10, __FILE__, __LINE__);
    __cra_free(p);
    void *o = __cra_malloc_dbg(10, __FILE__, __LINE__);
    p = __cra_realloc_dbg(o, 20, __FILE__, __LINE__);
    __cra_free(p);

    void *p1 = __cra_malloc_dbg(10, __FILE__, __LINE__);
    void *p2 = __cra_calloc_dbg(1, 10, __FILE__, __LINE__);
    o = __cra_malloc(10); // no _dbg
    void *p3 = __cra_realloc_dbg(o, 40, __FILE__, __LINE__);
    __cra_free(p1);
    __cra_free(p2);
    __cra_free(p3);

    __cra_memory_leak_report();
}

int
main(void)
{
    // test_error();
    test_normal();
    test_malloc_free_dbg1(true);
    test_malloc_free_dbg2();
    test_malloc_dbg_free();

    return 0;
}
