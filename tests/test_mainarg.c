#include "crabase/collections/cra_collects.h"
#include "crabase/cra_assert.h"
#include "crabase/cra_mainarg.h"
#include "crabase/cra_malloc.h"

static bool
test_help_parser(int op, const char *option, const char *value, void *user_data)
{
    CRA_UNUSED(user_data);

    switch (op)
    {
        case CRA_MA_LONG_OP:
            assert_always(strcmp(option, "apple") == 0);
            assert_always(value == NULL);
            return true;
        case 'f':
            assert_always(strcmp(option, "fig") == 0);
            assert_always(strcmp(value, "10") == 0);
            return true;
        default:
            assert_always(false);
            return false;
    }
}

void
test_help_version(void)
{
    CRA_MAINARG_DEF(ma, "app 1.0.0", "app [options]", "Test Help Program", test_help_parser,

                    (0, "apple", "Apple"),                     // Apple
                    ('b', NULL, "Banana"),                     // Banana
                    ('c', "cherry", "Cherry"),                 // Cherry
                    (0, "dog", "<number>", "Number of Dogs"),  // Dogs
                    ('e', NULL, "<number>", "Number of Eggs"), // Eggs
                    ('f', "fig", "<number>", "Number of Figs") // Figs

    );

    const char *argv[] = { "test_help", "-f", "10", "--apple", "-h" };
    int         argc = CRA_NARRAY(argv);
    assert_always(!cra_mainarg_parse(&ma, argc, argv, NULL)); // print help information and return false

    const char *argv2[] = { "test_version", "--version" };
    int         argc2 = CRA_NARRAY(argv2);
    assert_always(!cra_mainarg_parse(&ma, argc2, argv2, NULL)); // print version information and return false
}

void
test_error_options(void)
{
    CRA_MAINARG_DEF(ma, "app 1.1", "app [options]", "Test Error Options Program", test_help_parser,

                    // ('h', "help", "Show help"),      // '-h, --help'是保留选项，无须手动定义
                    // ('v', "version", "Show version") // '-v, --version'是保留选项，无须手动定义

                    // (0, NULL, "xxx") // 短选项和长选项至少要有一个

                    // ('l', "listen", NULL) // `description`是必填项

                    // (0, "x", NULL, "xxxx") // 长选项必须至少有2个字符

                    (0, "xx", NULL, "xxxx") // OK

    );
    CRA_UNUSED(ma);
}

struct my_values
{
    bool  b;
    int   i;
    float f;
    char *s;
};

static bool
test_error_args_parser(int op, const char *option, const char *value, void *user_data)
{
    struct my_values *values = (struct my_values *)user_data;
    assert_always(values != NULL);
    switch (op)
    {
        case 'b':
            assert_always(strcmp(option, "bool") == 0);
            if (strcmp(value, "yes") == 0)
                values->b = true;
            else if (strcmp(value, "no") == 0)
                values->b = false;
            else
                return false;
            return true;
        case 'i':
            return false;
        case 'f':
            return false;
        case 's':
            assert_always(value != NULL);
            values->s = (char *)value;
            return true;
        default:
            assert_always(false);
            return false;
    }
}

void
test_error_args(void)
{
    CRA_MAINARG_DEF(ma, "app 1.2", "app [options]", "Test Error Args Program", test_error_args_parser,

                    ('b', "bool", "<yes|no>", "Set boolean"), // bool
                    ('i', NULL, "<num>", "Set integer"),      // int
                    ('f', NULL, "<num>", "Set float"),        // float
                    ('s', NULL, "<str>", "Set string")        // string

    );

    struct my_values values;

    // -b 缺值，返回false
    const char *argv1[] = { "test_error_args", "-b" };
    int         argc1 = CRA_NARRAY(argv1);
    memset(&values, 0, sizeof(values));
    assert_always(!cra_mainarg_parse(&ma, argc1, argv1, &values));
    assert_always(values.b == false);

    // -b 的值不是yes或no，返回false
    const char *argv2[] = { "test_error_args", "-b", "ok" };
    int         argc2 = CRA_NARRAY(argv2);
    memset(&values, 0, sizeof(values));
    assert_always(!cra_mainarg_parse(&ma, argc2, argv2, &values));
    assert_always(values.b == false);

    // -b 的值正确，返回true
    const char *argv3[] = { "test_error_args", "-b", "yes" };
    int         argc3 = CRA_NARRAY(argv3);
    memset(&values, 0, sizeof(values));
    assert_always(cra_mainarg_parse(&ma, argc3, argv3, &values));
    assert_always(values.b == true);

    // -s 无值，返回false
    const char *argv4[] = { "test_error_args", "-s" };
    int         argc4 = CRA_NARRAY(argv4);
    memset(&values, 0, sizeof(values));
    assert_always(!cra_mainarg_parse(&ma, argc4, argv4, &values));
    assert_always(values.s == NULL);

    // -s 有值，返回true
    const char *argv5[] = { "test_error_args", "-s", "string3" };
    int         argc5 = CRA_NARRAY(argv5);
    memset(&values, 0, sizeof(values));
    assert_always(cra_mainarg_parse(&ma, argc5, argv5, &values));
    assert_always(values.s != NULL);
    assert_always(strcmp(values.s, "string3") == 0);
}

struct my_values2
{
    bool  ipv4only;
    bool  ipv6only;
    bool  listen;
    char *output;
    float delay;
    int   max_incoming;
    bool  crlf;

    char *host;
    int   port;
};

static bool
test_get_val_parser(int op, const char *option, const char *value, void *user_data)
{
    struct my_values2 *values = (struct my_values2 *)user_data;
    assert_always(values != NULL);
    switch (op)
    {
        case '4':
            assert(value == NULL);
            values->ipv4only = true;
            return true;
        case '6':
            assert(value == NULL);
            values->ipv6only = true;
            return true;
        case 'l':
            assert(value == NULL);
            values->listen = true;
            return true;
        case 'o':
            assert(value != NULL);
            values->output = (char *)value;
            return true;
        case 'd':
            assert(value != NULL);
            values->delay = (float)atof(value);
            if (values->delay < 1.0 || values->delay > 60.0)
            {
                printf("delay must be between 1.0 and 60.0\n");
                return false;
            }
            return true;
        case 'n':
            assert(value != NULL);
            values->max_incoming = atoi(value);
            if (values->max_incoming < 1 || values->max_incoming > 1024)
            {
                printf("max_incoming must be between 1 and 1024\n");
                return false;
            }
            return true;
        case 'C':
            assert(value != NULL);
            values->crlf = (strcmp(value, "yes") == 0);
            return true;
        case CRA_MA_NON_OP:
            assert_always(option == NULL);
            assert_always(value != NULL);
            if (!values->host)
            {
                values->host = (char *)value;
            }
            else
            {
                values->port = atoi(value);
                if (values->port < 1025 || values->port > 65535)
                {
                    printf("port must be between 1025 and 65535\n");
                    return false;
                }
            }
            return true;
        default:
            assert_always(false);
            return false;
    }
}

void
test_get_val(void)
{
    CRA_MAINARG_DEF(ma, "app 2.0", "app [options] [hostname] [port]", "Test Get Val Program", test_get_val_parser,

                    ('4', NULL, "Use IPv4 only"),                                   // IPv4 only
                    ('6', NULL, "Use IPv6 only"),                                   // IPv6 only
                    ('l', "listen", "Bind and listen for incoming connections"),    // listen
                    ('o', "output", "<filename>", "Dump session data to a file"),   // output
                    ('d', "delay", "<time>", "Wait between read/writes"),           // delay
                    ('n', NULL, "<num>", "Set max number of incoming connections"), // n
                    ('C', "crlf", "<on|off>", "Use CRLF for EOL sequence")          // crlf

    );

    const char *argv[] = {
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

    struct my_values2 values = { 0 };
    assert_always(cra_mainarg_parse(&ma, argc, argv, &values));
    assert_always(!values.ipv4only);
    assert_always(values.ipv6only);
    assert_always(values.listen);
    assert_always(values.output && strcmp(values.output, "/dev/null") == 0);
    assert_always(cra_cmp_float(values.delay, 3.5f) == 0);
    assert_always(values.max_incoming == 20);
    assert_always(values.crlf);
    assert_always(values.host && strcmp(values.host, "0.0.0.0") == 0);
    assert_always(values.port == 8888);

    const char *argv2[] = { "xx", "--help" };
    int         argc2 = CRA_NARRAY(argv2);
    assert_always(!cra_mainarg_parse(&ma, argc2, argv2, &values)); // print help
}

int
main(void)
{
    test_help_version();
    test_error_options();
    test_error_args();
    test_get_val();

    cra_memory_leak_report();
    return 0;
}
