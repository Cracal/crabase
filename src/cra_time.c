/**
 * @file cra_time.c
 * @author Cracal
 * @brief time
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifdef CRA_OS_LINUX
#include <sys/time.h>
#endif
#include "cra_time.h"
#include "cra_assert.h"

static inline uint64_t
cra_mul_clamp_max(uint64_t a, uint64_t b)
{
    if (a > UINT64_MAX / b)
        return UINT64_MAX;
    return a * b;
}

static inline uint64_t
cra_add_clamp_max(uint64_t a, uint64_t b)
{
    if (a > UINT64_MAX - b)
        return UINT64_MAX;
    return a + b;
}

#ifdef CRA_OS_WIN

static inline void
cra_make_fraction(uint64_t num, uint64_t denom, uint64_t *frac_num, uint64_t *frac_denom)
{
    assert(denom > 0);

    // 最大公约数
    uint64_t temp;
    uint64_t a = num;
    uint64_t b = denom;
    while (b != 0)
    {
        temp = b;
        b = a % b;
        a = temp;
    }
    *frac_num = num / a;
    *frac_denom = denom / a;
}

static inline uint64_t
cra_counter_to_ns(uint64_t counter, uint64_t num, uint64_t denom)
{
    assert(denom > 0);

    if (denom == 1)
        return cra_mul_clamp_max(counter, num);

    uint64_t n, r;
    n = counter / denom;
    counter %= denom;
    r = cra_mul_clamp_max(counter, num) / denom;
    n = cra_mul_clamp_max(n, num);
    n = cra_add_clamp_max(n, r);
    return n;
}

#else

static inline uint64_t
cra_timespec_to_ns(const struct timespec *ts)
{
    uint64_t ns = ts->tv_sec;
    ns = cra_mul_clamp_max(ns, 1000000000);
    ns = cra_add_clamp_max(ns, ts->tv_nsec);
    return ns;
}

static inline uint64_t
cra_timespec_to_us(const struct timespec *ts)
{
    uint64_t us = ts->tv_sec;
    us = cra_add_clamp_max(us, 1000000);
    us = cra_add_clamp_max(us, ts->tv_nsec / 1000);
    return us;
}

#endif

uint64_t
cra_monotonic_ns(void)
{
    uint64_t ns;

#ifdef CRA_OS_WIN
    static uint64_t s_num = 0;
    static uint64_t s_denom = 0;
    if (s_denom == 0)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        cra_make_fraction(1000000000, freq.QuadPart, &s_num, &s_denom);
    }

    assert(s_denom > 0);
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    ns = cra_counter_to_ns(count.QuadPart, s_num, s_denom);
#else
    // struct timeval tv;
    // gettimeofday(&tv, NULL);
    // return tv.tv_sec * 1000000 + tv.tv_usec;
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return 0;
    ns = cra_timespec_to_ns(&ts);
#endif

    return ns;
}

// ======================================

void
cra_print_tm(const struct tm *const t)
{
    assert(t);
    printf("struct tm {\n");
    printf("  tm_year  = %d,\n", t->tm_year + 1900);
    printf("  tm_mon   = %d,\n", t->tm_mon + 1);
    printf("  tm_mday  = %d,\n", t->tm_mday);
    printf("  tm_hour  = %d,\n", t->tm_hour);
    printf("  tm_min   = %d,\n", t->tm_min);
    printf("  tm_sec   = %d,\n", t->tm_sec);
    printf("  tm_wday  = %d,\n", t->tm_wday);
    printf("  tm_yday  = %d,\n", t->tm_yday);
    printf("  tm_isdst = %d,\n", t->tm_isdst);
    printf("}\n");
}

uint64_t
cra_datetime_epoch_us(void)
{
    uint64_t epoch;

#ifdef CRA_OS_WIN
    FILETIME       ft;
    ULARGE_INTEGER large;
    GetSystemTimePreciseAsFileTime(&ft);
    large.HighPart = ft.dwHighDateTime;
    large.LowPart = ft.dwLowDateTime;
    // 1601-01-01 00:00:00.000000 -> 1970-01-01 00:00:00.000000(Unix epoch)
    epoch = large.QuadPart / 10 - 11644473600000000;
#else
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        return 0;
    epoch = cra_timespec_to_us(&ts);
#endif
    return epoch;
}

void
cra_datetime_from_epoch(CraDateTime *dt, uint64_t utc_epoch, bool tz_utc)
{
    struct tm t;
    time_t    sec;

    assert(dt);

    cra_datetime_epoch_to_sec_and_us(utc_epoch, &sec, &dt->usec);
    if (tz_utc)
        cra_gmtime(sec, &t);
    else
        cra_localtime(sec, &t);

    dt->year = t.tm_year + 1900;
    dt->mon = t.tm_mon + 1;
    dt->day = t.tm_mday;
    dt->hour = t.tm_hour;
    dt->min = t.tm_min;
    dt->sec = t.tm_sec;
    dt->isdst = t.tm_isdst;
}

#if 0
void
cra_datetime_now_utc(CraDateTime *dt)
{
#ifdef CRA_OS_WIN
    SYSTEMTIME st;
    GetSystemTime(&st);
    dt->year = st.wYear;
    dt->mon = st.wMonth;
    dt->day = st.wDay;
    dt->hour = st.wHour;
    dt->min = st.wMinute;
    dt->sec = st.wSecond;
    dt->msec = st.wMilliseconds;
#else
    struct tm       t;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    cra_gmtime(ts.tv_sec, &t);
    dt->year = t.tm_year + 1900;
    dt->mon = t.tm_mon + 1;
    dt->day = t.tm_mday;
    dt->hour = t.tm_hour;
    dt->min = t.tm_min;
    dt->sec = t.tm_sec;
    dt->msec = ts.tv_nsec / 1000000;
#endif
}

void
cra_datetime_now_localtime(CraDateTime *dt)
{
#ifdef CRA_OS_WIN
    SYSTEMTIME st;
    GetLocalTime(&st);
    dt->year = st.wYear;
    dt->mon = st.wMonth;
    dt->day = st.wDay;
    dt->hour = st.wHour;
    dt->min = st.wMinute;
    dt->sec = st.wSecond;
    dt->msec = st.wMilliseconds;
#else
    struct tm       t;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    cra_localtime(ts.tv_sec, &t);
    dt->year = t.tm_year + 1900;
    dt->mon = t.tm_mon + 1;
    dt->day = t.tm_mday;
    dt->hour = t.tm_hour;
    dt->min = t.tm_min;
    dt->sec = t.tm_sec;
    dt->msec = ts.tv_nsec / 1000000;
#endif
}
#endif

void
cra_print_datetime(const CraDateTime *const dt)
{
    assert(dt);
    printf("CraDateTime {\n");
    printf("  year  = %d,\n", dt->year);
    printf("  mon   = %d,\n", dt->mon);
    printf("  mday  = %d,\n", dt->day);
    printf("  hour  = %d,\n", dt->hour);
    printf("  min   = %d,\n", dt->min);
    printf("  sec   = %d,\n", dt->sec);
    printf("  usec  = %d,\n", dt->usec);
    printf("  isdst = %d,\n", dt->isdst);
    printf("}\n");
}
