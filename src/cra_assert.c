#include "cra_assert.h"

static void
cra_assert_default_func(const char *expression, const char *fname, const char *file, int line)
{
    fprintf(stderr, "assert failed: `%s`, %s:%d:%s\n", expression, file, line, fname);
    fflush(stderr);

#if defined(CRA_COMPILER_GNUC)
    __builtin_trap();
#elif defined(CRA_COMPILER_MSVC)
    __debugbreak();
#endif
    abort();
}

void (*__cra_g_assert__)(const char *, const char *, const char *, int) = cra_assert_default_func;
