#include <stdarg.h>
#include "crabase/cra_time.h"
#include "crabase/cra_assert.h"
#include "crabase/cra_malloc.h"
#include "crabase/cra_refcnt.h"
#include "crabase/cra_timewheel.h"

#ifdef CRA_OS_WINDOWS
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

static inline void
printt(const char *fmt, ...)
{
    va_list     ap;
    CraDateTime dt;

    cra_datetime_now(&dt, false);

    int ms = dt.sec * 1000 + dt.usec / 1000;
    printf("%4d-%02d-%02d %02d:%02d:%02d.%-6d [%5dms] ", dt.year, dt.mon, dt.day, dt.hour, dt.min, dt.sec, dt.usec, ms);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

static CRA_TIMER_FN_DEF(on_timeout1)
{
    if (!wheel)
    {
        printt("timer[%" PRId32 "ms]: add failed\n", delay);
        return;
    }

    if (*(bool *)user_data)
    {
        printt("timer[%" PRId32 "ms]: expired\n", delay);
        cra_timewheel_add(wheel, delay, user_data, on_timeout1);
    }
    else
    {
        printt("timer[%" PRId32 "ms]: has been canceled\n", delay);
    }
}

static bool flag;

static CRA_TIMER_FN_DEF(on_timeout2)
{
    if (!wheel)
    {
        printt("timer[%" PRId32 "ms]: add failed\n", delay);
        flag = false;
        return;
    }

    if (!user_data)
    {
        printt("timer[%" PRId32 "ms]: expired. no condition\n", delay);
        flag = false;
        return;
    }

    if (*(bool *)user_data)
    {
        *(bool *)user_data = false;
        printt("timer[%" PRId32 "ms]: expired. clear condition\n", delay);
        cra_timewheel_add(wheel, delay, NULL, on_timeout2);
    }
}

void
test_timewheel(void)
{
    CraTimewheel wheel;
    cra_timewheel_init(&wheel);
    wheel.ticks = UINT32_MAX - 100;

    bool condition = true;
    cra_timewheel_add(&wheel, 1000, &condition, on_timeout1);
    cra_timewheel_add(&wheel, 5000, &condition, on_timeout2);

    printt("start timewheel\n");

    flag = true;
    uint32_t timestamp = 1000;
    int32_t  timeout = cra_timewheel_update(&wheel, timestamp);
    while (flag)
    {
        // cra_msleep(timeout);
        // timestamp += timeout;
        cra_msleep(timeout / 2);
        timestamp += timeout / 2;
        timeout = cra_timewheel_update(&wheel, timestamp);
    }

    printt("stop timewheel\n");

    cra_timewheel_uninit(&wheel);
}

// ==========================

typedef struct
{
    int     begin;
    int     cnt;
    char   *end;
    CraRef *ref;
} MyTimerData;

static void
on_mytimerdata_uninit(void *data)
{
    MyTimerData *d = (MyTimerData *)data;
    printt("uninit MyTimerData{begin: %d, end: \"%s\"}\n", d->begin, d->end);
}

static CRA_TIMER_FN_DEF(set_flag_to_false)
{
    flag = false;
    printt("timer[%" PRId32 "ms]: set flag to false\n", delay);
    CRA_UNUSED(wheel);
    CRA_UNUSED(user_data);
}

static CRA_TIMER_FN_DEF(on_timeout3)
{
    assert_always(wheel);
    assert_always(user_data);
    MyTimerData *d = (MyTimerData *)user_data;
    printt("timer[%" PRId32 "ms]: expired. begin: %d, end: \"%s\", cnt: %d, refcnt: %d\n", delay, d->begin, d->end,
           d->cnt, (int)d->ref->cnt);
    if (--d->cnt == 0)
    {
        cra_ref_unref(d->ref);
        printt("timer[%" PRId32 "ms]: unref()\n", delay);

        cra_timewheel_add(wheel, 1000, NULL, set_flag_to_false);
    }
    else
    {
        cra_timewheel_add(wheel, delay, user_data, on_timeout3);
    }
}

static CRA_TIMER_FN_DEF(on_timeout4)
{
    assert_always(wheel);
    assert_always(user_data);
    printt("timer[%" PRId32 "ms]: unref()\n", delay);
    cra_ref_unref((CraRef *)user_data);
}

void
test_timewheel2(void)
{
    CraTimewheel *wheel = cra_alloc(CraTimewheel);
    cra_timewheel_init(wheel);

    CraRef      *ref = cra_ref_make(sizeof(MyTimerData), on_mytimerdata_uninit);
    MyTimerData *data = (MyTimerData *)cra_ref_get_ptr_uncheck(ref);
    data->end = "hello world";
    data->begin = 100;
    data->cnt = 10;
    data->ref = ref;

    cra_ref_ref(ref);
    cra_timewheel_add(wheel, 500, data, on_timeout3);
    cra_ref_ref(ref);
    cra_timewheel_add(wheel, 3000, ref, on_timeout4);
    cra_ref_unref(ref);

    flag = true;
    int32_t timeout = cra_timewheel_update(wheel, cra_monotonic_ms());
    while (flag)
    {
        cra_msleep(timeout);
        timeout = cra_timewheel_update(wheel, cra_monotonic_ms());
    }

    cra_timewheel_uninit(wheel);
    cra_dealloc(wheel);
}

void
test_timeout_less_than_tick(void)
{
    CraTimewheel wheel;
    cra_timewheel_init(&wheel);

    bool condition = true;
    cra_timewheel_add(&wheel, 1, &condition, on_timeout1);
    cra_timewheel_add(&wheel, 3000, NULL, set_flag_to_false);

    flag = true;
    int32_t timeout = cra_timewheel_update(&wheel, cra_monotonic_ms());
    while (flag)
    {
        cra_msleep(timeout);
        timeout = cra_timewheel_update(&wheel, cra_monotonic_ms());
    }

    cra_timewheel_uninit(&wheel);
}

void
test_timer_clear(void)
{
    CraTimewheel *wheel;
    wheel = cra_alloc(CraTimewheel);
    cra_timewheel_init(wheel);

    bool condition = true;
    cra_timewheel_add(wheel, 500, &condition, on_timeout1);
    cra_timewheel_add(wheel, 1000, &condition, on_timeout1);
    cra_timewheel_add(wheel, 2000, &condition, on_timeout1);
    cra_timewheel_add(wheel, 3000, &condition, on_timeout1);
    cra_timewheel_add(wheel, 10000, NULL, set_flag_to_false);

    flag = true;
    int32_t timeout = cra_timewheel_update(wheel, cra_monotonic_ms());
    while (flag)
    {
        cra_msleep(timeout);
        timeout = cra_timewheel_update(wheel, cra_monotonic_ms());
    }

    cra_timewheel_uninit(wheel);
    cra_dealloc(wheel);
}

static CRA_TIMER_FN_DEF(on_timeout5)
{
    // printt("timer[%" PRId32 "ms]: expired\n", delay);
    if (wheel)
        cra_timewheel_add(wheel, delay, user_data, on_timeout5);
}

static uint64_t last = 0;
static CRA_TIMER_FN_DEF(on_timeout6)
{
    if (!wheel)
        return;

    uint64_t now = cra_monotonic_ms();
    if (last == 0)
    {
        last = now;
        printt("timer[%" PRId32 "ms]: first expired\n", delay);
    }
    else
    {
        uint64_t delta = now - last;
        printt("timer[%" PRId32 "ms]: delta: %" PRIu64 "ms\n", delay, delta);
        last = now;
    }

    if (--(*(int *)user_data) > 0)
        cra_timewheel_add(wheel, delay, user_data, on_timeout6);
    else
        flag = false;
}

void
test(void)
{
    int n = 1000000;

    CraTimewheel wheel;
    cra_timewheel_init(&wheel);

    uint64_t start = cra_monotonic_ms();
    for (int i = 0; i < n; ++i)
    {
        cra_timewheel_add(&wheel, (rand() + 10) % 800000, NULL, on_timeout5);
    }
    int cnt = 10;
    cra_timewheel_add(&wheel, 1000, &cnt, on_timeout6);
    uint64_t end = cra_monotonic_ms();

    printt("add %d timers, cost %" PRIu64 "ms\n", n, end - start);

    flag = true;
    uint64_t timestamp = cra_monotonic_ms();
    int32_t  timeout = cra_timewheel_update(&wheel, timestamp);
    while (flag)
    {
        cra_msleep(timeout);
        // timestamp += timeout;
        timestamp = cra_monotonic_ms();
        timeout = cra_timewheel_update(&wheel, timestamp);
    }

    start = cra_monotonic_ms();
    cra_timewheel_uninit(&wheel);
    end = cra_monotonic_ms();
    printt("uninit cost %" PRIu64 "ms\n", end - start);
}

int
main(void)
{
#ifdef CRA_OS_WINDOWS
    timeBeginPeriod(1);
#endif

    // test_timewheel();
    // printf("----------------------- ^_^ -----------------------\n");
    // test_timewheel2();
    // printf("----------------------- ^_^ -----------------------\n");
    // test_timeout_less_than_tick();
    // printf("----------------------- ^_^ -----------------------\n");
    // test_timer_clear();
    // printf("----------------------- ^_^ -----------------------\n");
    test();
    printf("----------------------- ^_^ -----------------------\n");

#ifdef CRA_OS_WINDOWS
    timeEndPeriod(1);
#endif

    cra_memory_leak_report();
    return 0;
}
