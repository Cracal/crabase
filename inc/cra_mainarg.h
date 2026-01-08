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

typedef union
{
    bool    b;
    int64_t i;
    double  f;
    char   *s;
} CraMainArgVal_u;

typedef struct _CraDict           CraDict;
typedef struct _CraAList          CraAList;
typedef struct _CraMemPool        CraMemPool;
typedef struct _CraMainArg        CraMainArg;
typedef struct _CraMainArgElement CraMainArgElement;

typedef bool (*cra_mainarg_fn)(CraMainArgVal_u *retval, const char *opval, void *arg);

struct _CraMainArgElement
{
    char          *op;
    char          *option;
    char          *valtip;
    char          *optip;
    cra_mainarg_fn func;
    void          *arg;
};

#define CRA_MAINARG_ELEMENT_SET(_op, _option, _valtip, _optip, _func, _arg) \
    { _op, _option, _valtip, _optip, _func, _arg }
#define CRA_MAINARG_ELEMENT_BOL(_op, _option, _optip) CRA_MAINARG_ELEMENT_SET(_op, _option, NULL, _optip, NULL, NULL)
#define CRA_MAINARG_ELEMENT_VAL(_op, _option, _valtip, _optip, _func, _arg) \
    CRA_MAINARG_ELEMENT_SET(_op, _option, _valtip, _optip, _func, _arg)
#define CRA_MAINARG_ELEMENT_END() { NULL }

struct _CraMainArg
{
    int         tipstart;
    char       *introduction;
    char       *program;
    char       *usage;
    CraDict    *items; // Dict<char *, Item *>
    CraAList   *notop; // AList<char *>
    CraMemPool *pool;  // MemPool<Item>
};

CRA_API void
cra_mainarg_init(CraMainArg *ma, char *program, char *intro, char *usage, CraMainArgElement options[]);

CRA_API void
cra_mainarg_uninit(CraMainArg *ma);

CRA_API void
cra_mainarg_parse_args(CraMainArg *ma, int argc, char *argv[]);

CRA_API CraMainArgVal_u
cra_mainarg_get_val(CraMainArg *ma, char *option, CraMainArgVal_u default_val);
#define cra_mainarg_get_b(_ma, _option, _default_val)                           \
    cra_mainarg_get_val(_ma, _option, (CraMainArgVal_u){ .b = _default_val }).b
#define cra_mainarg_get_i(_ma, _option, _default_val)                           \
    cra_mainarg_get_val(_ma, _option, (CraMainArgVal_u){ .i = _default_val }).i
#define cra_mainarg_get_f(_ma, _option, _default_val)                           \
    cra_mainarg_get_val(_ma, _option, (CraMainArgVal_u){ .f = _default_val }).f
#define cra_mainarg_get_s(_ma, _option, _default_val)                           \
    cra_mainarg_get_val(_ma, _option, (CraMainArgVal_u){ .s = _default_val }).s

CRA_API int
cra_mainarg_get_notop_count(CraMainArg *ma);

CRA_API CraMainArgVal_u
cra_mainarg_get_notop_val(CraMainArg *ma, int index, CraMainArgVal_u default_val, cra_mainarg_fn func, void *arg);
#define cra_mainarg_get_notop_b(_ma, _index, _default_val, _func, _arg)                           \
    cra_mainarg_get_notop_val(_ma, _index, (CraMainArgVal_u){ .b = _default_val }, _func, _arg).b
#define cra_mainarg_get_notop_i(_ma, _index, _default_val, _func, _arg)                           \
    cra_mainarg_get_notop_val(_ma, _index, (CraMainArgVal_u){ .i = _default_val }, _func, _arg).i
#define cra_mainarg_get_notop_f(_ma, _index, _default_val, _func, _arg)                           \
    cra_mainarg_get_notop_val(_ma, _index, (CraMainArgVal_u){ .f = _default_val }, _func, _arg).f
#define cra_mainarg_get_notop_s(_ma, _index, _default_val, _func, _arg)                           \
    cra_mainarg_get_notop_val(_ma, _index, (CraMainArgVal_u){ .s = _default_val }, _func, _arg).s

CRA_API void
cra_mainarg_print_help(CraMainArg *ma);

// ============================

// `opval` = "on" or "off"
CRA_API bool
cra_mainarg_stob(CraMainArgVal_u *retval, const char *opval, void *_);

CRA_API bool
cra_mainarg_stoi(CraMainArgVal_u *retval, const char *opval, void *_);

CRA_API bool
cra_mainarg_stof(CraMainArgVal_u *retval, const char *opval, void *_);

CRA_API bool
cra_mainarg_stos(CraMainArgVal_u *retval, const char *opval, void *_);

// `values`: char *values[2] = {"true str", "false str"}
CRA_API bool
cra_mainarg_stob_values(CraMainArgVal_u *retval, const char *opval, void *values);

// `range`: int64_t range[2] = {min, max}. [min, max)
CRA_API bool
cra_mainarg_stoi_in_range(CraMainArgVal_u *retval, const char *opval, void *range);

// `range`: double range[2] = {min, max}. [min, max)
CRA_API bool
cra_mainarg_stof_in_range(CraMainArgVal_u *retval, const char *opval, void *range);

// `array`: char *array[] = {"str1", ..., "strN", NULL}.
CRA_API bool
cra_mainarg_stos_in_array(CraMainArgVal_u *retval, const char *opval, void *array);

#endif