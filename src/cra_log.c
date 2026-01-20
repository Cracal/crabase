/**
 * @file cra_log.c
 * @author Cracal
 * @brief log
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "cra_log.h"
#include "collections/cra_llist.h"
#include "cra_assert.h"
#include "cra_malloc.h"
#include "cra_time.h"
#include "threads/cra_cdl.h"
#include "threads/cra_locker.h"
#include "threads/cra_thread.h"
#include <stdarg.h>

typedef struct
{
    short         timezoneoffset;
    bool          initialized;
    bool          with_utc;
    CraLogLevel_e level;
    CraLogTo_i  **logto;

} CraLogger;

static CraLogger *s_logger = NULL;

CraLogLevel_e
cra_log_get_level(void)
{
    return s_logger->level;
}

void
cra_log_set_level(CraLogLevel_e level)
{
    s_logger->level = level;
}

void
cra_log_startup(CraLogLevel_e level, bool with_localtime, CraLogTo_i **logto)
{
    if (s_logger)
        return;

    s_logger = cra_alloc(CraLogger);
    s_logger->initialized = true;
    s_logger->level = level;
    s_logger->with_utc = !with_localtime;
    if (with_localtime)
    {
        CraDateTime utc, local;
        cra_datetime_now_utc(&utc);
        cra_datetime_now_localtime(&local);
        s_logger->timezoneoffset = (short)(local.hour - utc.hour);
    }
    else
    {
        s_logger->timezoneoffset = 0;
    }
    s_logger->logto = logto;
}

void
cra_log_cleanup(void)
{
    if (!s_logger)
        return;

    (*s_logger->logto)->destroy(s_logger->logto);

    cra_dealloc(s_logger);
    s_logger = NULL;
}

static inline const char *
cra_level_to_str(CraLogLevel_e level)
{
    switch (level)
    {
        case CRA_LOG_LEVEL_TRACE:
            return "TRACE";
        case CRA_LOG_LEVEL_DEBUG:
            return "DEBUG";
        case CRA_LOG_LEVEL_INFO:
            return "INFO ";
        case CRA_LOG_LEVEL_WARN:
            return "WARN ";
        case CRA_LOG_LEVEL_ERROR:
            return "ERROR";
        case CRA_LOG_LEVEL_FATAL:
            return "FATAL";
        case CRA_LOG_LEVEL_NO_LOG:
        default:
            return "UNKNOWN";
    }
}

void
__cra_log_message(const char   *logname,
                  CraLogLevel_e level,
#ifdef CRA_LOG_FILE_LINE
                  const char *file,
                  int         line,
#endif
                  const char *fmt,
                  ...)
{
    int  len;
    char msg[CRA_LOG_MSG_MAX];

    assert(logname);
    assert(fmt);

    if (!s_logger)
    {
        fprintf(stderr, "Please init Logger(call `cra_log_startup`) first.\n");
        return;
    }

    if (level < s_logger->level)
        return;

    // time
    int         spaces;
    CraDateTime dt;
    if (s_logger->with_utc)
    {
        cra_datetime_now_utc(&dt);
        spaces = dt.ms >= 100 ? 1 : (dt.ms >= 10 ? 2 : 3);
        len = snprintf(msg,
                       sizeof(msg),
                       "%04d-%02d-%02dT%02d:%02d:%02d.%dZ%*.s",
                       dt.year,
                       dt.mon,
                       dt.day,
                       dt.hour,
                       dt.min,
                       dt.sec,
                       dt.ms,
                       spaces,
                       "");
    }
    else
    {
        cra_datetime_now_localtime(&dt);
        spaces = dt.ms >= 100 ? 1 : (dt.ms >= 10 ? 2 : 3);
        len = snprintf(msg,
                       sizeof(msg),
                       "%04d-%02d-%02dT%02d:%02d:%02d.%d%+03hd:00%*.s",
                       dt.year,
                       dt.mon,
                       dt.day,
                       dt.hour,
                       dt.min,
                       dt.sec,
                       dt.ms,
                       s_logger->timezoneoffset,
                       spaces,
                       "");
    }

    // tid level logname
    cra_tid_t   tid = cra_thrd_get_current_tid();
    const char *lv = cra_level_to_str(level);
    len += snprintf(msg + len, sizeof(msg) - len, "%-5lu %s %s ", tid, lv, logname);
    if (len >= (int)sizeof(msg) - 1)
        goto fix_length;

    // msg
    va_list ap;
    va_start(ap, fmt);
    len += vsnprintf(msg + len, sizeof(msg) - len, fmt, ap);
    va_end(ap);
    if (len >= (int)sizeof(msg) - 1)
        goto fix_length;

#ifdef CRA_LOG_FILE_LINE
    // file:line
    if (msg[len - 1] == '\n')
        len--;
    len += snprintf(msg + len, sizeof(msg) - len, " - %s:%d", file, line);
#endif

    // \n
    if (len >= (int)sizeof(msg) - 1)
    {
    fix_length:
        len = sizeof(msg) - 1;
        msg[len - 4] = '.';
        msg[len - 3] = '.';
        msg[len - 2] = '.';
        msg[len - 1] = '\n';
    }
    else
    {
        if (msg[len - 1] != '\n')
        {
            // [...XX\0] => [...XX\n\0]
            msg[len++] = '\n';
            msg[len] = '\0';
        }
    }

    // write log
    (*s_logger->logto)->append(s_logger->logto, msg, len, level);
}

#if 1 // log async

typedef struct
{
    size_t current;
    char   buffer[CRA_LOG_BUFFER_SIZE];
} CraLogBuffer;

struct _CraLogAsync
{
    cra_thrd_t  thread;
    cra_mutex_t mutex;
    cra_cond_t  cond;

    CraLogBuffer *current;
    CraLogBuffer *backup;
    CraLList     *buffers; // LList<CraLogBuffer *>

    CraLogTo_i                 **logto;
    cra_log_async_write2logto_fn write;

    CraCDL cdl;

    bool started;
};

static inline CraLogBuffer *
cra_log_buffer_create(void)
{
    CraLogBuffer *buf = cra_alloc(CraLogBuffer);
    buf->current = 0;
    return buf;
}

static inline void
cra_log_buffer_destroy(CraLogBuffer *buf)
{
    if (buf)
        cra_dealloc(buf);
}

static inline void
cra_log_buffer_append(CraLogBuffer *buf, const char *msg, int len)
{
    assert(len > 0 && buf->current + len <= CRA_LOG_BUFFER_SIZE);
    memcpy(buf->buffer + buf->current, msg, len);
    buf->current += len;
}

static inline void
cra_log_buffer_reset(CraLogBuffer *buf)
{
    buf->current = 0;
}

static CRA_THRD_FUNC(cra_log_async_thread)
{
    CraLogAsync  *obj = (CraLogAsync *)arg;
    CraLogBuffer *buf1 = cra_log_buffer_create();
    CraLogBuffer *buf2 = cra_log_buffer_create();
    CraLList     *buffers = cra_alloc(CraLList);
    cra_llist_init0(CraLogBuffer *, buffers, false);

    obj->started = true;
    cra_cdl_count_down(&obj->cdl);
    while (obj->started)
    {
        cra_mutex_lock(&obj->mutex);
        cra_cond_wait_timeout(&obj->cond, &obj->mutex, CRA_LOG_WRITE_INTERVAL);
        if (obj->current->current > 0)
        {
            cra_llist_append(obj->buffers, &obj->current);
            if (buf1)
            {
                obj->current = buf1;
                buf1 = NULL;
            }
            else if (buf2)
            {
                obj->current = buf2;
                buf2 = NULL;
            }
            else
            {
                obj->current = cra_log_buffer_create();
            }
        }
        cra_swap_ptr((void **)&buffers, (void **)&obj->buffers);
        cra_mutex_unlock(&obj->mutex);

        CraLogBuffer *buf;
        while (cra_llist_pop_front(buffers, &buf))
        {
            obj->write(obj->logto, buf->buffer, buf->current);
            cra_log_buffer_reset(buf);
            if (!buf1)
            {
                buf1 = buf;
            }
            else if (!buf2)
            {
                buf2 = buf;
            }
            else if (!obj->backup)
            {
                cra_mutex_lock(&obj->mutex);
                if (!obj->backup)
                    obj->backup = buf;
                else
                    cra_log_buffer_destroy(buf);
                cra_mutex_unlock(&obj->mutex);
            }
            else
            {
                cra_log_buffer_destroy(buf);
            }
        }
    }

    assert(cra_llist_get_count(buffers) == 0);
    cra_log_buffer_destroy(buf1);
    cra_log_buffer_destroy(buf2);
    cra_llist_uninit(buffers);
    cra_dealloc(buffers);

    return (cra_thrd_ret_t)0;
}

void
cra_log_async_write(CraLogAsync *obj, const char *msg, int len)
{
    cra_mutex_lock(&obj->mutex);

    if (!obj->started)
        goto end;

    if (obj->current->current + len > CRA_LOG_BUFFER_SIZE)
    {
        cra_llist_append(obj->buffers, &obj->current);
        if (obj->backup)
        {
            obj->current = obj->backup;
            obj->backup = NULL;
        }
        else
        {
            obj->current = cra_log_buffer_create();
        }
        cra_cond_signal(&obj->cond);
    }
    cra_log_buffer_append(obj->current, msg, len);

end:
    cra_mutex_unlock(&obj->mutex);
}

void
cra_log_async_write_array(CraLogAsync *obj, const CraLogAsyncBuffer buffers[])
{
    cra_mutex_lock(&obj->mutex);

    if (!obj->started)
        goto end;

    for (; !!buffers->buffer; buffers++)
    {
        if (obj->current->current + buffers->length > CRA_LOG_BUFFER_SIZE)
        {
            cra_llist_append(obj->buffers, &obj->current);
            if (obj->backup)
            {
                obj->current = obj->backup;
                obj->backup = NULL;
            }
            else
            {
                obj->current = cra_log_buffer_create();
            }
            cra_cond_signal(&obj->cond);
        }
        cra_log_buffer_append(obj->current, buffers->buffer, buffers->length);
    }

end:
    cra_mutex_unlock(&obj->mutex);
}

CraLogAsync *
cra_log_async_create(cra_log_async_write2logto_fn on_write, CraLogTo_i **logto)
{
    assert(on_write);
    assert(logto && *logto);

    CraLogAsync *obj = cra_alloc(CraLogAsync);
    cra_mutex_init(&obj->mutex);
    cra_cond_init(&obj->cond);
    obj->current = cra_log_buffer_create();
    obj->backup = cra_log_buffer_create();
    obj->buffers = cra_alloc(CraLList);
    cra_llist_init0(CraLogBuffer *, obj->buffers, false);
    obj->started = false; // set to `true` in thread func
    obj->logto = logto;
    obj->write = on_write;
    cra_cdl_init(&obj->cdl, 1);
    assert_always(cra_thrd_create(&obj->thread, cra_log_async_thread, obj));
    cra_cdl_wait(&obj->cdl);
    return obj;
}

void
cra_log_async_destory(CraLogAsync **pobj)
{
    assert(pobj && *pobj);
    assert((*pobj)->started);

    cra_mutex_lock(&(*pobj)->mutex);
    (*pobj)->started = false;
    cra_cond_broadcast(&(*pobj)->cond);
    cra_mutex_unlock(&(*pobj)->mutex);

    cra_thrd_join((*pobj)->thread);

    CraLogBuffer *buf;
    while (cra_llist_pop_front((*pobj)->buffers, &buf))
    {
        (*pobj)->write((*pobj)->logto, buf->buffer, buf->current);
        cra_log_buffer_destroy(buf);
    }
    cra_log_buffer_destroy((*pobj)->current);
    cra_log_buffer_destroy((*pobj)->backup);
    cra_llist_uninit((*pobj)->buffers);
    cra_dealloc((*pobj)->buffers);
    cra_mutex_destroy(&(*pobj)->mutex);
    cra_cond_destroy(&(*pobj)->cond);
    cra_cdl_uninit(&(*pobj)->cdl);

    cra_dealloc(*pobj);
    *pobj = NULL;
}

#endif // end log async

#if 1 // log to stdout

struct _CraLogToStdoutSync
{
    CRA_LOGTO_HEAD;
    CraLogAsync *async;
    bool         is_working;
};

#define CRA_COLOR_RESET "\033[0m"

#define CRA_COLOR_GRAY   "\033[30m"
#define CRA_COLOR_GREEN  "\033[32m"
#define CRA_COLOR_BLUE   "\033[34m"
#define CRA_COLOR_YELLOW "\033[33m"
#define CRA_COLOR_RED    "\033[31m"
#define CRA_COLOR_BG_RED "\033[41m"

static inline const char *
cra_get_color_fmt(CraLogLevel_e level)
{
    switch (level)
    {
        case CRA_LOG_LEVEL_TRACE:
            return CRA_COLOR_GRAY;
        case CRA_LOG_LEVEL_DEBUG:
            return CRA_COLOR_GREEN;
        case CRA_LOG_LEVEL_INFO:
            return CRA_COLOR_BLUE;
        case CRA_LOG_LEVEL_WARN:
            return CRA_COLOR_YELLOW;
        case CRA_LOG_LEVEL_ERROR:
            return CRA_COLOR_RED;
        case CRA_LOG_LEVEL_FATAL:
            return CRA_COLOR_BG_RED;

        default:
            assert_always(false);
            return NULL;
    }
}

static void
cra_logto_stdout_append(CraLogTo_i **self, const char *msg, int msglen, CraLogLevel_e level)
{
    CraLogToStdoutSync *obj = (CraLogToStdoutSync *)self;
    assert(obj->is_working);
    const char *color = cra_get_color_fmt(level);
    if (obj->async)
    {
        CraLogAsyncBuffer msg_with_color[] = {
            { .length = 5, .buffer = color },
            { .length = msglen, .buffer = msg },
            { .length = 4, .buffer = CRA_COLOR_RESET },
            { 0 },
        };
        cra_log_async_write_array(obj->async, msg_with_color);
    }
    else
    {
        fprintf(stdout, "%s%.*s%s", color, msglen, msg, CRA_COLOR_RESET);
    }
}

static void
cra_logto_stdout_write(CraLogTo_i **self, const char *content, size_t len)
{
    CRA_UNUSED_VALUE(self);
    fwrite(content, len, 1, stdout);
}

static void
cra_logto_stdout_destroy(CraLogTo_i **self)
{
    CraLogToStdoutSync *obj = (CraLogToStdoutSync *)self;
    assert(obj->is_working);
    obj->is_working = false;
    if (obj->async)
        cra_log_async_destory(&obj->async);
    cra_dealloc(self);
}

CraLogToStdoutSync *
cra_logto_stdout_create(bool async)
{
    CraLogToStdoutSync *obj = cra_alloc(CraLogToStdoutSync);
    static CraLogTo_i   s_i = {
          .destroy = cra_logto_stdout_destroy,
          .append = cra_logto_stdout_append,
    };
    CRA_LOGTO_I(obj) = &s_i;
    obj->is_working = true;
    obj->async = async ? cra_log_async_create(cra_logto_stdout_write, (CraLogTo_i **)obj) : NULL;
    return obj;
}

#endif // end log to stdout

#if 1 // log to file

#if 1 // file

#ifdef CRA_COMPILER_MSVC
#include <direct.h> // for _mkdir
#include <io.h>     // for _access

#define strdup              _strdup
#define exist(_name)        _access(_name, 0)
#define mkdir(_path, _mode) _mkdir(_path)

#define fopen cra_fopen
static inline FILE *
cra_fopen(const char *name, const char *mode)
{
    FILE *fp;
    if (0 != fopen_s(&fp, name, mode))
        return NULL;
    return fp;
}

#else
#include <sys/stat.h>

#define exist(_name) access(_name, F_OK)
#ifdef CRA_COMPILER_MINGW
#define mkdir(_path, _mode) mkdir(_path)
#endif

#endif

static int
cra_mkdirs(const char *path, int mode)
{
    size_t i = 0;
    int    ret = -1;
    char  *p = NULL; // path;

    CRA_UNUSED_VALUE(mode);

    if (0 == exist(path))
        return 0;
    if (0 == mkdir(path, mode))
        return 0;

    if (!(p = strdup(path)))
        return -1;

    for (; p[i] != '\0'; i++)
    {
        if ((p[i] != '/' && p[i] != '\\') || i == 0)
            continue;

        p[i] = '\0';
        if (0 != exist(p))
        {
            if (0 != (ret = mkdir(p, 0755)))
                goto end;
        }
        p[i] = '/';
    }
    if (p[i - 1] != '/')
        ret = mkdir(path, mode);
end:
    free(p);
    return ret;
}
#define mkdirs cra_mkdirs

#endif // end file

struct _CraLogToFile
{
    CRA_LOGTO_HEAD;
    CraLogAsync *async;
    FILE        *fp;
    size_t       cur_size; // current file size
    size_t       max_size; // max file size
    char         filename[CRA_LOG_FILENAME_MAX];
    cra_mutex_t  mutex;      // for `cur_size`
    unsigned int time_index; // point to the time string head in filename
    bool         is_working;
    bool         with_localtime;
};

// filename: path/to/name_sync_yyyyMMddThhmmss_ms[Z].log

#define CRA_LOG_LOG_FILE_TIME_UTC      "%04d%02d%02dT%02d%02d%02d_%dZ.log"
#define CRA_LOG_LOG_FILE_TIME_LOCAL    "%04d%02d%02dT%02d%02d%02d_%d.log"
#define CRA_LOG_FILENAME_DATETIME_SIZE sizeof(".yyyyMMddThhmmss_mssZ.log")

#define CRA_CLOSE_FILE(_fp) \
    do                      \
    {                       \
        if (_fp)            \
        {                   \
            fclose(_fp);    \
            _fp = NULL;     \
        }                   \
    } while (0)

static void
cra_logto_file_open_file(CraLogToFile *obj)
{
    assert(!obj->fp);
    CraDateTime dt;
    const char *datetime_fmt;
    if (obj->with_localtime)
    {
        datetime_fmt = CRA_LOG_LOG_FILE_TIME_LOCAL;
        cra_datetime_now_localtime(&dt);
    }
    else
    {
        datetime_fmt = CRA_LOG_LOG_FILE_TIME_UTC;
        cra_datetime_now_utc(&dt);
    }
    snprintf(obj->filename + obj->time_index,
             CRA_LOG_FILENAME_DATETIME_SIZE,
             datetime_fmt,
             dt.year,
             dt.mon,
             dt.day,
             dt.hour,
             dt.min,
             dt.sec,
             dt.ms);
    obj->cur_size = 0;
    obj->fp = fopen(obj->filename, "w");
    if (!obj->fp)
        fprintf(stderr, "Logger: open file[%s] error(%d).\n", obj->filename, cra_get_last_error());
}

static void
cra_logto_file_write(CraLogTo_i **self, const char *content, size_t len)
{
    assert(len > 0);
    CRA_UNUSED_VALUE(self);
    CraLogToFile *obj = (CraLogToFile *)self;
    if (obj->fp)
    {
        fwrite(content, len, 1, obj->fp);
        obj->cur_size += len;
        if (obj->cur_size >= obj->max_size)
        {
            // create a new file
            CRA_CLOSE_FILE(obj->fp);
            cra_logto_file_open_file(obj);
        }
    }
    else
    {
        fprintf(stderr, "Logger: no log file.\n");
    }
}

static void
cra_logto_file_append(CraLogTo_i **self, const char *msg, int msglen, CraLogLevel_e level)
{
    CraLogToFile *obj = (CraLogToFile *)self;
    CRA_UNUSED_VALUE(level);
    assert(obj->is_working);
    if (obj->async)
    {
        cra_log_async_write(obj->async, msg, msglen);
    }
    else
    {
        cra_mutex_lock(&obj->mutex);
        cra_logto_file_write(self, msg, msglen);
        cra_mutex_unlock(&obj->mutex);
    }
}

static void
cra_logto_file_destroy(CraLogTo_i **self)
{
    CraLogToFile *obj = (CraLogToFile *)self;
    assert(obj->is_working);
    obj->is_working = false;
    if (obj->async)
        cra_log_async_destory(&obj->async);
    CRA_CLOSE_FILE(obj->fp);
    cra_mutex_destroy(&obj->mutex);
    cra_dealloc(self);
}

CraLogToFile *
cra_logto_file_create(bool async, bool with_localtime, const char *path, const char *name, size_t max_file_size)
{
    assert(path);
    assert(name);
    assert(max_file_size > 1024);

    size_t path_len = strnlen(path, 8192);
    size_t name_len = strnlen(name, 8192);
    assert_always(name_len > 0 && path_len + name_len + CRA_LOG_FILENAME_DATETIME_SIZE <= CRA_LOG_FILENAME_MAX);

    CraLogToFile     *obj = cra_alloc(CraLogToFile);
    static CraLogTo_i s_i = {
        .destroy = cra_logto_file_destroy,
        .append = cra_logto_file_append,
    };
    CRA_LOGTO_I(obj) = &s_i;
    cra_mutex_init(&obj->mutex);
    obj->time_index = (unsigned int)(path_len + name_len + 1); // "path/to/name_"
    // is path end with '/'?
    if (path_len > 0 && path[path_len - 1] != '/')
    {
        snprintf(obj->filename, sizeof(obj->filename), "%s/%s_", path, name);
        obj->time_index += 1; // '/'
    }
    else
    {
        snprintf(obj->filename, sizeof(obj->filename), "%s%s_", path, name);
    }
    obj->fp = NULL;
    obj->cur_size = 0;
    obj->is_working = true;
    obj->max_size = max_file_size;
    obj->with_localtime = with_localtime;
    // mkdir
    if (path_len > 0)
    {
        if (mkdirs(path, 0755) != 0)
        {
            int err = cra_get_last_error();
            fprintf(stderr, "Logger: mkdirs(%s) failed. error: %d.\n", path, err);
            exit(1);
        }
    }
    // open file
    cra_logto_file_open_file(obj);
    obj->async = async ? cra_log_async_create(cra_logto_file_write, (CraLogTo_i **)obj) : NULL;
    return obj;
}

#endif // end log to file
