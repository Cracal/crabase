#include "cra_log.h"
#include "cra_malloc.h"
#include "cra_refcnt.h"
#include "cra_timewheel.h"

#define CRA_LOG_NAME "test-timewheel"

static void on_timeout1(CraTimer_base *timer)
{
    cra_log_info("timeout! [timer: 0x%x, tick: %ums, repeat: %u]\n", timer, timer->timeout_ms, timer->repeat);
}

typedef struct
{
    CraTimer_base base;
    CraTimer_base *t1;
} MyTimer;

static bool flag;

static void on_timeout2(CraTimer_base *timer)
{
    MyTimer *t = (MyTimer *)timer;
    cra_timer_base_cancel(t->t1);

    cra_log_info("timeout! [timer: 0x%x, tick: %ums, repeat: %u] kill t1[0x%x]\n", timer, timer->timeout_ms, timer->repeat, t->t1);

    flag = false;
}

void test_timewheel(void)
{
    CraTimer_base t1;
    MyTimer t2;
    CraTimewheel wheel;

    cra_timewheel_init(&wheel, 50, 20);

    cra_timer_base_init(&t1, UINT32_MAX, 1000, on_timeout1, NULL);
    cra_timer_base_init((CraTimer_base *)&t2, 1, 5000, on_timeout2, NULL);
    t2.t1 = &t1;

    assert_always(cra_timewheel_add(&wheel, &t1));
    assert_always(cra_timewheel_add(&wheel, (CraTimer_base *)&t2));

    flag = true;
    while (flag)
    {
        cra_msleep(50);
        cra_timewheel_tick(&wheel);
    }

    cra_timewheel_uninit(&wheel);
}

// ==========================

typedef struct
{
    int start;
    CraTimer_base base;
    char *end;
} MyTimer2;

static void on_free_mytimer2(void *timer)
{
    MyTimer2 *t = (MyTimer2 *)timer;
    cra_log_info("free MyTimer2{start: %d, end: \"%s\"}[0x%x]", t->start, t->end, t);
}

static void on_cancel(CraTimer_base *timer)
{
    MyTimer2 *t = container_of(timer, MyTimer2, base);
    CRA_REFCNT(MyTimer2) *trc = (void *)container_of(t, CRA_REFCNT(MyTimer2), o);
    cra_refcnt_unref0(trc);

    flag = false;
}

static void on_timeout3(CraTimer_base *timer)
{
    MyTimer2 *t = container_of(timer, MyTimer2, base);
    CRA_REFCNT(MyTimer2) *trc = (void *)container_of(t, CRA_REFCNT(MyTimer2), o);
    cra_log_info("timeout! [timer: 0x%x, tick: %ums, repeat: %u, refcnt: %u, start: %d, end: %s]",
                 timer, timer->timeout_ms, timer->repeat, trc->rc.cnt, t->start, t->end);
}

void test_timewheel2(void)
{
    CRA_REFCNT(MyTimer2) *t = (void *)cra_alloc(CRA_REFCNT(MyTimer2));
    cra_refcnt_init(t, false, false, true, false, on_free_mytimer2);
    cra_timer_base_init(&t->o.base, 10, 500, on_timeout3, on_cancel);
    t->o.start = 100;
    t->o.end = "hello world";

    CraTimewheel *wheel = cra_alloc(CraTimewheel);
    cra_timewheel_init(wheel, 50, 25);
    cra_timewheel_add(wheel, &t->o.base);

    flag = true;
    while (flag)
    {
        cra_msleep(50);
        cra_timewheel_tick(wheel);
    }

    cra_timewheel_uninit(wheel);
    cra_dealloc(wheel);
}

static void stop_timewheel(CraTimer_base *timer)
{
    CRA_UNUSED_VALUE(timer);
    flag = false;
}

void test_timeout_less_than_tick(void)
{
    CraTimer_base timer;
    cra_timer_base_init(&timer, 3, 8, on_timeout1, stop_timewheel);

    CraTimewheel wheel;
    cra_timewheel_init(&wheel, 1000, 64);

    cra_timewheel_add(&wheel, &timer);

    flag = true;
    while (flag)
    {
        cra_sleep(1);
        cra_timewheel_tick(&wheel);
    }

    cra_timewheel_uninit(&wheel);
}

static void on_remove(CraTimer_base *timer)
{
    cra_log_info("remove timer[%u, %u].", timer->timeout_ms, timer->repeat);
    cra_dealloc(timer);
}

void test_timer_clear(void)
{
    CraTimer_base *t1, *t2, *t3, *t4, *stop;
    CraTimewheel *wheel;

    wheel = cra_alloc(CraTimewheel);
    cra_timewheel_init(wheel, 50, 20);

    t1 = cra_alloc(CraTimer_base);
    t2 = cra_alloc(CraTimer_base);
    t3 = cra_alloc(CraTimer_base);
    t4 = cra_alloc(CraTimer_base);
    stop = cra_alloc(CraTimer_base);
    cra_timer_base_init(t1, UINT32_MAX, 500, on_timeout1, on_remove);
    cra_timer_base_init(t2, UINT32_MAX, 1000, on_timeout1, on_remove);
    cra_timer_base_init(t3, UINT32_MAX, 2000, on_timeout1, on_remove);
    cra_timer_base_init(t4, UINT32_MAX, 3000, on_timeout1, on_remove);
    cra_timer_base_init(stop, 1, 10000, stop_timewheel, on_remove);

    cra_timewheel_add(wheel, t1);
    cra_timewheel_add(wheel, t2);
    cra_timewheel_add(wheel, t3);
    cra_timewheel_add(wheel, t4);
    cra_timewheel_add(wheel, stop);

    flag = true;
    while (flag)
    {
        cra_msleep(50);
        cra_timewheel_tick(wheel);
    }

    cra_timewheel_uninit(wheel);
    cra_dealloc(wheel);
}

int main(void)
{
    cra_log_startup(CRA_LOG_LEVEL_DEBUG, false, true);

    test_timewheel();
    cra_log_info("----------------------- ^_^ -----------------------");
    test_timewheel2();
    cra_log_info("----------------------- ^_^ -----------------------");
    test_timeout_less_than_tick();
    cra_log_info("----------------------- ^_^ -----------------------");
    test_timer_clear();

    cra_log_cleanup();
    cra_memory_leak_report(stdout);
    return 0;
}
