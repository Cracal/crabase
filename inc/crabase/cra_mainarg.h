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
#ifndef __CRABASE_CRA_MAINARG_H__
#define __CRABASE_CRA_MAINARG_H__
#include "cra_macro.h"

typedef struct CraMainArgItem CraMainArgItem;
typedef struct CraMainArg     CraMainArg;

// [-o][, --option] [val]  desc
struct CraMainArgItem
{
    int         op;     // 1. short option, 2. key of long option
    int         length; // len(option) + len(val) + 1
    const char *option;
    const char *val;
    const char *desc;
};

struct CraMainArg
{
    const CraMainArgItem *items;
    const char           *usage;
    const char           *version;
    const char           *description;

    bool (*parse)(int op, const char *option, const char *value, void *user_data);
};

#define CRA_MAINARG_IS_STR(str) _Generic((0, str), char *: 1, const char *: 1, default: 0)
#define CRA_MAINARG_LEN_OF_STR(str, amount)                                                          \
    _Generic((0, str), char *: sizeof(str) - amount, const char *: sizeof(str) - amount, default: 0)

#define CRA_MAINARG_ITEM_CHECK3(op, option, desc)                                                               \
    static_assert(op != 'v' && op != 'h', "version and help options are reserved");                             \
    static_assert(op != 0 || CRA_MAINARG_IS_STR(option), "at least one of short option or long option must be " \
                                                         "set");                                                \
    static_assert(!CRA_MAINARG_IS_STR(option) || CRA_MAINARG_LEN_OF_STR(option, 0) > 2,                         \
                  "long option must be at least 2 characters long");                                            \
    static_assert(CRA_MAINARG_IS_STR(desc), "desc must be set");
#define CRA_MAINARG_ITEM_CHECK4(op, option, val, desc) CRA_MAINARG_ITEM_CHECK3(op, option, desc)
#define CRA_MAINARG_ITEM_CHECK(...) CRA_CAT(CRA_MAINARG_ITEM_CHECK, CRA_CNT(__VA_ARGS__))(__VA_ARGS__)

#define CRA_MAINARG_LENGTH(option, val) CRA_MAINARG_LEN_OF_STR(option, 1) + CRA_MAINARG_LEN_OF_STR(val, 0)

#define CRA_MAINARG_ITEM_DEF3(op, option, desc)      { op, CRA_MAINARG_LENGTH(option, 0), option, NULL, desc }
#define CRA_MAINARG_ITEM_DEF4(op, option, val, desc) { op, CRA_MAINARG_LENGTH(option, val), option, val, desc }
#define CRA_MAINARG_ITEM_DEF(...)                    CRA_CAT(CRA_MAINARG_ITEM_DEF, CRA_CNT(__VA_ARGS__))(__VA_ARGS__),

// name: name of the main arg variant
// version: version of the program
// usage: usage of the program command line
// description: description of the program
// parse_fn: parse function: bool (*)(int op, const char *option, const char *value, void *user_data)
// ...: options definitions: (op: char, option: char * [, val: char *],  desc: char *)
//       ('4', "ipv4", "Force IPv4 address")
//       ('d', "debug", "<level>", "Set debug level")
//       (0, "debug", "<level>", "Set debug level")   op == 0 -> no short option
//       ('d', NULL, "<level>", "Set debug level")    option == NULL -> no long option
#define CRA_MAINARG_DEF(name, version, usage, description, parse_fn, ...)                                      \
    CRA_ITER(CRA_UNTUPLE_M, CRA_MAINARG_ITEM_CHECK, __VA_ARGS__)                                               \
    const CraMainArgItem name##_items[] = { CRA_ITER(CRA_UNTUPLE_M, CRA_MAINARG_ITEM_DEF, __VA_ARGS__){ 0 } }; \
    const CraMainArg     name = { name##_items, usage, version, description, parse_fn }

#define CRA_MA_NON_OP  (-1)
#define CRA_MA_LONG_OP (0)

CRA_API bool
cra_mainarg_parse(const CraMainArg *ma, int argc, const char *argv[], void *user_data);

#endif
