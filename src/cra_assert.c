#include "cra_assert.h"

static void
cra_assert_default_func(const char *condition, const char *fname, const char *file, int line)
{
    fprintf(stderr, "assert failed: `%s`, %s:%d:%s\n", condition, file, line, fname);
    fflush(stderr);

#if defined(CRA_COMPILER_GNUC)
    __builtin_trap();
#elif defined(CRA_COMPILER_MSVC)
    __debugbreak();
#endif
}

void (*__cra_g_assert__)(const char *cond, const char *fname, const char *file, int line) = cra_assert_default_func;
