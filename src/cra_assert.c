#include "cra_assert.h"

static void
cra_assert_default_func(const char *condition, const char *fname, const char *file, int line)
{
    fprintf(stderr, "assert failed: `%s`, %s:%d:%s\n", condition, file, line, fname);
#if defined(CRA_COMPILER_GNUC)
    __builtin_trap();
#elif defined(CRA_COMPILER_MSVC)
    __debugbreak();
#endif
    exit(1);
}

void (*__cra_assert_func)(const char *cond, const char *fname, const char *file, int line) = cra_assert_default_func;

void
cra_assert_set_func(void (*func)(const char *condition, const char *fname, const char *file, int line))
{
    __cra_assert_func = func;
}