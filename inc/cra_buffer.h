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
    unsigned int size;
    unsigned int read_idx;
    unsigned int write_idx;
    unsigned char *buffer;
} CraBuffer;

CRA_API void cra_buffer_init(CraBuffer *buffer, unsigned int init_size);
CRA_API void cra_buffer_uninit(CraBuffer *buffer);

#define cra_buffer_readable(_buffer) ((_buffer)->write_idx - (_buffer)->read_idx)
#define cra_buffer_writable(_buffer) ((_buffer)->size - (_buffer)->write_idx)
#define cra_buffer_size(_buffer) ((_buffer)->size)

#define cra_buffer_read_start(_buffer) ((_buffer)->buffer + (_buffer)->read_idx)
#define cra_buffer_write_start(_buffer) ((_buffer)->buffer + (_buffer)->write_idx)

CRA_API void cra_buffer_append(CraBuffer *buffer, const void *data, unsigned int len);
CRA_API unsigned int cra_buffer_retrieve(CraBuffer *buffer, void *data, unsigned int len);

CRA_API unsigned int cra_buffer_append_size(CraBuffer *buffer, unsigned int len);
CRA_API unsigned int cra_buffer_retrieve_size(CraBuffer *buffer, unsigned int len);

#endif