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

// default : 8KB
#define CRA_LOG_MSG_MAX 8192

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

typedef struct _CraLogTo_i CraLogTo_i;
struct _CraLogTo_i
{
    void (*destroy)(CraLogTo_i **self);
    void (*append)(CraLogTo_i **self, const char *msg, int len, CraLogLevel_e level);
};

#define CRA_LOGTO_HEAD    const CraLogTo_i **i
#define CRA_LOGTO_I(_obj) (*(const CraLogTo_i **)(_obj))

#if 1 // Logger

CRA_API CraLogLevel_e
cra_log_get_level(void);

CRA_API void
cra_log_set_level(CraLogLevel_e level);

CRA_API void
cra_log_startup(CraLogLevel_e level, bool with_localtime, CraLogTo_i **logto);

CRA_API void
cra_log_cleanup(void);

CRA_API void
__cra_log_message(const char   *logname,
                  CraLogLevel_e level,
#ifdef CRA_LOG_FILE_LINE
                  const char *file,
                  int         line,
#endif
                  const char *fmt,
                  ...);
#if CRA_LOG_FILE_LINE
#define cra_log_message_with_logname(_logname, _level, _fmt, ...)                \
    __cra_log_message(_logname, _level, __FILE__, __LINE__, _fmt, ##__VA_ARGS__)
#define cra_log_message(_level, _fmt, ...)                                           \
    __cra_log_message(CRA_LOG_NAME, _level, __FILE__, __LINE__, _fmt, ##__VA_ARGS__)
#else
#define cra_log_message_with_logname(_logname, _level, _fmt, ...) \
    __cra_log_message(_logname, _level, _fmt, ##__VA_ARGS__)
#define cra_log_message(_logname, _level, _fmt, ...) __cra_log_message(CRA_LOG_NAME, _level, _fmt, ##__VA_ARGS__)
#endif

#define cra_log_trace_with_logname(_logname, _fmt, ...)                              \
    cra_log_message_with_logname(_logname, CRA_LOG_LEVEL_TRACE, _fmt, ##__VA_ARGS__)
#define cra_log_debug_with_logname(_logname, _fmt, ...)                              \
    cra_log_message_with_logname(_logname, CRA_LOG_LEVEL_DEBUG, _fmt, ##__VA_ARGS__)
#define cra_log_info_with_logname(_logname, _fmt, ...)                              \
    cra_log_message_with_logname(_logname, CRA_LOG_LEVEL_INFO, _fmt, ##__VA_ARGS__)
#define cra_log_warn_with_logname(_logname, _fmt, ...)                              \
    cra_log_message_with_logname(_logname, CRA_LOG_LEVEL_WARN, _fmt, ##__VA_ARGS__)
#define cra_log_error_with_logname(_logname, _fmt, ...)                              \
    cra_log_message_with_logname(_logname, CRA_LOG_LEVEL_ERROR, _fmt, ##__VA_ARGS__)
#define cra_log_fatal_with_logname(_logname, _fmt, ...)                              \
    cra_log_message_with_logname(_logname, CRA_LOG_LEVEL_FATAL, _fmt, ##__VA_ARGS__)

#define cra_log_trace(_fmt, ...) cra_log_message(CRA_LOG_LEVEL_TRACE, _fmt, ##__VA_ARGS__)
#define cra_log_debug(_fmt, ...) cra_log_message(CRA_LOG_LEVEL_DEBUG, _fmt, ##__VA_ARGS__)
#define cra_log_info(_fmt, ...)  cra_log_message(CRA_LOG_LEVEL_INFO, _fmt, ##__VA_ARGS__)
#define cra_log_warn(_fmt, ...)  cra_log_message(CRA_LOG_LEVEL_WARN, _fmt, ##__VA_ARGS__)
#define cra_log_error(_fmt, ...) cra_log_message(CRA_LOG_LEVEL_ERROR, _fmt, ##__VA_ARGS__)
#define cra_log_fatal(_fmt, ...) cra_log_message(CRA_LOG_LEVEL_FATAL, _fmt, ##__VA_ARGS__)

#endif // end Logger

#if 1 // log async

// default: 3s
#define CRA_LOG_WRITE_INTERVAL (3 * 1000)
// default: 2MB
#define CRA_LOG_BUFFER_SIZE    (2 * 1024 * 1024)

typedef struct _CraLogAsync CraLogAsync;

typedef void (*cra_log_async_write2logto_fn)(CraLogTo_i **logto, const char *content, size_t len);

typedef struct
{
    int         length;
    const char *buffer;
} CraLogAsyncBuffer;

CRA_API CraLogAsync *
cra_log_async_create(cra_log_async_write2logto_fn on_write, CraLogTo_i **logto);

CRA_API void
cra_log_async_destory(CraLogAsync **pobj);

CRA_API void
cra_log_async_write(CraLogAsync *obj, const char *msg, int len);

// buffers = [{len1, buf1}, {len2, buf2}, ..., {0}]
CRA_API void
cra_log_async_write_array(CraLogAsync *obj, const CraLogAsyncBuffer buffers[]);

#endif // end log async

#if 1 // log to stdout

typedef struct _CraLogToStdoutSync CraLogToStdoutSync;

CRA_API CraLogToStdoutSync *
cra_logto_stdout_create(bool async);

#endif // end log to stdout

#if 1 // log to file

// default: 260 bytes
#define CRA_LOG_FILENAME_MAX MAX_PATH

typedef struct _CraLogToFile CraLogToFile;

CRA_API CraLogToFile *
cra_logto_file_create(bool async, bool with_localtime, const char *path, const char *name, size_t max_file_size);

#endif // end log to file

#endif