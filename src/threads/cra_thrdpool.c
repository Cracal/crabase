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
#include "cra_malloc.h"
#include "threads/cra_cdl.h"
#include "threads/cra_thrdpool.h"

typedef struct
{
    CraThrdPoolArgs2 args;
    cra_thrdpool_task_fn2 func;
} CraThrdPoolTask;

struct _CraThrdPoolWorker
{
    CraCDL *cdl;
    CraThrdPool *pool;
    cra_thrd_t thrd;
};

static CRA_THRD_FUNC(__cra_thrdpool_worker)
{
    CraThrdPoolTask task;
    CraThrdPoolWorker *worker = (CraThrdPoolWorker *)arg;
    CraBlkDeque *taskque = &worker->pool->task_que;
    CraThrdPool *pool = worker->pool;
    cra_tid_t tid = cra_thrd_get_current_tid();

    cra_cdl_count_down(worker->cdl);

    while (true)
    {
        if (!cra_blkdeque_pop_left(taskque, &task))
            break;

        cra_atomic_dec32(&pool->idle_threads);
        task.args.tid = tid;
        task.func(&task.args);
        cra_atomic_inc32(&pool->idle_threads);

        // 终止条件：
        // 线程池状态已停止；
        // 并且
        // 如果不需要处理在任务队列中的任务则直接退出；
        // 否则，处理完所有任务后再退出
        if (!pool->is_running &&
            (!pool->handle_exist_task || cra_blkdeque_is_empty(taskque)))
        {
            break;
        }
    }

    return (cra_thrd_ret_t){0};
}

void cra_thrdpool_init(CraThrdPool *pool, int threads, size_t task_max)
{
    assert(threads > 0 && task_max > 0);
    pool->can_in = true;
    pool->handle_exist_task = true;
    pool->discard_policy = CRA_TPTASK_DISCARD_SELF;
    pool->idle_threads = threads;
    pool->threadcnt = 0;
    pool->task_max = task_max;
    cra_blkdeque_init0(CraThrdPoolTask, &pool->task_que, task_max, false, NULL);
    pool->threads = (CraThrdPoolWorker *)cra_malloc(sizeof(CraThrdPoolWorker) * threads);

#if 1 // create threads
    CraCDL cdl;
    cra_cdl_init(&cdl, threads);
    pool->is_running = true;
    for (int i = 0; i < threads; i++, pool->threadcnt++)
    {
        pool->threads[i].pool = pool;
        pool->threads[i].cdl = &cdl;
        if (!cra_thrd_create(&pool->threads[i].thrd, __cra_thrdpool_worker, &pool->threads[i]))
            goto error_ret;
    }

    cra_cdl_wait(&cdl);
    cra_cdl_uninit(&cdl);
#endif // end create threads

    return;

error_ret:
    cra_cdl_uninit(&cdl);
    cra_thrdpool_uninit(pool);
}

void cra_thrdpool_uninit(CraThrdPool *pool)
{
    pool->handle_exist_task = false;
    cra_thrdpool_wait(pool);
    cra_blkdeque_uninit(&pool->task_que);
    cra_free(pool->threads);
}

void cra_thrdpool_wait(CraThrdPool *pool)
{
    pool->can_in = false;
    pool->is_running = false;
    cra_blkdeque_dont_block_in(&pool->task_que);
    cra_blkdeque_dont_block_out(&pool->task_que);

    for (int i = 0; i < pool->threadcnt; i++)
    {
        cra_thrd_join(pool->threads[i].thrd);
    }
}

static inline bool cra_thrdpool_discard_task(CraThrdPool *pool)
{
    if (cra_blkdeque_get_count(&pool->task_que) < pool->task_max)
        return true;

    switch (pool->discard_policy)
    {
    case CRA_TPTASK_DISCARD_FIRST:
        cra_blkdeque_pop_left(&pool->task_que, NULL);
        return true;
    case CRA_TPTASK_DISCARD_LAST:
        cra_blkdeque_pop(&pool->task_que, NULL);
        return true;
    case CRA_TPTASK_DISCARD_SELF:
        break; // do nothing
    default:
        fprintf(stderr, "thread pool %p 不支持的策略.\n", (void *)pool);
        break;
    }
    return false;
}

bool cra_thrdpool_add_task2(CraThrdPool *pool, cra_thrdpool_task_fn2 func, void *arg1, void *arg2)
{
    CraThrdPoolTask task;

    if (pool == NULL || !pool->can_in)
        goto end;

    task.func = func;
    task.args.tid = 0;
    task.args.arg1 = arg1;
    task.args.arg2 = arg2;

add_task:
    if (cra_blkdeque_push_nonblocking(&pool->task_que, &task))
        return true;
    if (cra_thrdpool_discard_task(pool))
        goto add_task;

end:
    fprintf(stderr, "cra_thrdpool_add_task() -- 任务被拒绝.\n");
    return false;
}
