#include "crabase/cra_mainarg.h"
#include "crabase/cra_malloc.h"

#define X CRA_MAINARG_LENGTH
static const CraMainArgItem item_help = CRA_MAINARG_ITEM_DEF3('h', "help", "Display this help screen");
static const CraMainArgItem item_version = CRA_MAINARG_ITEM_DEF3('v', "version", "Display version information");
#undef X

static void
cra_mainarg_print_help(const CraMainArg *ma)
{
    printf("%s\nUsage: %s\n\n", ma->description, ma->usage);

    int longest = item_version.length;
    for (const CraMainArgItem *item = ma->items; item->desc; ++item)
    {
        if (item->option && item->length > longest)
            longest = item->length;
    }

    int desc_start = longest + 2;
    assert(desc_start > 0);

    printf("Options:\n");
    printf("  -%c, --%s%*.s%s\n", item_help.op, item_help.option, desc_start - item_help.length, "", item_help.desc);
    printf("  -%c, --%s%*.s%s\n", item_version.op, item_version.option, desc_start - item_version.length, "",
           item_version.desc);
    for (const CraMainArgItem *item = ma->items; item->desc; ++item)
    {
        if (item->op != CRA_MA_LONG_OP)
            printf("  -%c", item->op);
        else
            printf("    ");
        if (item->option)
            printf("%c --%s", item->op != CRA_MA_LONG_OP ? ',' : ' ', item->option);
        if (item->val)
            printf(" %s", item->val);
        if (!item->option)
            printf("    ");
        printf("%*.s%s\n", desc_start - item->length, "", item->desc);
    }
    printf("\n");
}

static const CraMainArgItem *
cra_mainarg_match_option(const CraMainArgItem *items, const char *arg)
{
    if (*arg == '-' && *arg != '\0')
        ++arg;
    if (*arg == '-' && *arg != '\0')
        ++arg;

//               short option                                  long option
#define XX(item) (*arg == (item)->op && *(arg + 1) == '\0') || ((item)->option && strcmp((item)->option, arg) == 0)

    if (XX(&item_help))
        return &item_help;
    if (XX(&item_version))
        return &item_version;

    for (const CraMainArgItem *item = items; item->desc; ++item)
    {
        if (XX(item))
            return item;
    }

#undef XX

    return NULL;
}

bool
cra_mainarg_parse(const CraMainArg *ma, int argc, const char *argv[], void *user_data)
{
    assert(ma);
    assert(argv);
    assert(ma->items);
    assert(ma->usage);
    assert(ma->version);
    assert(ma->parse);

    int                   op;
    const char           *arg;
    const char           *option;
    const char           *value;
    const CraMainArgItem *item;

    for (int i = 1; i < argc; ++i)
    {
        arg = argv[i];
        item = cra_mainarg_match_option(ma->items, arg);
        if (!item)
        {
            op = CRA_MA_NON_OP;
            option = NULL;
            value = arg;
        }
        else
        {
            if (item == &item_help)
            {
                cra_mainarg_print_help(ma);
                return false;
            }
            if (item == &item_version)
            {
                printf("%s\n\n", ma->version);
                return false;
            }

            if (item->val && ++i >= argc)
            {
                fprintf(stderr, "Missing value for option: %s\n", arg);
                goto fail;
            }
            op = item->op;
            option = item->option;
            value = item->val ? argv[i] : NULL;
        }

        if (!ma->parse(op, option, value, user_data))
        {
            fprintf(stderr, "Failed to parse option: %s\n", arg);
        fail:
            fprintf(stderr, "Type '-h' or '--help' for help.\n\n");
            return false;
        }
    }

    return true;
}
