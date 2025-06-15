/**
 * @file cra_thrdpool.h
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

typedef enum _CraThrdPoolDiscardPolicy_e
{
    CRA_TPTASK_DISCARD_SELF = 0,
    CRA_TPTASK_DISCARD_FIRST,
    CRA_TPTASK_DISCARD_LAST,
} CraThrdPoolDiscardPolicy_e;

typedef struct _CraThrdPoolWorker CraThrdPoolWorker;

typedef struct _CraThrdPool
{
    bool can_in;
    bool is_running;
    bool handle_exist_task;
    CraThrdPoolDiscardPolicy_e discard_policy;
    cra_atomic_int32_t idle_threads;
    int threadcnt;
    size_t task_max;
    CraBlkDeque task_que;
    CraThrdPoolWorker *threads;
} CraThrdPool;

typedef struct
{
    cra_tid_t tid;
} CraThrdPoolArgs0;

typedef struct
{
    CraThrdPoolArgs0;
    void *arg1;
} CraThrdPoolArgs1;

typedef struct
{
    CraThrdPoolArgs1;
    void *arg2;
} CraThrdPoolArgs2;

typedef void (*cra_thrdpool_task_fn0)(const CraThrdPoolArgs0 *);
typedef void (*cra_thrdpool_task_fn1)(const CraThrdPoolArgs1 *);
typedef void (*cra_thrdpool_task_fn2)(const CraThrdPoolArgs2 *);

#define CRA_THRDPOOL_TASK_INFINITE CRA_BLK_DEQUE_INFINITE

CRA_API void cra_thrdpool_init(CraThrdPool *pool, int threads, size_t task_max);
CRA_API void cra_thrdpool_uninit(CraThrdPool *pool);

CRA_API void cra_thrdpool_wait(CraThrdPool *pool);

static inline void cra_thrdpool_set_discard_policy(CraThrdPool *pool, CraThrdPoolDiscardPolicy_e policy)
{
    pool->discard_policy = policy;
}

CRA_API bool cra_thrdpool_add_task2(CraThrdPool *pool, cra_thrdpool_task_fn2 func, void *arg1, void *arg2);
static inline bool cra_thrdpool_add_task1(CraThrdPool *pool, cra_thrdpool_task_fn1 func, void *arg)
{
    return cra_thrdpool_add_task2(pool, (cra_thrdpool_task_fn2)func, arg, NULL);
}
static inline bool cra_thrdpool_add_task0(CraThrdPool *pool, cra_thrdpool_task_fn0 func)
{
    return cra_thrdpool_add_task2(pool, (cra_thrdpool_task_fn2)func, NULL, NULL);
}

#endif