#include "cra_assert.h"
#include "cra_log.h"
#include "cra_malloc.h"
#include "cra_refcnt.h"
#include "cra_timewheel.h"

#define CRA_LOG_NAME "test-timewheel"

static void
on_timeout1(CraTimer_base *timer)
{
    cra_log_info("timeout! [timer: 0x%x, tick: %ums, repeat: %u]\n", timer, timer->timeout_ms, timer->repeat);
}

typedef struct
{
    CraTimer_base  base;
    CraTimer_base *t1;
} MyTimer;

static bool flag;

static void
on_timeout2(CraTimer_base *timer)
{
    MyTimer *t = (MyTimer *)timer;

    if (t->t1)
    {
        cra_timer_base_cancel(t->t1);
        cra_log_info("timeout! [timer: 0x%x, tick: %ums, repeat: %u] kill t1[0x%x]\n",
                     timer,
                     timer->timeout_ms,
                     timer->repeat,
                     t->t1);
        t->t1 = NULL;
    }
    else
    {
        flag = false;
        cra_log_info("timeout! [timer: 0x%x]: stop timewheel!", timer);
    }
}

void
test_timewheel(void)
{
    CraTimer_base t1;
    MyTimer       t2;
    CraTimewheel  wheel;

    cra_timewheel_init(&wheel, 50, 20);

    cra_timer_base_init(&t1, CRA_TIMER_INFINITE, 1000, on_timeout1, NULL);
    cra_timer_base_init((CraTimer_base *)&t2, 2, 5000, on_timeout2, NULL);
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
    int           begin;
    CraTimer_base base;
    char         *end;
} MyTimer2;

static void
on_free_mytimer2(CraRefcnt *timer)
{
    CRA_REFCNT_DEF(MyTimer2) *t = (void *)timer;
    cra_log_info("free MyTimer2{begin: %d, end: \"%s\"}[0x%x]", CRA_REFCNT_OBJ(t)->begin, CRA_REFCNT_OBJ(t)->end, t);
    cra_dealloc(t);
}

static void
on_cancel(CraTimer_base *timer)
{
    MyTimer2 *t = container_of(timer, MyTimer2, base);
    CRA_REFCNT_DEF(MyTimer2) *trc = (void *)container_of(t, CRA_REFCNT_DEF(MyTimer2), o);
    cra_refcnt_unref0(CRA_REFCNT_RC(trc));

    flag = false;
}

static void
on_timeout3(CraTimer_base *timer)
{
    MyTimer2 *t = container_of(timer, MyTimer2, base);
    CRA_REFCNT_DEF(MyTimer2) *trc = (void *)container_of(t, CRA_REFCNT_DEF(MyTimer2), o);
    cra_log_info("timeout! [timer: 0x%x, tick: %ums, repeat: %u, refcnt: %u, begin: %d, end: \"%s\"]",
                 timer,
                 timer->timeout_ms,
                 timer->repeat,
                 trc->rc.cnt,
                 t->begin,
                 t->end);
}

void
test_timewheel2(void)
{
    CRA_REFCNT_DEF(MyTimer2) *t = (void *)cra_alloc(CRA_REFCNT_DEF(MyTimer2));
    cra_refcnt_init(CRA_REFCNT_RC(t), on_free_mytimer2);
    cra_timer_base_init(&CRA_REFCNT_OBJ(t)->base, 10, 500, on_timeout3, on_cancel);
    CRA_REFCNT_OBJ(t)->begin = 100;
    CRA_REFCNT_OBJ(t)->end = "hello world";

    CraTimewheel *wheel = cra_alloc(CraTimewheel);
    cra_timewheel_init(wheel, 50, 25);
    cra_timewheel_add(wheel, &CRA_REFCNT_OBJ(t)->base);

    flag = true;
    while (flag)
    {
        cra_msleep(50);
        cra_timewheel_tick(wheel);
    }

    cra_timewheel_uninit(wheel);
    cra_dealloc(wheel);
}

static void
stop_timewheel(CraTimer_base *timer)
{
    cra_log_info("timer[timeout: %ums, repeat: %u]: stop timing wheel.", timer->timeout_ms, timer->repeat);
    flag = false;
}

void
test_timeout_less_than_tick(void)
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

static void
on_remove(CraTimer_base *timer)
{
    cra_log_info("remove timer[%u, %u].", timer->timeout_ms, timer->repeat);
    cra_dealloc(timer);
}

void
test_timer_clear(void)
{
    CraTimer_base *t1, *t2, *t3, *t4, *stop;
    CraTimewheel  *wheel;

    wheel = cra_alloc(CraTimewheel);
    cra_timewheel_init(wheel, 50, 20);

    t1 = cra_alloc(CraTimer_base);
    t2 = cra_alloc(CraTimer_base);
    t3 = cra_alloc(CraTimer_base);
    t4 = cra_alloc(CraTimer_base);
    stop = cra_alloc(CraTimer_base);
    cra_timer_base_init(t1, CRA_TIMER_INFINITE, 500, on_timeout1, on_remove);
    cra_timer_base_init(t2, CRA_TIMER_INFINITE, 1000, on_timeout1, on_remove);
    cra_timer_base_init(t3, CRA_TIMER_INFINITE, 2000, on_timeout1, on_remove);
    cra_timer_base_init(t4, CRA_TIMER_INFINITE, 3000, on_timeout1, on_remove);
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

static void
on_timeout_cancel_self(CraTimer_base *timer)
{
    static int i = 3;
    if (i-- <= 0)
    {
        cra_log_info("timer[timeout: %ums, repeat: %u]: cancel self", timer->timeout_ms, timer->repeat);
        // cra_timer_base_cancel(timer);
        // cra_timer_base_cls_active(timer);
        cra_timer_base_set_deactive(timer);
    }
    else
    {
        cra_log_info("timer[timeout: %ums, repeat: %u]: timeout! ", timer->timeout_ms, timer->repeat);
    }
}

void
test_timer_cancel_self(void)
{
    CraTimewheel  timewheel;
    CraTimer_base timer1, timer2;

    cra_timewheel_init(&timewheel, 50, 20);
    cra_timer_base_init(&timer1, CRA_TIMER_INFINITE, 1000, on_timeout_cancel_self, NULL);
    cra_timer_base_init(&timer2, 1, 7000, stop_timewheel, NULL);

    cra_timewheel_add(&timewheel, &timer1);
    cra_timewheel_add(&timewheel, &timer2);

    flag = true;
    while (flag)
    {
        cra_msleep(50);
        cra_timewheel_tick(&timewheel);
    }

    cra_timewheel_uninit(&timewheel);
}

int
main(void)
{
    cra_log_startup(CRA_LOG_LEVEL_DEBUG, true, (CraLogTo_i **)cra_logto_stdout_create(false));

    test_timewheel();
    cra_log_info("----------------------- ^_^ -----------------------");
    test_timewheel2();
    cra_log_info("----------------------- ^_^ -----------------------");
    test_timeout_less_than_tick();
    cra_log_info("----------------------- ^_^ -----------------------");
    test_timer_clear();
    cra_log_info("----------------------- ^_^ -----------------------");
    test_timer_cancel_self();

    cra_log_cleanup();
    cra_memory_leak_report();
    return 0;
}
