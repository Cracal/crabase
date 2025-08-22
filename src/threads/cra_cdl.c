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
#include "threads/cra_cdl.h"

void
cra_cdl_init(CraCDL *cdl, int count)
{
    cdl->count = count;
    cra_cond_init(&cdl->cond);
    cra_mutex_init(&cdl->mutex);
}

void
cra_cdl_uninit(CraCDL *cdl)
{
    cra_cond_destroy(&cdl->cond);
    cra_mutex_destroy(&cdl->mutex);
}
