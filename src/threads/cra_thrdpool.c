/**
 * @file cra_thrdpool.c
 * @author Cracal
 * @brief 线程池
 * @version 0.1
 * @date 2024-10-17
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "cra_assert.h"
#include "cra_malloc.h"
#include "threads/cra_cdl.h"
#include "threads/cra_blockdq.h"
#include "threads/cra_thrdpool.h"

typedef struct CraThrdPoolTask CraThrdPoolTask;

struct CraThrdPoolWorker
{
    cra_thrd_t   th;
    CraCDL      *cdl;
    CraThrdPool *pool;
};

struct CraThrdPoolTask
{
    union
    {
        void (*excute0)(void);
        void (*excute1)(void *);
        void (*excute2)(void *, void *);
        void (*excute3)(void *, void *, void *);

        void *user_data[4];
    };
    int count;
};

static CRA_THRD_FUNC(cra_thrdpool_worker)
{
    CraThrdPoolWorker *worker = (CraThrdPoolWorker *)arg;
    CraThrdPool       *pool = worker->pool;
    CraThrdPoolTask    task;

    cra_cdl_count_down(worker->cdl);

    while (pool->running)
    {
        if ((cra_blockdq_pop_front)(pool->taskque, &task))
        {
            assert(task.excute0);
            cra_atomic_dec(&pool->idlecnt, CRA_MO_RELAXED);
            switch (task.count)
            {
                case 0:
                    task.excute0();
                    break;
                case 1:
                    task.excute1(task.user_data[1]);
                    break;
                case 2:
                    task.excute2(task.user_data[1], task.user_data[2]);
                    break;
                case 3:
                    task.excute3(task.user_data[1], task.user_data[2], task.user_data[3]);
                    break;
                default:
                    fprintf(stderr, "cra_thrdpool_worker() -- Invalid task.\n");
                    abort();
                    break;
            }
            cra_atomic_inc(&pool->idlecnt, CRA_MO_RELAXED);
        }
        else
        {
            break;
        }
    }

    return (cra_thrd_ret_t){ 0 };
}

void
cra_thrdpool_init(CraThrdPool *pool, int nthreads)
{
    CraCDL cdl;

    assert(pool);
    assert(nthreads > 0);

    pool->running = true;
    pool->idlecnt = nthreads;
    pool->nworker = nthreads;

    pool->workers = (CraThrdPoolWorker *)cra_malloc(sizeof(CraThrdPoolWorker) * nthreads);
    if (!pool->workers)
    {
        fprintf(stderr, "cra_thrdpool_init() -- Create workers failed.\n");
        exit(EXIT_FAILURE);
    }
    pool->taskque = cra_alloc(CraBlockdq);
    if (!pool->taskque || !cra_blockdq_init(CraThrdPoolTask, pool->taskque))
    {
        fprintf(stderr, "cra_thrdpool_init() -- Create taskque failed.\n");
        exit(EXIT_FAILURE);
    }

    cra_cdl_init(&cdl, nthreads);

    for (int i = 0; i < nthreads; i++)
    {
        pool->workers[i].cdl = &cdl;
        pool->workers[i].pool = pool;
        if (!cra_thrd_create(&pool->workers[i].th, cra_thrdpool_worker, &pool->workers[i]))
        {
            fprintf(stderr, "cra_thrdpool_init() -- Create thread %d failed.\n", i);
            exit(EXIT_FAILURE);
        }
    }

    cra_cdl_wait(&cdl);
    cra_cdl_uninit(&cdl);
}

void
cra_thrdpool_uninit(CraThrdPool *pool, bool wait_tasks)
{
    assert(pool);

    if (!wait_tasks)
        pool->running = false;

    cra_blockdq_shutdown(pool->taskque);

    for (int i = 0; i < pool->nworker; i++)
        cra_thrd_join(pool->workers[i].th);

    cra_blockdq_uninit(pool->taskque);
    cra_dealloc(pool->taskque);
    cra_free(pool->workers);

    bzero(pool, sizeof(*pool));
}

bool
cra_thrdpool_add_task0(CraThrdPool *pool, void (*excute0)(void))
{
    CraThrdPoolTask task = { .excute0 = excute0, .count = 0 };
    return (cra_blockdq_push_back)(pool->taskque, &task);
}

bool
cra_thrdpool_add_task1(CraThrdPool *pool, void (*excute1)(void *), void *arg)
{
    CraThrdPoolTask task = { .excute1 = excute1, .count = 1 };
    task.user_data[1] = arg;
    return (cra_blockdq_push_back)(pool->taskque, &task);
}

bool
cra_thrdpool_add_task2(CraThrdPool *pool, void (*excute2)(void *, void *), void *arg1, void *arg2)
{
    CraThrdPoolTask task = { .excute2 = excute2, .count = 2 };
    task.user_data[1] = arg1;
    task.user_data[2] = arg2;
    return (cra_blockdq_push_back)(pool->taskque, &task);
}

bool
cra_thrdpool_add_task3(CraThrdPool *pool, void (*excute3)(void *, void *, void *), void *arg1, void *arg2, void *arg3)
{
    CraThrdPoolTask task = { .excute3 = excute3, .count = 3 };
    task.user_data[1] = arg1;
    task.user_data[2] = arg2;
    task.user_data[3] = arg3;
    return (cra_blockdq_push_back)(pool->taskque, &task);
}
