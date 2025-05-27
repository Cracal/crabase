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

#define assert_always(_expr) \
    ((void)(!!(_expr) || (fprintf(stderr, "assert `%s` failed. - %s:%d\n", #_expr, __FILE__, __LINE__), abort(), 0)))

#ifdef CRA_WITH_MY_ASSERT // my assert

#undef assert
#ifdef NDEBUG
#define assert(_expr) ((void)0)
#else
#define assert assert_always
#endif

#endif // end my assert

#endif