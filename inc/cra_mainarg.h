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

typedef struct CraDict           CraDict;
typedef struct CraAList          CraAList;
typedef struct CraMemPool        CraMemPool;
typedef struct CraMainArg        CraMainArg;
typedef struct CraMainArgElement CraMainArgElement;

typedef bool (*cra_mainarg_fn)(CraMainArgVal_u *retval, const char *opval, void *arg);

struct CraMainArgElement
{
    char          *op;
    char          *option;
    char          *valtip;
    char          *optip;
    cra_mainarg_fn func;
    void          *arg;
};

#define CRA_MAINARG_ELEMENT_BEGIN(name) CraMainArgElement name[] = {
#define CRA_MAINARG_ELEMENT_END() {0}}
#define CRA_MAINARG_ELEMENT_SET(op, option, valtip, optip, func, arg) { op, option, valtip, optip, func, arg },
#define CRA_MAINARG_ELEMENT_BOL(op, option, optip) CRA_MAINARG_ELEMENT_SET(op, option, NULL, optip, NULL, NULL)
#define CRA_MAINARG_ELEMENT_VAL                    CRA_MAINARG_ELEMENT_SET

struct CraMainArg
{
    int         tipstart;
    const char *introduction;
    char       *program;
    const char *usage;
    CraDict    *items;    // Dict<char *, Item *>
    CraMemPool *pool;     // MemPool<Item>
    CraAList   *pos_args; // AList<char *>
};

CRA_API void
cra_mainarg_init(CraMainArg *ma, char *program, const char *intro, const char *usage, CraMainArgElement options[]);

CRA_API void
cra_mainarg_uninit(CraMainArg *ma);

CRA_API void
cra_mainarg_parse_args(CraMainArg *ma, int argc, char *argv[]);

CRA_API CraMainArgVal_u
cra_mainarg_get_val(CraMainArg *ma, char *option, CraMainArgVal_u default_val);
#define cra_mainarg_get_b(ma, option, default_val)                           \
    cra_mainarg_get_val(ma, option, (CraMainArgVal_u){ .b = default_val }).b
#define cra_mainarg_get_i(ma, option, default_val)                           \
    cra_mainarg_get_val(ma, option, (CraMainArgVal_u){ .i = default_val }).i
#define cra_mainarg_get_f(ma, option, default_val)                           \
    cra_mainarg_get_val(ma, option, (CraMainArgVal_u){ .f = default_val }).f
#define cra_mainarg_get_s(ma, option, default_val)                           \
    cra_mainarg_get_val(ma, option, (CraMainArgVal_u){ .s = default_val }).s

CRA_API int
cra_mainarg_get_pos_args_count(CraMainArg *ma);

CRA_API CraMainArgVal_u
cra_mainarg_get_pos_args_val(CraMainArg *ma, int index, CraMainArgVal_u default_val, cra_mainarg_fn func, void *arg);
#define cra_mainarg_get_pos_args_b(ma, index, default_val, func, arg)                           \
    cra_mainarg_get_pos_args_val(ma, index, (CraMainArgVal_u){ .b = default_val }, func, arg).b
#define cra_mainarg_get_pos_args_i(ma, index, default_val, func, arg)                           \
    cra_mainarg_get_pos_args_val(ma, index, (CraMainArgVal_u){ .i = default_val }, func, arg).i
#define cra_mainarg_get_pos_args_f(ma, index, default_val, func, arg)                           \
    cra_mainarg_get_pos_args_val(ma, index, (CraMainArgVal_u){ .f = default_val }, func, arg).f
#define cra_mainarg_get_pos_args_s(ma, index, default_val, func, arg)                           \
    cra_mainarg_get_pos_args_val(ma, index, (CraMainArgVal_u){ .s = default_val }, func, arg).s

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