/**
 * @file cra_log.h
 * @author Cracal
 * @brief log
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_LOG_H__
#define __CRA_LOG_H__
#include "cra_defs.h"

// default: 260 bytes
#define CRA_LOG_PATH_MAX 260

// default: 2MB
#define CRA_LOG_BUFFER_SIZE (2 * 1024 * 1024)

// default : 8KB
#define CRA_LOG_MSG_MAX 8192

// default: 3s
#define CRA_LOG_WRITE_INTERVAL (3 * 1000)

// default: CRA_LOG_PATH_MAX + 100
#define CRA_LOG_FILENAME_MAX (CRA_LOG_PATH_MAX + 100)

typedef enum _CraLogLevel_e
{
    CRA_LOG_LEVEL_TRACE,
    CRA_LOG_LEVEL_DEBUG,
    CRA_LOG_LEVEL_INFO,
    CRA_LOG_LEVEL_WARN,
    CRA_LOG_LEVEL_ERROR,
    CRA_LOG_LEVEL_FATAL,
    CRA_LOG_LEVEL_NO_LOG,
} CraLogLevel_e;

CRA_API bool cra_log_startup(CraLogLevel_e level, bool run_write_thread, bool with_localtime);
CRA_API void cra_log_cleanup(void);

CRA_API void cra_log_set_level(CraLogLevel_e level);
CRA_API CraLogLevel_e cra_log_get_level(void);

CRA_API void cra_log_output_to_fp(FILE *fp);
CRA_API bool cra_log_output_to_file(const char *path, const char *main_name, size_t size_per_file);

#ifdef CRA_LOG_FILE_LINE
CRA_API void __cra_log_message(const char *logname, CraLogLevel_e level, const char *file, int line, const char *fmt, ...);
#define cra_log_message(_logname, _level, _fmt, ...) \
    __cra_log_message(_logname, _level, __FILE__, __LINE__, _fmt, ##__VA_ARGS__)
#else
CRA_API void __cra_log_message(const char *logname, CraLogLevel_e level, const char *fmt, ...);
#define cra_log_message(_logname, _level, _fmt, ...) \
    __cra_log_message(_logname, _level, _fmt, ##__VA_ARGS__)
#endif

#define cra_log_trace_with_logname(_logname, _fmt, ...) \
    cra_log_message(_logname, CRA_LOG_LEVEL_TRACE, _fmt, ##__VA_ARGS__)
#define cra_log_debug_with_logname(_logname, _fmt, ...) \
    cra_log_message(_logname, CRA_LOG_LEVEL_DEBUG, _fmt, ##__VA_ARGS__)
#define cra_log_info_with_logname(_logname, _fmt, ...) \
    cra_log_message(_logname, CRA_LOG_LEVEL_INFO, _fmt, ##__VA_ARGS__)
#define cra_log_warn_with_logname(_logname, _fmt, ...) \
    cra_log_message(_logname, CRA_LOG_LEVEL_WARN, _fmt, ##__VA_ARGS__)
#define cra_log_error_with_logname(_logname, _fmt, ...) \
    cra_log_message(_logname, CRA_LOG_LEVEL_ERROR, _fmt, ##__VA_ARGS__)
#define cra_log_fatal_with_logname(_logname, _fmt, ...) \
    cra_log_message(_logname, CRA_LOG_LEVEL_FATAL, _fmt, ##__VA_ARGS__)

#define cra_log_trace(_fmt, ...) \
    cra_log_trace_with_logname(CRA_LOG_NAME, _fmt, ##__VA_ARGS__)
#define cra_log_debug(_fmt, ...) \
    cra_log_debug_with_logname(CRA_LOG_NAME, _fmt, ##__VA_ARGS__)
#define cra_log_info(_fmt, ...) \
    cra_log_info_with_logname(CRA_LOG_NAME, _fmt, ##__VA_ARGS__)
#define cra_log_warn(_fmt, ...) \
    cra_log_warn_with_logname(CRA_LOG_NAME, _fmt, ##__VA_ARGS__)
#define cra_log_error(_fmt, ...) \
    cra_log_error_with_logname(CRA_LOG_NAME, _fmt, ##__VA_ARGS__)
#define cra_log_fatal(_fmt, ...) \
    cra_log_fatal_with_logname(CRA_LOG_NAME, _fmt, ##__VA_ARGS__)

#endif