/**
 * @file test-log.c
 * @author Cracal
 * @brief test log
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "cra_log.h"
#include "cra_malloc.h"
#include "cra_time.h"
#include "threads/cra_thrdpool.h"

void
test_log(void)
{
#define CRA_LOG_NAME "TEST-LOG"
    cra_log_startup(CRA_LOG_LEVEL_TRACE, true, (CraLogTo_i **)cra_logto_stdout_create(false));
    cra_log_trace("hello trace");
    cra_log_debug("hello debug");
    cra_log_info("hello info");
    cra_log_warn("hello warn");
    cra_log_error("hello error");
    cra_log_fatal("hello fatal");
    cra_log_cleanup();

    cra_log_fatal("noooooooo!");

    cra_log_startup(CRA_LOG_LEVEL_INFO, false, (CraLogTo_i **)cra_logto_stdout_create(true));
    cra_log_trace("no trace");
    cra_log_debug("no debug");
    cra_log_info("output info");
    cra_log_warn("output warn");
    cra_log_error("output error");
    cra_log_fatal("output fatal");
    cra_log_cleanup();

    cra_log_fatal("noooooooo!!!");

#undef CRA_LOG_NAME
}

void
test_log_out_of_msg_buf(void)
{
#define CRA_LOG_NAME "TEST-OUT-OF-MSG"
    cra_log_startup(CRA_LOG_LEVEL_WARN, false, (CraLogTo_i **)cra_logto_stdout_create(true));

#ifdef CRA_LOG_FILE_LINE
#define LEN CRA_LOG_MSG_MAX - 53 - 65 - 1
#else
#define LEN CRA_LOG_MSG_MAX - 53 - 1
#endif
    char msg[LEN + 2];
    memset(msg, 'A', sizeof(msg));

    // > MSG_MAX
    msg[LEN + 2 - 1] = '\0';
    cra_log_error("%s", msg);

    // > MSG_MAX
    msg[LEN + 2 - 2] = '\0';
    cra_log_error("%s", msg);

    // == MSG_MAX
    msg[LEN + 2 - 3] = '\0';
    cra_log_error("%s", msg);

    cra_log_cleanup();
#undef CRA_LOG_NAME
}

static void
write_log(const CraThrdPoolArgs0 *arg)
{
    for (int i = 0; i < 10000; i++)
        cra_log_info_with_logname("WRITE-LOG-THREAD", "Hello world %d from %lu.", i, (unsigned long)arg->tid);
}

void
test_log_multithreads_sync(void)
{
    cra_log_startup(
      CRA_LOG_LEVEL_DEBUG,
      false,
      (CraLogTo_i **)cra_logto_file_create(false, false, "log/test", "multi_threads_sync", 10 * 1024 * 1024));

    CraThrdPool pool;
    cra_thrdpool_init(&pool, 4, 4);

    unsigned long start_ms = cra_tick_ms();
    for (int i = 0; i < 4; i++)
        cra_thrdpool_add_task0(&pool, write_log);

    cra_thrdpool_wait(&pool);
    unsigned long end_ms = cra_tick_ms();

    cra_thrdpool_uninit(&pool);

    printf("test_log_multithreads_sync() takes %lums.\n", end_ms - start_ms);

    cra_log_cleanup();
}

void
test_log_multithreads_async(void)
{
    cra_log_startup(
      CRA_LOG_LEVEL_DEBUG,
      true,
      (CraLogTo_i **)cra_logto_file_create(true, true, "log/test", "multi_threads_async", 10 * 1024 * 1024));

    CraThrdPool pool;
    cra_thrdpool_init(&pool, 4, 4);

    unsigned long start_ms = cra_tick_ms();
    for (int i = 0; i < 4; i++)
        cra_thrdpool_add_task0(&pool, write_log);

    cra_thrdpool_wait(&pool);
    unsigned long end_ms = cra_tick_ms();

    cra_thrdpool_uninit(&pool);

    printf("test_log_multithreads_async() takes %lums.\n", end_ms - start_ms);

    cra_log_cleanup();
}

void
test_log_syn(void)
{
    cra_log_startup(CRA_LOG_LEVEL_DEBUG, false, (CraLogTo_i **)cra_logto_stdout_create(false));

    unsigned long start, end;

    start = cra_tick_ms();
    for (int i = 0; i < 10000; i++)
        cra_log_message_with_logname("LOG-SYN", CRA_LOG_LEVEL_INFO, "hello world %d !!", i);
    end = cra_tick_ms();

    cra_log_cleanup();

    printf("test_log_syn() takes %lums\n", end - start);
}

void
test_log_asyn(void)
{
    cra_log_startup(CRA_LOG_LEVEL_DEBUG, true, (CraLogTo_i **)cra_logto_stdout_create(true));

    unsigned long start, end;

    cra_log_info_with_logname("LOG-ASYN", "LOGGGGGGG~~~~");
    cra_sleep(4);

    start = cra_tick_ms();
    for (int i = 0; i < 10000; i++)
        cra_log_message_with_logname("LOG-ASYN", CRA_LOG_LEVEL_INFO, "hello world %d !!", i);
    end = cra_tick_ms();

    cra_log_cleanup();

    printf("test_log_asyn() takes %lums\n", end - start);
}

void
test_log_to_file_sync(void)
{
#define CRA_LOG_NAME "LOG-FILE"
    cra_log_startup(
      CRA_LOG_LEVEL_TRACE,
      false,
      (CraLogTo_i **)cra_logto_file_create(false, false, "./log/test/", "test-log-sync", 2 * 1024 * 1024));

    unsigned long start, end;
    int           n = 1000000;

    start = cra_tick_ms();
    for (int i = 0; i < n; i++)
        cra_log_info("hello log %d!!!", i + 1);
    end = cra_tick_ms();

    cra_log_cleanup();

    printf("test_log_to_file_sync() takes %lums\t%.2fmsg/s\n", end - start, n / ((end - start) / 1000.0f));
#undef CRA_LOG_NAME
}

void
test_log_to_file_async(void)
{
#define CRA_LOG_NAME "LOG-FILE"
    cra_log_startup(CRA_LOG_LEVEL_TRACE,
                    true,
                    (CraLogTo_i **)cra_logto_file_create(true, true, "./log/test/", "test-log-async", 2 * 1024 * 1024));

    unsigned long start, end;
    int           n = 1000000;

    start = cra_tick_ms();
    for (int i = 0; i < n; i++)
        cra_log_info("hello log %d!!!", i + 1);
    end = cra_tick_ms();

    cra_log_cleanup();

    printf("test_log_to_file_async() takes %lums\t%.2fmsg/s\n", end - start, n / ((end - start) / 1000.0f));
#undef CRA_LOG_NAME
}

void
test_log_time(void)
{
#define CRA_LOG_NAME "TEST-LOG"
    cra_log_startup(
      CRA_LOG_LEVEL_TRACE,
      true,
      (CraLogTo_i **)cra_logto_file_create(false, true, "log/test", "test-log-localtime", 2 * 1024 * 1024));

    cra_log_trace("hello trace");
    cra_log_debug("hello debug");
    cra_log_info("hello info");
    cra_log_warn("hello warn");
    cra_log_error("hello error");
    cra_log_fatal("hello fatal");

    cra_log_cleanup();

    cra_msleep(500);

    cra_log_startup(CRA_LOG_LEVEL_TRACE,
                    false,
                    (CraLogTo_i **)cra_logto_file_create(false, true, "log/test/", "test-log-utc", 2 * 1024 * 1024));

    cra_log_trace("hello trace");
    cra_log_debug("hello debug");
    cra_log_info("hello info");
    cra_log_warn("hello warn");
    cra_log_error("hello error");
    cra_log_fatal("hello fatal");

    cra_log_cleanup();
#undef CRA_LOG_NAME
}

int
main(void)
{
    test_log();
    cra_sleep(1);
    test_log_out_of_msg_buf();
    cra_sleep(1);
    test_log_multithreads_sync();
    test_log_multithreads_async();
    cra_sleep(2);
    test_log_syn();
    cra_sleep(1);
    test_log_asyn();
    cra_sleep(1);
    test_log_to_file_sync();
    test_log_to_file_async();
    test_log_time();

    cra_memory_leak_report();
    return 0;
}
