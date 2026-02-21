#include "cra_buffer.h"
#include "cra_assert.h"
#include "cra_malloc.h"

bool
cra_buffer_init(CraBuffer *buffer, size_t init_size)
{
    assert(buffer != NULL);
    assert_always(init_size > 0);

    buffer->size = init_size;
    buffer->read_idx = 0;
    buffer->write_idx = 0;
    buffer->buffer = (unsigned char *)cra_malloc(init_size);
    return !!buffer->buffer;
}

void
cra_buffer_uninit(CraBuffer *buffer)
{
    assert(buffer);
    assert(buffer->buffer);

    cra_free(buffer->buffer);
    bzero(buffer, sizeof(*buffer));
}

size_t
cra_buffer_resize(CraBuffer *buffer, size_t new_size)
{
    assert(buffer);
    assert(buffer->buffer);

    size_t readable = cra_buffer_readable(buffer);
    if (buffer->read_idx > 0)
    {
        memmove(buffer->buffer, cra_buffer_read_start(buffer), readable);
        buffer->write_idx -= buffer->read_idx;
        buffer->read_idx = 0;
    }

    if (new_size != 0 && (new_size = CRA_MAX(new_size, readable)) != buffer->size)
    {
        buffer->buffer = (unsigned char *)cra_realloc(buffer->buffer, new_size);
        if (buffer->buffer == NULL)
            return 0;
        buffer->size = new_size;
    }
    return buffer->size;
}

bool
cra_buffer_append(CraBuffer *buffer, const void *data, size_t len)
{
    assert(data);
    assert(buffer);
    assert(len > 0);
    assert(buffer->buffer);

    if (cra_buffer_writable(buffer) < len)
    {
        size_t new_size = CRA_MAX(buffer->size + (buffer->size >> 1), buffer->size + len);
        if (cra_buffer_resize(buffer, new_size) == 0)
            return false;
    }

    memcpy(cra_buffer_write_start(buffer), data, len);
    buffer->write_idx += len;
    return true;
}

size_t
cra_buffer_retrieve(CraBuffer *buffer, void *data, size_t len)
{
    assert(data);
    assert(buffer);
    assert(len > 0);
    assert(buffer->buffer);

    len = CRA_MIN(cra_buffer_readable(buffer), len);
    memcpy(data, cra_buffer_read_start(buffer), len);
    buffer->read_idx += len;
    if (buffer->read_idx == buffer->write_idx)
        buffer->read_idx = buffer->write_idx = 0;
    return len;
}

size_t
cra_buffer_append_size(CraBuffer *buffer, size_t len)
{
    assert(buffer);
    assert(len > 0);
    assert(buffer->buffer);

    len = CRA_MIN(len, cra_buffer_writable(buffer));
    buffer->write_idx += len;
    return len;
}

size_t
cra_buffer_retrieve_size(CraBuffer *buffer, size_t len)
{
    assert(buffer);
    assert(len > 0);
    assert(buffer->buffer);

    len = CRA_MIN(len, cra_buffer_readable(buffer));
    buffer->read_idx += len;
    if (buffer->read_idx == buffer->write_idx)
        buffer->read_idx = buffer->write_idx = 0;
    return len;
}
