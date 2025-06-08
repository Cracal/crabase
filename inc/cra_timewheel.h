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

typedef struct _CraTimewheel CraTimewheel;
typedef struct _CraTimer_base CraTimer_base;
typedef void (*cra_timer_fn)(CraTimer_base *);

#if 1 // timer

struct _CraTimer_base
{
    bool active;
    unsigned int repeat;
    unsigned int timeout_ms;
    cra_timer_fn on_timeout;        // timeout && active == true
    cra_timer_fn on_timeout_cancel; // timeout && (active == false || repeat == 0)
};

CRA_API void cra_timer_base_init(CraTimer_base *base, unsigned int repeat, unsigned int timeout_ms, cra_timer_fn on_timeout, cra_timer_fn on_timeout_cancel);

static inline bool cra_timer_base_is_active(CraTimer_base *base) { return base->active; }
static inline void cra_timer_base_cancel(CraTimer_base *base) { base->active = false; }
#define cra_timer_base_clear_active cra_timer_base_cancel

#endif // end timer

typedef struct _CraLList CraLList;

struct _CraTimewheel
{
    unsigned int tick_ms;
    unsigned int wheel_size;
    unsigned int current;
    CraLList **wheel_buckets; // [CraLList<CraTimer_base *>]
    CraLList *freenodelist;   // CraLList<CraLListNode<CraTimer_base *> *>
    CraTimewheel *upper_wheel;
};

CRA_API void cra_timewheel_init(CraTimewheel *wheel, unsigned int tick_ms, unsigned int wheel_size);
CRA_API void cra_timewheel_uninit(CraTimewheel *wheel);

CRA_API bool cra_timewheel_add(CraTimewheel *wheel, CraTimer_base *timer);

CRA_API void cra_timewheel_tick(CraTimewheel *wheel);

static inline unsigned int cra_timewheel_get_tick_time(CraTimewheel *wheel) { return wheel->tick_ms; }
static inline unsigned int cra_timewheel_get_wheel_size(CraTimewheel *wheel) { return wheel->wheel_size; }

#endif