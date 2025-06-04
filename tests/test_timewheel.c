#include "cra_log.h"
#include "cra_malloc.h"
#include "cra_timewheel.h"

#define CRA_LOG_NAME "test-timewheel"

static void on_timeout1(CraTimerRc *timer)
{
    unsigned int tick = cra_timer_get_timeout(timer);
    unsigned int repeat = cra_timer_get_repeat(timer);
    cra_log_info("timeout! [timer: 0x%x, tick: %ums, repeat: %u]\n", timer, tick, repeat);
}

static void on_timeout2(CraTimerRc *timer)
{
    unsigned int tick = cra_timer_get_timeout(timer);
    unsigned int repeat = cra_timer_get_repeat(timer);
    CraTimerRc *t1 = (CraTimerRc *)cra_timer_get_arg(timer);
    cra_timer_set_arg(timer, NULL);

    cra_log_info("timeout! [timer: 0x%x, tick: %ums, repeat: %u] kill t1[0x%x]\n", timer, tick, repeat, t1);

    cra_timer_clear_active(t1);
    cra_refcnt_unref0(t1);
}

static int flag = 0;

static void before_destroy(CraTimerRc *timer)
{
    cra_log_info("timer[0x%x] will be free\n", timer);
    flag++;
}

void test_timewheel(void)
{
    CraTimerRc *t1, *t2;
    CraTimewheel *wheel;

    wheel = cra_timewheel_create(50, 20);

    t1 = cra_timer_new(UINT32_MAX, 1000, NULL, on_timeout1, before_destroy);
    cra_refcnt_ref(t1);
    t2 = cra_timer_new(1, 5000, t1, on_timeout2, before_destroy);

    assert_always(cra_timewheel_add(wheel, t1));
    assert_always(cra_timewheel_add(wheel, t2));

    cra_refcnt_unref0(t1);
    cra_refcnt_unref0(t2);

    while (flag < 2)
    {
        cra_msleep(50);
        cra_timewheel_tick(wheel);
    }

    cra_timewheel_destroy(wheel);
}

int main(void)
{
    cra_log_startup(CRA_LOG_LEVEL_DEBUG, false, true);

    test_timewheel();

    cra_log_cleanup();
    cra_memory_leak_report(stdout);
    return 0;
}
