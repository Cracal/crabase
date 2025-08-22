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
#include "cra_assert.h"
#include "cra_defs.h"

typedef struct _CraTimewheel  CraTimewheel;
typedef struct _CraTimer_base CraTimer_base;
typedef void                  (*cra_timer_base_fn)(CraTimer_base *);

#if 1 // timer

#define CRA_TIMER_INFINITE   (-1)
#define CRA_TIMER_REPEAT_MIN 1
#define CRA_TIMER_REPEAT_MAX 1073741823

struct _CraTimer_base
{
    uint8_t           active : 1;
    int32_t           repeat : 31;
    uint32_t          timeout_ms;
    cra_timer_base_fn on_timeout;      // timeout && active == true
    cra_timer_base_fn on_remove_timer; // 当定时器被移出time wheel时调用
};

CRA_API void
cra_timer_base_init(CraTimer_base    *base,
                    int               repeat,
                    uint32_t          timeout_ms,
                    cra_timer_base_fn on_timeout,
                    cra_timer_base_fn on_remove_timer);

static inline bool
cra_timer_base_is_active(CraTimer_base *base)
{
    return base->active;
}

static inline void
cra_timer_base_set_active(CraTimer_base *base)
{
    base->active = true;
}

static inline void
cra_timer_base_set_deactive(CraTimer_base *base)
{
    base->active = false;
}

static inline int32_t
cra_timer_base_get_repeat(CraTimer_base *base)
{
    return base->repeat;
}

static inline void
cra_timer_base_set_repeat(CraTimer_base *base, int32_t repeat)
{
    assert((repeat >= CRA_TIMER_REPEAT_MIN && repeat <= CRA_TIMER_REPEAT_MAX) || repeat == CRA_TIMER_INFINITE);
    base->repeat = repeat;
}

static inline uint32_t
cra_timer_base_get_timeout(CraTimer_base *base)
{
    return base->timeout_ms;
}

static inline void
cra_timer_base_set_timeout(CraTimer_base *base, int32_t timeout_ms)
{
    assert(timeout_ms > 0);
    base->timeout_ms = timeout_ms;
}

static inline void
cra_timer_base_cancel(CraTimer_base *base)
{
    base->active = false;
}

#define cra_timer_base_clear_active cra_timer_base_cancel

#endif // end timer

typedef struct _CraLList CraLList;

struct _CraTimewheel
{
    uint32_t      tick_ms;
    uint32_t      current;
    uint32_t      wheel_size;
    CraLList    **wheel_buckets; // [CraLList<CraTimer_base *>]
    CraLList     *freenodelist;  // CraLList<CraLListNode<CraTimer_base *> *>
    CraTimewheel *upper_wheel;
};

CRA_API void
cra_timewheel_init(CraTimewheel *wheel, uint32_t tick_ms, uint32_t wheel_size);

CRA_API void
cra_timewheel_uninit(CraTimewheel *wheel);

CRA_API bool
cra_timewheel_add(CraTimewheel *wheel, CraTimer_base *timer);

CRA_API void
cra_timewheel_tick(CraTimewheel *wheel);

static inline uint32_t
cra_timewheel_get_tick_time(CraTimewheel *wheel)
{
    return wheel->tick_ms;
}

static inline uint32_t
cra_timewheel_get_wheel_size(CraTimewheel *wheel)
{
    return wheel->wheel_size;
}

#endif