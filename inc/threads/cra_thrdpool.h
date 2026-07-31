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

typedef enum CraThrdPoolFull_e   CraThrdPoolFull_e;
typedef struct CraThrdPoolWorker CraThrdPoolWorker;
typedef struct CraThrdPool       CraThrdPool;
typedef struct CraBlockdq        CraBlockdq;

struct CraThrdPool
{
    bool               running;
    cra_atomic_int32_t idlecnt;
    int                nworker;
    CraThrdPoolWorker *workers;
    CraBlockdq        *taskque; // Blockdq<Task>
};

enum CraThrdPoolFull_e
{
    CRA_THRDPOOL_FULL_WAIT,
    CRA_THRDPOOL_FULL_DROP_NEWEST,
    CRA_THRDPOOL_FULL_DROP_OLDEST,
    CRA_THRDPOOL_FULL_RETURN_FALSE
};

#define CRA_THRDPOOL_INFINITE_TASKS SIZE_MAX

CRA_API void
cra_thrdpool_init(CraThrdPool *pool, int nthreads, int max_tasks, CraThrdPoolFull_e full_policy);

// `wait_tasks`: Wait for all tasks to finish.
CRA_API void
cra_thrdpool_uninit(CraThrdPool *pool, bool wait_tasks);

CRA_API bool
cra_thrdpool_add_task0(CraThrdPool *pool, void (*excute0)(void));

CRA_API bool
cra_thrdpool_add_task1(CraThrdPool *pool, void (*excute1)(void *), void *arg);

CRA_API bool
cra_thrdpool_add_task2(CraThrdPool *pool, void (*excute2)(void *, void *), void *arg1, void *arg2);

CRA_API bool
cra_thrdpool_add_task3(CraThrdPool *pool, void (*excute3)(void *, void *, void *), void *arg1, void *arg2, void *arg3);

CRA_API bool
cra_thrdpool_add_task1_drop(CraThrdPool *pool, void (*drop_cb)(void *), void (*excute1)(void *), void *arg);

CRA_API bool
cra_thrdpool_add_task2_drop(CraThrdPool *pool,
                            void         (*drop_cb)(void *, void *),
                            void         (*excute2)(void *, void *),
                            void        *arg1,
                            void        *arg2);

CRA_API bool
cra_thrdpool_add_task3_drop(CraThrdPool *pool,
                            void         (*drop_cb)(void *, void *, void *),
                            void         (*excute3)(void *, void *, void *),
                            void        *arg1,
                            void        *arg2,
                            void        *arg3);

#endif