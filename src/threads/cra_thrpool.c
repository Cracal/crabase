/**
 * @file cra_thrpool.c
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
#include "threads/cra_thrpool.h"

typedef struct
{
    CraThrPoolArgs2 args;
    cra_thrpool_task_fn2 func;
} CraThrPoolTask;

struct _CraThrPoolWorker
{
    CraCDL *cdl;
    CraThrPool *pool;
    cra_thrd_t thrd;
};

static CRA_THRD_FUNC(__cra_thrpool_worker)
{
    CraThrPoolTask task;
    CraThrPoolWorker *worker = (CraThrPoolWorker *)arg;
    CraBlkDeque *taskque = &worker->pool->task_que;
    CraThrPool *pool = worker->pool;
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

void cra_thrpool_init(CraThrPool *pool, int threads, size_t task_max)
{
    assert(threads > 0 && task_max > 0);
    pool->can_in = true;
    pool->handle_exist_task = true;
    pool->discard_policy = CRA_TPTASK_DISCARD_SELF;
    pool->idle_threads = threads;
    pool->threadcnt = 0;
    pool->task_max = task_max;
    cra_blkdeque_init0(CraThrPoolTask, &pool->task_que, task_max, false, NULL);
    pool->threads = (CraThrPoolWorker *)cra_malloc(sizeof(CraThrPoolWorker) * threads);

#if 1 // create threads
    CraCDL cdl;
    cra_cdl_init(&cdl, threads);
    pool->is_running = true;
    for (int i = 0; i < threads; i++, pool->threadcnt++)
    {
        pool->threads[i].pool = pool;
        pool->threads[i].cdl = &cdl;
        if (!cra_thrd_create(&pool->threads[i].thrd, __cra_thrpool_worker, &pool->threads[i]))
            goto error_ret;
    }

    cra_cdl_wait(&cdl);
    cra_cdl_uninit(&cdl);
#endif // end create threads

    return;

error_ret:
    cra_cdl_uninit(&cdl);
    cra_thrpool_uninit(pool);
}

void cra_thrpool_uninit(CraThrPool *pool)
{
    pool->handle_exist_task = false;
    cra_thrpool_wait(pool);
    cra_blkdeque_uninit(&pool->task_que);
    cra_free(pool->threads);
}

void cra_thrpool_wait(CraThrPool *pool)
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

static inline bool cra_thrpool_discard_task(CraThrPool *pool)
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

bool cra_thrpool_add_task2(CraThrPool *pool, cra_thrpool_task_fn2 func, void *arg1, void *arg2)
{
    CraThrPoolTask task;

    if (pool == NULL || !pool->can_in)
        goto end;

    task.func = func;
    task.args.tid = 0;
    task.args.arg1 = arg1;
    task.args.arg2 = arg2;

add_task:
    if (cra_blkdeque_push_nonblocking(&pool->task_que, &task))
        return true;
    if (cra_thrpool_discard_task(pool))
        goto add_task;

end:
    fprintf(stderr, "cra_thrpool_add_task() -- 任务被拒绝.\n");
    return false;
}
