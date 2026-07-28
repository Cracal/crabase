#define __CRA_BUFFER_IMPL__
#include "cra_buffer.h"
#include "cra_malloc.h"

bool
cra_buffer_init(CraBuffer *buffer, unsigned int init_size, unsigned int head_size)
{
    assert(buffer);
    assert(init_size > 0);

    buffer->size = init_size;
    buffer->head = head_size;
    buffer->ridx = 0;
    buffer->widx = 0;
    return !!(buffer->data = cra_malloc(init_size + head_size));
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
cra_buffer_write_head(CraBuffer *buffer, const void *head)
{
    assert(buffer);
    assert(buffer->data);

    if (buffer->head > 0)
    {
        memcpy(CRA_BUFFER_HEAD_PTR(buffer) + buffer->ridx, head, buffer->head);
        return true;
    }
    return false;
}

bool
cra_buffer_append(CraBuffer *buffer, const void *data, unsigned int len)
{
    unsigned int nwritable;

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
            unsigned int   nreadable = cra_buffer_get_readable_size(buffer);
            unsigned char *dst = CRA_BUFFER_DATA_PTR(buffer);
            unsigned char *src = dst + buffer->ridx;
            memmove(dst, src, nreadable);
            buffer->widx -= buffer->ridx;
            buffer->ridx = 0;
        }
        else
        {
            unsigned int   new_size = CRA_MAX(buffer->widx + len, buffer->size * 2);
            unsigned char *new_buff = cra_realloc(buffer->data, new_size + buffer->head);
            if (!new_buff)
                return false;
            buffer->data = new_buff;
            buffer->size = new_size;
        }
    }

    memcpy(CRA_BUFFER_DATA_PTR(buffer) + buffer->widx, data, len);
    buffer->widx += len;
    return true;
}

unsigned int
cra_buffer_retrieve(CraBuffer *buffer, void *data, unsigned int len)
{
    unsigned int readed;
    void        *read_start;

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
