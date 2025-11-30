#include "collections/cra_collects.h"
#include "cra_assert.h"
#include "cra_mainarg.h"
#include "cra_malloc.h"

void
print_help(void *user)
{
    CRA_UNUSED_VALUE(user);
    printf("don't set option(-h, --help).\n");
    exit(0);
}

void
print_version(void *user)
{
    CRA_UNUSED_VALUE(user);
    printf("v1.0.0\n");
    exit(0);
}

void
print_verbose(void *user)
{
    CRA_UNUSED_VALUE(user);
    printf("show verbose\n");
}

void
print_more_verbose(void *user)
{
    CRA_UNUSED_VALUE(user);
    printf("show mooooooooooore verbose\n");
}

void
delete_something(void *user)
{
    CRA_UNUSED_VALUE(user);
    printf("delete something...\n");
}

void
deny_func(void *user)
{
    CRA_UNUSED_VALUE(user);
    printf("deny something...\n");
}

void
enable_chinese(void *user)
{
    CRA_UNUSED_VALUE(user);
    printf("中文……\n");
}

void
test1(void)
{
    int   argc;
    char *argv[] = { "program", "network", "-H", "0.0.0.0", "--port", "8765", "--help" };

    CraMainArg ma;

    argc = CRA_NARRAY(argv);

    cra_mainarg_init(&ma, "[options]", "This is a test program.", 98);

    // cra_mainarg_build_cmd(&ma, "network", "-h", "--help", "Help", print_help, NULL); // no need to set "-h, --help"
    cra_mainarg_build_str(&ma, "network", "-H", "--host", "Hostname", "hostname");
    cra_mainarg_build_int(&ma, "network", "-p", "--port", "Port", "port");
    cra_mainarg_build_cmd(&ma, NULL, "-v", "--version", "Display program's version information", print_version, NULL);
    cra_mainarg_build_cmd(&ma, NULL, NULL, "--verbose", "Set verbosity level", print_verbose, NULL);
    cra_mainarg_build_cmd(&ma, NULL, "-m", NULL, "more verbose", print_more_verbose, NULL);
    cra_mainarg_build_cmd(&ma, "other", "-d", NULL, "Delete something", delete_something, NULL);
    cra_mainarg_build_flt(&ma, "other", "-w", NULL, "Set weight", "weight(float)");
    cra_mainarg_build_two(&ma, "other", "-z", NULL, "Zero memory");
    cra_mainarg_build_str(
      &ma,
      NULL,
      "-o",
      "--output",
      "test multi-line tips. hello world, hello world, hello world, hello world, hello world, hello world, hello "
      "world, hello world, hello world, hello world, hello world, hello world, hello world, hello world, hello world, "
      "hello world, hello world, hello world, hello world, hello world, hello world, hello world, hello world, hello "
      "world, hello world, hello world, goodnight. ",
      "output-path");
    cra_mainarg_build_cmd(&ma, NULL, NULL, "--deny", "Deny sooooooooooooooooooooooooooothing", deny_func, NULL);
    cra_mainarg_build_cmd(&ma,
                          NULL,
                          "-Z",
                          "--zh",
                          "如果是中文呢？如果是 中文呢？如果是中文呢？如果是中文呢？ 如果是中文呢？如果是中文呢？",
                          enable_chinese,
                          NULL);

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

    cra_mainarg_init(&ma, "[hostname] [port] [options]", "This is a test program.", 78);

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

    i64 = cra_mainarg_get_int(&ma, "info", "weight", 0); // error(type mismatch)
    assert_always(i64 == 0);
    i64 = cra_mainarg_get_int(&ma, "NOOO", "-n", 100); // error(no module)
    assert_always(i64 == 100);
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

int
main(void)
{
    // test1();
    test_value();

    cra_memory_leak_report();
    return 0;
}
