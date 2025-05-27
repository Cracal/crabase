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
#include "cra_time.h"
#ifdef CRA_OS_LINUX
#include <sys/time.h>
#endif

static inline double cra_monotonic(void)
{
#ifdef CRA_OS_WIN
    static LONGLONG s_freq = 0;
    if (s_freq == 0)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        s_freq = freq.QuadPart;
    }
    if (s_freq != 0)
    {
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        return (count.QuadPart / (double)s_freq);
    }
    return 0;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec / 1000000;
#endif
}

double cra_monotonic_sec(void)
{
    return cra_monotonic();
}

unsigned long cra_tick_ms(void)
{
    return (unsigned long)(cra_monotonic() * 1000);
}

unsigned long long cra_tick_us(void)
{
    return (unsigned long long)(cra_monotonic() * 1000000);
}

void cra_print_tm(const struct tm *const _tm)
{
    printf("struct tm {\n");
    printf("  tm_year  = %d,\n", _tm->tm_year + 1900);
    printf("  tm_mon   = %d,\n", _tm->tm_mon + 1);
    printf("  tm_mday  = %d,\n", _tm->tm_mday);
    printf("  tm_hour  = %d,\n", _tm->tm_hour);
    printf("  tm_min   = %d,\n", _tm->tm_min);
    printf("  tm_sec   = %d,\n", _tm->tm_sec);
    printf("  tm_wday  = %d,\n", _tm->tm_wday);
    printf("  tm_yday  = %d,\n", _tm->tm_yday);
    printf("  tm_isdst = %d,\n", _tm->tm_isdst);
    printf("}\n");
}

void cra_datetime_now_utc(CraDateTime *dt)
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
    dt->ms = st.wMilliseconds;
#else
    struct tm _tm;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    cra_gmtime(ts.tv_sec, &_tm);
    dt->year = _tm.tm_year + 1900;
    dt->mon = _tm.tm_mon + 1;
    dt->day = _tm.tm_mday;
    dt->hour = _tm.tm_hour;
    dt->min = _tm.tm_min;
    dt->sec = _tm.tm_sec;
    dt->ms = ts.tv_nsec / 1000000;
#endif
}

void cra_datetime_now_localtime(CraDateTime *dt)
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
    dt->ms = st.wMilliseconds;
#else
    struct tm _tm;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    cra_gmtime(tv.tv_sec, &_tm);
    dt->year = _tm.tm_year + 1900;
    dt->mon = _tm.tm_mon + 1;
    dt->day = _tm.tm_mday;
    dt->hour = _tm.tm_hour;
    dt->min = _tm.tm_min;
    dt->sec = _tm.tm_sec;
    dt->ms = tv.tv_usec / 1000;
#endif
}

void cra_print_datetime(const CraDateTime *const dt)
{
    printf("CraDateTime {\n");
    printf("  year  = %d,\n", dt->year);
    printf("  mon   = %d,\n", dt->mon);
    printf("  mday  = %d,\n", dt->day);
    printf("  hour  = %d,\n", dt->hour);
    printf("  min   = %d,\n", dt->min);
    printf("  sec   = %d,\n", dt->sec);
    printf("  ms    = %d,\n", dt->ms);
    printf("}\n");
}
