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

typedef void (*cra_timer_base_fn)(CraTimer_base *);

#if 1 // timer

#define CRA_TIMER_INFINITE ((1u << 31) - 1)

struct _CraTimer_base
{
    uint32_t          active : 1;
    uint32_t          repeat : 31;
    uint32_t          timeout_ms;
    cra_timer_base_fn on_timeout;      // timeout && active == true
    cra_timer_base_fn on_remove_timer; // 当定时器被移出time wheel时调用
};

CRA_API void
cra_timer_base_init(CraTimer_base    *base,
                    uint32_t          repeat,
                    uint32_t          timeout_ms,
                    cra_timer_base_fn on_timeout,
                    cra_timer_base_fn on_remove_timer);

static inline bool
cra_timer_base_is_active(CraTimer_base *base)
{
    return (bool)base->active;
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

#define cra_timer_base_cancel     cra_timer_base_set_deactive
#define cra_timer_base_cls_active cra_timer_base_set_deactive

static inline uint32_t
cra_timer_base_get_repeat(CraTimer_base *base)
{
    return base->repeat;
}

static inline void
cra_timer_base_set_repeat(CraTimer_base *base, uint32_t repeat)
{
    assert(repeat > 0 && repeat <= CRA_TIMER_INFINITE);
    base->repeat = repeat;
}

static inline uint32_t
cra_timer_base_get_timeout(CraTimer_base *base)
{
    return base->timeout_ms;
}

static inline void
cra_timer_base_set_timeout(CraTimer_base *base, uint32_t timeout_ms)
{
    assert(timeout_ms > 0);
    base->timeout_ms = timeout_ms;
}

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