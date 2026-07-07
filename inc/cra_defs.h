/**
 * @file cra_defs.h
 * @author Cracal
 * @brief defs
 * @version 0.1
 * @date 2024-09-17
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_DEFS_H__
#define __CRA_DEFS_H__
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#if 1 // OS

#if defined(_WIN64)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#define CRA_OS_WIN
#define CRA_OS_WINDOWS

#elif defined(__linux__)

#include <limits.h>
#include <unistd.h>

#define CRA_OS_LINUX

#else

#error "暂不支持该系统"

#endif

#if !defined(_M_X64) && !defined(__x86_64__)
#error "只支持64位操作系统"
#endif

#endif // end OS

#if 1 // compiler

#ifdef _MSC_VER

#if _MSC_VER < 1939
#warning "没有在低于1939的版本上测试过，可能会出错"
#endif

#define CRA_COMPILER_MSVC

#elif defined(__GNUC__)

#if __GNUC__ < 13
#warning "没有在低于13的版本上测试过，可能会出错"
#endif

#define CRA_COMPILER_GNUC

#ifdef __MINGW64__
#include <limits.h>
#include <unistd.h>

#define CRA_COMPILER_MINGW
#endif

#else

#error "暂不支持该编译器"

#endif

#endif // end compiler

#if 1 // CRA_EXTERN

#ifdef __cplusplus
#define CRA_EXTERN extern "C"
#else
#define CRA_EXTERN extern
#endif

#endif // end CRA_EXTERN

#if 1 // CRA_API

#ifdef CRA_COMPILER_MSVC
#define CRA_EXPORT_API __declspec(dllexport) CRA_EXTERN
#define CRA_IMPORT_API __declspec(dllimport) CRA_EXTERN
#else
#define CRA_EXPORT_API __attribute__((visibility("default"))) CRA_EXTERN
#define CRA_IMPORT_API CRA_EXTERN
#endif

#ifdef CRA_BUILD_DLL
#define CRA_API CRA_EXPORT_API
#else
#define CRA_API CRA_IMPORT_API
#endif

#endif // end CRA_API

#if 1 // cra_[get|set]_last_error()

#ifdef CRA_COMPILER_MSVC
#define cra_get_last_error GetLastError
#define cra_set_last_error SetLastError
#else
#include <errno.h>
static inline int
cra_get_last_error()
{
    return errno;
}

static inline void
cra_set_last_error(int err)
{
    errno = err;
}
#endif

#endif // end cra_[get|set]_last_error()

#define CRA_BITS(x) (1 << (x))

#define CRA_UNUSED_VALUE(p) (void)(p)

#define CRA_MAX(a, b)          ((a) > (b) ? (a) : (b))
#define CRA_MIN(a, b)          ((a) < (b) ? (a) : (b))
#define CRA_CLAMP(v, max, min) ((v) > (max) ? (max) : ((v) < (min) ? (min) : (v)))

#define _CRA_CAT(a, b) a##b
#define CRA_CAT(a, b)  _CRA_CAT(a, b)
#define CRA_UCAT(a, b) CRA_CAT(a, CRA_CAT(_, b))

#define CRA_CNT_CAT(a, b) a b
#if 1 // CRA_CNT_GET
#define CRA_CNT_GET(_0,   \
                    _1,   \
                    _2,   \
                    _3,   \
                    _4,   \
                    _5,   \
                    _6,   \
                    _7,   \
                    _8,   \
                    _9,   \
                    _10,  \
                    _11,  \
                    _12,  \
                    _13,  \
                    _14,  \
                    _15,  \
                    _16,  \
                    _17,  \
                    _18,  \
                    _19,  \
                    _20,  \
                    _21,  \
                    _22,  \
                    _23,  \
                    _24,  \
                    _25,  \
                    _26,  \
                    _27,  \
                    _28,  \
                    _29,  \
                    _30,  \
                    _31,  \
                    _32,  \
                    _33,  \
                    _34,  \
                    _35,  \
                    _36,  \
                    _37,  \
                    _38,  \
                    _39,  \
                    _40,  \
                    _41,  \
                    _42,  \
                    _43,  \
                    _44,  \
                    _45,  \
                    _46,  \
                    _47,  \
                    _48,  \
                    _49,  \
                    _50,  \
                    _51,  \
                    _52,  \
                    _53,  \
                    _54,  \
                    _55,  \
                    _56,  \
                    _57,  \
                    _58,  \
                    _59,  \
                    _60,  \
                    _61,  \
                    _62,  \
                    _63,  \
                    _64,  \
                    _65,  \
                    _66,  \
                    _67,  \
                    _68,  \
                    _69,  \
                    _70,  \
                    _71,  \
                    _72,  \
                    _73,  \
                    _74,  \
                    _75,  \
                    _76,  \
                    _77,  \
                    _78,  \
                    _79,  \
                    _80,  \
                    _81,  \
                    _82,  \
                    _83,  \
                    _84,  \
                    _85,  \
                    _86,  \
                    _87,  \
                    _88,  \
                    _89,  \
                    _90,  \
                    _91,  \
                    _92,  \
                    _93,  \
                    _94,  \
                    _95,  \
                    _96,  \
                    _97,  \
                    _98,  \
                    _99,  \
                    _100, \
                    _101, \
                    _102, \
                    _103, \
                    _104, \
                    _105, \
                    _106, \
                    _107, \
                    _108, \
                    _109, \
                    _110, \
                    _111, \
                    _112, \
                    _113, \
                    _114, \
                    _115, \
                    _116, \
                    _117, \
                    _118, \
                    _119, \
                    _120, \
                    _121, \
                    _122, \
                    _123, \
                    _124, \
                    _125, \
                    _126, \
                    _127, \
                    _128, \
                    _129, \
                    _130, \
                    _131, \
                    _132, \
                    _133, \
                    _134, \
                    _135, \
                    _136, \
                    _137, \
                    _138, \
                    _139, \
                    _140, \
                    _141, \
                    _142, \
                    _143, \
                    _144, \
                    _145, \
                    _146, \
                    _147, \
                    _148, \
                    _149, \
                    _150, \
                    _151, \
                    _152, \
                    _153, \
                    _154, \
                    _155, \
                    _156, \
                    _157, \
                    _158, \
                    _159, \
                    _160, \
                    _161, \
                    _162, \
                    _163, \
                    _164, \
                    _165, \
                    _166, \
                    _167, \
                    _168, \
                    _169, \
                    _170, \
                    _171, \
                    _172, \
                    _173, \
                    _174, \
                    _175, \
                    _176, \
                    _177, \
                    _178, \
                    _179, \
                    _180, \
                    _181, \
                    _182, \
                    _183, \
                    _184, \
                    _185, \
                    _186, \
                    _187, \
                    _188, \
                    _189, \
                    _190, \
                    _191, \
                    _192, \
                    _193, \
                    _194, \
                    _195, \
                    _196, \
                    _197, \
                    _198, \
                    _199, \
                    _200, \
                    _201, \
                    _202, \
                    _203, \
                    _204, \
                    _205, \
                    _206, \
                    _207, \
                    _208, \
                    _209, \
                    _210, \
                    _211, \
                    _212, \
                    _213, \
                    _214, \
                    _215, \
                    _216, \
                    _217, \
                    _218, \
                    _219, \
                    _220, \
                    _221, \
                    _222, \
                    _223, \
                    _224, \
                    _225, \
                    _226, \
                    _227, \
                    _228, \
                    _229, \
                    _230, \
                    _231, \
                    _232, \
                    _233, \
                    _234, \
                    _235, \
                    _236, \
                    _237, \
                    _238, \
                    _239, \
                    _240, \
                    _241, \
                    _242, \
                    _243, \
                    _244, \
                    _245, \
                    _246, \
                    _247, \
                    _248, \
                    _249, \
                    _250, \
                    _251, \
                    _252, \
                    _253, \
                    _254, \
                    _255, \
                    _256, \
                    _N,   \
                    ...)  \
    _N
#endif

#if 1 // CRA_CNT
#define CRA_CNT(...)            \
    CRA_CNT_CAT(CRA_CNT_GET,    \
                (0,             \
                 ##__VA_ARGS__, \
                 256,           \
                 255,           \
                 254,           \
                 253,           \
                 252,           \
                 251,           \
                 250,           \
                 249,           \
                 248,           \
                 247,           \
                 246,           \
                 245,           \
                 244,           \
                 243,           \
                 242,           \
                 241,           \
                 240,           \
                 239,           \
                 238,           \
                 237,           \
                 236,           \
                 235,           \
                 234,           \
                 233,           \
                 232,           \
                 231,           \
                 230,           \
                 229,           \
                 228,           \
                 227,           \
                 226,           \
                 225,           \
                 224,           \
                 223,           \
                 222,           \
                 221,           \
                 220,           \
                 219,           \
                 218,           \
                 217,           \
                 216,           \
                 215,           \
                 214,           \
                 213,           \
                 212,           \
                 211,           \
                 210,           \
                 209,           \
                 208,           \
                 207,           \
                 206,           \
                 205,           \
                 204,           \
                 203,           \
                 202,           \
                 201,           \
                 200,           \
                 199,           \
                 198,           \
                 197,           \
                 196,           \
                 195,           \
                 194,           \
                 193,           \
                 192,           \
                 191,           \
                 190,           \
                 189,           \
                 188,           \
                 187,           \
                 186,           \
                 185,           \
                 184,           \
                 183,           \
                 182,           \
                 181,           \
                 180,           \
                 179,           \
                 178,           \
                 177,           \
                 176,           \
                 175,           \
                 174,           \
                 173,           \
                 172,           \
                 171,           \
                 170,           \
                 169,           \
                 168,           \
                 167,           \
                 166,           \
                 165,           \
                 164,           \
                 163,           \
                 162,           \
                 161,           \
                 160,           \
                 159,           \
                 158,           \
                 157,           \
                 156,           \
                 155,           \
                 154,           \
                 153,           \
                 152,           \
                 151,           \
                 150,           \
                 149,           \
                 148,           \
                 147,           \
                 146,           \
                 145,           \
                 144,           \
                 143,           \
                 142,           \
                 141,           \
                 140,           \
                 139,           \
                 138,           \
                 137,           \
                 136,           \
                 135,           \
                 134,           \
                 133,           \
                 132,           \
                 131,           \
                 130,           \
                 129,           \
                 128,           \
                 127,           \
                 126,           \
                 125,           \
                 124,           \
                 123,           \
                 122,           \
                 121,           \
                 120,           \
                 119,           \
                 118,           \
                 117,           \
                 116,           \
                 115,           \
                 114,           \
                 113,           \
                 112,           \
                 111,           \
                 110,           \
                 109,           \
                 108,           \
                 107,           \
                 106,           \
                 105,           \
                 104,           \
                 103,           \
                 102,           \
                 101,           \
                 100,           \
                 99,            \
                 98,            \
                 97,            \
                 96,            \
                 95,            \
                 94,            \
                 93,            \
                 92,            \
                 91,            \
                 90,            \
                 89,            \
                 88,            \
                 87,            \
                 86,            \
                 85,            \
                 84,            \
                 83,            \
                 82,            \
                 81,            \
                 80,            \
                 79,            \
                 78,            \
                 77,            \
                 76,            \
                 75,            \
                 74,            \
                 73,            \
                 72,            \
                 71,            \
                 70,            \
                 69,            \
                 68,            \
                 67,            \
                 66,            \
                 65,            \
                 64,            \
                 63,            \
                 62,            \
                 61,            \
                 60,            \
                 59,            \
                 58,            \
                 57,            \
                 56,            \
                 55,            \
                 54,            \
                 53,            \
                 52,            \
                 51,            \
                 50,            \
                 49,            \
                 48,            \
                 47,            \
                 46,            \
                 45,            \
                 44,            \
                 43,            \
                 42,            \
                 41,            \
                 40,            \
                 39,            \
                 38,            \
                 37,            \
                 36,            \
                 35,            \
                 34,            \
                 33,            \
                 32,            \
                 31,            \
                 30,            \
                 29,            \
                 28,            \
                 27,            \
                 26,            \
                 25,            \
                 24,            \
                 23,            \
                 22,            \
                 21,            \
                 20,            \
                 19,            \
                 18,            \
                 17,            \
                 16,            \
                 15,            \
                 14,            \
                 13,            \
                 12,            \
                 11,            \
                 10,            \
                 9,             \
                 8,             \
                 7,             \
                 6,             \
                 5,             \
                 4,             \
                 3,             \
                 2,             \
                 1,             \
                 0))
#endif

#if 1 // assert CRA_CNT
static_assert(CRA_CNT() == 0, "CRA_CNT() failed for 0 arguments");
static_assert(CRA_CNT(a) == 1, "CRA_CNT() failed for 1 argument");
static_assert(CRA_CNT(a, b) == 2, "CRA_CNT() failed for 2 arguments");
static_assert(CRA_CNT(256,
                      255,
                      254,
                      253,
                      252,
                      251,
                      250,
                      249,
                      248,
                      247,
                      246,
                      245,
                      244,
                      243,
                      242,
                      241,
                      240,
                      239,
                      238,
                      237,
                      236,
                      235,
                      234,
                      233,
                      232,
                      231,
                      230,
                      229,
                      228,
                      227,
                      226,
                      225,
                      224,
                      223,
                      222,
                      221,
                      220,
                      219,
                      218,
                      217,
                      216,
                      215,
                      214,
                      213,
                      212,
                      211,
                      210,
                      209,
                      208,
                      207,
                      206,
                      205,
                      204,
                      203,
                      202,
                      201,
                      200,
                      199,
                      198,
                      197,
                      196,
                      195,
                      194,
                      193,
                      192,
                      191,
                      190,
                      189,
                      188,
                      187,
                      186,
                      185,
                      184,
                      183,
                      182,
                      181,
                      180,
                      179,
                      178,
                      177,
                      176,
                      175,
                      174,
                      173,
                      172,
                      171,
                      170,
                      169,
                      168,
                      167,
                      166,
                      165,
                      164,
                      163,
                      162,
                      161,
                      160,
                      159,
                      158,
                      157,
                      156,
                      155,
                      154,
                      153,
                      152,
                      151,
                      150,
                      149,
                      148,
                      147,
                      146,
                      145,
                      144,
                      143,
                      142,
                      141,
                      140,
                      139,
                      138,
                      137,
                      136,
                      135,
                      134,
                      133,
                      132,
                      131,
                      130,
                      129,
                      128,
                      127,
                      126,
                      125,
                      124,
                      123,
                      122,
                      121,
                      120,
                      119,
                      118,
                      117,
                      116,
                      115,
                      114,
                      113,
                      112,
                      111,
                      110,
                      109,
                      108,
                      107,
                      106,
                      105,
                      104,
                      103,
                      102,
                      101,
                      100,
                      99,
                      98,
                      97,
                      96,
                      95,
                      94,
                      93,
                      92,
                      91,
                      90,
                      89,
                      88,
                      87,
                      86,
                      85,
                      84,
                      83,
                      82,
                      81,
                      80,
                      79,
                      78,
                      77,
                      76,
                      75,
                      74,
                      73,
                      72,
                      71,
                      70,
                      69,
                      68,
                      67,
                      66,
                      65,
                      64,
                      63,
                      62,
                      61,
                      60,
                      59,
                      58,
                      57,
                      56,
                      55,
                      54,
                      53,
                      52,
                      51,
                      50,
                      49,
                      48,
                      47,
                      46,
                      45,
                      44,
                      43,
                      42,
                      41,
                      40,
                      39,
                      38,
                      37,
                      36,
                      35,
                      34,
                      33,
                      32,
                      31,
                      30,
                      29,
                      28,
                      27,
                      26,
                      25,
                      24,
                      23,
                      22,
                      21,
                      20,
                      19,
                      18,
                      17,
                      16,
                      15,
                      14,
                      13,
                      12,
                      11,
                      10,
                      9,
                      8,
                      7,
                      6,
                      5,
                      4,
                      3,
                      2,
                      1) == 256,
              "CRA_CNT() failed for 256 arguments");
#endif

#define CRA_NARRAY(_array) (sizeof(_array) / sizeof((_array)[0]))

#ifndef offsetof
#define offsetof(type, member) ((size_t)&((type *)0)->member)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

#ifdef CRA_COMPILER_MSVC
typedef intptr_t ssize_t;
#endif
#ifndef SSIZE_MAX
#define SSIZE_MAX INTPTR_MAX
#endif
#ifndef SSIZE_MIN
#define SSIZE_MIN INTPTR_MIN
#endif

#ifdef CRA_COMPILER_MSVC
// 消除“使用了非标准扩展: 结构/联合中的零大小数组”
#pragma warning(disable : 4200)
#endif

#ifdef CRA_OS_WIN

#define bzero ZeroMemory

#define cra_sleep(_sec)   Sleep((_sec) * 1000)
#define cra_msleep(_msec) Sleep(_msec)

#else

#define cra_sleep         sleep
#define cra_msleep(_msec) usleep((_msec) * 1000)

#endif

static inline void
cra_swap_ptr(void **ptr1, void **ptr2)
{
    void *temp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp;
}

#if 1 // file & dir utils

#ifdef CRA_OS_WIN

#include <direct.h>

#define CRA_PATH_SEP1 '/'
#define CRA_PATH_SEP2 '\\'

typedef int cra_mode_t;

static inline bool
cra_is_dir(const char *path)
{
    DWORD attr = GetFileAttributesA(path);
    if (attr == INVALID_FILE_ATTRIBUTES)
        return false;
    return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

static inline int
cra_mkdir(const char *path, cra_mode_t mode)
{
    CRA_UNUSED_VALUE(mode);
    return _mkdir(path);
}

#else

#include <sys/stat.h>

#define CRA_PATH_SEP1 '/'
#define CRA_PATH_SEP2 '/'

typedef mode_t cra_mode_t;

static inline bool
cra_is_dir(const char *path)
{
    struct stat st;
    if (stat(path, &st) == -1)
        return false;
    return S_ISDIR(st.st_mode);
}

static inline int
cra_mkdir(const char *path, cra_mode_t mode)
{
    return mkdir(path, mode);
}

#endif

#define CRA_IS_PATH_SEP(_c) (_c == CRA_PATH_SEP1 || _c == CRA_PATH_SEP2)

// 0:     success
// other: error code
CRA_API int
cra_mkdirs(const char *path, cra_mode_t mode);

CRA_API char *
cra_basename(char *path);

CRA_API char *
cra_dirname(char *path);

#endif // end file & dir utils

#if 1 // interfaces

// ========================== initializable ==========================

#define CRA_INITIALIZABLE_INIT_FN(_name)   bool _name(void *obj, size_t length, void *params)
#define CRA_INITIALIZABLE_UNINIT_FN(_name) void _name(void *obj)
#define CRA_INITIALIZABLE_DEF(_name)       const CraInitializable_i _name

typedef struct CraInitializable_i
{
    CRA_INITIALIZABLE_INIT_FN((*init));
    CRA_INITIALIZABLE_UNINIT_FN((*uninit));
} CraInitializable_i;

static inline bool
cra_initializable_init(const CraInitializable_i *i, void *obj, size_t length, void *params)
{
    return i->init(obj, length, params);
}

static inline void
cra_initializable_uninit(const CraInitializable_i *i, void *obj)
{
    i->uninit(obj);
}

// ========================== appendable ==========================

#define CRA_APPENDABLE_APPEND_FN(_name) bool _name(void *obj, CraTwoVals *vals)
#define CRA_APPENDABLE_DEF(_name)       const CraAppendable_i _name

typedef struct CraTwoVals
{
    void *val1_ref;
    void *val2_ref;
} CraTwoVals;

typedef struct CraAppendable_i
{
    CRA_APPENDABLE_APPEND_FN((*append));
} CraAppendable_i;

static inline bool
cra_appendable_append(const CraAppendable_i *i, void *obj, CraTwoVals *vals)
{
    return i->append(obj, vals);
}

// ========================== iterable ==========================

#define CRA_ITERABLE_INIT_FN(_name) bool _name(void *obj, CraIterator *it, size_t *retcnt, bool reverse)
#define CRA_ITERABLE_NEXT_FN(_name) bool _name(CraIterator *it, CraTwoVals *vals)
#define CRA_ITERABLE_PREV_FN(_name) bool _name(CraIterator *it, CraTwoVals *vals)
#define CRA_ITERABLE_DEF(_name)     const CraIterable_i _name

union CraIteratorVal_u
{
    size_t idx;
    void  *cur;
};

typedef struct CraIterator
{
    void                  *obj;
    union CraIteratorVal_u ic1;
    union CraIteratorVal_u ic2;
} CraIterator;

typedef struct CraIterable_i
{
    CRA_ITERABLE_INIT_FN((*init));
    CRA_ITERABLE_NEXT_FN((*next));
    CRA_ITERABLE_PREV_FN((*prev));
} CraIterable_i;

static inline bool
cra_iterable_init(const CraIterable_i *i, void *obj, CraIterator *it, size_t *retcnt, bool reverse)
{
    return i->init(obj, it, retcnt, reverse);
}

static inline bool
cra_iterable_next(const CraIterable_i *i, CraIterator *it, CraTwoVals *vals)
{
    return i->next(it, vals);
}

static inline bool
cra_iterable_prev(const CraIterable_i *i, CraIterator *it, CraTwoVals *vals)
{
    return i->prev(it, vals);
}

#define CRA_FOREACH(_iterable_i, _obj, _val_name)                                                           \
    for (CraIterator _val_name##_it = { 0 };                                                                \
         _val_name##_it.ic1.idx == 0 && cra_iterable_init(_iterable_i, _obj, &_val_name##_it, NULL, false); \
         _val_name##_it.ic1.idx = 1)                                                                        \
        for (CraTwoVals _val_name = { 0 }; cra_iterable_next(_iterable_i, &_val_name##_it, &_val_name);)

#define CRA_FOREACH_REVERSE(_iterable_i, _obj, _val_name)                                                  \
    for (CraIterator _val_name##_it = { 0 };                                                               \
         _val_name##_it.ic1.idx == 0 && cra_iterable_init(_iterable_i, _obj, &_val_name##_it, NULL, true); \
         _val_name##_it.ic1.idx = 1)                                                                       \
        for (CraTwoVals _val_name = { 0 }; cra_iterable_prev(_iterable_i, &_val_name##_it, &_val_name);)

#endif // end interfaces

#endif