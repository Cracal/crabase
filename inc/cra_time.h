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
#include "cra_defs.h"
#include <time.h>

CRA_API uint64_t
cra_monotonic_ns(void);

CRA_API uint64_t
cra_monotonic_us(void);

CRA_API uint64_t
cra_monotonic_ms(void);

CRA_API double
cra_monotonic_sec(void);

// ======================================

static inline void
cra_gmtime(time_t secs, struct tm *t)
{
#ifdef CRA_OS_WIN
    gmtime_s(t, &secs);
#else
    gmtime_r(&secs, t);
#endif
}

static inline void
cra_localtime(time_t secs, struct tm *t)
{
#ifdef CRA_OS_WIN
    localtime_s(t, &secs);
#else
    localtime_r(&secs, t);
#endif
}

CRA_API void
cra_print_tm(const struct tm *const t);

typedef struct CraDateTime
{
    int year;
    int mon;
    int day;
    int hour;
    int min;
    int sec;
    int usec;
} CraDateTime;

static inline void
cra_datetime_epoch_to_sec_and_ns(uint64_t utc_epoch, time_t *sec, time_t *ns)
{
    if (sec)
        *sec = (time_t)(utc_epoch / 1000000000);
    if (ns)
        *ns = (time_t)(utc_epoch % 1000000000);
}

// retrun Unix epoch in nanoseconds
CRA_API uint64_t
cra_datetime_epoch_ns(void);

CRA_API void
cra_datetime_from_epoch(CraDateTime *dt, uint64_t utc_epoch, bool tz_utc);

static inline void
cra_datetime_now(CraDateTime *dt, bool tz_utc)
{
    uint64_t epoch = cra_datetime_epoch_ns();
    cra_datetime_from_epoch(dt, epoch, tz_utc);
}

CRA_API void
cra_print_datetime(const CraDateTime *const dt);

#endif