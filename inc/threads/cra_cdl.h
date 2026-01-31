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

typedef struct _CraCDL
{
    volatile int count;
    cra_cond_t   cond;
    cra_mutex_t  mutex;
} CraCDL;

CRA_API void
cra_cdl_init(CraCDL *cdl, int count);

CRA_API void
cra_cdl_uninit(CraCDL *cdl);

static inline void
cra_cdl_reset(CraCDL *cdl, int count)
{
    cra_mutex_lock(&cdl->mutex);
    cdl->count = count;
    cra_mutex_unlock(&cdl->mutex);
}

static inline int
cra_cdl_get_count(CraCDL *cdl)
{
    int count;
    cra_mutex_lock(&cdl->mutex);
    count = cdl->count;
    cra_mutex_unlock(&cdl->mutex);
    return count;
}

static inline void
cra_cdl_count_down(CraCDL *cdl)
{
    cra_mutex_lock(&cdl->mutex);
    if (--cdl->count <= 0)
        cra_cond_broadcast(&cdl->cond);
    cra_mutex_unlock(&cdl->mutex);
}

static inline void
cra_cdl_wait(CraCDL *cdl)
{
    cra_mutex_lock(&cdl->mutex);
    while (cdl->count > 0)
        cra_cond_wait(&cdl->cond, &cdl->mutex);
    cra_mutex_unlock(&cdl->mutex);
}

static inline bool
cra_cdl_wait_timeout(CraCDL *cdl, int timeout_ms)
{
    bool ret;
    cra_mutex_lock(&cdl->mutex);
    ret = cra_cond_wait_timeout(&cdl->cond, &cdl->mutex, timeout_ms);
    cra_mutex_unlock(&cdl->mutex);
    return ret;
}

#endif