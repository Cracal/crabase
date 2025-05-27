/**
 * @file cra_locker.h
 * @author Cracal
 * @brief 互斥锁和条件变量
 * @version 0.1 0.2
 * @date 2023-12-12
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_LOCKER_H__
#define __CRA_LOCKER_H__
#include "cra_defs.h"

#if !defined(__STDC_NO_THREADS__) && !(defined(CRA_COMPILER_MSVC) || defined(CRA_COMPILER_MINGW))

#include <time.h>
#include <threads.h>

// mutex
typedef mtx_t cra_mutex_t;
#define cra_mutex_init(pmtx) (void)mtx_init(pmtx, mtx_plain)
#define cra_mutex_destroy mtx_destroy
#define cra_mutex_lock (void)mtx_lock
#define cra_mutex_trylock(pmtx) (mtx_trylock(pmtx) == 0)
#define cra_mutex_unlock (void)mtx_unlock

// cond
typedef cnd_t cra_cond_t;
#define cra_cond_init (void)cnd_init
#define cra_cond_destroy cnd_destroy
#define cra_cond_wait (void)cnd_wait
static inline bool cra_cond_wait_timeout(cra_cond_t *cond, cra_mutex_t *mtx, int timeout_ms)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    return cnd_timedwait(cond, mtx, &ts) == 0;
}
#define cra_cond_signal (void)cnd_signal
#define cra_cond_broadcast (void)cnd_broadcast

#elif defined(CRA_COMPILER_MSVC)

// mutex
typedef CRITICAL_SECTION cra_mutex_t;
#define cra_mutex_init InitializeCriticalSection
#define cra_mutex_destroy DeleteCriticalSection
#define cra_mutex_lock EnterCriticalSection
#define cra_mutex_trylock TryEnterCriticalSection
#define cra_mutex_unlock LeaveCriticalSection

// cond
typedef CONDITION_VARIABLE cra_cond_t;
#define cra_cond_init InitializeConditionVariable
#define cra_cond_destroy(pcond)
#define cra_cond_wait(pcond, pmutex) (void)SleepConditionVariableCS(pcond, pmutex, INFINITE)
#define cra_cond_wait_timeout(pcond, pmutex, ms) SleepConditionVariableCS(pcond, pmutex, ms)
#define cra_cond_signal WakeConditionVariable
#define cra_cond_broadcast WakeAllConditionVariable

#elif defined(CRA_COMPILER_GNUC)

#include <time.h>
#include <pthread.h>

// mutex
typedef pthread_mutex_t cra_mutex_t;
#define cra_mutex_init(pmtx) (void)pthread_mutex_init(pmtx, NULL)
#define cra_mutex_destroy pthread_mutex_destroy
#define cra_mutex_lock (void)pthread_mutex_lock
#define cra_mutex_trylock(pmtx) (pthread_mutex_trylock(pmtx) == 0)
#define cra_mutex_unlock (void)pthread_mutex_unlock

// cond
typedef pthread_cond_t cra_cond_t;
#define cra_cond_init(pcond) (void)pthread_cond_init(pcond, NULL)
#define cra_cond_destroy pthread_cond_destroy
#define cra_cond_wait (void)pthread_cond_wait
static inline bool cra_cond_wait_timeout(cra_cond_t *cond, cra_mutex_t *mtx, int timeout_ms)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    return pthread_cond_timedwait(cond, mtx, &ts) == 0;
}
#define cra_cond_signal (void)pthread_cond_signal
#define cra_cond_broadcast (void)pthread_cond_broadcast

#endif

#endif