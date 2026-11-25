/**
 * @file cra_buffer.h
 * @author Cracal
 * @brief buffer
 * @version 0.1
 * @date 2025-01-14
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRABASE_CRA_BUFFER_H__
#define __CRABASE_CRA_BUFFER_H__
#include "cra_defs.h"
#include "cra_assert.h"

typedef struct CraBufFixed CraBufFixed;
typedef struct CraBuffer   CraBuffer;

struct CraBufFixed
{
    uint32_t      size;   // capacity
    uint32_t      curr;   // current size
    unsigned char data[]; // data
};

struct CraBuffer
{
    size_t         size; // capacity
    size_t         ridx; // read index
    size_t         widx; // write index
    unsigned char *data; // data
};

CRA_API CraBufFixed *
cra_buf_fixed_new(uint32_t size);

CRA_API void
cra_buf_fixed_del(CraBufFixed *buf);

static inline uint32_t
cra_buf_fixed_get_remaining_size(CraBufFixed *buf)
{
    return buf->size - buf->curr;
}

static inline bool
cra_buf_fixed_append(CraBufFixed *buf, const void *data, uint32_t len)
{
    if (cra_buf_fixed_get_remaining_size(buf) < len)
        return false;
    memcpy(buf->data + buf->curr, data, len);
    buf->curr += len;
    return true;
}

static inline uint32_t
cra_buf_fixed_retrieve(CraBufFixed *buf, void *data, uint32_t len)
{
    uint32_t n = CRA_MIN(len, buf->curr);
    if (n > 0)
    {
        memcpy(data, buf->data, n);
        buf->curr -= n;
    }
    return n;
}

static inline void
cra_buf_fixed_append_size(CraBufFixed *buf, uint32_t len)
{
    uint32_t n = CRA_MIN(len, cra_buf_fixed_get_remaining_size(buf));
    buf->curr += n;
}

static inline uint32_t
cra_buf_fixed_retrieve_size(CraBufFixed *buf, uint32_t len)
{
    uint32_t n = CRA_MIN(len, buf->curr);
    if (n > 0)
        buf->curr -= n;
    return n;
}

// =================

CRA_API bool
cra_buffer_init(CraBuffer *buffer, size_t init_size);

CRA_API void
cra_buffer_uninit(CraBuffer *buffer);

CRA_API bool
cra_buffer_append(CraBuffer *buffer, const void *data, size_t len);

CRA_API size_t
cra_buffer_retrieve(CraBuffer *buffer, void *data, size_t len);

static inline size_t
cra_buffer_get_size(CraBuffer *buffer)
{
    assert(buffer);
    return buffer->size;
}

static inline void *
cra_buffer_get_data(CraBuffer *buffer)
{
    assert(buffer);
    return buffer->data;
}

static inline size_t
cra_buffer_get_readable_size(CraBuffer *buffer)
{
    assert(buffer);
    assert(buffer->widx >= buffer->ridx);
    return buffer->widx - buffer->ridx;
}

static inline void *
cra_buffer_get_read_start(CraBuffer *buffer)
{
    assert(buffer);
    assert(buffer->data);
    return buffer->data + buffer->ridx;
}

static inline void *
cra_buffer_get_write_start(CraBuffer *buffer)
{
    assert(buffer);
    assert(buffer->data);
    return buffer->data + buffer->widx;
}

static inline void
cra_buffer_reset(CraBuffer *buffer)
{
    assert(buffer);
    buffer->ridx = 0;
    buffer->widx = 0;
}

static inline void
cra_buffer_append_size(CraBuffer *buffer, size_t len)
{
    assert(buffer);
    assert(buffer->size >= buffer->widx);
    len = CRA_MIN(len, buffer->size - buffer->widx);
    buffer->widx += len;
}

static inline size_t
cra_buffer_retrieve_size(CraBuffer *buffer, size_t len)
{
    size_t nreadable = cra_buffer_get_readable_size(buffer);
    if (len < nreadable)
    {
        buffer->ridx += len;
        return len;
    }
    else
    {
        cra_buffer_reset(buffer);
        return nreadable;
    }
}

#endif
