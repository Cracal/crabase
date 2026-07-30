/**
 * @file cra_cdl.h
 * @author Cracal
 * @brief count down latch
 * @version 0.1
 * @date 2024-09-25
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __CRA_CDL_H__
#define __CRA_CDL_H__
#include "cra_lock.h"

typedef struct CraCDL
{
    volatile int count;
    cra_mutex_t  mutex;
    cra_cond_t   condi;
} CraCDL;

static inline void
cra_cdl_init(CraCDL *cdl, int count)
{
    cdl->count = count;
    cra_cond_init(&cdl->condi);
    cra_mutex_init(&cdl->mutex);
}

static inline void
cra_cdl_uninit(CraCDL *cdl)
{
    cra_cond_destroy(&cdl->condi);
    cra_mutex_destroy(&cdl->mutex);
}

static inline void
cra_cdl_count_down(CraCDL *cdl)
{
    cra_mutex_lock(&cdl->mutex);
    if (--cdl->count == 0)
        cra_cond_broadcast(&cdl->condi);
    cra_mutex_unlock(&cdl->mutex);
}

static inline void
cra_cdl_wait(CraCDL *cdl)
{
    cra_mutex_lock(&cdl->mutex);
    while (cdl->count > 0)
        cra_cond_wait(&cdl->condi, &cdl->mutex);
    cra_mutex_unlock(&cdl->mutex);
}

#endif