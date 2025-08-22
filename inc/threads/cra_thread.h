/**
 * @file cra_thread.h
 * @author Cracal
 * @brief Thread
 * @version 0.1
 * @date 2024-09-21
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_THREAD_H__
#define __CRA_THREAD_H__
#include "cra_defs.h"

#if !defined(__STDC_NO_THREADS__) && !(defined(CRA_COMPILER_MSVC) || defined(CRA_COMPILER_MINGW))

#include <threads.h>

typedef thrd_t       cra_thrd_t;
typedef int          cra_thrd_ret_t;
typedef thrd_start_t cra_thrd_start_fn;

#define cra_thrd_local thread_local

#define CRA_THRD_FUNC(fname) int fname(void *arg)

static inline bool
cra_thrd_create(cra_thrd_t *th, cra_thrd_start_fn func, void *arg)
{
    return thrd_success == thrd_create(th, func, arg);
}

static inline bool
cra_thrd_join(cra_thrd_t th)
{
    return thrd_success == thrd_join(th, NULL);
}

#elif defined(CRA_COMPILER_MSVC)

typedef HANDLE                 cra_thrd_t;
typedef DWORD                  cra_thrd_ret_t;
typedef LPTHREAD_START_ROUTINE cra_thrd_start_fn;

#define cra_thrd_local __declspec(thread)

#define CRA_THRD_FUNC(fname) DWORD WINAPI fname(void *arg)

static inline bool
cra_thrd_create(cra_thrd_t *th, cra_thrd_start_fn func, void *arg)
{
    return (*th = CreateThread(NULL, 0, func, arg, 0, NULL)) != NULL;
}

static inline bool
cra_thrd_join(cra_thrd_t th)
{
    if (WAIT_OBJECT_0 == WaitForSingleObject(th, INFINITE))
        return CloseHandle(th);
    return false;
}

#elif defined(CRA_COMPILER_GNUC)

#include <pthread.h>

typedef pthread_t cra_thrd_t;
typedef void     *cra_thrd_ret_t;
typedef void     *(*cra_thrd_start_fn)(void *);

#define cra_thrd_local __thread

#define CRA_THRD_FUNC(fname) void *fname(void *arg)

static inline bool
cra_thrd_create(cra_thrd_t *th, cra_thrd_start_fn func, void *arg)
{
    return 0 == pthread_create(th, NULL, func, arg);
}

static inline bool
cra_thrd_join(cra_thrd_t th)
{
    return 0 == pthread_join(th, NULL);
}

#endif

typedef unsigned long cra_tid_t;

CRA_API cra_tid_t
cra_thrd_get_current_tid(void);

#endif