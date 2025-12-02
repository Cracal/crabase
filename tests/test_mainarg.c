#include "collections/cra_collects.h"
#include "cra_assert.h"
#include "cra_mainarg.h"
#include "cra_malloc.h"

void
test1(void)
{
    int   argc;
    // char *argv[] = { "program", "-a", "a",  "-b", "b",       "--CC", "c", "module1",
    //                  "--DD",    "d",  "-e", "e",  "module2", "--FF", "f" };
    char *argv[] = { "/path/to/program", "module2", "--FF", "f",    "-b", "b", "-a", "a", "--CC", "c",
                     "module1",          "-e",      "e",    "--DD", "d" };

    CraMainArg ma;

    argc = CRA_NARRAY(argv);

    cra_mainarg_init(&ma, argv[0], "[options]", "This is a test program.", 98);

    // cra_mainarg_build_cmd(&ma, NULL, "-h", "--help", "Help", print_help, NULL); // no need to set "-h, --help"
    // cra_mainarg_build_str(&ma, NULL, NULL, NULL, NULL, NULL); // ERROR
    // cra_mainarg_build_str(&ma, NULL, NULL, NULL, "tip", "valtip"); // ERROR
    // cra_mainarg_build_str(&ma, "module", NULL, NULL, "tip", "valtip"); // ERROR
    // cra_mainarg_build_str(&ma, "module", "-x", NULL, NULL, "valtip"); // ERROR

    cra_mainarg_build_str(&ma, NULL, "-a", "--AA", "Aa", "aa");
    cra_mainarg_build_str(&ma, NULL, "-b", NULL, "Bb", "bb");
    cra_mainarg_build_str(&ma, NULL, NULL, "--CC", "Cc", "cc");
    cra_mainarg_build_str(&ma, "module1", "-d", "--DD", "Dd", "dd");
    cra_mainarg_build_str(&ma, "module1", "-e", NULL, "Ee", "ee");
    cra_mainarg_build_str(&ma, "module2", NULL, "--FF", "Ff", "ff");

    cra_mainarg_parse_args(&ma, argc, argv);

    cra_mainarg_uninit(&ma);
}

void
test_help(void)
{
    int   argc;
    char *argv[] = { "program", "--help" };

    argc = CRA_NARRAY(argv);

    CraMainArg ma;
    cra_mainarg_init(&ma, argv[0], "[options]", "help test", 68);
    cra_mainarg_build_int(&ma, NULL, "-a", "--aaa", "aaa", "num");
    cra_mainarg_build_int(
      &ma,
      "module1",
      "-a",
      "--aaa",
      "hello world hello world hello world hello good world hello world hello world hello world hello "
      "world hello world hello world hello world hello world ",
      "number");
    cra_mainarg_build_int(&ma,
                          "module2",
                          "-a",
                          "--aaa",
                          "中文中文中文中文中文中文中文中文中文中文中文中文中文中文中文中文中文中文中文中文中文中文中文"
                          "中文中文中文中文中文中文中文中文中文中文中文中文中文",
                          "n");

    cra_mainarg_parse_args(&ma, argc, argv);

    cra_mainarg_uninit(&ma);
}

void
test_value(void)
{
    int   argc;
    char *argv[] = {
        "testvalue", "-z", "on", "info", "--name", "Tom", "-w", "62.5", "--tall", "192", "0.0.0.0", "9999"
    };

    bool       two;
    int64_t    i64;
    double     f64;
    char      *str;
    char      *host;
    int        port;
    CraMainArg ma;

    argc = CRA_NARRAY(argv);

    cra_mainarg_init(&ma, argv[0], "[options]", "This is a test program.", 78);

    cra_mainarg_build_two(&ma, NULL, "-z", NULL, "Zero memory");
    cra_mainarg_build_str(&ma, NULL, "-o", "--output", "Set output path", "path");
    cra_mainarg_build_int(&ma, "info", "-t", "--tall", "Set user's height(cm)", "height");
    cra_mainarg_build_flt(&ma, "info", "-w", "--weight", "Set user's weight(kg)", "weight");
    cra_mainarg_build_str(&ma, "info", NULL, "--name", "Set user's name", "name");

    cra_mainarg_parse_args(&ma, argc, argv);

    two = cra_mainarg_get_two(&ma, NULL, "z", false);
    assert_always(two);
    str = (char *)cra_mainarg_get_str(&ma, NULL, "--output", "/dev/null");
    assert_always(strcmp(str, "/dev/null") == 0);
    i64 = cra_mainarg_get_int(&ma, "info", "tall", 180);
    assert_always(i64 == 192);
    f64 = cra_mainarg_get_flt(&ma, "info", "-w", 50.0);
    assert_always(cra_compare_double(f64, 62.5) == 0);
    str = (char *)cra_mainarg_get_str(&ma, "info", "--name", "Jack");
    assert_always(strcmp(str, "Tom") == 0);

    host = (char *)cra_mainarg_get_unbuild_str(&ma, 0, NULL);
    assert_always(host && strcmp(host, "0.0.0.0") == 0);
    port = (int)cra_mainarg_get_unbuild_int(&ma, 1, 0);
    assert_always(port == 9999);

    // i64 = cra_mainarg_get_int(&ma, "info", "weight", 0); // error(type mismatch)
    // i64 = cra_mainarg_get_int(&ma, "NOOO", "-n", 100); // error(no module)
    f64 = cra_mainarg_get_flt(&ma, NULL, "--abc", 0.0); // error(no option)
    assert_always(cra_compare_double(f64, 0.0) == 0);
    f64 = cra_mainarg_get_flt(&ma, "info", "--abc", 1.0); // error(no option)
    assert_always(cra_compare_double(f64, 1.0) == 0);

    // error(out of unbuild array)
    str = (char *)cra_mainarg_get_unbuild_str(&ma, 2, "nooooo");
    assert_always(strcmp(str, "nooooo") == 0);
    i64 = cra_mainarg_get_unbuild_int(&ma, 100, 20);
    assert_always(i64 == 20);
    f64 = cra_mainarg_get_unbuild_flt(&ma, 2, 4.5);
    assert_always(cra_compare_double(f64, 4.5) == 0);

    cra_mainarg_uninit(&ma);
}

static void
func(void *name)
{
    printf("%s called.\n", (char *)name);
}

void
test_hyphen(void)
{
    // char *argv[] = { "app", "-n", "100", "--float", "1.5", "-s", "hello" };
    char *argv[] = { "app", "-n", "100", "-float", "1.5", "-s", "hello" };
    int   argc = CRA_NARRAY(argv);

    int64_t i64;
    double  f64;
    char   *str;

    CraMainArg ma;
    cra_mainarg_init(&ma, argv[0], "[options]", "test cmd", 98);
    cra_mainarg_build_int(&ma, NULL, "-n", "--num", "Set number", "num");
    cra_mainarg_build_flt(&ma, NULL, "f", "-float", "Set number", "num");
    cra_mainarg_build_str(&ma, NULL, "s", "str", "Set number", "num");

    cra_mainarg_parse_args(&ma, argc, argv);

    i64 = cra_mainarg_get_int(&ma, NULL, "-n", 0);
    assert_always(i64 == 100);
    i64 = cra_mainarg_get_int(&ma, NULL, "n", 0);
    assert_always(i64 == 100);
    i64 = cra_mainarg_get_int(&ma, NULL, "--num", 0);
    assert_always(i64 == 100);
    i64 = cra_mainarg_get_int(&ma, NULL, "-num", 0);
    assert_always(i64 == 100);
    i64 = cra_mainarg_get_int(&ma, NULL, "num", 0);
    assert_always(i64 == 100);

    f64 = cra_mainarg_get_flt(&ma, NULL, "-f", 0);
    assert_always(cra_compare_double(f64, 1.5) == 0);
    f64 = cra_mainarg_get_flt(&ma, NULL, "f", 0);
    assert_always(cra_compare_double(f64, 1.5) == 0);
    f64 = cra_mainarg_get_flt(&ma, NULL, "--float", 0);
    assert_always(cra_compare_double(f64, 1.5) == 0);
    f64 = cra_mainarg_get_flt(&ma, NULL, "-float", 0);
    assert_always(cra_compare_double(f64, 1.5) == 0);
    f64 = cra_mainarg_get_flt(&ma, NULL, "float", 0);
    assert_always(cra_compare_double(f64, 1.5) == 0);

    str = cra_mainarg_get_str(&ma, NULL, "-s", "no");
    assert_always(strcmp(str, "hello") == 0);
    str = cra_mainarg_get_str(&ma, NULL, "s", "no");
    assert_always(strcmp(str, "hello") == 0);
    str = cra_mainarg_get_str(&ma, NULL, "--str", "no");
    assert_always(strcmp(str, "hello") == 0);
    str = cra_mainarg_get_str(&ma, NULL, "-str", "no");
    assert_always(strcmp(str, "hello") == 0);
    str = cra_mainarg_get_str(&ma, NULL, "str", "no");
    assert_always(strcmp(str, "hello") == 0);

    cra_mainarg_uninit(&ma);
}

void
test_cmd(void)
{
    char *argv[] = { "app", "-1", "-2", "--func3" };
    int   argc = CRA_NARRAY(argv);

    CraMainArg ma;
    cra_mainarg_init(&ma, argv[0], "[options]", "test cmd", 98);
    cra_mainarg_build_cmd(&ma, NULL, "-1", "--func1", "function 1", func, "function 1");
    cra_mainarg_build_cmd(&ma, NULL, "-2", "--func2", "function 2", func, "function 2");
    cra_mainarg_build_cmd(&ma, NULL, "-3", "--func3", "function 3", func, "function 3");

    cra_mainarg_parse_args(&ma, argc, argv);

    cra_mainarg_uninit(&ma);
}

static bool
check_int(int64_t val, void *ignore)
{
    CRA_UNUSED_VALUE(ignore);
    return val > 10 && val < 100;
}

static bool
check_flt(double val, void *ignore)
{
    CRA_UNUSED_VALUE(ignore);
    return val >= -1.5 && val < 5.5;
}

static bool
check_str(char *val, void *ignore)
{
    CRA_UNUSED_VALUE(ignore);
    return strcmp(val, "A") == 0 || strcmp(val, "B") == 0;
}

void
test_check(void)
{
    // char *argv[] = { "app", "-a", "1000" }; // ERROR
    // char *argv[] = { "app", "-a", "50" }; // OK
    // char *argv[] = { "app", "-b", "-8.8" }; // ERROR
    // char *argv[] = { "app", "-b", "-1.5" }; // OK
    // char *argv[] = { "app", "-c", "HELLO" }; // ERROR
    // char *argv[] = { "app", "-c", "A" }; // OK

    // char *argv[] = { "app", "1000" }; // ERROR
    // char *argv[] = { "app", "99" }; // OK
    // char *argv[] = { "app", "6.6" }; // ERROR
    // char *argv[] = { "app", "5.4" }; // OK
    // char *argv[] = { "app", "noooo" }; // ERROR
    char *argv[] = { "app", "B" }; // OK
    int   argc = CRA_NARRAY(argv);

    CraMainArg ma;
    cra_mainarg_init(&ma, argv[0], "[options]", "test cmd", 98);
    cra_mainarg_build_int_check(&ma, NULL, "-a", NULL, "apple", "i", check_int, NULL);
    cra_mainarg_build_flt_check(&ma, NULL, "-b", NULL, "banana", "f", check_flt, NULL);
    cra_mainarg_build_str_check(&ma, NULL, "-c", NULL, "city", "s", check_str, NULL);

    cra_mainarg_parse_args(&ma, argc, argv);

    // cra_mainarg_get_unbuild_int_check(&ma, 0, 0, check_int, NULL);
    // cra_mainarg_get_unbuild_flt_check(&ma, 0, 0.0, check_flt, NULL);
    cra_mainarg_get_unbuild_str_check(&ma, 0, NULL, check_str, NULL);

    cra_mainarg_uninit(&ma);
}

int
main(void)
{
    // test1();
    test_help();
    // test_value();
    // test_hyphen();
    // test_cmd();
    // test_check();

    cra_memory_leak_report();
    return 0;
}
