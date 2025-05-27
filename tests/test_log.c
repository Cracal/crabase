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

void test_log(void)
{
#define CRA_LOG_NAME "TEST-LOG"
    cra_log_startup(CRA_LOG_LEVEL_TRACE, false, false);
    cra_log_trace("hello trace");
    cra_log_debug("hello debug");
    cra_log_info("hello info");
    cra_log_warn("hello warn");
    cra_log_error("hello error");
    cra_log_fatal("hello fatal");
    cra_log_cleanup();
#undef CRA_LOG_NAME
}

void test_log_syn(void)
{
    cra_log_startup(CRA_LOG_LEVEL_DEBUG, false, false);

    unsigned long start, end;

    start = cra_tick_ms();
    for (int i = 0; i < 10000; i++)
        cra_log_message("LOG-SYN", CRA_LOG_LEVEL_INFO, "hello world %d !!", i);
    end = cra_tick_ms();

    cra_log_cleanup();

    printf("耗时: %lums\n", end - start);
}

void test_log_asyn(void)
{
    cra_log_startup(CRA_LOG_LEVEL_DEBUG, true, false);

    unsigned long start, end;

    cra_log_info_with_logname("LOG-ASYN", "LOGGGGGGG~~~~");
    cra_sleep(4);

    start = cra_tick_ms();
    for (int i = 0; i < 10000; i++)
        cra_log_message("LOG-ASYN", CRA_LOG_LEVEL_INFO, "hello world %d !!", i);
    end = cra_tick_ms();

    cra_log_cleanup();

    printf("耗时: %lums\n", end - start);
}

void test_log_to_file_sync(void)
{
#define CRA_LOG_NAME "LOG-FILE"
    cra_log_startup(CRA_LOG_LEVEL_TRACE, false, false);

    unsigned long start, end;

    cra_log_output_to_file("./log/test", "test-log", 2 * 1024 * 1024);

    start = cra_tick_ms();
    for (int i = 0; i < 1000000; i++)
        cra_log_info("hello log %d!!!", i + 1);
    end = cra_tick_ms();

    cra_log_cleanup();

    printf("耗时: %lums\n", end - start);
#undef CRA_LOG_NAME
}

void test_log_to_file_async(void)
{
#define CRA_LOG_NAME "LOG-FILE"
    cra_log_startup(CRA_LOG_LEVEL_TRACE, true, false);

    unsigned long start, end;

    cra_log_output_to_file("./log/test", "test-log", 2 * 1024 * 1024);

    start = cra_tick_ms();
    for (int i = 0; i < 1000000; i++)
        cra_log_info("hello log %d!!!", i + 1);
    end = cra_tick_ms();

    cra_log_cleanup();

    printf("耗时: %lums\n", end - start);
#undef CRA_LOG_NAME
}

void test_log_time(void)
{
#define CRA_LOG_NAME "TEST-LOG"
    cra_log_startup(CRA_LOG_LEVEL_TRACE, false, true);
    cra_log_output_to_file("log/test/", "test-log", 2 * 1024 * 1024);

    cra_log_trace("hello trace");
    cra_log_debug("hello debug");
    cra_log_info("hello info");
    cra_log_warn("hello warn");
    cra_log_error("hello error");
    cra_log_fatal("hello fatal");

    cra_log_cleanup();

    cra_msleep(500);

    cra_log_startup(CRA_LOG_LEVEL_TRACE, false, false);
    cra_log_output_to_file("log/test/", "test-log", 2 * 1024 * 1024);

    cra_log_trace("hello trace");
    cra_log_debug("hello debug");
    cra_log_info("hello info");
    cra_log_warn("hello warn");
    cra_log_error("hello error");
    cra_log_fatal("hello fatal");

    cra_log_cleanup();
#undef CRA_LOG_NAME
}

int main(void)
{
    test_log();
    cra_sleep(1);
    test_log_syn();
    cra_sleep(1);
    test_log_asyn();
    cra_sleep(1);
    test_log_to_file_sync();
    cra_sleep(1);
    test_log_to_file_async();
    cra_sleep(1);
    test_log_time();

    cra_memory_leak_report(stdout);
    return 0;
}
