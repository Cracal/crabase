#include "cra_macro.h"

void
make_code(int n)
{
    printf("CRA_ITER(INC);\n");
    printf("assert_always(n == 0);\n");

    for (int i = 1; i <= n; i++)
    {
        printf("n = CRA_ITER(INC");
        for (int j = 1; j <= i; j++)
            printf(", n");
        printf(");\n");
        printf("assert_always(n == %d);\n", i);
    }
}

void
test_iter(void)
{
#define INC(n) +1
    int n = 0;

    CRA_ITER(INC);
    assert_always(n == 0);
    n = CRA_ITER(INC, n);
    assert_always(n == 1);
    n = CRA_ITER(INC, n, n);
    assert_always(n == 2);
    n = CRA_ITER(INC, n, n, n);
    assert_always(n == 3);
    n = CRA_ITER(INC, n, n, n, n);
    assert_always(n == 4);
    n = CRA_ITER(INC, n, n, n, n, n);
    assert_always(n == 5);
    n = CRA_ITER(INC, n, n, n, n, n, n);
    assert_always(n == 6);
    n = CRA_ITER(INC, n, n, n, n, n, n, n);
    assert_always(n == 7);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n);
    assert_always(n == 8);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n);
    assert_always(n == 9);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 10);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 11);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 12);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 13);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 14);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 15);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 16);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 17);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 18);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 19);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 20);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 21);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 22);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 23);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 24);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 25);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 26);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 27);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 28);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 29);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 30);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 31);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 32);
    n =
      CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 33);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n);
    assert_always(n == 34);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n);
    assert_always(n == 35);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n);
    assert_always(n == 36);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n);
    assert_always(n == 37);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n);
    assert_always(n == 38);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n);
    assert_always(n == 39);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n);
    assert_always(n == 40);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n);
    assert_always(n == 41);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n);
    assert_always(n == 42);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 43);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 44);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 45);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 46);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 47);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 48);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 49);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 50);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 51);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 52);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 53);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 54);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 55);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 56);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 57);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 58);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 59);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 60);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 61);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 62);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 63);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 64);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 65);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 66);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 67);
    n =
      CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
               n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 68);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n);
    assert_always(n == 69);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n);
    assert_always(n == 70);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n);
    assert_always(n == 71);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n);
    assert_always(n == 72);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n);
    assert_always(n == 73);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n);
    assert_always(n == 74);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n);
    assert_always(n == 75);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n);
    assert_always(n == 76);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 77);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 78);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 79);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 80);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 81);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 82);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 83);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 84);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 85);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 86);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 87);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 88);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 89);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 90);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 91);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 92);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 93);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 94);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 95);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 96);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 97);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 98);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 99);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 100);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 101);
    n =
      CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
               n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
               n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 102);
    n =
      CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
               n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
               n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 103);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n);
    assert_always(n == 104);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n);
    assert_always(n == 105);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n);
    assert_always(n == 106);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n);
    assert_always(n == 107);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n);
    assert_always(n == 108);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n);
    assert_always(n == 109);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n);
    assert_always(n == 110);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 111);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 112);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 113);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 114);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 115);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 116);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 117);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 118);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 119);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 120);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 121);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 122);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 123);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 124);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 125);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 126);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 127);
    n = CRA_ITER(INC, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
                 n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n);
    assert_always(n == 128);

    printf("n = %d\n", n);
}

int
main(void)
{
    // make_code(128);
    test_iter();
}
