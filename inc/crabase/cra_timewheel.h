/**
 * @file cra_timewheel.h
 * @author Cracal
 * @brief timing wheel
 * @version 0.1
 * @date 2024-12-14
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRABASE_CRA_TIMEWHEEL_H__
#define __CRABASE_CRA_TIMEWHEEL_H__
#include "cra_defs.h"

typedef struct CraTimewheel CraTimewheel;
typedef struct CraTimerNode CraTimerNode;

#define CRA_TIMER_BASE_BITS   8
#define CRA_TIMER_BASE_SIZE   (1u << CRA_TIMER_BASE_BITS)
#define CRA_TIMER_UPPER_BITS  6
#define CRA_TIMER_UPPER_SIZE  (1u << CRA_TIMER_UPPER_BITS)
#define CRA_TIMER_UPPER_MAX   4
#define CRA_TIMER_SLOT_NUM    (CRA_TIMER_BASE_SIZE + CRA_TIMER_UPPER_SIZE * CRA_TIMER_UPPER_MAX)
#define CRA_TIMER_BITMAP_SIZE (CRA_TIMER_SLOT_NUM >> 6)

struct CraTimewheel
{
    uint32_t      ticks;
    uint32_t      ntimers;
    uint64_t      last_ms;
    CraTimerNode *upper[CRA_TIMER_UPPER_MAX][CRA_TIMER_UPPER_SIZE];
    CraTimerNode *base[CRA_TIMER_BASE_SIZE];
    CraTimerNode *list; // spare nodes
    uint64_t      bitmap[CRA_TIMER_BITMAP_SIZE];
};

// wheel == NULL -> error occurred
#define CRA_TIMER_FN_DEF(name) void name(CraTimewheel *wheel, int32_t delay, void *user_data)

#ifndef CRA_TIMER_TICK_MS
#define CRA_TIMER_TICK_MS 10
#endif

CRA_API void
cra_timewheel_init(CraTimewheel *wheel);

CRA_API void
cra_timewheel_uninit(CraTimewheel *wheel);

CRA_API void
cra_timewheel_add(CraTimewheel *wheel, int32_t delay_ms, void *user_data, CRA_TIMER_FN_DEF(cb));

CRA_API int32_t
cra_timewheel_update(CraTimewheel *wheel, uint64_t now_ms);

#endif
