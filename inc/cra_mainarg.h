/**
 * @file cra_opconf.h
 * @author Cracal
 * @brief main args
 * @version 0.1
 * @date 2025-11-27
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __CRA_MAINARG_H__
#define __CRA_MAINARG_H__
#include "cra_defs.h"

/**
 * OP:  -op [opval], --option [opval]
 * OPS: OP1 [...OPn]
 *
 * program OPS
 * program module1 OPS1 [...moduleN OPSn]
 *
 */

typedef struct _CraMainArg CraMainArg;
typedef struct _CraAList   CraAList;
typedef struct _CraDict    CraDict;

typedef void (*cra_mainarg_fn)(void *user);

typedef enum
{
    CRA_MAINARG_TYPE_TWO, // on|off
    CRA_MAINARG_TYPE_INT, // int64
    CRA_MAINARG_TYPE_FLT, // float64
    CRA_MAINARG_TYPE_STR, // char *
    CRA_MAINARG_TYPE_CMD, // command
} CraMainArgType_e;

struct _CraMainArg
{
    int       tipstart;
    int       line_max;
    char     *usage;
    char     *introduction;
    char     *program; // program's name
    CraDict  *modules; // option module. Dict<char *, Dict<char *, MainArgItem *> *>
    CraAList *unbuild; // those options without BUILD. AList<char *>
};

CRA_API void
cra_mainarg_init(CraMainArg *ma, char *usage, char *introduction, int linemax);

CRA_API void
cra_mainarg_uninit(CraMainArg *ma);

CRA_API void
__cra_mainarg_build_val(CraMainArg      *ma,
                        char            *module_name,
                        char            *op,
                        char            *option,
                        char            *tip,
                        CraMainArgType_e type,
                        cra_mainarg_fn   func,
                        void            *ptr);
#define cra_mainarg_build_two(_ma, _module_name, _op, _option, _tip)                                       \
    __cra_mainarg_build_val(_ma, _module_name, _op, _option, _tip, CRA_MAINARG_TYPE_TWO, NULL, "<on|off>")
#define cra_mainarg_build_int(_ma, _module_name, _op, _option, _tip, _valtip)                           \
    __cra_mainarg_build_val(_ma, _module_name, _op, _option, _tip, CRA_MAINARG_TYPE_INT, NULL, _valtip)
#define cra_mainarg_build_flt(_ma, _module_name, _op, _option, _tip, _valtip)                           \
    __cra_mainarg_build_val(_ma, _module_name, _op, _option, _tip, CRA_MAINARG_TYPE_FLT, NULL, _valtip)
#define cra_mainarg_build_str(_ma, _module_name, _op, _option, _tip, _valtip)                           \
    __cra_mainarg_build_val(_ma, _module_name, _op, _option, _tip, CRA_MAINARG_TYPE_STR, NULL, _valtip)
#define cra_mainarg_build_cmd(_ma, _module_name, _op, _option, _tip, _fn, _user)                     \
    __cra_mainarg_build_val(_ma, _module_name, _op, _option, _tip, CRA_MAINARG_TYPE_CMD, _fn, _user)

CRA_API void
cra_mainarg_parse_args(CraMainArg *ma, int argc, char *argv[]);

CRA_API bool
cra_mainarg_get_two(CraMainArg *ma, const char *module, const char *name, bool default_value);

CRA_API int64_t
cra_mainarg_get_int(CraMainArg *ma, const char *module, const char *name, int64_t default_value);

CRA_API double
cra_mainarg_get_flt(CraMainArg *ma, const char *module, const char *name, double default_value);

CRA_API const char *
cra_mainarg_get_str(CraMainArg *ma, const char *module, const char *name, const char *default_value);

static inline CraAList *
cra_mainarg_get_unbuild(CraMainArg *ma)
{
    return ma->unbuild;
}

CRA_API int64_t
cra_mainarg_get_unbuild_int(CraMainArg *ma, unsigned int index, int64_t default_value);

CRA_API double
cra_mainarg_get_unbuild_flt(CraMainArg *ma, unsigned int index, double default_value);

CRA_API const char *
cra_mainarg_get_unbuild_str(CraMainArg *ma, unsigned int index, const char *default_value);

CRA_API void
cra_mainarg_print_help(CraMainArg *ma);

#endif