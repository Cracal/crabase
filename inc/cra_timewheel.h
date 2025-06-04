/**
 * @file cra_timewheel.h
 * @author Cracal
 * @brief time wheel
 * @version 0.1
 * @date 2024-12-14
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_TIMEWHEEL_H__
#define __CRA_TIMEWHEEL_H__
#include "cra_defs.h"
#include "cra_refcnt.h"

typedef struct _CraTimerRc CraTimerRc;
typedef struct _CraTimewheel CraTimewheel;

typedef void (*cra_timer_fn)(CraTimerRc *);

#if 1 // timer

CRA_API CraTimerRc *cra_timer_new(unsigned int repeat, unsigned int timeout_ms, void *arg, cra_timer_fn on_timeout, cra_timer_fn before_destroy);

CRA_API bool cra_timer_is_active(CraTimerRc *timer);
CRA_API void cra_timer_cancel(CraTimerRc *timer);
#define cra_timer_clear_active cra_timer_cancel

CRA_API unsigned int cra_timer_get_repeat(CraTimerRc *timer);
CRA_API void cra_timer_set_repeat(CraTimerRc *timer, unsigned int repeat);

CRA_API unsigned int cra_timer_get_timeout(CraTimerRc *timer);
CRA_API void cra_timer_set_timeout(CraTimerRc *timer, unsigned int timeout_ms);

CRA_API void *cra_timer_get_arg(CraTimerRc *timer);
CRA_API void cra_timer_set_arg(CraTimerRc *timer, void *arg);

#endif // end timer

CRA_API CraTimewheel *cra_timewheel_create(unsigned int tick_ms, unsigned int wheel_size);
CRA_API void cra_timewheel_destroy(CraTimewheel *wheel);

CRA_API bool cra_timewheel_add(CraTimewheel *wheel, CraTimerRc *timer);

CRA_API void cra_timewheel_tick(CraTimewheel *wheel);

CRA_API unsigned int cra_timewheel_get_tick_time(CraTimewheel *wheel);
CRA_API unsigned int cra_timewheel_get_wheel_size(CraTimewheel *wheel);

#endif