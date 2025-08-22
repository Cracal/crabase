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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#endif