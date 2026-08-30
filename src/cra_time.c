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
#include <intrin.h>
#pragma intrinsic(_umul128)
#endif
#include "cra_time.h"
#include "cra_assert.h"

// return UINT64_MAX if overflow
static inline uint64_t
cra_mul_u64_128(uint64_t a, uint64_t b, uint64_t *rethigh)
{
    uint64_t low, high;

#ifdef __SIZEOF_INT128__
    unsigned __int128 product = (unsigned __int128)a * b;
    high = (uint64_t)(product >> 64);
    low = (uint64_t)product;
#elif defined(CRA_COMPILER_MSVC)
    low = _umul128(a, b, &high);
#else
    uint64_t carry = 0;

    uint64_t a_lo = (uint32_t)a;
    uint64_t a_hi = a >> 32;
    uint64_t b_lo = (uint32_t)b;
    uint64_t b_hi = b >> 32;

    uint64_t p0 = a_lo * b_lo;
    uint64_t p1 = a_lo * b_hi;
    uint64_t p2 = a_hi * b_lo;
    uint64_t p3 = a_hi * b_hi;

    low = p0;

    uint64_t add1 = (p1 & 0xFFFFFFFF) << 32;
    uint64_t sum = low + add1;
    if (sum < low)
        carry++;
    low = sum;

    uint64_t add2 = (p2 & 0xFFFFFFFF) << 32;
    sum = low + add2;
    if (sum < low)
        carry++;
    low = sum;

    high = p3 + (p1 >> 32) + (p2 >> 32) + carry;
#endif

    if (rethigh)
        *rethigh = high;
    return low;
}

uint64_t
cra_monotonic_ns(void)
{
#ifdef CRA_OS_WIN
    LARGE_INTEGER   count;
    static LONGLONG s_freq = 0;
    if (s_freq == 0)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        s_freq = freq.QuadPart;
    }

    assert(s_freq >= 1);
    QueryPerformanceCounter(&count);
    return cra_mul_u64_128(count.QuadPart, 1000000000, NULL) / s_freq;
#else
    // struct timeval tv;
    // gettimeofday(&tv, NULL);
    // return tv.tv_sec * 1000000 + tv.tv_usec;
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return cra_mul_u64_128(tp.tv_sec, 1000000000, NULL) + tp.tv_nsec;
#endif
}

uint64_t
cra_monotonic_us(void)
{
    return cra_monotonic_ns() / 1000;
}

uint64_t
cra_monotonic_ms(void)
{
    return cra_monotonic_ns() / 1000000;
}

double
cra_monotonic_sec(void)
{
    return (double)(cra_monotonic_ns() / 1000000000);
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
cra_datetime_epoch_ns(void)
{
    int64_t epoch;

#ifdef CRA_OS_WIN
    FILETIME       ft;
    ULARGE_INTEGER large;
    GetSystemTimePreciseAsFileTime(&ft);
    large.HighPart = ft.dwHighDateTime;
    large.LowPart = ft.dwLowDateTime;
    // 1601-01-01 00:00:00.000000000 -> 1970-01-01 00:00:00.000000000(Unix epoch)
    epoch = large.QuadPart * 100 - 11644473600000000000;
#else
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        return 0;
    epoch = cra_mul_u64_128(ts.tv_sec, 1000000000, NULL) + ts.tv_nsec;
#endif
    return epoch;
}

void
cra_datetime_from_epoch(CraDateTime *dt, uint64_t utc_epoch, bool tz_utc)
{
    struct tm t;
    time_t    ns;
    time_t    sec;

    assert(dt);

    cra_datetime_epoch_to_sec_and_ns(utc_epoch, &sec, &ns);
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
    dt->usec = (int)(ns / 1000);
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
    printf("}\n");
}
