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
#include "cra_atomic.h"
#include "cra_malloc.h"
#include "threads/cra_lock.h"
#include "threads/cra_thread.h"
#include "collections/cra_alist.h"
#include "collections/cra_llist.h"

typedef struct CraLogOutputAsync CraLogOutputAsync;
typedef struct CraLogBuf         CraLogBuf;

struct CraLogBuf
{
    CraLogger   *log;
    unsigned int len;
    char         buf[CRA_LOG_BUF_SIZE];
};

struct CraLogOutputAsync
{
    bool              running;
    bool              initialized;
    cra_atomic_flag_t initialized_lock;
    unsigned int      alloc_buf_cnt;
    cra_thrd_t        thrd;
    cra_cond_t        condi;
    cra_mutex_t       mutex;
    CraAList          loggers;  // AList<CraLogger *>
    CraLList         *buffers1; // LList<CraLogBuf *>
    CraLList         *buffers2; // LList<CraLogBuf *>
    CraAList          buf_pool; // AList<CraLogBuf *>
};

struct CraLogger
{
    CraLogLv_e         level; // must be first member
    cra_atomic_int32_t refcnt;
    char               name[CRA_LOG_NAME_MAX];
    short              tz_hour;
    uint8_t            active   : 4;
    uint8_t            use_zulu : 4;
    bool               to_file; // if 1, then log to file
    // for async log output(to file) only
    unsigned int       file_size_max;
    unsigned int       file_size_cur;
    unsigned int       filename_time_start;
    char               filename[CRA_LOG_FILENAME_MAX];
    CraLogBuf         *buffer;
    FILE              *fp;
    size_t             index; // index in loggers
    time_t             last_flush;
    time_t             last_roll;
};

#define cra_log_ref(_logger) cra_atomic_inc(&(_logger)->refcnt, CRA_MO_RELAXED)
static inline void
cra_log_unref(CraLogger *logger)
{
    if (cra_atomic_dec(&logger->refcnt, CRA_MO_ACQ_REL) == 1)
    {
        assert(!logger->active);
        assert(logger->buffer == NULL);

        if (logger->fp)
            fclose(logger->fp);

        cra_dealloc(logger);
    }
}

#if 1 // LogOutputAsync

static CraLogOutputAsync s_log_async = { .initialized_lock = CRA_ATOMIC_FLAG_INIT };

#define cra_async_initialized_lock() while (cra_atomic_flag_test_and_set(&s_log_async.initialized_lock, CRA_MO_ACQUIRE))
#define cra_async_initialized_unlock() cra_atomic_flag_clear(&s_log_async.initialized_lock, CRA_MO_RELEASE)

static CraLogBuf *
cra_log_output_async_get_buf(void)
{
    CraLogBuf *buf = NULL;
    if (!cra_alist_pop_back(&s_log_async.buf_pool, &buf) && s_log_async.alloc_buf_cnt < CRA_LOG_BUF_MAX_CNT)
    {
        buf = cra_alloc(CraLogBuf);
        if (buf)
            ++s_log_async.alloc_buf_cnt;
    }

    if (buf)
    {
        buf->len = 0;
        buf->log = NULL;
    }

    return buf;
}

static inline void
cra_log_output_async_put_buf(CraLogBuf *buf)
{
    cra_alist_append(&s_log_async.buf_pool, &buf);
}

static inline void
cra_log_output_async_del_buf(CraLogBuf *buf)
{
    cra_dealloc(buf);
}

static void
cra_log_output_async_roll_file(CraLogger *log, time_t now, time_t day)
{
    // if user is not call cra_log_config(), then use default config
    if (log->file_size_max == 0)
    {
        cra_log_config(log, CRA_LOG_DEFAULT_FILE_MAX, CRA_LOG_DEFAULT_DIR);
    }

    // close old file if exists
    if (log->fp)
    {
        fclose(log->fp);
        log->last_flush = now;
    }

    // make new filename
    // "path/to/logname_yyyyMMdd_hhmmss_SSS.log"
    // "path/to/logname_yyyyMMdd_hhmmss_SSSZ.log"

    CraDateTime dt;
    if (log->use_zulu)
        cra_datetime_now_utc(&dt);
    else
        cra_datetime_now_localtime(&dt);

    snprintf(log->filename + log->filename_time_start,
             sizeof(log->filename) - log->filename_time_start,
             "%04d%02d%02d_%02d%02d%02d_%d%s.log",
             dt.year,
             dt.mon,
             dt.day,
             dt.hour,
             dt.min,
             dt.sec,
             dt.ms,
             log->use_zulu ? "Z" : "");

// open new file
#ifdef CRA_OS_WIN
    if (fopen_s(&log->fp, log->filename, "a+") != 0)
#else
    if ((log->fp = fopen(log->filename, "a+e")) == NULL)
#endif
    {
        fprintf(stderr, "Logger: failed to open file `%s`.\n", log->filename);
        return;
    }

    log->last_roll = day;
    log->file_size_cur = 0;
}

static void
cra_log_output_async_write_to_file(CraLogBuf *buf)
{
    time_t     day;
    time_t     now;
    CraLogger *log;

    assert(buf);
    assert(buf->log);
    assert(buf->len > 0);

    log = buf->log;

    now = time(NULL);
    day = now / (24 * 60 * 60);

    // create new file conditions:
    // 1. fp is NULL (check before write)
    // 2. file size exceeds max_file_size (check after write)
    // 3. day changes (check after write)

    // 1. 2.
    if (log->fp == NULL || day != log->last_roll)
    {
        cra_log_output_async_roll_file(log, now, day);
        if (log->fp == NULL)
            return; // XXX: drop log
    }

    // write to file
    fwrite(buf->buf, buf->len, 1, log->fp);
    log->file_size_cur += buf->len;

    // 3.
    if (log->file_size_cur + buf->len > log->file_size_max)
    {
        cra_log_output_async_roll_file(log, now, day);
    }

    // check if need to flush file
    else if (now - log->last_flush > CRA_LOG_FLUSH_INTERVAL)
    {
        fflush(log->fp);
        log->last_flush = now;
    }
}

static CRA_THRD_FUNC(cra_log_output_async_thread)
{
    CraLogBuf *buf;
    CraLogger *logger;

    CRA_UNUSED_VALUE(arg);

    while (s_log_async.running)
    {
        cra_mutex_lock(&s_log_async.mutex);
        while (s_log_async.buffers1->count == 0 && s_log_async.running)
        {
            cra_cond_wait_timeout(&s_log_async.condi, &s_log_async.mutex, CRA_LOG_OUTPUT_INTERVAL);

            for (size_t i = 0; i < s_log_async.loggers.count; ++i)
            {
                if (!cra_alist_get(&s_log_async.loggers, i, &logger) || (!logger->buffer || logger->buffer->len == 0))
                    continue;

                cra_log_ref(logger);
                logger->buffer->log = logger;
                cra_llist_append(s_log_async.buffers1, &logger->buffer);
                logger->buffer = cra_log_output_async_get_buf();
            }
        }

        cra_swap_ptr((void **)&s_log_async.buffers1, (void **)&s_log_async.buffers2);

        cra_mutex_unlock(&s_log_async.mutex);

        while (cra_llist_pop_front(s_log_async.buffers2, &buf))
        {
            cra_log_output_async_write_to_file(buf);
            cra_log_unref(buf->log);

            cra_mutex_lock(&s_log_async.mutex);
            cra_log_output_async_put_buf(buf);
            cra_mutex_unlock(&s_log_async.mutex);
        }
    }

    while (cra_llist_pop_front(s_log_async.buffers1, &buf))
    {
        cra_log_output_async_write_to_file(buf);
        cra_log_unref(buf->log);

        cra_log_output_async_del_buf(buf);
    }

    return (cra_thrd_ret_t)0;
}

static void
cra_log_output_async_init(void)
{
    assert(!s_log_async.initialized);

    s_log_async.running = true;
    s_log_async.initialized = true;
    s_log_async.alloc_buf_cnt = CRA_LOG_BUF_INIT_CNT;

    cra_cond_init(&s_log_async.condi);
    cra_mutex_init(&s_log_async.mutex);

    if (!cra_alist_init(CraLogger *, &s_log_async.loggers))
    {
        fprintf(stderr, "Logger: failed to init loggers.\n");
        exit(EXIT_FAILURE);
    }

    s_log_async.buffers1 = cra_alloc(CraLList);
    s_log_async.buffers2 = cra_alloc(CraLList);
    if (!s_log_async.buffers1 || !s_log_async.buffers2)
    {
        fprintf(stderr, "Logger: failed to create buffers.\n");
        exit(EXIT_FAILURE);
    }

    if (!cra_llist_init(CraLogBuf *, s_log_async.buffers1) || !cra_llist_init(CraLogBuf *, s_log_async.buffers2))
    {
        fprintf(stderr, "Logger: failed to init buffers.\n");
        exit(EXIT_FAILURE);
    }

    if (!cra_alist_init(CraLogBuf *, &s_log_async.buf_pool))
    {
        fprintf(stderr, "Logger: failed to init buf pool.\n");
        exit(EXIT_FAILURE);
    }

    for (unsigned int i = 0; i < s_log_async.alloc_buf_cnt; ++i)
    {
        CraLogBuf *buf = cra_alloc(CraLogBuf);
        if (!buf)
        {
            fprintf(stderr, "Logger: failed to create log buffer.\n");
            exit(EXIT_FAILURE);
        }
        cra_alist_append(&s_log_async.buf_pool, &buf);
    }

    // create thread
    if (!cra_thrd_create(&s_log_async.thrd, cra_log_output_async_thread, NULL))
    {
        fprintf(stderr, "Logger: failed to create log output async thread.\n");
        exit(EXIT_FAILURE);
    }
}

static void
cra_log_output_async_uninit(void)
{
    assert(!s_log_async.running);
    assert(s_log_async.initialized);
    assert(s_log_async.loggers.count == 0);
    assert(s_log_async.buffers1->count == 0);
    assert(s_log_async.buffers2->count == 0);

    s_log_async.initialized = false;
    s_log_async.alloc_buf_cnt = 0;

    cra_cond_destroy(&s_log_async.condi);
    cra_mutex_destroy(&s_log_async.mutex);

    cra_alist_uninit(&s_log_async.loggers);

    cra_llist_uninit(s_log_async.buffers1);
    cra_llist_uninit(s_log_async.buffers2);
    cra_dealloc(s_log_async.buffers1);
    cra_dealloc(s_log_async.buffers2);

    CraLogBuf *buf;
    while (cra_alist_pop_back(&s_log_async.buf_pool, &buf))
    {
        cra_dealloc(buf);
    }

    cra_alist_uninit(&s_log_async.buf_pool);
}

static void
cra_log_output_async_append(CraLogger *logger, const char *msg, int len)
{
    assert(logger->active);
    assert(s_log_async.initialized);

    cra_mutex_lock(&s_log_async.mutex);

    if (!s_log_async.running)
        goto end;

    if (!logger->buffer)
    {
    get_new_buf:
        logger->buffer = cra_log_output_async_get_buf();
        if (!logger->buffer)
        {
#ifdef _DEBUG
            fprintf(stderr, "Logger: no log buffer available, this message will be dropped.\n");
#endif
            cra_mutex_unlock(&s_log_async.mutex);
            return;
        }

        assert(sizeof(logger->buffer->buf) >= (size_t)len);
        goto copy_msg;
    }

    if (logger->buffer->len + len > sizeof(logger->buffer->buf))
    {
        cra_log_ref(logger);
        logger->buffer->log = logger;
        cra_llist_append(s_log_async.buffers1, &logger->buffer);
        cra_cond_signal(&s_log_async.condi);
        goto get_new_buf;
    }

copy_msg:
    memcpy(logger->buffer->buf + logger->buffer->len, msg, len);
    logger->buffer->len += len;

end:
    cra_mutex_unlock(&s_log_async.mutex);
}

static void
cra_log_output_async_add_logger(CraLogger *logger)
{
    assert(logger->active);

    if (!s_log_async.initialized)
    {
        cra_async_initialized_lock();
        if (!s_log_async.initialized)
            cra_log_output_async_init();
        cra_async_initialized_unlock();
    }

    cra_log_ref(logger);

    cra_mutex_lock(&s_log_async.mutex);
    logger->index = s_log_async.loggers.count;
    logger->buffer = cra_log_output_async_get_buf();
    cra_alist_append(&s_log_async.loggers, &logger);
    cra_mutex_unlock(&s_log_async.mutex);
}

static void
cra_log_output_async_del_logger(CraLogger *logger)
{
    CraLogger *last_logger;

    assert(!logger->active);
    assert(s_log_async.running);

    cra_mutex_lock(&s_log_async.mutex);

    // pop last logger
    if (!cra_alist_pop_back(&s_log_async.loggers, &last_logger))
    {
        cra_mutex_unlock(&s_log_async.mutex);
        fprintf(stderr, "Logger: no logger found.\n");
        exit(EXIT_FAILURE);
    }
    // if last logger is not logger, insert it to logger index position
    if (last_logger != logger)
    {
        last_logger->index = logger->index;
        cra_alist_set(&s_log_async.loggers, logger->index, &last_logger);
    }

    if (logger->buffer)
    {
        if (logger->buffer->len > 0)
        {
            cra_log_ref(logger);
            logger->buffer->log = logger;
            cra_llist_append(s_log_async.buffers1, &logger->buffer);
            cra_cond_signal(&s_log_async.condi);
        }
        else
        {
            cra_log_output_async_put_buf(logger->buffer);
        }
        logger->buffer = NULL;
    }

    if (s_log_async.loggers.count == 0)
    {
        s_log_async.running = false;
        cra_cond_signal(&s_log_async.condi);

        cra_mutex_unlock(&s_log_async.mutex);

        cra_thrd_join(s_log_async.thrd);
        cra_log_output_async_uninit();
    }
    else
    {
        cra_mutex_unlock(&s_log_async.mutex);
    }

    cra_log_unref(logger);
}

#endif // end LogOutputAsync

#if 1 // Logger

const char *
cra_log_get_name(CraLogger *logger)
{
    return logger->name;
}

// "path/to/logname_yyyyMMdd_hhmmss_SSSZ.log"
// "path/to/logname_yyyyMMdd_hhmmss_SSS.log"
static void
cra_log_make_log_dir(CraLogger *logger, const char *path)
{
    assert(path);
    assert(logger);

    size_t name_len = strnlen(logger->name, CRA_LOG_NAME_MAX);
    size_t path_len = strnlen(path, CRA_LOG_FILENAME_MAX);
    size_t index = 0;

    if (path_len > 0)
    {
        size_t n = path_len + name_len + sizeof("/_yyyyMMdd_hhmmss_SSS.log") + (logger->use_zulu ? 1 : 0);
        if (n > CRA_LOG_FILENAME_MAX)
        {
            if (!CRA_IS_PATH_SEP(path[path_len - 1]) || (n - 1) > CRA_LOG_FILENAME_MAX)
            {
                fprintf(stderr, "Logger: `path` is too long.\n");
                exit(EXIT_FAILURE);
            }
        }

        memcpy(logger->filename, path, path_len);
        if (!CRA_IS_PATH_SEP(logger->filename[path_len - 1]))
            logger->filename[path_len++] = CRA_PATH_SEP1;
        logger->filename[path_len] = '\0';

        // create log dir if it does not exist
        int err = cra_mkdirs(logger->filename, 0755);
        if (err != 0)
        {
            fprintf(stderr, "Logger: failed to create log dir `%s` with error code %d.\n", logger->filename, err);
            exit(EXIT_FAILURE);
        }
    }

    memcpy(logger->filename + path_len, logger->name, name_len);
    index = path_len + name_len;
    logger->filename[index++] = '_';
    logger->filename[index] = '\0';
    logger->filename_time_start = (unsigned int)index;
}

void
cra_log_config(CraLogger *logger, unsigned int max_file_size, const char *log_dir)
{
    assert(logger);
    assert(log_dir);
    assert(logger->to_file);
    assert(max_file_size >= CRA_LOG_BUF_SIZE);

    logger->file_size_max = max_file_size;
    cra_log_make_log_dir(logger, log_dir);
}

CraLogger *
cra_log_open(const char *name, CraLogLv_e lv, bool use_zulu, bool output_to_file)
{
    CraLogger *logger;

    assert(name);

    logger = cra_alloc(CraLogger);
    if (!logger)
    {
        fprintf(stderr, "Logger: failed to create logger.\n");
        exit(EXIT_FAILURE);
    }

    bzero(logger, sizeof(*logger));

    logger->level = lv;
    logger->refcnt = 1;
    logger->active = true;
    logger->use_zulu = use_zulu;
    logger->to_file = output_to_file;
    snprintf(logger->name, sizeof(logger->name), "%s", name);

    if (!use_zulu)
    {
        time_t    now, local, utc;
        struct tm local_tm, utc_tm;

        now = time(NULL);
        cra_gmtime(now, &utc_tm);
        cra_localtime(now, &local_tm);

        local = mktime(&local_tm);
        utc = mktime(&utc_tm);

        logger->tz_hour = (short)((local - utc) / (3600));
    }

    if (output_to_file)
        cra_log_output_async_add_logger(logger);

    return logger;
}

void
cra_log_close(CraLogger *logger)
{
    assert(logger);
    logger->active = false;
    if (logger->to_file)
        cra_log_output_async_del_logger(logger);
    cra_log_unref(logger);
}

static inline void
cra_log_sync_append(char *msg, size_t n, CraLogLv_e lv)
{
    // TRACE: white
    // DEBUG: green
    // INFO:  blue
    // WARN:  yellow
    // ERROR: red
    // FATAL: bold red
    // RESET: no color
    static const char *lv_colors[] = { "\033[0;37m", "\033[0;32m", "\033[0;34m", "\033[0;33m",
                                       "\033[0;31m", "\033[1;31m", "\033[0m" };

    msg[n - 1] = '\0';
    printf("%s%s%s\n", lv_colors[lv], msg, lv_colors[6]);
}

void(cra_log_msg)(CraLogger *logger, CraLogLv_e lv, const char *fmt, ...)
{
    int         s;
    int         n;
    va_list     ap;
    CraDateTime dt;
    char        msg[CRA_LOG_LINE_MAX];

    assert(logger);
    assert(sizeof(msg) > 100);
    assert(sizeof(msg) <= INT_MAX);
    assert(logger->level <= lv);

    if (!logger->active)
        return;

    // format time

    if (logger->use_zulu)
    {
        cra_datetime_now_utc(&dt);
        n = snprintf(msg + 20, sizeof(msg) - 20, "%dZ", dt.ms);
        s = 5 - n;
    }
    else
    {
        cra_datetime_now_localtime(&dt);
        n = snprintf(msg + 20, sizeof(msg) - 20, "%d%+03hd:00", dt.ms, logger->tz_hour);
        s = 10 - n;
    }
    n += snprintf(msg, 20, "%4d-%02d-%02dT%02d:%02d:%02d", dt.year, dt.mon, dt.day, dt.hour, dt.min, dt.sec);
    msg[19] = '.';
    ++n;

    // format level & tid
    n +=
      snprintf(msg + n, sizeof(msg) - n, "%*.s%s %-8lu", s, "", cra_log_level_to_str(lv), cra_thrd_get_current_tid());

    // format message
    va_start(ap, fmt);
    n += vsnprintf(msg + n, sizeof(msg) - n, fmt, ap);
    va_end(ap);

    if (n >= (int)sizeof(msg))
    {
        n = sizeof(msg) - 5;
        msg[n++] = '.';
        msg[n++] = '.';
        msg[n++] = '.';
        msg[n++] = '\n';
        msg[n] = '\0';
    }

    if (logger->to_file)
    {
        // log to file
        cra_log_output_async_append(logger, msg, n);
    }
    else
    {
        // log to console
        cra_log_sync_append(msg, n, lv);
    }
}

#endif // end Logger
