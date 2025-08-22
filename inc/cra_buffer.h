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

typedef struct _CraBuffer
{
    size_t         size;
    size_t         read_idx;
    size_t         write_idx;
    unsigned char *buffer;
} CraBuffer;

CRA_API void
cra_buffer_init(CraBuffer *buffer, size_t init_size);

CRA_API void
cra_buffer_uninit(CraBuffer *buffer);

#define cra_buffer_readable(_buffer) ((_buffer)->write_idx - (_buffer)->read_idx)
#define cra_buffer_writable(_buffer) ((_buffer)->size - (_buffer)->write_idx)
#define cra_buffer_size(_buffer)     ((_buffer)->size)

#define cra_buffer_read_start(_buffer)  ((_buffer)->buffer + (_buffer)->read_idx)
#define cra_buffer_write_start(_buffer) ((_buffer)->buffer + (_buffer)->write_idx)

CRA_API size_t
cra_buffer_resize(CraBuffer *buffer, size_t new_size);

CRA_API void
cra_buffer_append(CraBuffer *buffer, const void *data, size_t len);

CRA_API size_t
cra_buffer_retrieve(CraBuffer *buffer, void *data, size_t len);

CRA_API size_t
cra_buffer_append_size(CraBuffer *buffer, size_t len);

CRA_API size_t
cra_buffer_retrieve_size(CraBuffer *buffer, size_t len);

static inline void
cra_buffer_retrieve_all_size(CraBuffer *buffer)
{
    buffer->read_idx = 0;
    buffer->write_idx = 0;
}

#endif