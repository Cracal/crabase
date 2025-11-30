#include "cra_mainarg.h"
#include "collections/cra_alist.h"
#include "collections/cra_dict.h"
#include "cra_assert.h"
#include "cra_malloc.h"

#define DEFAULT_MODULE_NAME "--cra-default-modlue--"

typedef struct _CraMainArgItem CraMainArgItem;
struct _CraMainArgItem
{
    CraMainArgType_e type;
    short            refcnt;
    bool             assigned;
    char            *longarg;
    char            *arg;
    char            *tip;
    union
    {
        void *user;
        char *valtip;
    };
    union
    {
        bool           two;
        int64_t        i64;
        double         f64;
        char          *str;
        cra_mainarg_fn func;
    } val;
};

static const char *s_valtypes[] = { "two", "i64", "f64", "str", "cmd" };

static CraMainArgItem *
cra_mainarg_make_item(CraMainArgType_e type, char *op, char *option, char *tip)
{
    CraMainArgItem *item;

    item = cra_alloc(CraMainArgItem);
    item->type = type;
    item->refcnt = op && option ? 2 : 1;
    item->assigned = false;
    item->longarg = option;
    item->arg = op;
    item->tip = tip;
    return item;
}

static void
cra_mainarg_free_item(CraMainArgItem *item, bool force)
{
    if (force || --item->refcnt == 0)
        cra_dealloc(item);
}

static CraMainArgItem *
cra_mainarg_get_item(CraDict *module, char *name)
{
    CraMainArgItem *item = NULL;
    if (*name == '-')
        ++name;
    if (*name == '-')
        ++name;
    cra_dict_get(module, &name, &item);
    return item;
}

static CraDict *
cra_mainarg_get_module(CraMainArg *ma, char *module_name, bool makeifneed)
{
    CraDict *dict = NULL;
    if (!module_name)
        module_name = DEFAULT_MODULE_NAME;
    if (!cra_dict_get(ma->modules, &module_name, &dict) && makeifneed)
    {
        dict = cra_alloc(CraDict);
        cra_dict_init0(
          char *, CraMainArg *, dict, false, (cra_hash_fn)cra_hash_string1_p, (cra_compare_fn)cra_compare_string_p);
        assert_always(cra_dict_add(ma->modules, &module_name, &dict));
    }
    return dict;
}

static void
cra_mainarg_free_module(CraDict *module)
{
    CraDictIter      it;
    CraMainArgItem **pitem;

    for (cra_dict_iter_init(module, &it); cra_dict_iter_next(&it, NULL, (void **)&pitem);)
        cra_mainarg_free_item(*pitem, false);
    cra_dict_uninit(module);
    cra_dealloc(module);
}

void
cra_mainarg_init(CraMainArg *ma, char *usage, char *introduction, int linemax)
{
    assert(usage);
    assert(introduction);
    assert(linemax > 0);

    ma->tipstart = 0;
    ma->line_max = linemax;
    ma->usage = usage;
    ma->introduction = introduction;
    ma->program = NULL;
    ma->modules = cra_alloc(CraDict);
    cra_dict_init0(
      char *, CraDict *, ma->modules, false, (cra_hash_fn)cra_hash_string1_p, (cra_compare_fn)cra_compare_string_p);
    ma->unbuild = cra_alloc(CraAList);
    cra_alist_init0(char *, ma->unbuild, false);
}

void
cra_mainarg_uninit(CraMainArg *ma)
{
    CraDictIter it;
    CraDict   **pmodule;

    assert(ma);
    assert(ma->modules);

    for (cra_dict_iter_init(ma->modules, &it); cra_dict_iter_next(&it, NULL, (void **)&pmodule);)
        cra_mainarg_free_module(*pmodule);
    cra_dict_uninit(ma->modules);
    cra_dealloc(ma->modules);
    cra_alist_uninit(ma->unbuild);
    cra_dealloc(ma->unbuild);
    bzero(ma, sizeof(*ma));
}

void
__cra_mainarg_build_val(CraMainArg      *ma,
                        char            *module_name,
                        char            *op,
                        char            *option,
                        char            *tip,
                        CraMainArgType_e type,
                        cra_mainarg_fn   func,
                        void            *ptr) // type == CMD => ptr is param to func; else => ptr is valtip
{
    CraMainArgItem *item = NULL;
    CraDict        *module;
    char           *opname;
    int             len, tipstart = 0;

    assert_always(op || option);

    if (type == CRA_MAINARG_TYPE_CMD)
    {
        if (!func)
        {
            fprintf(stderr, "MainArg Error: op(%s) need a function.\n\n", op);
            goto error_exit;
        }
    }
    else
    {
        if (!ptr)
        {
            fprintf(stderr, "MainArg Error: op(%s) need a value tip.\n\n", op);
            goto error_exit;
        }
    }

    module = cra_mainarg_get_module(ma, module_name, true);
    item = cra_mainarg_make_item(type, op, option, tip);
    item->user = ptr; // valtip or function's param
    item->val.func = func;

    if (op)
    {
        assert(strcmp(op, "-h") != 0); // don't set -h

        // op = "-X"
        if (op[0] != '-' || op[1] == '-' || (len = (int)strlen(op)) != 2)
        {
            fprintf(stderr, "MainArg Error: op(%s) != `-X`.\n\n", op);
            goto error_exit;
        }
        opname = op + 1;
        if (!cra_dict_add(module, &opname, &item))
        {
            fprintf(stderr, "MainArg Error: op(%s) was added repeatedly.\n\n", op);
            goto error_exit;
        }
        tipstart = len;
    }
    if (option)
    {
        assert(strcmp(option, "--help") != 0); // don't set --help

        // option = "--X...X"
        if (option[0] != '-' || option[1] != '-' || option[2] == '-' || (len = (int)strlen(option)) < 4)
        {
            fprintf(stderr, "MainArg Error: option(%s) != `--X..X`.\n\n", option);
            goto error_exit;
        }
        opname = option + 2;
        if (!cra_dict_add(module, &opname, &item))
        {
            fprintf(stderr, "MainArg Error: option(%s) was added repeatedly.\n\n", option);
            if (op)
                cra_dict_remove(module, &op);
            goto error_exit;
        }
        tipstart += len;
    }

    // calc tip message start point.
    // "  [  ]-X, --X..X [VAL] tip message start"
    //         ( )           ( )           (, )                              (VAL)                              ( )
    // tipstart += 2 + (module_name ? 2 : 0) + 2 + (type != CRA_MAINARG_TYPE_CMD ? (int)strlen(item->valtip) : 0) + 2;
    tipstart += (module_name ? 2 : 0) + (type != CRA_MAINARG_TYPE_CMD ? (int)strlen(item->valtip) : 0) + 6;
    if (tipstart > ma->tipstart)
        ma->tipstart = tipstart;
    return;

error_exit:
    if (item)
        cra_mainarg_free_item(item, true);
    exit(EXIT_FAILURE);
}

void
cra_mainarg_parse_args(CraMainArg *ma, int argc, char *argv[])
{
    char           *end;
    char           *op = NULL;
    CraMainArgItem *item = NULL;
    CraDict        *module = NULL;
    char           *module_name = NULL;
    CraDict        *default_module = NULL;
    char           *default_name = DEFAULT_MODULE_NAME;

    assert(argc > 0);
    ma->program = argv[0];

    assert_always(cra_dict_get(ma->modules, &default_name, &default_module));
    for (int i = 1; i < argc; i++)
    {
        op = argv[i];
        // module?
        if (op[0] != '-')
        {
            module = cra_mainarg_get_module(ma, op, false);
            if (module)
            {
                module_name = op;
            }
            else
            {
                // unbind arg
                cra_alist_append(ma->unbuild, &op);
                module_name = NULL;
            }
            continue;
        }
        // get item
        if (!module || !(item = cra_mainarg_get_item(module, op)))
        {
            // in default module?
            if (!(item = cra_mainarg_get_item(default_module, op)))
            {
                bool print_helper = (strcmp("-h", op) == 0 || strcmp("--help", op) == 0);
                if (!print_helper)
                {
                    fprintf(stderr,
                            "MainArg Error: option(%s) is an invalid option in module(%s).\n\n",
                            op,
                            module_name ? module_name : "<DEFAULT>");
                }
                cra_mainarg_print_help(ma);
                exit(!print_helper);
            }
        }

        // set value

        if (item->type == CRA_MAINARG_TYPE_CMD)
        {
            assert(item->val.func);
            item->val.func(item->user);
            continue;
        }

        if ((i + 1) >= argc)
        {
            fprintf(stderr, "MainArg Error: option(%s) need a value.\n\n", op);
            cra_mainarg_print_help(ma);
            exit(EXIT_FAILURE);
        }
        end = NULL;
        switch (item->type)
        {
            case CRA_MAINARG_TYPE_TWO:
                if (strcmp("on", argv[++i]) == 0)
                    item->val.two = true;
                else if (strcmp("off", argv[i]) == 0)
                    item->val.two = false;
                else
                    goto errorval;
                break;
            case CRA_MAINARG_TYPE_INT:
                item->val.i64 = strtoll(argv[++i], &end, 10);
                if (argv[i] == end)
                    goto errorval;
                break;
            case CRA_MAINARG_TYPE_FLT:
                item->val.f64 = strtod(argv[++i], &end);
                if (argv[i] == end)
                    goto errorval;
                break;
            case CRA_MAINARG_TYPE_STR:
                item->val.str = argv[++i];
                break;
            default:
                assert_always(false);
                break;
        }
        item->assigned = true;
        continue;

    errorval:
        fprintf(stderr, "MainArg Error: option(%s) need %s type value.\n\n", argv[i - 1], s_valtypes[item->type]);
        cra_mainarg_print_help(ma);
        exit(EXIT_FAILURE);
    }
}

static CraMainArgItem *
cra_mainarg_get_val_item(CraMainArg *ma, const char *module_name, const char *name, CraMainArgType_e type)
{
    CraDict        *module;
    CraMainArgItem *item;

    module = cra_mainarg_get_module(ma, (char *)module_name, false);
    if (!module)
        return NULL;

    item = cra_mainarg_get_item(module, (char *)name);
    if (!item || !item->assigned)
        return NULL;
    // check type
    if (type != item->type)
    {
        fprintf(
          stderr, "MainArg Error: value's type mismatch(%s != %s).\n\n", s_valtypes[type], s_valtypes[item->type]);
        return NULL;
    }
    return item;
}

bool
cra_mainarg_get_two(CraMainArg *ma, const char *module, const char *name, bool default_value)
{
    CraMainArgItem *item = cra_mainarg_get_val_item(ma, module, name, CRA_MAINARG_TYPE_TWO);
    if (item)
        return item->val.two;
    return default_value;
}

int64_t
cra_mainarg_get_int(CraMainArg *ma, const char *module, const char *name, int64_t default_value)
{
    CraMainArgItem *item = cra_mainarg_get_val_item(ma, module, name, CRA_MAINARG_TYPE_INT);
    if (item)
        return item->val.i64;
    return default_value;
}

double
cra_mainarg_get_flt(CraMainArg *ma, const char *module, const char *name, double default_value)
{
    CraMainArgItem *item = cra_mainarg_get_val_item(ma, module, name, CRA_MAINARG_TYPE_FLT);
    if (item)
        return item->val.f64;
    return default_value;
}

const char *
cra_mainarg_get_str(CraMainArg *ma, const char *module, const char *name, const char *default_value)
{
    CraMainArgItem *item = cra_mainarg_get_val_item(ma, module, name, CRA_MAINARG_TYPE_STR);
    if (item)
        return item->val.str;
    return default_value;
}

int64_t
cra_mainarg_get_unbuild_int(CraMainArg *ma, unsigned int index, int64_t default_value)
{
    int64_t ret;
    char   *str, *end = NULL;

    if (!!(str = (char *)cra_mainarg_get_unbuild_str(ma, index, NULL)))
    {
        ret = strtoll(str, &end, 10);
        if (str != end)
            return ret;
    }
    return default_value;
}

double
cra_mainarg_get_unbuild_flt(CraMainArg *ma, unsigned int index, double default_value)
{
    double ret;
    char  *str, *end = NULL;

    if (!!(str = (char *)cra_mainarg_get_unbuild_str(ma, index, NULL)))
    {
        ret = strtod(str, &end);
        if (str != end)
            return ret;
    }
    return default_value;
}

const char *
cra_mainarg_get_unbuild_str(CraMainArg *ma, unsigned int index, const char *default_value)
{
    const char *ret;
    if (cra_alist_get(ma->unbuild, index, &ret))
        return ret;
    return default_value;
}

static inline int
cra_mainarg_get_char_bytes(unsigned char first_byte)
{
    // !!UTF-8 supported!! ONLY
    if (first_byte < 0x80)
        return 1;
    else if ((first_byte & 0xe0) == 0xc0)
        return 2;
    else if ((first_byte & 0xf0) == 0xe0)
        return 3;
    else if ((first_byte & 0xf8) == 0xf0)
        return 4;

    assert_always(false && "error first byte");
    return -1;
}

static void
cra_mainarg_print_one_char(char **pstr)
{
    int bytes = cra_mainarg_get_char_bytes((unsigned char)**pstr);
    *pstr += printf("%.*s", bytes, *pstr);
}

static void
cra_mainarg_print_tip(const char *tip, int tipstart, int linemax)
{
    int   i;
    char *str;

    if (tipstart >= linemax)
        linemax += 20;

    i = tipstart;
    str = (char *)tip;
    while (*str != '\0')
    {
        if (i >= linemax)
        {
            while (cra_mainarg_get_char_bytes((unsigned char)*str) == 1 && *str != ' ' && *str != '\0')
                printf("%c", *str++); // 不要打断英文单词

            printf("\n%*.s", tipstart, "");
            i = tipstart;

            if (cra_mainarg_get_char_bytes((unsigned char)*str) == 1 && *str == ' ')
                ++str; // 空格不要出现在开头
            continue;
        }
        cra_mainarg_print_one_char(&str);
        ++i;
    }
    printf("\n");
}

static void
cra_mainarg_print_items(CraMainArg *ma, CraDict *module, int prev_spaces)
{
    int              len;
    CraDictIter      it;
    CraMainArgItem **pitem = NULL, *item = NULL;

    for (cra_dict_iter_init(module, &it); cra_dict_iter_next(&it, NULL, (void **)&pitem);)
    {
        if (item == *pitem)
            continue;

        item = *pitem;
        // "  [  ][-o][, ][--option][ <value>]  tip"
        len = printf("%*.s%s%s%s %s",
                     prev_spaces,
                     "",
                     item->arg ? item->arg : "  ",
                     item->arg && item->longarg ? ", " : (!item->arg && item->longarg ? "  " : ""),
                     item->longarg ? item->longarg : "",
                     item->type != CRA_MAINARG_TYPE_CMD ? item->valtip : "");
        //  spaces before tip message
        printf("%*.s", ma->tipstart - len, "");
        // print tip message
        cra_mainarg_print_tip(item->tip, ma->tipstart, ma->line_max);
    }
}

void
cra_mainarg_print_help(CraMainArg *ma)
{
    size_t      program_len;
    char       *program;
    CraDict    *default_module;
    char      **module_name;
    CraDict   **pmodule;
    CraDictIter it;

    program_len = strlen(ma->program) + 1;
    program = (char *)cra_malloc(program_len);
    memcpy(program, ma->program, program_len);

    program = cra_basename(program);
    printf("Usage: %s %s\n\n", program, ma->usage);
    printf("%s\n", ma->introduction);
    printf("type '%s -h' or '%s --help' to list available options.\n\n", program, program);
    cra_free(program);
    program = NULL;

    // print defaut module first
    default_module = cra_mainarg_get_module(ma, DEFAULT_MODULE_NAME, false);
    if (default_module)
        cra_mainarg_print_items(ma, default_module, 2);

    for (cra_dict_iter_init(ma->modules, &it); cra_dict_iter_next(&it, (void **)&module_name, (void **)&pmodule);)
    {
        if (*pmodule == default_module)
            continue;
        printf("%s\n", *module_name);
        cra_mainarg_print_items(ma, *pmodule, 4);
    }
}
