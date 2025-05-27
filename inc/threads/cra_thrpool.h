/**
 * @file cra_thpool.h
 * @author Cracal
 * @brief 线程池
 * @version 0.1
 * @date 2024-10-17
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_THPOOL_H__
#define __CRA_THPOOL_H__
#include "cra_atomic.h"
#include "cra_thread.h"
#include "cra_locker.h"
#include "cra_blk_deque.h"

typedef enum
{
    CRA_TPTASK_DISCARD_SELF = 0,
    CRA_TPTASK_DISCARD_FIRST,
    CRA_TPTASK_DISCARD_LAST,
} CraThrPoolDiscardPolicy_e;

typedef struct _CraThrPoolWorker CraThrPoolWorker;

typedef struct _CraThrPool
{
    bool can_in;
    bool is_running;
    bool handle_exist_task;
    CraThrPoolDiscardPolicy_e discard_policy;
    cra_atomic_int32_t idle_threads;
    int threadcnt;
    size_t task_max;
    CraBlkDeque task_que;
    CraThrPoolWorker *threads;
} CraThrPool;

typedef struct
{
    cra_tid_t tid;
} CraThrPoolArgs0;

typedef struct
{
    CraThrPoolArgs0;
    void *arg1;
} CraThrPoolArgs1;

typedef struct
{
    CraThrPoolArgs1;
    void *arg2;
} CraThrPoolArgs2;

typedef void (*cra_thrpool_task_fn0)(const CraThrPoolArgs0 *);
typedef void (*cra_thrpool_task_fn1)(const CraThrPoolArgs1 *);
typedef void (*cra_thrpool_task_fn2)(const CraThrPoolArgs2 *);

#define CRA_THRPOOL_TASK_INFINITE CRA_BLK_DEQUE_INFINITE

CRA_API void cra_thrpool_init(CraThrPool *pool, int threads, size_t task_max);
CRA_API void cra_thrpool_uninit(CraThrPool *pool);

CRA_API void cra_thrpool_wait(CraThrPool *pool);

static inline void cra_thrpool_set_discard_policy(CraThrPool *pool, CraThrPoolDiscardPolicy_e policy)
{
    pool->discard_policy = policy;
}

CRA_API bool cra_thrpool_add_task2(CraThrPool *pool, cra_thrpool_task_fn2 func, void *arg1, void *arg2);
static inline bool cra_thrpool_add_task1(CraThrPool *pool, cra_thrpool_task_fn1 func, void *arg)
{
    return cra_thrpool_add_task2(pool, (cra_thrpool_task_fn2)func, arg, NULL);
}
static inline bool cra_thrpool_add_task0(CraThrPool *pool, cra_thrpool_task_fn0 func)
{
    return cra_thrpool_add_task2(pool, (cra_thrpool_task_fn2)func, NULL, NULL);
}

#endif