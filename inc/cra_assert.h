/**
 * @file cra_assert.h
 * @author Cracal
 * @brief my assert
 * @version 0.1
 * @date 2024-10-21
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_ASSERT_H__
#define __CRA_ASSERT_H__
#include "cra_defs.h"
#include <assert.h>

CRA_API void (*__cra_g_assert__)(const char *, const char *, const char *, int);

static inline void
cra_assert_set_func(void (*func)(const char *, const char *, const char *, int))
{
    __cra_g_assert__ = func;
}

#define assert_always(expr) ((void)(!!(expr) || (__cra_g_assert__(#expr, __func__, __FILE__, __LINE__), 0)))

#undef assert
#ifdef NDEBUG
#define assert(expr) ((void)0)
#else
#define assert assert_always
#endif

#endif