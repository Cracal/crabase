#include "cra_buffer.h"
#include "cra_malloc.h"

bool
cra_buffer_init(CraBuffer *buffer, size_t init_size)
{
    assert(buffer);
    assert(init_size > 0);

    buffer->size = init_size;
    buffer->ridx = 0;
    buffer->widx = 0;
    return !!(buffer->data = cra_malloc(init_size));
}

void
cra_buffer_uninit(CraBuffer *buffer)
{
    assert(buffer);
    assert(buffer->data);

    cra_free(buffer->data);
    bzero(buffer, sizeof(*buffer));
}

bool
cra_buffer_append(CraBuffer *buffer, const void *data, size_t len)
{
    size_t nwritable;

    assert(data);
    assert(buffer);
    assert(len > 0);
    assert(buffer->data);
    assert(buffer->size >= buffer->widx);
    assert(buffer->widx >= buffer->ridx);

    nwritable = buffer->size - buffer->widx;
    if (nwritable < len)
    {
        if (nwritable + buffer->ridx >= len)
        {
            size_t         nreadable = cra_buffer_get_readable_size(buffer);
            unsigned char *dst = buffer->data;
            unsigned char *src = buffer->data + buffer->ridx;
            memmove(dst, src, nreadable);
            buffer->widx -= buffer->ridx;
            buffer->ridx = 0;
        }
        else
        {
            size_t         new_size = CRA_MAX(buffer->widx + len, buffer->size * 2);
            unsigned char *new_buff = cra_realloc(buffer->data, new_size);
            if (!new_buff)
                return false;
            buffer->data = new_buff;
            buffer->size = new_size;
        }
    }

    memcpy(buffer->data + buffer->widx, data, len);
    buffer->widx += len;
    return true;
}

size_t
cra_buffer_retrieve(CraBuffer *buffer, void *data, size_t len)
{
    size_t readed;
    void  *read_start;

    assert(data);
    assert(buffer);
    assert(buffer->data);
    assert(buffer->widx >= buffer->ridx);

    read_start = cra_buffer_get_read_start(buffer);
    readed = cra_buffer_retrieve_size(buffer, len);

    if (readed > 0)
        memcpy(data, read_start, readed);

    return readed;
}
