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
#ifndef __CRA_BUFFER_H__
#define __CRA_BUFFER_H__
#include "cra_defs.h"
#include "cra_assert.h"

typedef struct CraBuffer CraBuffer;

struct CraBuffer
{
    unsigned int   size; // capacity
    unsigned int   head; // head size
    unsigned int   ridx; // read index
    unsigned int   widx; // write index
    unsigned char *data; // data
};

#define CRA_BUFFER_HEAD_PTR(buffer) (buffer)->data
#define CRA_BUFFER_DATA_PTR(buffer) (CRA_BUFFER_HEAD_PTR(buffer) + (buffer)->head)

CRA_API bool
cra_buffer_init(CraBuffer *buffer, unsigned int init_size, unsigned int head_size);

CRA_API void
cra_buffer_uninit(CraBuffer *buffer);

CRA_API bool
cra_buffer_write_head(CraBuffer *buffer, const void *head);

CRA_API bool
cra_buffer_append(CraBuffer *buffer, const void *data, unsigned int len);

CRA_API unsigned int
cra_buffer_retrieve(CraBuffer *buffer, void *data, unsigned int len);

static inline unsigned int
cra_buffer_get_size(CraBuffer *buffer)
{
    assert(buffer);
    return buffer->size;
}

static inline unsigned int
cra_buffer_get_head_size(CraBuffer *buffer)
{
    assert(buffer);
    return buffer->head;
}

static inline unsigned int
cra_buffer_get_readable_size(CraBuffer *buffer)
{
    assert(buffer);
    assert(buffer->widx >= buffer->ridx);
    return buffer->widx - buffer->ridx;
}

static inline unsigned int
cra_buffer_get_readable_size_with_head(CraBuffer *buffer)
{
    return cra_buffer_get_readable_size(buffer) + cra_buffer_get_head_size(buffer);
}

static inline const void *
cra_buffer_get_read_start_with_head(CraBuffer *buffer)
{
    assert(buffer);
    assert(buffer->data);
    return CRA_BUFFER_HEAD_PTR(buffer) + buffer->ridx;
}

static inline void *
cra_buffer_get_read_start(CraBuffer *buffer)
{
    assert(buffer);
    assert(buffer->data);
    return CRA_BUFFER_DATA_PTR(buffer) + buffer->ridx;
}

static inline void *
cra_buffer_get_write_start(CraBuffer *buffer)
{
    assert(buffer);
    assert(buffer->data);
    return CRA_BUFFER_DATA_PTR(buffer) + buffer->widx;
}

static inline void
cra_buffer_reset(CraBuffer *buffer)
{
    assert(buffer);
    buffer->ridx = 0;
    buffer->widx = 0;
}

static inline void
cra_buffer_append_size(CraBuffer *buffer, unsigned int len)
{
    assert(buffer);
    assert(buffer->size >= buffer->widx);
    len = CRA_MIN(len, buffer->size - buffer->widx);
    buffer->widx += len;
}

static inline unsigned int
cra_buffer_retrieve_size(CraBuffer *buffer, unsigned int len)
{
    unsigned int nreadable = cra_buffer_get_readable_size(buffer);
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

#ifndef __CRA_BUFFER_IMPL__
#undef CRA_BUFFER_HEAD_PTR
#undef CRA_BUFFER_DATA_PTR
#endif

#endif