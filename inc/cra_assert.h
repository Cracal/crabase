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

CRA_API void
cra_assert_set_func(void (*func)(const char *condition, const char *fname, const char *file, int line));

CRA_API void (*__cra_assert_func)(const char *condition, const char *fname, const char *file, int line);

#define assert_always(_condition)                                                                          \
    ((void)(!!(_condition) || (__cra_assert_func(#_condition, __func__, __FILE__, __LINE__), exit(1), 0)))

#undef assert
#ifdef NDEBUG
#define assert(_condition) ((void)0)
#else
#define assert assert_always
#endif

#endif