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
#include <inttypes.h>

#if 1 // OS

#if defined(_WIN32) || defined(_WIN64)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#define CRA_OS_WIN
#define CRA_OS_WINDOWS
#define CRA_OS_NAME "Windows"

#elif defined(__linux__)

#include <limits.h>
#include <unistd.h>

#define CRA_OS_LINUX
#define CRA_OS_NAME "Linux"

#else

#error "暂不支持该操作系统"

#endif

#endif // end OS

#if 1 // ARCH

#if defined(_M_X64) || defined(__x86_64__)

#define CRA_ARCH_X86_64
#define CRA_ARCH_NAME "x86_64"

#elif defined(_M_IX86) || defined(__i386__)

#error "不支持32位架构"

#else

#error "暂不支持该架构"

#endif

#endif // end ARCH

#if 1 // compiler

#if defined(__clang__)

#error "TODO"

#if defined(_MSC_VER)
#define CRA_COMPILER_CLANG_CL
#define CRA_COMPILER_NAME "Clang-cl"
#else
#define CRA_COMPILER_CLANG
#define CRA_COMPILER_NAME "Clang"
#endif

#elif defined(__GNUC__) || defined(__GNUG__)

#if __GNUC__ < 13
#warning "没有在低于13的版本上测试过，可能会出错"
#endif

#define CRA_COMPILER_GCC
#define CRA_COMPILER_GNUC
#define CRA_COMPILER_NAME "GCC"

#elif defined(_MSC_VER)

#if _MSC_VER < 1939
#warning "没有在低于1939的版本上测试过，可能会出错"
#endif

#define CRA_COMPILER_MSVC
#define CRA_COMPILER_NAME "MSVC"

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
cra_get_last_error(void)
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

#define CRA_UNUSED(p) (void)(p)

#define CRA_MAX(a, b)          ((a) > (b) ? (a) : (b))
#define CRA_MIN(a, b)          ((a) < (b) ? (a) : (b))
#define CRA_CLAMP(v, max, min) ((v) > (max) ? (max) : ((v) < (min) ? (min) : (v)))

#define _CRA_CAT(a, b) a##b
#define CRA_CAT(a, b)  _CRA_CAT(a, b)
#define CRA_UCAT(a, b) CRA_CAT(a, CRA_CAT(_, b))

#define CRA_NARRAY(array) (sizeof(array) / sizeof((array)[0]))

#ifndef offsetof
#define offsetof(type, member) ((size_t)&((type *)0)->member)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

#ifdef CRA_COMPILER_MSVC
typedef ptrdiff_t ssize_t;
#endif
#ifndef SSIZE_MAX
#define SSIZE_MAX PTRDIFF_MAX
#endif
#ifndef SSIZE_MIN
#define SSIZE_MIN PTRDIFF_MIN
#endif

#ifdef CRA_COMPILER_MSVC
// 消除“使用了非标准扩展: 结构/联合中的零大小数组”
#pragma warning(disable : 4200)
#endif

#ifdef CRA_OS_WIN

#define bzero ZeroMemory

#define cra_sleep(sec)   Sleep((sec) * 1000)
#define cra_msleep(msec) Sleep(msec)

#else

#define cra_sleep        sleep
#define cra_msleep(msec) usleep((msec) * 1000)

#endif

static inline void
cra_swap_ptr(void **ptr1, void **ptr2)
{
    void *temp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp;
}

#define CRA_FOREACH_NEXT_DEF(Type) bool CRA_CAT(Type, _iter_next)(CRA_CAT(Type, Iter) * it, Type * obj, void *retval)
#define CRA_FOREACH_PREV_DEF(Type) bool CRA_CAT(Type, _iter_prev)(CRA_CAT(Type, Iter) * it, Type * obj, void *retval)
#define CRA_FOREACH_NEXT_DEF2(Type)                                                                  \
    bool CRA_CAT(Type, _iter_next)(CRA_CAT(Type, Iter) * it, Type * obj, void *retkey, void *retval)
#define CRA_FOREACH_PREV_DEF2(Type)                                                                  \
    bool CRA_CAT(Type, _iter_prev)(CRA_CAT(Type, Iter) * it, Type * obj, void *retkey, void *retval)

#define CRA_FOREACH(Type, obj, ... /* vals */)                                 \
    for (CRA_CAT(Type, Iter) CRA_CAT(it, __LINE__) = { 0 };                    \
         CRA_CAT(Type, _iter_next)(&CRA_CAT(it, __LINE__), obj, __VA_ARGS__);)
#define CRA_FOREACH_REVERSE(Type, obj, ... /* vals */)                         \
    for (CRA_CAT(Type, Iter) CRA_CAT(it, __LINE__) = { 0 };                    \
         CRA_CAT(Type, _iter_prev)(&CRA_CAT(it, __LINE__), obj, __VA_ARGS__);)

#endif