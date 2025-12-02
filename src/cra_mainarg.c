#include "cra_mainarg.h"
#include "collections/cra_alist.h"
#include "collections/cra_dict.h"
#include "cra_assert.h"
#include "cra_malloc.h"
#include "cra_mempool.h"

#define DEFAULT_MODULE_NAME    "--cra-default-modlue--"
#define ERROR_BEGIN            "MainArg Error: "
#define ERROR_ENDT             " type '-h' or '--help' to get some help.\n\n"
#define ERROR_END              "\n\n"
#define PRINT_ERROR(fmt, ...)  fprintf(stderr, ERROR_BEGIN fmt ERROR_END, ##__VA_ARGS__)
#define PRINT_ERRORT(fmt, ...) fprintf(stderr, ERROR_BEGIN fmt ERROR_ENDT, ##__VA_ARGS__)
#define PRINT_ERROR_EXIT(fmt, ...)       \
    do                                   \
    {                                    \
        PRINT_ERROR(fmt, ##__VA_ARGS__); \
        exit(EXIT_FAILURE);              \
    } while (0)
#define PRINT_ERRORT_EXIT(fmt, ...)       \
    do                                    \
    {                                     \
        PRINT_ERRORT(fmt, ##__VA_ARGS__); \
        exit(EXIT_FAILURE);               \
    } while (0)

#define MAINARG_TWO 0
#define MAINARG_INT 1
#define MAINARG_FLT 2
#define MAINARG_STR 3
#define MAINARG_CMD 4

typedef struct
{
    bool  assigned;
    int   type;
    char *op;
    char *option;
    char *tip;
    char *valtip;
    void *arg;
    union
    {
        cra_mainarg_fn  cmd;
        cra_mainarg_fn2 check;
    };
    CraMainArgVal val;

} CraMainArgItem;

static CraMainArgItem *
cra_mainarg_make_item(CraMainArg *ma, int type, char *op, char *option, char *tip, char *valtip)
{
    CraMainArgItem *item = (CraMainArgItem *)cra_mempool_alloc(ma->items);
    bzero(item, sizeof(*item));
    item->type = type;
    item->op = op;
    item->option = option;
    item->tip = tip;
    item->valtip = valtip;
    return item;
}

static void
cra_mainarg_free_item(CraMainArg *ma, CraMainArgItem *item)
{
    cra_mempool_dealloc(ma->items, item);
}

static CraDict *
cra_mainarg_get_module(CraMainArg *ma, char *module_name, bool make)
{
    CraDict *module = NULL;
    if (!module_name)
        module_name = DEFAULT_MODULE_NAME;
    if (!cra_dict_get(ma->modules, &module_name, &module) && make)
    {
        module = cra_alloc(CraDict);
        cra_dict_init0(char *,
                       CraMainArgItem *,
                       module,
                       false,
                       (cra_hash_fn)cra_hash_string1_p,
                       (cra_compare_fn)cra_compare_string_p);
        cra_dict_add(ma->modules, &module_name, &module);
    }
    return module;
}

static void
cra_mainarg_free_module(CraMainArg *ma, CraDict *module)
{
    CraDictIter      it;
    CraMainArgItem **pitem;
    for (cra_dict_iter_init(module, &it); cra_dict_iter_next(&it, NULL, (void **)&pitem);)
        cra_mainarg_free_item(ma, *pitem);
    cra_dict_uninit(module);
    cra_dealloc(module);
}

void
cra_mainarg_init(CraMainArg *ma, char *program, char *usage, char *introduction, int linemax)
{
    assert(program);
    assert(usage);
    assert(introduction);
    assert(linemax > 0);

    ma->program = cra_basename(program);
    ma->usage = usage;
    ma->introdution = introduction;
    ma->line_max = linemax;
    ma->tipstart = 0;
    ma->modules = cra_alloc(CraDict);
    ma->unbuild = cra_alloc(CraAList);
    ma->items = cra_alloc(CraMemPool);
    cra_dict_init0(
      char *, CraDict *, ma->modules, false, (cra_hash_fn)cra_hash_string1_p, (cra_compare_fn)cra_compare_string_p);
    cra_alist_init0(char *, ma->unbuild, false);
    cra_mempool_init(ma->items, sizeof(CraMainArgItem), 8);

    cra_mainarg_get_module(ma, NULL, true); // make default module
}

void
cra_mainarg_uninit(CraMainArg *ma)
{
    CraDictIter it;
    CraDict   **pmodule;

    for (cra_dict_iter_init(ma->modules, &it); cra_dict_iter_next(&it, NULL, (void **)&pmodule);)
        cra_mainarg_free_module(ma, *pmodule);

    cra_dict_uninit(ma->modules);
    cra_alist_uninit(ma->unbuild);
    cra_mempool_uninit(ma->items);
    cra_dealloc(ma->modules);
    cra_dealloc(ma->unbuild);
    cra_dealloc(ma->items);
    cra_free(ma->program);
}

static int
cra_mainarg_handle_op(char **op, char **option)
{
    char *name;
    int   tipstart, option_len;

    if (!(*op) && !(*option))
        PRINT_ERROR_EXIT("op and option cannot both be null.");

    tipstart = 2;
    name = *op;
    if (name)
    {
        if (*name == '-')
            ++name;
        if (*name == '-' || *name == '\0' || *(name + 1) != '\0')
            PRINT_ERROR_EXIT("op(%s) error. corrent: '-X' or 'X'.", *op);
        if (*name == 'h')
            goto error_help;
        *op = name;
    }
    name = *option;
    if (name)
    {
        if (*name == '-')
        {
            ++name;
            if (*name == '-')
                ++name;
        }

        if ((option_len = (int)strnlen(name, INT_MAX)) < 2)
            PRINT_ERROR_EXIT("option(%s) error. corrent: '--X..X' or '-X..X' or 'X..X'.", *option);
        if (strcmp(name, "help") == 0)
        {
        error_help:
            PRINT_ERROR_EXIT("no need to build '-h' and '--help'.");
        }
        *option = name;
        tipstart += option_len + 4; // ", --"
    }

    return tipstart;
}

static void
cra_mainarg_module_add_item(CraMainArg *ma, char *module_name, CraMainArgItem *item, int tipstart)
{
    char    *name = module_name;
    CraDict *module = cra_mainarg_get_module(ma, name, true);

    if (!name)
        name = "DEFAULT";

    if (item->op && !cra_dict_add(module, &item->op, &item))
        PRINT_ERROR_EXIT("op(-%s) already exists in module(%s).", item->op, name);
    if (item->option && !cra_dict_add(module, &item->option, &item))
        PRINT_ERROR_EXIT("option(--%s) already exists in module(%s).", item->option, name);

    // "  [  ][-x][, ][--x..x] [valtip] tip "
    tipstart += (module_name ? 2 : 0) + (item->valtip ? (int)strlen(item->valtip) : 0) + 5;
    if (tipstart > ma->tipstart)
        ma->tipstart = tipstart;
}

static inline CraMainArgItem *
cra_mainarg_build_val(CraMainArg *ma, char *module, int type, char *op, char *option, char *tip, char *valtip)
{
    assert(tip);
    CraMainArgItem *item;
    int             tipstart;
    tipstart = cra_mainarg_handle_op(&op, &option);
    item = cra_mainarg_make_item(ma, type, op, option, tip, valtip);
    cra_mainarg_module_add_item(ma, module, item, tipstart);
    return item;
}

void
cra_mainarg_build_cmd(CraMainArg *ma, char *module, char *op, char *option, char *tip, cra_mainarg_fn fn, void *arg)
{
    CraMainArgItem *item;
    item = cra_mainarg_build_val(ma, module, MAINARG_CMD, op, option, tip, NULL);
    item->cmd = fn;
    item->arg = arg;
}

void
cra_mainarg_build_two(CraMainArg *ma, char *module, char *op, char *option, char *tip)
{
    cra_mainarg_build_val(ma, module, MAINARG_TWO, op, option, tip, "<on|off>");
}

void
cra_mainarg_build_int_check(CraMainArg *ma,
                            char       *module,
                            char       *op,
                            char       *option,
                            char       *tip,
                            char       *valtip,
                            bool        (*check)(int64_t, void *),
                            void       *arg)
{
    CraMainArgItem *item;
    item = cra_mainarg_build_val(ma, module, MAINARG_INT, op, option, tip, valtip);
    item->check = (cra_mainarg_fn2)check;
    item->arg = arg;
}

void
cra_mainarg_build_flt_check(CraMainArg *ma,
                            char       *module,
                            char       *op,
                            char       *option,
                            char       *tip,
                            char       *valtip,
                            bool        (*check)(double, void *),
                            void       *arg)
{
    CraMainArgItem *item;
    item = cra_mainarg_build_val(ma, module, MAINARG_FLT, op, option, tip, valtip);
    item->check = (cra_mainarg_fn2)check;
    item->arg = arg;
}

void
cra_mainarg_build_str_check(CraMainArg *ma,
                            char       *module,
                            char       *op,
                            char       *option,
                            char       *tip,
                            char       *valtip,
                            bool        (*check)(char *, void *),
                            void       *arg)
{
    CraMainArgItem *item;
    item = cra_mainarg_build_val(ma, module, MAINARG_STR, op, option, tip, valtip);
    item->check = (cra_mainarg_fn2)check;
    item->arg = arg;
}

static CraMainArgItem *
cra_mainarg_get_item(CraDict *module, char *option)
{
    CraMainArgItem *item;
    if (*option == '-')
        ++option;
    if (*option == '-')
        ++option;
    if (cra_dict_get(module, &option, &item))
        return item;
    return NULL;
}

static int
cra_mainarg_set_val(char *name, CraMainArgItem *item, char *val)
{
    char *end = NULL;

    if (item->type == MAINARG_CMD)
    {
        item->cmd(item->arg);
        return 0;
    }

    if (!val)
        PRINT_ERRORT_EXIT("option(%s) need a value.", name);

    switch (item->type)
    {
        case MAINARG_TWO:
            if (strcmp(val, "on") == 0)
                item->val.two = true;
            else if (strcmp(val, "off") == 0)
                item->val.two = false;
            else
                goto invalid_value;
            break;
        case MAINARG_INT:
            item->val.i64 = strtoll(val, &end, 10);
            if (val == end)
                goto invalid_value;
            break;
        case MAINARG_FLT:
            item->val.f64 = strtod(val, &end);
            if (val == end)
                goto invalid_value;
            break;
        case MAINARG_STR:
            item->val.str = val;
            break;
        default:
            assert_always(false);
            break;
    }
    // check value
    if (item->check && !item->check(item->val, item->arg))
        goto invalid_value;
    // value assigned
    item->assigned = true;

    return 1;

invalid_value:
    PRINT_ERRORT_EXIT("option(%s) got an invalid value(%s).", name, val);
}

void
cra_mainarg_parse_args(CraMainArg *ma, int argc, char *argv[])
{
    char           *name;
    CraMainArgItem *item;
    CraDict        *module;

    assert_always(argv);

    module = NULL;
    for (int i = 1; i < argc; i++)
    {
        name = argv[i];
        // module?
        if (*name != '-')
        {
            module = NULL;
            module = cra_mainarg_get_module(ma, name, false);
            if (!module)
            {
                // unbuild!
                cra_alist_append(ma->unbuild, &name);
            }
            continue;
        }

        // get item
        if (!module || !(item = cra_mainarg_get_item(module, name)))
        {
            // maybe the option 'name' is in the default module
            module = cra_mainarg_get_module(ma, NULL, false);
            if (!(item = cra_mainarg_get_item(module, name)))
            {
                if (strcmp(name, "-h") == 0 || strcmp(name, "--help") == 0)
                {
                    cra_mainarg_print_help(ma);
                    exit(EXIT_SUCCESS);
                }
                PRINT_ERRORT_EXIT("invalid option '%s'.", name);
            }
        }

        // set value
        i += cra_mainarg_set_val(name, item, (i + 1) < argc ? argv[i + 1] : NULL);
    }
}

static inline CraMainArgItem *
cra_mainarg_get_val(CraMainArg *ma, char *module_name, char *name, int type)
{
    CraDict        *m;
    CraMainArgItem *item;

    m = cra_mainarg_get_module(ma, module_name, false);
    if (!m)
        PRINT_ERRORT_EXIT("module(%s) doesn't exist.", !!module_name ? module_name : "DEFAULT");
    item = cra_mainarg_get_item(m, name);
    if (!item || !item->assigned)
        return NULL;
    if (item->type != type)
        PRINT_ERRORT_EXIT("option(%s) value type mismatch.", name);
    return item;
}

bool
cra_mainarg_get_two(CraMainArg *ma, char *module, char *name, bool defaultval)
{
    CraMainArgItem *item;
    item = cra_mainarg_get_val(ma, module, name, MAINARG_TWO);
    if (item)
        return item->val.two;
    return defaultval;
}

int64_t
cra_mainarg_get_int(CraMainArg *ma, char *module, char *name, int64_t defaultval)
{
    CraMainArgItem *item;
    item = cra_mainarg_get_val(ma, module, name, MAINARG_INT);
    if (item)
        return item->val.i64;
    return defaultval;
}

double
cra_mainarg_get_flt(CraMainArg *ma, char *module, char *name, double defaultval)
{
    CraMainArgItem *item;
    item = cra_mainarg_get_val(ma, module, name, MAINARG_FLT);
    if (item)
        return item->val.f64;
    return defaultval;
}

char *
cra_mainarg_get_str(CraMainArg *ma, char *module, char *name, char *defaultval)
{
    CraMainArgItem *item;
    item = cra_mainarg_get_val(ma, module, name, MAINARG_STR);
    if (item)
        return item->val.str;
    return defaultval;
}

size_t
cra_mainarg_get_unbuild_count(CraMainArg *ma)
{
    return cra_alist_get_count(ma->unbuild);
}

bool
cra_mainarg_get_unbuild_two(CraMainArg *ma, size_t index, bool defaultval)
{
    char *str;
    if (!cra_alist_get(ma->unbuild, index, &str))
        return defaultval;
    if (strcmp(str, "on") == 0)
        return true;
    else if (strcmp(str, "off") == 0)
        return false;

    PRINT_ERRORT_EXIT("invalid unbuild value(%zu, %s).", index, str);
}

int64_t
cra_mainarg_get_unbuild_int_check(CraMainArg *ma,
                                  size_t      index,
                                  int64_t     defaultval,
                                  bool        (*check)(int64_t, void *),
                                  void       *arg)
{
    int64_t ret;
    char   *str, *end = NULL;
    if (!cra_alist_get(ma->unbuild, index, &str))
        return defaultval;
    ret = strtoll(str, &end, 10);
    if (str == end || (check && !check(ret, arg)))
        PRINT_ERRORT_EXIT("invalid unbuild value(%zu, %s).", index, str);
    return ret;
}

double
cra_mainarg_get_unbuild_flt_check(CraMainArg *ma,
                                  size_t      index,
                                  double      defaultval,
                                  bool        (*check)(double, void *),
                                  void       *arg)
{
    double ret;
    char  *str, *end = NULL;
    if (!cra_alist_get(ma->unbuild, index, &str))
        return defaultval;
    ret = strtod(str, &end);
    if (str == end || (check && !check(ret, arg)))
        PRINT_ERRORT_EXIT("invalid unbuild value(%zu, %s).", index, str);
    return ret;
}

char *
cra_mainarg_get_unbuild_str_check(CraMainArg *ma,
                                  size_t      index,
                                  char       *defaultval,
                                  bool        (*check)(char *, void *),
                                  void       *arg)
{
    char *ret;
    if (!cra_alist_get(ma->unbuild, index, &ret))
        return defaultval;
    if (check && !check(ret, arg))
        PRINT_ERRORT_EXIT("invalid unbuild value(%zu, %s).", index, ret);
    return ret;
}

static int
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
cra_mainarg_print_tip(int tipstart, int linemax, char *tip)
{
    for (int i = tipstart; *tip != '\0'; ++i)
    {
        if (i >= linemax)
        {
            while (cra_mainarg_get_char_bytes((unsigned char)*tip) == 1 && *tip != ' ' && *tip != '\0')
                printf("%c", *tip++);

            printf("\n%*.s", tipstart, "");
            i = tipstart;

            if (cra_mainarg_get_char_bytes((unsigned char)*tip) == 1 && *tip == ' ')
                ++tip; // 空格不要出现在开头
            continue;
        }
        cra_mainarg_print_one_char(&tip);
    }
    printf("\n");
}

static void
cra_mainarg_print_module(CraDict *module, int tipstart, int linemax, int spaces)
{
    int              l;
    CraDictIter      it;
    CraMainArgItem **pitem, *item = NULL;

    for (cra_dict_iter_init(module, &it); cra_dict_iter_next(&it, NULL, (void **)&pitem);)
    {
        if (item == *pitem)
            continue;

        item = *pitem;

        // "  [  ][-X][, ][--X..X] [valtip] [tip]"
        l = printf("%*.s%c%c%s%s%s %s",
                   spaces,
                   "",
                   item->op ? '-' : ' ',
                   item->op ? *item->op : ' ',
                   (item->op && item->option) ? ", " : (item->option ? "  " : ""),
                   item->option ? "--" : "",
                   item->option ? item->option : "",
                   item->valtip ? item->valtip : "");
        // print tip
        assert(tipstart - l >= 0);
        printf("%*.s", tipstart - l, "");
        cra_mainarg_print_tip(tipstart, linemax, item->tip);
    }
}

void
cra_mainarg_print_help(CraMainArg *ma)
{
    CraDictIter it;
    CraDict   **pmodule;
    char      **pmodule_name;
    CraDict    *module_default;

    printf("Usage: %s %s\n\n%s\n\n", ma->program, ma->usage, ma->introdution);

    // print default module first
    module_default = cra_mainarg_get_module(ma, NULL, false);
    if (module_default)
        cra_mainarg_print_module(module_default, ma->tipstart, ma->line_max, 2);

    for (cra_dict_iter_init(ma->modules, &it); cra_dict_iter_next(&it, (void **)&pmodule_name, (void **)&pmodule);)
    {
        if (*pmodule != module_default)
        {
            printf("%s\n", *pmodule_name);
            cra_mainarg_print_module(*pmodule, ma->tipstart, ma->line_max, 4);
        }
    }
}
