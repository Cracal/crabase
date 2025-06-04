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
#include <stdarg.h>
#include "cra_log.h"
#include "cra_time.h"
#include "cra_malloc.h"
#include "threads/cra_locker.h"
#include "threads/cra_thread.h"
#include "collections/cra_llist.h"

#if 1 // file

#ifdef CRA_COMPILER_MSVC
#include <direct.h> // for _mkdir
#include <io.h>     // for _access

#define strdup _strdup
#define exist(_name) _access(_name, 0)
#define mkdir(_path, _mode) _mkdir(_path)

static inline FILE *cra_fopen(const char *name, const char *mode)
{
    FILE *fp;
    if (0 != fopen_s(&fp, name, mode))
        return NULL;
    return fp;
}
#define fopen cra_fopen

#else

#define exist(_name) access(_name, F_OK)
#ifdef CRA_COMPILER_MINGW
#define mkdir(_path, _mode) mkdir(_path)
#endif

#endif

static int cra_mkdirs(/*const*/ char *path, int mode)
{
    int ret = -1;
    char *p = path;

    CRA_UNUSED_VALUE(mode);

    if (0 == exist(path))
        return 0;
    if (0 == mkdir(path, mode))
        return 0;

    // if (!(p = strdup(path)))
    //     return -1;

    for (size_t i = 0; p[i] != '\0'; i++)
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
    ret = mkdir(path, mode);
end:
    // free(p);
    return ret;
}
#define mkdirs cra_mkdirs

#endif // end file

// default: 260 chars
#ifndef CRA_LOG_PATH_MAX
#define CRA_LOG_PATH_MAX 260
#endif

// default: 2MB
#ifndef CRA_LOG_BUFFER_SIZE
#define CRA_LOG_BUFFER_SIZE (2 * 1024 * 1024)
#endif

// default : 8KB
#ifndef CRA_LOG_MSG_MAX
#define CRA_LOG_MSG_MAX 8192
#endif

// default: 3s
#ifndef CRA_LOG_WRITE_INTERVAL
#define CRA_LOG_WRITE_INTERVAL (3 * 1000)
#endif

#define CRA_LOG_FILENAME_MAX (CRA_LOG_PATH_MAX + 100)

#define CRA_LOG_CHECK_CLOSE_FP                                         \
    if (s_logger.fp && s_logger.fp != stdout && s_logger.fp != stderr) \
        fclose(s_logger.fp);                                           \
    s_logger.fp = NULL

typedef struct
{
    size_t current;
    char buf[CRA_LOG_BUFFER_SIZE];
} CraLogBuffer;

#ifndef NDEBUG
unsigned int s_create_buf_cnt = 0;
#endif

static inline CraLogBuffer *cra_create_buf(void)
{
    CraLogBuffer *buf = (CraLogBuffer *)cra_malloc(sizeof(CraLogBuffer));
    buf->current = 0;
    return buf;
}

static inline void cra_destroy_buf(CraLogBuffer *buf)
{
    cra_free(buf);
}

static void cra_destroy_buf_for_llist(void *buf)
{
    cra_destroy_buf(*(CraLogBuffer **)buf);
}

#define CRA_LOG_TIME_FMT_UTC "%04d-%02d-%02dT%02d:%02d:%02d.%dZ "
#define CRA_LOG_TIME_FMT_LOCAL "%04d-%02d-%02dT%02d:%02d:%02d.%d "

#define CRA_LOG_LOG_FILE_TIME_UTC "%04d%02d%02dT%02d%02d%02d_%dZ.log"
#define CRA_LOG_LOG_FILE_TIME_LOCAL "%04d%02d%02dT%02d%02d%02d_%d.log"
#define CRA_LOG_FILENAME_DATETIME_SIZE sizeof(".yyyyMMddThhmmss_mssZ.log")

typedef struct
{
    bool initialized;
    bool with_thread;
    bool running;
    bool with_utc;

    CraLogLevel_e level;

    cra_thrd_t thrd;
    cra_cond_t cond;
    cra_mutex_t mutex;

    FILE *fp;
    int pathend;
    int filestart;
    size_t filemax;
    size_t filecurrsize;
    char filename[CRA_LOG_FILENAME_MAX];

    CraDateTime lastdt;
    void (*time_fn)(CraDateTime *);
    char *time_msg_fmt;
    char *time_file_fmt;

    CraLogBuffer *buf1;
    CraLogBuffer *buf2;
    CraLList *buffers; // LList<CraLogBuffer *>
} CraLog;

static CraLog s_logger = {0};

static inline const char *cra_level_to_str(CraLogLevel_e level)
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

static bool cra_log_create_logfile(void)
{
    CraDateTime *dt = &s_logger.lastdt;
    // 日志文件名的时间部分
    snprintf(s_logger.filename + s_logger.filestart, CRA_LOG_FILENAME_MAX,
             s_logger.time_file_fmt, dt->year, dt->mon, dt->day,
             dt->hour, dt->min, dt->sec, dt->ms);

    s_logger.filename[s_logger.pathend] = '\0';
    if (0 != exist(s_logger.filename))
    {
        if (0 != mkdirs(s_logger.filename, 0755))
            goto error_ret;
    }
    s_logger.filename[s_logger.pathend] = '/';

    s_logger.filecurrsize = 0;

    CRA_LOG_CHECK_CLOSE_FP;
    if (NULL != (s_logger.fp = fopen(s_logger.filename, "w")))
        return true;
error_ret:
    s_logger.filestart = -1;
    return false;
}

#define CRA_LOG_ADD_AND_CHECK_FILESIZE(_len)                                          \
    do                                                                                \
    {                                                                                 \
        if (s_logger.filestart != -1)                                                 \
        {                                                                             \
            CraDateTime _dt;                                                          \
            (s_logger.filecurrsize) += (_len);                                        \
            s_logger.time_fn(&_dt);                                                   \
            if ((_dt.mon + _dt.day) != (s_logger.lastdt.mon + s_logger.lastdt.day) || \
                (s_logger.filecurrsize) >= s_logger.filemax)                          \
            {                                                                         \
                memcpy(&s_logger.lastdt, &_dt, sizeof(CraDateTime));                  \
                cra_log_create_logfile();                                             \
            }                                                                         \
        }                                                                             \
    } while (0)

static CRA_THRD_FUNC(cra_log_thread)
{
    CRA_UNUSED_VALUE(arg);

    CraLogBuffer *buf;
    CraLogBuffer *buf1 = NULL;
    CraLogBuffer *buf2 = NULL;
    CraLList *buffers;

    buffers = cra_alloc(CraLList);
    cra_llist_init0(CraLogBuffer *, buffers, false, cra_destroy_buf_for_llist);

    while (s_logger.running)
    {
        cra_mutex_lock(&s_logger.mutex);

        if (cra_llist_get_count(s_logger.buffers) == 0)
            cra_cond_wait_timeout(&s_logger.cond, &s_logger.mutex, CRA_LOG_WRITE_INTERVAL);

        cra_llist_append(s_logger.buffers, &s_logger.buf1);
        if (buf1)
        {
            s_logger.buf1 = buf1;
            buf1 = NULL;
        }
        else if (buf2)
        {
            s_logger.buf1 = buf2;
            buf2 = NULL;
        }
        else
        {
            s_logger.buf1 = cra_create_buf();
#ifndef NDEBUG
            s_create_buf_cnt++;
#endif
        }
        cra_swap_ptr((void **)&buffers, (void **)&s_logger.buffers);

        cra_mutex_unlock(&s_logger.mutex);

        while (cra_llist_pop_front(buffers, &buf))
        {
            if (s_logger.fp)
            {
                fwrite(buf->buf, buf->current, 1, s_logger.fp);
                CRA_LOG_ADD_AND_CHECK_FILESIZE(buf->current);
            }
            buf->current = 0;
            if (!buf1)
                buf1 = buf;
            else if (!buf2)
                buf2 = buf;
            else
                cra_destroy_buf(buf);
        }
        fflush(s_logger.fp);
    }

#ifndef NDEBUG
    printf("the log system has created buffers %u times in total.\n", s_create_buf_cnt);
#endif

    if (buf1)
        cra_destroy_buf(buf1);
    if (buf2)
        cra_destroy_buf(buf2);
    cra_llist_uninit(buffers);
    cra_dealloc(buffers);

    return (cra_thrd_ret_t){0};
}

static void cra_log_write_log(const char *message, size_t len)
{
    // 同步写入
    if (!s_logger.with_thread)
    {
        if (s_logger.fp)
        {
            fputs(message, s_logger.fp);
            CRA_LOG_ADD_AND_CHECK_FILESIZE(len);
        }
        return;
    }

    // 异步写入
    cra_mutex_lock(&s_logger.mutex);
    if ((CRA_LOG_BUFFER_SIZE - s_logger.buf1->current) < len)
    {
        cra_llist_append(s_logger.buffers, &s_logger.buf1);
        if (s_logger.buf2)
        {
            s_logger.buf1 = s_logger.buf2;
            s_logger.buf2 = NULL;
        }
        else
        {
            s_logger.buf1 = cra_create_buf();
#ifndef NDEBUG
            s_create_buf_cnt++;
#endif
        }
        cra_cond_signal(&s_logger.cond);
    }
    memcpy(s_logger.buf1->buf + s_logger.buf1->current, message, len);
    s_logger.buf1->current += len;
    cra_mutex_unlock(&s_logger.mutex);
}

bool cra_log_startup(CraLogLevel_e level, bool run_write_thread, bool with_localtime)
{
    assert(CRA_LOG_MSG_MAX <= CRA_LOG_BUFFER_SIZE);

    if (s_logger.initialized)
        return true;

    s_logger.initialized = true;
    s_logger.with_thread = run_write_thread;
    s_logger.with_utc = !with_localtime;

    if (with_localtime)
    {
        s_logger.time_fn = cra_datetime_now_localtime;
        s_logger.time_msg_fmt = CRA_LOG_TIME_FMT_LOCAL;
        s_logger.time_file_fmt = CRA_LOG_LOG_FILE_TIME_LOCAL;
    }
    else
    {
        s_logger.time_fn = cra_datetime_now_utc;
        s_logger.time_msg_fmt = CRA_LOG_TIME_FMT_UTC;
        s_logger.time_file_fmt = CRA_LOG_LOG_FILE_TIME_UTC;
    }

    s_logger.level = level;

    s_logger.fp = stdout;
    s_logger.filestart = -1;
    s_logger.pathend = -1;
    s_logger.filemax = 0;
    s_logger.filecurrsize = 0;
    s_logger.filename[0] = '\0';

    cra_cond_init(&s_logger.cond);
    cra_mutex_init(&s_logger.mutex);

    if (run_write_thread)
    {
        s_logger.running = true;
        s_logger.buf1 = cra_create_buf();
#ifndef NDEBUG
        s_create_buf_cnt = 1;
#endif
        s_logger.buf2 = NULL;
        s_logger.buffers = cra_alloc(CraLList);
        cra_llist_init0(CraLogBuffer *, s_logger.buffers, false, cra_destroy_buf_for_llist);
        if (!cra_thrd_create(&s_logger.thrd, cra_log_thread, NULL))
            goto fail;
    }

    return true;

fail:
    cra_llist_uninit(s_logger.buffers);
    cra_dealloc(s_logger.buffers);
    cra_free(s_logger.buf1);
    s_logger.initialized = false;
    cra_cond_destroy(&s_logger.cond);
    cra_mutex_destroy(&s_logger.mutex);
    return false;
}

void cra_log_cleanup(void)
{
    if (!s_logger.initialized)
        return;

    s_logger.initialized = false;
    if (s_logger.with_thread)
    {
        s_logger.running = false;
        cra_cond_signal(&s_logger.cond);
        cra_thrd_join(s_logger.thrd);
        if (!!s_logger.buf1)
            cra_destroy_buf(s_logger.buf1);
        if (!!s_logger.buf2)
            cra_destroy_buf(s_logger.buf2);
        cra_llist_uninit(s_logger.buffers);
        cra_dealloc(s_logger.buffers);
    }

    CRA_LOG_CHECK_CLOSE_FP;
    cra_cond_destroy(&s_logger.cond);
    cra_mutex_destroy(&s_logger.mutex);
}

void cra_log_set_level(CraLogLevel_e level)
{
    cra_mutex_lock(&s_logger.mutex);
    s_logger.level = level;
    cra_mutex_unlock(&s_logger.mutex);
}
CraLogLevel_e cra_log_get_level(void) { return s_logger.level; }

void cra_log_output_to_fp(FILE *fp)
{
    assert(s_logger.initialized);
    cra_mutex_lock(&s_logger.mutex);
    CRA_LOG_CHECK_CLOSE_FP;
    s_logger.fp = fp;
    s_logger.filestart = -1;
    cra_mutex_unlock(&s_logger.mutex);
}

bool cra_log_output_to_file(const char *path, const char *main_name, size_t size_per_file)
{
    int len;
    bool ret;
    size_t path_len = strlen(path);

    assert_always(size_per_file > 100);
    assert_always(path_len + strlen(main_name) < CRA_LOG_FILENAME_MAX - CRA_LOG_FILENAME_DATETIME_SIZE);

    cra_mutex_lock(&s_logger.mutex);

    // path/to/main_name.
    if (path[path_len - 1] == '/' || path[path_len - 1] == '\\')
    {
        s_logger.pathend = (int)(path_len - 1);
        len = snprintf(s_logger.filename, CRA_LOG_FILENAME_MAX, "%s%s.", path, main_name);
    }
    else
    {
        s_logger.pathend = (int)path_len;
        len = snprintf(s_logger.filename, CRA_LOG_FILENAME_MAX, "%s/%s.", path, main_name);
    }
    s_logger.filestart = len;
    s_logger.filemax = size_per_file;

    CraDateTime dt;
    s_logger.time_fn(&dt);
    memcpy(&s_logger.lastdt, &dt, sizeof(CraDateTime));
    ret = cra_log_create_logfile();

    cra_mutex_unlock(&s_logger.mutex);
    return ret;
}

#ifdef CRA_LOG_FILE_LINE
void __cra_log_message(const char *logname, CraLogLevel_e level, const char *file, int line, const char *fmt, ...)
#else
void __cra_log_message(const char *logname, CraLogLevel_e level, const char *fmt, ...)
#endif
{
    size_t len;
    va_list ap;
    cra_tid_t tid;
    CraDateTime dt;
    char message[CRA_LOG_MSG_MAX];

    assert(s_logger.initialized);

    if (level < s_logger.level)
        return;

    // time
    s_logger.time_fn(&dt);
    len = snprintf(message, sizeof(message), s_logger.time_msg_fmt,
                   dt.year, dt.mon, dt.day,
                   dt.hour, dt.min, dt.sec, dt.ms);

    // tid
    tid = cra_thrd_get_current_tid();
    len += snprintf(message + len, sizeof(message) - len, "%-5lu ", tid);

    // logname
    len += snprintf(message + len, sizeof(message) - len, "%s ", logname);
    if (len >= sizeof(message) - 1)
        goto fix_length;

    // level
    len += snprintf(message + len, sizeof(message) - len, "%s  ", cra_level_to_str(level));

    // msg
    va_start(ap, fmt);
    len += vsnprintf(message + len, sizeof(message) - len, fmt, ap);
    va_end(ap);
    if (len >= sizeof(message) - 1)
        goto fix_length;

#ifdef CRA_LOG_FILE_LINE
    // file:line
    if (message[len - 1] == '\n')
        len--;
    len += snprintf(message + len, sizeof(message) - len, " - %s:%d", file, line);
#endif

    // \n
    if (len >= sizeof(message) - 1)
    {
    fix_length:
        len = sizeof(message) - 1;
        message[len - 4] = '.';
        message[len - 3] = '.';
        message[len - 2] = '.';
        message[len - 1] = '\n';
    }
    else
    {
        if (message[len - 1] != '\n')
        {
            message[len++] = '\n';
            message[len] = '\0';
        }
    }

    // write log
    cra_log_write_log(message, len);
}
