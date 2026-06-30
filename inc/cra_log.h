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

typedef enum
{
    CRA_LOG_LV_TRACE = 0,
    CRA_LOG_LV_DEBUG,
    CRA_LOG_LV_INFO,
    CRA_LOG_LV_WARN,
    CRA_LOG_LV_ERROR,
    CRA_LOG_LV_FATAL,
    CRA_LOG_LV_COUNT,
#define CRA_LOG_LV_NO_LOG CRA_LOG_LV_COUNT
} CraLogLv_e;

typedef struct CraLogger CraLogger;

#define CRA_LOG_BUF_SIZE     (4 * 1024 * 1024) // 4MB
#define CRA_LOG_BUF_MAX_CNT  32
#define CRA_LOG_BUF_INIT_CNT 2

#define CRA_LOG_FILENAME_MAX     1024
#define CRA_LOG_OUTPUT_INTERVAL  3000                // 3s
#define CRA_LOG_FLUSH_INTERVAL   (10 * 60 * 1000)    // 10 min
#define CRA_LOG_DEFAULT_DIR      "./log/"            // default log dir
#define CRA_LOG_DEFAULT_FILE_MAX (100 * 1024 * 1024) // 100MB

#define CRA_LOG_NAME_MAX 32
#define CRA_LOG_LINE_MAX 2048

static inline const char *
cra_log_level_to_str(CraLogLv_e lv)
{
    switch (lv)
    {
        case CRA_LOG_LV_TRACE:
            return "TRACE";
        case CRA_LOG_LV_DEBUG:
            return "DEBUG";
        case CRA_LOG_LV_INFO:
            return "INFO ";
        case CRA_LOG_LV_WARN:
            return "WARN ";
        case CRA_LOG_LV_ERROR:
            return "ERROR";
        case CRA_LOG_LV_FATAL:
            return "FATAL";
        default:
            return "INVAL";
    }
}

#define cra_log_get_level(_logger)      (*(const CraLogLv_e *)(_logger))
#define cra_log_set_level(_logger, _lv) ((void)(*(CraLogLv_e *)(_logger) = (_lv)))

CRA_API const char *
cra_log_get_name(CraLogger *logger);

CRA_API void
cra_log_config(CraLogger *logger, unsigned int max_file_size, const char *log_dir);

// use_zulu:
//     true:  "yyyy-MM-dd HH:mm:ss.SSSZ level tid  msg[ -- file:line]\n"
//     false: "yyyy-MM-dd HH:mm:ss.SSS+/-hh:00 level tid  msg[ -- file:line]\n"
//
// output_to_file: Whether to output to file. (to console: sync; to file: async)
CRA_API CraLogger *
cra_log_open(const char *name, CraLogLv_e lv, bool use_zulu, bool output_to_file);

CRA_API void
cra_log_close(CraLogger *logger);

CRA_API void
cra_log_msg(CraLogger *logger, CraLogLv_e lv, const char *fmt, ...);
#ifdef CRA_LOG_FILE_LINE
#define cra_log_msg(_logger, _lv, _fmt, ...)                                                      \
    (void)((!!(_logger) && cra_log_get_level(_logger) <= (_lv)) &&                                \
           (cra_log_msg(_logger, _lv, _fmt " -- %s:%d\n", ##__VA_ARGS__, __FILE__, __LINE__), 0))
#else
#define cra_log_msg(_logger, _lv, _fmt, ...)                         \
    (void)((!!(_logger) && cra_log_get_level(_logger) <= (_lv)) &&   \
           (cra_log_msg(_logger, _lv, _fmt "\n", ##__VA_ARGS__), 0))
#endif

#define cra_log_trace(_logger, _fmt, ...) cra_log_msg(_logger, CRA_LOG_LV_TRACE, _fmt, ##__VA_ARGS__)
#define cra_log_debug(_logger, _fmt, ...) cra_log_msg(_logger, CRA_LOG_LV_DEBUG, _fmt, ##__VA_ARGS__)
#define cra_log_info(_logger, _fmt, ...)  cra_log_msg(_logger, CRA_LOG_LV_INFO, _fmt, ##__VA_ARGS__)
#define cra_log_warn(_logger, _fmt, ...)  cra_log_msg(_logger, CRA_LOG_LV_WARN, _fmt, ##__VA_ARGS__)
#define cra_log_error(_logger, _fmt, ...) cra_log_msg(_logger, CRA_LOG_LV_ERROR, _fmt, ##__VA_ARGS__)
#define cra_log_fatal(_logger, _fmt, ...) cra_log_msg(_logger, CRA_LOG_LV_FATAL, _fmt, ##__VA_ARGS__)

#endif