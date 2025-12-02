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

typedef union
{
    bool    two;
    int64_t i64;
    double  f64;
    char   *str;
} CraMainArgVal;

typedef struct _CraMainArg CraMainArg;
typedef struct _CraMemPool CraMemPool;
typedef struct _CraAList   CraAList;
typedef struct _CraDict    CraDict;

typedef void (*cra_mainarg_fn)(void *arg);
typedef bool (*cra_mainarg_fn2)(CraMainArgVal val, void *arg);

struct _CraMainArg
{
    int         tipstart;
    int         line_max;
    char       *program;
    char       *usage;
    char       *introdution;
    CraDict    *modules; // Dict<char *, Dict<char *, MainItem *> *>
    CraAList   *unbuild; // AList<char *>
    CraMemPool *items;
};

CRA_API void
cra_mainarg_init(CraMainArg *ma, char *program, char *usage, char *introduction, int linemax);

CRA_API void
cra_mainarg_uninit(CraMainArg *ma);

CRA_API void
cra_mainarg_build_int_check(CraMainArg *ma,
                            char       *module,
                            char       *op,
                            char       *option,
                            char       *tip,
                            char       *valtip,
                            bool        (*check)(int64_t, void *),
                            void       *arg);

CRA_API void
cra_mainarg_build_flt_check(CraMainArg *ma,
                            char       *module,
                            char       *op,
                            char       *option,
                            char       *tip,
                            char       *valtip,
                            bool        (*check)(double, void *),
                            void       *arg);

CRA_API void
cra_mainarg_build_str_check(CraMainArg *ma,
                            char       *module,
                            char       *op,
                            char       *option,
                            char       *tip,
                            char       *valtip,
                            bool        (*check)(char *, void *),
                            void       *arg);

CRA_API void
cra_mainarg_build_cmd(CraMainArg *ma, char *module, char *op, char *option, char *tip, cra_mainarg_fn fn, void *arg);

CRA_API void
cra_mainarg_build_two(CraMainArg *ma, char *module, char *op, char *option, char *tip);

#define cra_mainarg_build_int(_ma, _module, _op, _option, _tip, _valtip)               \
    cra_mainarg_build_int_check(_ma, _module, _op, _option, _tip, _valtip, NULL, NULL)

#define cra_mainarg_build_flt(_ma, _module, _op, _option, _tip, _valtip)               \
    cra_mainarg_build_flt_check(_ma, _module, _op, _option, _tip, _valtip, NULL, NULL)

#define cra_mainarg_build_str(_ma, _module, _op, _option, _tip, _valtip)               \
    cra_mainarg_build_str_check(_ma, _module, _op, _option, _tip, _valtip, NULL, NULL)

CRA_API void
cra_mainarg_parse_args(CraMainArg *ma, int argc, char *argv[]);

CRA_API bool
cra_mainarg_get_two(CraMainArg *ma, char *module, char *name, bool defaultval);

CRA_API int64_t
cra_mainarg_get_int(CraMainArg *ma, char *module, char *name, int64_t defaultval);

CRA_API double
cra_mainarg_get_flt(CraMainArg *ma, char *module, char *name, double defaultval);

CRA_API char *
cra_mainarg_get_str(CraMainArg *ma, char *module, char *name, char *defaultval);

CRA_API size_t
cra_mainarg_get_unbuild_count(CraMainArg *ma);

CRA_API int64_t
cra_mainarg_get_unbuild_int_check(CraMainArg *ma,
                                  size_t      index,
                                  int64_t     defaultval,
                                  bool        (*check)(int64_t, void *),
                                  void       *arg);

CRA_API double
cra_mainarg_get_unbuild_flt_check(CraMainArg *ma,
                                  size_t      index,
                                  double      defaultval,
                                  bool        (*check)(double, void *),
                                  void       *arg);

CRA_API char *
cra_mainarg_get_unbuild_str_check(CraMainArg *ma,
                                  size_t      index,
                                  char       *defaultval,
                                  bool        (*check)(char *, void *),
                                  void       *arg);

CRA_API bool
cra_mainarg_get_unbuild_two(CraMainArg *ma, size_t index, bool defaultval);

#define cra_mainarg_get_unbuild_int(_ma, _index, _defaultval)               \
    cra_mainarg_get_unbuild_int_check(_ma, _index, _defaultval, NULL, NULL)

#define cra_mainarg_get_unbuild_flt(_ma, _index, _defaultval)               \
    cra_mainarg_get_unbuild_flt_check(_ma, _index, _defaultval, NULL, NULL)

#define cra_mainarg_get_unbuild_str(_ma, _index, _defaultval)               \
    cra_mainarg_get_unbuild_str_check(_ma, _index, _defaultval, NULL, NULL)

CRA_API void
cra_mainarg_print_help(CraMainArg *ma);

#endif