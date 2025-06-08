/**
 * @file cra_time.h
 * @author Cracal
 * @brief time
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_TIME_H__
#define __CRA_TIME_H__
#include <time.h>
#include "cra_defs.h"

CRA_API double cra_monotonic_sec(void);

CRA_API unsigned long cra_tick_ms(void);
CRA_API unsigned long long cra_tick_us(void);

static inline void cra_gmtime(time_t secs, struct tm *_tm)
{
#ifdef CRA_OS_WIN
    gmtime_s(_tm, &secs);
#else
    gmtime_r(&secs, _tm);
#endif
}

static inline void cra_localtime(time_t secs, struct tm *_tm)
{
#ifdef CRA_OS_WIN
    localtime_s(_tm, &secs);
#else
    localtime_r(&secs, _tm);
#endif
}

CRA_API void cra_print_tm(const struct tm *const _tm);

typedef struct _CraDateTime
{
    int year;
    int mon;
    int day;
    int hour;
    int min;
    int sec;
    int ms;
} CraDateTime;

CRA_API void cra_datetime_now_utc(CraDateTime *dt);

CRA_API void cra_datetime_now_localtime(CraDateTime *dt);

CRA_API void cra_print_datetime(const CraDateTime *const dt);

#endif