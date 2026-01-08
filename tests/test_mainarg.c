#include "collections/cra_collects.h"
#include "cra_assert.h"
#include "cra_mainarg.h"
#include "cra_malloc.h"

void
test_help(void)
{
    CraMainArg ma;

    char             *program_name = "test_help";
    CraMainArgElement options[] = {
        CRA_MAINARG_ELEMENT_SET("-a", "--apple", "<number>", "Set number of apple", cra_mainarg_stoi, NULL),
        CRA_MAINARG_ELEMENT_SET("-b", "--banana", NULL, "With banana", NULL, NULL),
        CRA_MAINARG_ELEMENT_SET("-c",
                                NULL,
                                "<cherry type>",
                                "Set cherry type",
                                cra_mainarg_stos_in_array,
                                ((char *[]){ "type1", "type2", NULL })),
        CRA_MAINARG_ELEMENT_SET(NULL, "--dog", "<on|off>", "Dog", cra_mainarg_stob, NULL),
        CRA_MAINARG_ELEMENT_END(),
    };

    cra_mainarg_init(&ma, program_name, "A test help program.", "[options]", options);

    cra_mainarg_print_help(&ma);

    cra_mainarg_uninit(&ma);
}

void
test_error_options(void)
{
    CraMainArg ma;

    char             *program_name = "/path/to/test_error_optiosn";
    CraMainArgElement options[] = {
        // both NULL
        // CRA_MAINARG_ELEMENT_SET(NULL, NULL, "valtip", "Option tip", cra_mainarg_stob, NULL),
        // `op` != "-a"
        // CRA_MAINARG_ELEMENT_SET("a", NULL, "<val>", "Tip", cra_mainarg_stob, NULL),
        // `option` != "--apple"
        // CRA_MAINARG_ELEMENT_SET("-a", "-apple", "<val>", "Tip", cra_mainarg_stob, NULL),
        // `option` != "--apple"
        // CRA_MAINARG_ELEMENT_SET("-a", "apple", "<val>", "Tip", cra_mainarg_stob, NULL),
        // len(option) < 4
        // CRA_MAINARG_ELEMENT_SET("-a", "--a", "<val>", "Tip", cra_mainarg_stob, NULL),
        // repeat addition
        // CRA_MAINARG_ELEMENT_SET("-a", NULL, "<val>", "Tip", cra_mainarg_stob, NULL),
        // CRA_MAINARG_ELEMENT_SET("-a", NULL, "<val>", "Tip", cra_mainarg_stob, NULL),
        // no option tip
        // CRA_MAINARG_ELEMENT_SET("-a", "--apple", "<val>", NULL, cra_mainarg_stob, NULL),
        // `val` != NULL && `valtip` == NULL
        // CRA_MAINARG_ELEMENT_SET(NULL, "--apple", NULL, "Tip", cra_mainarg_stob, NULL),
        // no val option, no need a value
        CRA_MAINARG_ELEMENT_SET("-a", "--apple", "<valtip>", "Tip", NULL, NULL),
        CRA_MAINARG_ELEMENT_END(),
    };

    cra_mainarg_init(&ma, program_name, "A test error options program.", "[options]", options);

    cra_mainarg_uninit(&ma);
}

void
test_error_args(void)
{
    CraMainArg ma;

    CraMainArgElement options[] = {
        CRA_MAINARG_ELEMENT_VAL(
          "-b", "--bool", "<bool>", "Set boolean", cra_mainarg_stob_values, ((char *[]){ "yes", "no" })),
        CRA_MAINARG_ELEMENT_VAL(
          "-i", NULL, "<num>", "Set integer", cra_mainarg_stoi_in_range, ((int64_t[]){ 100, 501 })),
        CRA_MAINARG_ELEMENT_VAL(
          NULL, "--float", "<num>", "Set double", cra_mainarg_stof_in_range, ((double[]){ 2.5, 63.8 })),
        CRA_MAINARG_ELEMENT_VAL("-s",
                                "--string",
                                "<str>",
                                "Set string",
                                cra_mainarg_stos_in_array,
                                ((char *[]){ "str1", "string2", "str3", NULL })),
        CRA_MAINARG_ELEMENT_BOL("-c", "--command", "Call a function"),
        CRA_MAINARG_ELEMENT_END(),
    };

    char *argv[] = {
        "test_error_args",
        // "-h", // show help
        // "-c", // not error
        // value not match
        // "-b",
        // "ok",
        // "-i",
        // "501"
        // "--float",
        // "63.8",
        "-s",
        "string3",
    };
    int argc = CRA_NARRAY(argv);

    cra_mainarg_init(&ma, argv[0], "A test error args program.", "[options]", options);

    cra_mainarg_parse_args(&ma, argc, argv);

    cra_mainarg_uninit(&ma);
}

void
test_get_val(void)
{
    CraMainArg ma;

    CraMainArgElement options[] = {
        CRA_MAINARG_ELEMENT_BOL("-4", NULL, "Use IPv4 only"),
        CRA_MAINARG_ELEMENT_BOL("-6", NULL, "Use IPv6 only"),
        CRA_MAINARG_ELEMENT_BOL("-l", "--listen", "Bind and listen for incoming connections"),
        CRA_MAINARG_ELEMENT_VAL("-o", "--output", "<filename>", "Dump session data to a file", cra_mainarg_stos, NULL),
        CRA_MAINARG_ELEMENT_VAL(
          "-d", "--delay", "<time>", "Wait between read/writes", cra_mainarg_stof_in_range, ((double[]){ 1.0, 60.0 })),
        CRA_MAINARG_ELEMENT_VAL("-n",
                                NULL,
                                "<num>",
                                "Set max number of incoming connections",
                                cra_mainarg_stoi_in_range,
                                ((int64_t[]){ 1, 1024 })),
        CRA_MAINARG_ELEMENT_VAL("-C",
                                "--crlf",
                                "<yes|no>",
                                "Use CRLF for EOL sequence",
                                cra_mainarg_stob_values,
                                ((char *[]){ "yes", "no" })),
        CRA_MAINARG_ELEMENT_END(),
    };

    char *argv[] = {
        "test_get_val",
        // "-h",
        "-6",
        "-l",
        "--output",
        "/dev/null",
        "-d",
        "3.5",
        "--crlf",
        "yes",
        "-n",
        "20",
        "0.0.0.0",
        "8888",
    };
    int argc = CRA_NARRAY(argv);

    bool  b;
    int   i;
    float f;
    char *s;

    cra_mainarg_init(&ma, argv[0], "A test get val program.", "[options] [hostname] [port]", options);

    cra_mainarg_parse_args(&ma, argc, argv);

    b = cra_mainarg_get_b(&ma, "6", false);
    assert_always(b);
    b = cra_mainarg_get_b(&ma, "-6", false);
    assert_always(b);

    b = cra_mainarg_get_b(&ma, "4", false);
    assert_always(!b);

    b = cra_mainarg_get_b(&ma, "l", false);
    assert_always(b);
    b = cra_mainarg_get_b(&ma, "--listen", false);
    assert_always(b);

    b = cra_mainarg_get_b(&ma, "C", false);
    assert_always(b);

    i = (int)cra_mainarg_get_i(&ma, "-n", 0);
    assert_always(i == 20);

    f = (float)cra_mainarg_get_f(&ma, "delay", 0);
    assert_always(cra_compare_float(f, 3.5) == 0);

    s = cra_mainarg_get_s(&ma, "-o", NULL);
    assert_always(s != NULL && strcmp(s, "/dev/null") == 0);

    // not option

    i = cra_mainarg_get_notop_count(&ma);
    assert_always(i == 2);

    s = cra_mainarg_get_notop_s(&ma, 0, "localhost", cra_mainarg_stos, NULL);
    assert_always(strcmp(s, "0.0.0.0") == 0);

    i = (int)cra_mainarg_get_notop_i(&ma, 1, 9999, cra_mainarg_stoi_in_range, ((int64_t[]){ 1025, 65536 }));
    assert_always(i == 8888);

    cra_mainarg_uninit(&ma);
}

int
main(void)
{
    // test_help();
    // test_error_options();
    // test_error_args();
    test_get_val();

    cra_memory_leak_report();
    return 0;
}
