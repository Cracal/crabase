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
#include "crabase/threads/cra_thread.h"

#ifndef CRA_OS_WIN
#include <sys/syscall.h>
#endif

cra_tid_t
cra_get_current_tid(void)
{
    static cra_thrd_local cra_tid_t t_tid = 0;
    if (t_tid == 0)
    {
#ifdef CRA_OS_WIN
        t_tid = (cra_tid_t)GetCurrentThreadId();
#else
        t_tid = (cra_tid_t)syscall(SYS_gettid);
#endif
    }
    return t_tid;
}

const char *
cra_get_current_tid_str(int *retlen)
{
    static cra_thrd_local int  t_tid_len = 0;
    static cra_thrd_local char t_tid_str[32] = { 0 };
    if (t_tid_len == 0)
        t_tid_len = snprintf(t_tid_str, sizeof(t_tid_str), "%-6lu ", cra_get_current_tid());
    if (retlen)
        *retlen = t_tid_len;
    return t_tid_str;
}
