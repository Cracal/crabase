#include "cra_assert.h"

static void
new_assert(const char *condition, const char *fname, const char *file, int line)
{
    printf("new assert failed: (%s), function: %s, file: %s, line: %d\n", condition, fname, file, line);
}

void
test(void)
{
    assert(true);
    // assert(false);

    assert_always(true);
    // assert_always(100 + 30 > 1000);

    cra_assert_set_func(new_assert);
    assert_always(false);
}

int
main(void)
{
    test();
    return 0;
}
