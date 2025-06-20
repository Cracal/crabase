#include "cra_buffer.h"
#include "cra_assert.h"
#include "cra_malloc.h"

void cra_buffer_init(CraBuffer *buffer, unsigned int init_size)
{
    assert(buffer != NULL);
    assert_always(init_size > 0);

    buffer->size = init_size;
    buffer->read_idx = 0;
    buffer->write_idx = 0;
    buffer->buffer = cra_malloc(init_size);
}

void cra_buffer_uninit(CraBuffer *buffer)
{
    cra_free(buffer->buffer);
    bzero(buffer, sizeof(*buffer));
}

static void cra_buffer_expand(CraBuffer *buffer, unsigned int len)
{
    if (cra_buffer_writable(buffer) + buffer->read_idx < len)
    {
        buffer->buffer = cra_realloc(buffer->buffer, buffer->write_idx + len);
        buffer->size = buffer->write_idx + len;
    }
    else
    {
        memmove(buffer->buffer, cra_buffer_read_start(buffer), buffer->read_idx);
        buffer->write_idx -= buffer->read_idx;
        buffer->read_idx = 0;
    }
}

void cra_buffer_append(CraBuffer *buffer, const void *data, unsigned int len)
{
    if (cra_buffer_writable(buffer) < len)
        cra_buffer_expand(buffer, len);

    memcpy(cra_buffer_write_start(buffer), data, len);
    buffer->write_idx += len;
}

unsigned int cra_buffer_retrieve(CraBuffer *buffer, void *data, unsigned int len)
{
    len = CRA_MIN(cra_buffer_readable(buffer), len);
    memcpy(data, cra_buffer_read_start(buffer), len);
    buffer->read_idx += len;
    if (buffer->read_idx == buffer->write_idx)
        buffer->read_idx = buffer->write_idx = 0;
    return len;
}

unsigned int cra_buffer_append_size(CraBuffer *buffer, unsigned int len)
{
    len = CRA_MIN(len, cra_buffer_writable(buffer));
    buffer->write_idx += len;
    return len;
}

unsigned int cra_buffer_retrieve_size(CraBuffer *buffer, unsigned int len)
{
    len = CRA_MIN(len, cra_buffer_readable(buffer));
    buffer->read_idx += len;
    if (buffer->read_idx == buffer->write_idx)
        buffer->read_idx = buffer->write_idx = 0;
    return len;
}
