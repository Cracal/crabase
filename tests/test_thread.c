/**
 * @file test-thread.c
 * @author Cracal
 * @brief test thread
 * @version 0.1
 * @date 2024-09-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "threads/cra_lock.h"
#include "threads/cra_thread.h"

#if 1 // test thread

static CRA_THRD_FUNC(thrd_func)
{
    CRA_UNUSED_VALUE(arg);
    for (int i = 0; i < 10; i++)
    {
        printf("Thread %lu is runing...\n", cra_thrd_get_current_tid());
        cra_sleep(1);
    }
    return (cra_thrd_ret_t){ 0 };
}

void
test_thread(void)
{
    cra_thrd_t th1, th2;
    cra_thrd_create(&th1, thrd_func, NULL);
    cra_thrd_create(&th2, thrd_func, NULL);

    cra_thrd_join(th1);
    cra_thrd_join(th2);
}

#endif // end test thread

#if 1 // test lock

static cra_cond_t  s_cond;
static cra_mutex_t s_mutex;
static int         s_val = 0;

static CRA_THRD_FUNC(thrd_locker_func)
{
    CRA_UNUSED_VALUE(arg);

    cra_mutex_lock(&s_mutex);
    printf("test-lock-thread: get s_val: %d\n", s_val);
    cra_mutex_unlock(&s_mutex);

    cra_msleep(500);

    s_val = 200;
    printf("test-lock-thread: set s_val: %d\n", s_val);
    cra_cond_signal(&s_cond);

    return (cra_thrd_ret_t){ 0 };
}

static void
test_locker(void)
{
    cra_mutex_init(&s_mutex);
    cra_cond_init(&s_cond);

    cra_thrd_t thr;
    cra_thrd_create(&thr, thrd_locker_func, NULL);

    cra_mutex_lock(&s_mutex);
    printf("test-lock-main: get s_val: %d\n", s_val);
    s_val = 100;
    printf("test-lock-main: set s_val: %d\n", s_val);
    cra_mutex_unlock(&s_mutex);

    cra_cond_wait(&s_cond, &s_mutex);
    printf("test-lock-main: get s_val: %d\n", s_val);

    cra_thrd_join(thr);

    cra_mutex_destroy(&s_mutex);
    cra_cond_destroy(&s_cond);
}

#endif // end test lock

#if 1 // test mutex

static CRA_THRD_FUNC(thrd_mutex_func)
{
    cra_mutex_t *mutex = (cra_mutex_t *)arg;
    bool         b = cra_mutex_trylock(mutex);
    if (b)
    {
        printf("test-mutex-routine: 子线程第一次尝试获得锁成功……\n");
        cra_sleep(1);
        cra_mutex_unlock(mutex);
    }
    else
    {
        printf("test-mutex-routine: 子线程第一次尝试获得锁失败……\n");
    }

    cra_mutex_lock(mutex);
    printf("test-mutex-routine: 子线程第二次获得锁……\n");
    cra_mutex_unlock(mutex);

    return (cra_thrd_ret_t){ 0 };
}

static void
test_mutex(void)
{
    cra_thrd_t  t;
    cra_mutex_t mutex;

    cra_mutex_init(&mutex);

    cra_thrd_create(&t, thrd_mutex_func, &mutex);

    cra_mutex_lock(&mutex);
    printf("test-mutex-main: 主线程第一次获得锁……\n");
    cra_mutex_unlock(&mutex);

    cra_mutex_lock(&mutex);
    printf("test-mutex-main: 主线程第二次获得锁……\n");
    cra_sleep(1);
    cra_mutex_unlock(&mutex);

    cra_thrd_join(t);

    cra_mutex_destroy(&mutex);
}

#endif // end test mutex

#if 1 // test condvar

static CRA_THRD_FUNC(thrd_condvar)
{
    CRA_UNUSED_VALUE(arg);

    while (true)
    {
        cra_mutex_lock(&s_mutex);
        --s_val;
        cra_cond_signal(&s_cond);
        printf("test-cond-routine: signal once.\n");
        cra_mutex_unlock(&s_mutex);

        if (s_val == 0)
            break;

        if (s_val == 1)
            cra_msleep(3000);
        cra_msleep(100);
    }

    return (cra_thrd_ret_t){ 0 };
}

static void
test_condvar(void)
{
    bool       b;
    cra_thrd_t t;

    cra_mutex_init(&s_mutex);
    cra_cond_init(&s_cond);
    s_val = 3;

    cra_mutex_lock(&s_mutex);

    cra_thrd_create(&t, thrd_condvar, NULL);

    while (s_val > 0)
    {
        b = cra_cond_wait_timeout(&s_cond, &s_mutex, 1000);
        // res = cra_cond_wait(&s_cond, &s_mutex);
        if (!b)
        {
            printf("test-cond-main: cra_cond_wait_timeout() timeout.\n");
        }
        else
        {
            printf("test-cond-main: wait ok.\n");
        }
    }

    cra_mutex_unlock(&s_mutex);

    cra_thrd_join(t);

    cra_mutex_destroy(&s_mutex);
    cra_cond_destroy(&s_cond);
}

#endif // end test condvar

int
main(void)
{
    test_thread();
    test_locker();
    test_mutex();
    test_condvar();
    return 0;
}
