/**
 * @file cra_thread.c
 * @author Cracal
 * @brief Thread
 * @version 0.1
 * @date 2024-09-21
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "threads/cra_thread.h"

#ifndef CRA_OS_WIN
#include <sys/syscall.h>
#endif

cra_tid_t cra_thrd_get_current_tid(void)
{
    static cra_thrd_local cra_tid_t s_tid = 0;
    if (s_tid == 0)
    {
#ifdef CRA_OS_WIN
        s_tid = (cra_tid_t)GetCurrentThreadId();
#else
        s_tid = (cra_tid_t)syscall(SYS_gettid);
#endif
    }
    return s_tid;
}
