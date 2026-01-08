#include "cra_mainarg.h"
#include "collections/cra_alist.h"
#include "collections/cra_dict.h"
#include "cra_assert.h"
#include "cra_malloc.h"
#include "cra_mempool.h"

#define ERROR_BEGIN            "%s(ERROR): "
#define ERROR_ENDT             " Type '-h' or '--help' to get some help.\n\n"
#define ERROR_END              "\n\n"
#define PRINT_ERROR(fmt, ...)  fprintf(stderr, ERROR_BEGIN fmt ERROR_END, cra_basename(ma->program), ##__VA_ARGS__)
#define PRINT_ERRORT(fmt, ...) fprintf(stderr, ERROR_BEGIN fmt ERROR_ENDT, cra_basename(ma->program), ##__VA_ARGS__)
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

typedef struct _CraMainArgItem CraMainArgItem;

struct _CraMainArgItem
{
    CraMainArgElement *element;
    CraMainArgVal_u    val;
    bool               assigned;
};

static void
cra_mainarg_get_count(CraMainArgElement elements[], int *noption, int *nitem)
{
    CraMainArgElement *elem;
    *noption = *nitem = 0;
    for (elem = elements; memcmp(elem, &(CraMainArgElement){ NULL }, sizeof(CraMainArgElement)) != 0; ++elem)
    {
        ++(*nitem);
        *noption += (!!elem->op + !!elem->option);
    }
}

static inline char *
cra_mainarg_clear_op(char *op)
{
    if (*op == '-')
        ++op;
    if (*op == '-')
        ++op;
    return op;
}

static void
cra_mainarg_add_item(CraMainArg *ma, CraMainArgItem *item)
{
    char              *name;
    int                tipstart = 2;
    CraMainArgElement *elem = item->element;

    if (elem->op)
    {
        name = elem->op;
        if (*name != '-' || *(name + 1) == '\0' || *(name + 2) != '\0')
            PRINT_ERROR_EXIT("Invalid option('%s'). correct: '-X'.", elem->op);
        ++name;
        if (!cra_dict_add(ma->items, &name, &item))
        {
            --name;
            goto add_error;
        }
    }
    if (elem->option)
    {
        int len;
        name = elem->option;
        len = (int)strlen(name);
        if (len < 4 || *name != '-' || *(name + 1) != '-')
            PRINT_ERROR_EXIT("Invalid option('%s'). correct: '--X..X'.", elem->option);
        name += 2;
        if (!cra_dict_add(ma->items, &name, &item))
        {
            name -= 2;
            goto add_error;
        }
        tipstart += len;
    }
    // __[-X][,_][--X..X][_len(valtip)]
    tipstart += (elem->option ? 2 : 0) + (elem->valtip ? (int)strlen(elem->valtip) + 1 : 0) + 2;
    if (tipstart > ma->tipstart)
        ma->tipstart = tipstart;
    return;

add_error:
    PRINT_ERROR_EXIT("Option(%s) is already existed.", name);
}

static void
cra_mainarg_build(CraMainArg *ma, CraMainArgElement elements[], int nitems)
{
    int                i;
    CraMainArgItem    *item;
    CraMainArgElement *elem;
    for (i = 0, elem = elements; i < nitems; ++i, ++elem)
    {
#define CRA_OPTIONS_FMT "Option(%s%s%s)"
#define CRA_OPTIONS_ARG                                                                                \
    elem->op ? elem->op : "", (elem->op && elem->option) ? ", " : "", elem->option ? elem->option : ""

        if (!elem->op && !elem->option)
            PRINT_ERROR_EXIT("Option's `op` and `option` cannot both be null.");
        if (!elem->optip)
            PRINT_ERROR_EXIT(CRA_OPTIONS_FMT "'s `optip` mustn't be null.", CRA_OPTIONS_ARG);
        if ((elem->func && !elem->valtip))
            PRINT_ERROR_EXIT(CRA_OPTIONS_FMT " require `valtip`.", CRA_OPTIONS_ARG);
        if ((!elem->func && elem->valtip))
            PRINT_ERROR_EXIT(CRA_OPTIONS_FMT " doesn't require `valtip`.", CRA_OPTIONS_ARG);

#undef CRA_OPTIONS_FMT
#undef CRA_OPTIONS_ARG

        item = (CraMainArgItem *)cra_mempool_alloc(ma->pool);
        item->element = elem;
        item->val.i = 0;
        item->assigned = false;
        cra_mainarg_add_item(ma, item);
    }
}

void
cra_mainarg_init(CraMainArg *ma, char *program, char *intro, char *usage, CraMainArgElement options[])
{
    assert(program);
    assert(intro);
    assert(options);

    size_t len;
    int    noption, nitem;

    ma->tipstart = 0;
    len = strlen(program) + 1;
    ma->program = (char *)cra_malloc(len);
    memcpy(ma->program, program, len);
    ma->introduction = intro;
    ma->usage = usage;
    ma->items = cra_alloc(CraDict);
    ma->notop = cra_alloc(CraAList);
    ma->pool = cra_alloc(CraMemPool);

    cra_mainarg_get_count(options, &noption, &nitem);

    if (noption == 0 || nitem == 0)
        PRINT_ERROR_EXIT("Options array cannot empty.");

    cra_dict_init_size0(char *,
                        CraMainArgItem *,
                        ma->items,
                        noption,
                        false,
                        (cra_hash_fn)cra_hash_string1_p,
                        (cra_compare_fn)cra_compare_string_p);
    cra_alist_init0(char *, ma->notop, false);
    cra_mempool_init(ma->pool, sizeof(CraMainArgItem), nitem);

    cra_mainarg_build(ma, options, nitem);
}

void
cra_mainarg_uninit(CraMainArg *ma)
{
    cra_dict_uninit(ma->items);
    cra_alist_uninit(ma->notop);
    cra_mempool_uninit(ma->pool);
    cra_dealloc(ma->items);
    cra_dealloc(ma->notop);
    cra_dealloc(ma->pool);
    cra_free(ma->program);
}

void
cra_mainarg_parse_args(CraMainArg *ma, int argc, char *argv[])
{
    char           *option;
    char           *opval;
    CraMainArgItem *item;

    for (int i = 1; i < argc; ++i)
    {
        option = cra_mainarg_clear_op(argv[i]);
        if (!cra_dict_get(ma->items, &option, &item))
        {
            // print help?
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                cra_mainarg_print_help(ma);
                exit(EXIT_SUCCESS);
            }

            // add to notop
            cra_alist_append(ma->notop, &argv[i]);
            continue;
        }

        item->assigned = true;
        if (item->element->func)
        {
            if (++i < argc)
                opval = argv[i];
            else
                PRINT_ERRORT_EXIT("Options(%s) need a value.", argv[i - 1]);

            if (!item->element->func(&item->val, opval, item->element->arg))
                PRINT_ERRORT_EXIT("Option(%s) got an invalid option value(%s).", argv[i - 1], opval);
        }
        else
        {
            item->val.b = true;
        }
    }
}

CraMainArgVal_u
cra_mainarg_get_val(CraMainArg *ma, char *option, CraMainArgVal_u default_val)
{
    assert(option);

    char           *name;
    CraMainArgItem *item;

    name = cra_mainarg_clear_op(option);
    if (!cra_dict_get(ma->items, &name, &item))
        PRINT_ERRORT_EXIT("Unknown option(%s).", option);

    if (item->assigned)
        return item->val;
    return default_val;
}

int
cra_mainarg_get_notop_count(CraMainArg *ma)
{
    return (int)cra_alist_get_count(ma->notop);
}

CraMainArgVal_u
cra_mainarg_get_notop_val(CraMainArg *ma, int index, CraMainArgVal_u default_val, cra_mainarg_fn func, void *arg)
{
    assert(index >= 0);
    assert(func);

    char           *str;
    CraMainArgVal_u val;

    if (!cra_alist_get(ma->notop, index, &str))
        return default_val;

    if (!func(&val, str, arg))
        PRINT_ERROR_EXIT("Invalid item(%s).", str);
    return val;
}

void
cra_mainarg_print_help(CraMainArg *ma)
{
    int                l;
    CraDictIter        it;
    CraMainArgElement *elem;
    CraMainArgItem    *last;
    CraMainArgItem   **pitem;

    printf("%s\n", ma->introduction);
    printf("Usage: %s %s\n\n", cra_basename(ma->program), ma->usage);

    last = NULL;
    printf("Options:\n");
    printf("  -h, --help  %*.sShow options\n", ma->tipstart - 12, "");
    for (cra_dict_iter_init(ma->items, &it); cra_dict_iter_next(&it, NULL, (void **)&pitem);)
    {
        if (*pitem == last)
            continue;

        last = *pitem;
        elem = (*pitem)->element;
        // __[-X][,_][--X..X][_len(valtip)]__len(optip)
        l = printf("  %s%s%s %s",
                   elem->op ? elem->op : "  ",
                   elem->option ? (elem->op ? ", " : "  ") : "",
                   elem->option ? elem->option : "",
                   elem->valtip ? elem->valtip : "");
        printf("  %*.s%s\n", ma->tipstart - l, "", elem->optip);
    }
}

// ====================

bool
cra_mainarg_stob(CraMainArgVal_u *retval, const char *opval, void *_)
{
    assert(!_);
    CRA_UNUSED_VALUE(_);
    return cra_mainarg_stob_values(retval, opval, (char *[]){ "on", "off" });
}

bool
cra_mainarg_stoi(CraMainArgVal_u *retval, const char *opval, void *_)
{
    assert(retval);
    assert(opval);
    assert(!_);
    CRA_UNUSED_VALUE(_);

    char   *end = NULL;
    int64_t i = strtoll(opval, &end, 10);
    if (end != opval)
    {
        retval->i = i;
        return true;
    }
    return false;
}

bool
cra_mainarg_stof(CraMainArgVal_u *retval, const char *opval, void *_)
{
    assert(retval);
    assert(opval);
    assert(!_);
    CRA_UNUSED_VALUE(_);

    char  *end = NULL;
    double f = strtod(opval, &end);
    if (end != opval)
    {
        retval->f = f;
        return true;
    }
    return false;
}

bool
cra_mainarg_stos(CraMainArgVal_u *retval, const char *opval, void *_)
{
    assert(retval);
    assert(opval);
    assert(!_);
    CRA_UNUSED_VALUE(_);

    retval->s = (char *)opval;
    return true;
}

bool
cra_mainarg_stob_values(CraMainArgVal_u *retval, const char *opval, void *values)
{
    assert(retval);
    assert(opval);
    assert(values);

    char **strs = (char **)values;
    if (strcmp(opval, strs[0]) == 0)
    {
        retval->b = true;
        return true;
    }
    if (strcmp(opval, strs[1]) == 0)
    {
        retval->b = false;
        return true;
    }
    return false;
}

bool
cra_mainarg_stoi_in_range(CraMainArgVal_u *retval, const char *opval, void *range)
{
    assert(range);

    if (!cra_mainarg_stoi(retval, opval, NULL))
        return false;

    return retval->i >= ((int64_t *)range)[0] && retval->i < ((int64_t *)range)[1];
}

bool
cra_mainarg_stof_in_range(CraMainArgVal_u *retval, const char *opval, void *range)
{
    assert(range);

    if (!cra_mainarg_stof(retval, opval, NULL))
        return false;

    return retval->f >= ((double *)range)[0] && retval->f < ((double *)range)[1];
}

bool
cra_mainarg_stos_in_array(CraMainArgVal_u *retval, const char *opval, void *array)
{
    assert(retval);
    assert(opval);
    assert(array);

    char **strs = (char **)array;
    while (*strs)
    {
        if (strcmp(opval, *strs) == 0)
        {
            retval->s = (char *)opval;
            return true;
        }
        ++strs;
    }
    return false;
}
