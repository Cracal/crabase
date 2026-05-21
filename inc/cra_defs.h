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

CRA_API char *
cra_basename(char *path);

CRA_API char *
cra_dirname(char *path);

#if 1 // interfaces

// ========================== initializable ==========================

#define CRA_INITIALIZABLE_INIT_FN(_name)      bool _name(void *obj, size_t length, void *params)
#define CRA_INITIALIZABLE_UNINIT_FN(_name)    void _name(void *obj)
#define CRA_INITIALIZABLE_GET_COUNT_FN(_name) size_t _name(void *obj)
#define CRA_INITIALIZABLE_DEF(_name)          const NewCraInitializable_i _name

typedef struct NewCraInitializable_i
{
    CRA_INITIALIZABLE_INIT_FN((*init));
    CRA_INITIALIZABLE_UNINIT_FN((*uninit));
    CRA_INITIALIZABLE_GET_COUNT_FN((*get_count));
} NewCraInitializable_i;

static inline bool
cra_initializable_init(const NewCraInitializable_i *i, void *obj, size_t length, void *params)
{
    return i->init(obj, length, params);
}

static inline void
cra_initializable_uninit(const NewCraInitializable_i *i, void *obj)
{
    i->uninit(obj);
}

static inline size_t
cra_initializable_get_count(const NewCraInitializable_i *i, void *obj)
{
    return i->get_count(obj);
}

// ========================== appendable ==========================

#define CRA_APPENDABLE_APPEND_FN(_name) bool _name(void *obj, CraTwoVals *vals)
#define CRA_APPENDABLE_DEF(_name)       const NewCraAppendable_i _name

typedef struct CraTwoVals
{
    void *val1_ref;
    void *val2_ref;
} CraTwoVals;

typedef struct NewCraAppendable_i
{
    CRA_APPENDABLE_APPEND_FN((*append));
} NewCraAppendable_i;

static inline bool
cra_appendable_append(const NewCraAppendable_i *i, void *obj, CraTwoVals *vals)
{
    return i->append(obj, vals);
}

// ========================== iterable ==========================

#define CRA_ITERABLE_INIT_FN(_name) bool _name(void *obj, NewCraIterator *it, bool reverse)
#define CRA_ITERABLE_NEXT_FN(_name) bool _name(NewCraIterator *it, CraTwoVals *vals)
#define CRA_ITERABLE_PREV_FN(_name) bool _name(NewCraIterator *it, CraTwoVals *vals)
#define CRA_ITERABLE_DEF(_name)     const NewCraIterable_i _name

union CraIteratorVal_u
{
    size_t idx;
    void  *cur;
};

typedef struct NewCraIterator
{
    void                  *obj;
    union CraIteratorVal_u ic1;
    union CraIteratorVal_u ic2;
} NewCraIterator;

typedef struct NewCraIterable_i
{
    CRA_ITERABLE_INIT_FN((*init));
    CRA_ITERABLE_NEXT_FN((*next));
    CRA_ITERABLE_PREV_FN((*prev));
} NewCraIterable_i;

static inline bool
cra_iterable_init(const NewCraIterable_i *i, void *obj, NewCraIterator *it, bool reverse)
{
    return i->init(obj, it, reverse);
}

static inline bool
cra_iterable_next(const NewCraIterable_i *i, NewCraIterator *it, CraTwoVals *vals)
{
    return i->next(it, vals);
}

static inline bool
cra_iterable_prev(const NewCraIterable_i *i, NewCraIterator *it, CraTwoVals *vals)
{
    return i->prev(it, vals);
}

#define CRA_FOREACH(_iterable_i, _obj, _val_name)                                                        \
    for (NewCraIterator _val_name##_it = { 0 };                                                          \
         _val_name##_it.ic1.idx == 0 && cra_iterable_init(_iterable_i, _obj, &_val_name##_it, false);    \
         _val_name##_it.ic1.idx = 1)                                                                     \
        for (CraTwoVals _val_name = { 0 }; cra_iterable_next(_iterable_i, &_val_name##_it, &_val_name);)

#define CRA_FOREACH_REVERSE(_iterable_i, _obj, _val_name)                                                \
    for (NewCraIterator _val_name##_it = { 0 };                                                          \
         _val_name##_it.ic1.idx == 0 && cra_iterable_init(_iterable_i, _obj, &_val_name##_it, true);     \
         _val_name##_it.ic1.idx = 1)                                                                     \
        for (CraTwoVals _val_name = { 0 }; cra_iterable_prev(_iterable_i, &_val_name##_it, &_val_name);)

#endif // end interfaces

#endif