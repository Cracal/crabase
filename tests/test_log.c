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
#include "cra_time.h"
#include "cra_malloc.h"
#include "threads/cra_thrdpool.h"

void
test_log(void)
{
    CraLogger *logger1, *logger2;

    logger1 = cra_log_open("TestLog1", CRA_LOG_LV_TRACE, true, false);
    cra_log_trace(logger1, "hello trace");
    cra_log_debug(logger1, "hello debug");
    cra_log_info(logger1, "hello info");
    cra_log_warn(logger1, "hello warn");
    cra_log_error(logger1, "hello error");
    cra_log_fatal(logger1, "hello fatal");

    logger2 = cra_log_open("TestLog2", CRA_LOG_LV_INFO, false, false);
    cra_log_trace(logger2, "no trace");
    cra_log_debug(logger2, "no debug");
    cra_log_info(logger2, "output info");
    cra_log_warn(logger2, "output warn");
    cra_log_error(logger2, "output error");
    cra_log_fatal(logger2, "output fatal");

    cra_log_info(logger2, "logger.level = %s", cra_log_level_to_str(cra_log_get_level(logger2)));
    cra_log_set_level(logger2, CRA_LOG_LV_DEBUG);
    cra_log_debug(logger2, "logger.level = %s", cra_log_level_to_str(cra_log_get_level(logger2)));

    cra_log_warn(logger1, "logger1(%p) warn", logger1);
    cra_log_warn(logger2, "logger2(%p) warn", logger2);

    const char *name = cra_log_get_name(logger1);
    cra_log_info(logger1, "logger1.name = %s", name);
    name = cra_log_get_name(logger2);
    cra_log_info(logger2, "logger2.name = %s", name);

    cra_log_close(logger2);
    cra_log_close(logger1);

    cra_log_fatal(NULL, "noooooooo!");
}

void
test_log_to_file(void)
{
    CraLogger *logger1, *logger2;

    logger1 = cra_log_open("TestLogA", CRA_LOG_LV_INFO, false, true);
    cra_log_config(logger1, 8 * 1024 * 1024, "log/test");

    cra_log_debug(logger1, "hello debug");
    cra_log_info(logger1, "hello info");
    cra_log_warn(logger1, "hello warn");
    cra_log_error(logger1, "hello error");
    cra_log_fatal(logger1, "hello fatal");
    // cra_msleep(500);

    logger2 = cra_log_open("TestLogB", CRA_LOG_LV_INFO, true, true);

    cra_log_debug(logger2, "hello debug");
    cra_log_info(logger2, "hello info");
    cra_log_warn(logger2, "hello warn");
    cra_log_error(logger2, "hello error");
    cra_log_fatal(logger2, "hello fatal");

    cra_log_close(logger1);
    cra_log_close(logger2);
}

void
test_log_out_of_msg_buf(void)
{
#ifdef CRA_LOG_FILE_LINE
#define LEN                                                                                     \
    CRA_LOG_LINE_MAX - 39 - (sizeof(" -- ") - 1) - (sizeof(__FILE__) - 1) - (sizeof(":") - 1) - \
      (__LINE__ >= 1000 ? 4 : (__LINE__ >= 100 ? 3 : (__LINE__ >= 10 ? 2 : 1))) - 1
#else
#define LEN CRA_LOG_LINE_MAX - 39 - 1
#endif

    CraLogger *logger;
    char       msg[LEN + 2];

    memset(msg, 'A', sizeof(msg));

    logger = cra_log_open("TEST-OUT-OF-MSG", CRA_LOG_LV_ERROR, true, false);

    // >> MSG_MAX
    msg[LEN + 2 - 1] = '\0';
    cra_log_error(logger, "%s", msg);

    // > MSG_MAX
    msg[LEN + 2 - 2] = '\0';
    cra_log_error(logger, "%s", msg);

    // == MSG_MAX
    msg[LEN + 2 - 3] = '\0';
    cra_log_error(logger, "%s", msg);

    cra_log_close(logger);
}

static void
write_log(const CraThrdPoolArgs1 *arg)
{
    CraLogger *logger = (CraLogger *)arg->arg1;
    for (int i = 0; i < 10000; i++)
        cra_log_info(logger, "Hello world %d from %lu.", i, arg->tid);
}

#define N 8

void
test_log_multithreads_sync(void)
{
    CraLogger  *log;
    CraThrdPool pool;

    log = cra_log_open("TestMultiThreadSync", CRA_LOG_LV_DEBUG, true, false);

    cra_thrdpool_init(&pool, N, N);

    unsigned long start_ms = cra_tick_ms();

    for (int i = 0; i < N; i++)
        cra_thrdpool_add_task1(&pool, write_log, log);

    cra_thrdpool_wait(&pool);

    cra_log_close(log);

    unsigned long end_ms = cra_tick_ms();

    cra_thrdpool_uninit(&pool);

    printf("test_log_multithreads_sync()  takes %lums.\n", end_ms - start_ms);
}

void
test_log_multithreads_async(void)
{
    CraLogger  *log;
    CraThrdPool pool;

    log = cra_log_open("TestMultiThreadAsync", CRA_LOG_LV_DEBUG, true, true);
    cra_log_config(log, 4 * 1024 * 1024, "log/async/");

    cra_thrdpool_init(&pool, N, N);

    unsigned long start_ms = cra_tick_ms();

    for (int i = 0; i < N; i++)
        cra_thrdpool_add_task1(&pool, write_log, log);

    cra_thrdpool_wait(&pool);

    cra_log_close(log);

    unsigned long end_ms = cra_tick_ms();

    cra_thrdpool_uninit(&pool);

    printf("test_log_multithreads_async() takes %lums.\n", end_ms - start_ms);
}

#undef N

void
test_async(void)
{
    CraLogger    *logger;
    unsigned long start, end;
    int           n = 3000000;

    logger = cra_log_open("TestAsync", CRA_LOG_LV_TRACE, false, true);
    cra_log_config(logger, 10 * 1024 * 1024, "log/test_async");

    start = cra_tick_ms();

    for (int i = 0; i < n; i++)
    {
        cra_log_debug(logger,
                      "This is a test message, it is the %dth message. Some extra info: {logname: %s, tid: %lu}",
                      i + 1,
                      cra_log_get_name(logger),
                      cra_thrd_get_current_tid());
    }

    cra_log_close(logger);

    end = cra_tick_ms();

    printf("test_async() takes %lums\t%.2fmsg/s\n", end - start, n / ((end - start) / 1000.0f));
}

int
main(void)
{
    test_log();
    test_log_to_file();
    test_log_out_of_msg_buf();
    test_log_multithreads_sync();
    test_log_multithreads_async();
    test_async();

    cra_memory_leak_report();
    return 0;
}
