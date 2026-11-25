/**
 * @file test-buffer.c
 * @author Cracal
 * @brief test buffer
 * @version 0.1
 * @date 2025-01-14
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "crabase/cra_assert.h"
#include "crabase/cra_buffer.h"
#include "crabase/cra_malloc.h"

void
random_fill(char *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (rand() % 2 == 0)
            data[i] = 'a' + rand() % 26;
        else
            data[i] = 'A' + rand() % 26;
    }
}

void
test_buffer(void)
{
    CraBuffer buffer;
    char      src[256];
    char      tmp[256];

    random_fill(src, sizeof(src) - 1);
    src[sizeof(src) - 1] = '\0';

    assert_always(cra_buffer_init(&buffer, 128));
    assert_always(cra_buffer_get_size(&buffer) == 128);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    assert_always(cra_buffer_retrieve(&buffer, tmp, 10) == 0);
    assert_always(cra_buffer_retrieve_size(&buffer, 10) == 0);

    assert_always(cra_buffer_append(&buffer, src, sizeof(src)));
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src));

    assert_always(cra_buffer_retrieve(&buffer, tmp, 60) == 60);
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src) - 60);
    assert_always(strncmp(tmp, src, 60) == 0);

    assert_always(cra_buffer_retrieve_size(&buffer, 60) == 60);
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src) - 60 - 60);

    assert_always(cra_buffer_retrieve(&buffer, tmp, sizeof(tmp)) == sizeof(src) - 60 - 60);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    cra_buffer_append_size(&buffer, 100);
    assert_always(cra_buffer_get_readable_size(&buffer) == 100);

    cra_buffer_reset(&buffer);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    cra_buffer_append_size(&buffer, 100);
    assert_always(cra_buffer_get_readable_size(&buffer) == 100);

    assert_always(cra_buffer_retrieve(&buffer, tmp, 50) == 50);
    assert_always(cra_buffer_get_readable_size(&buffer) == 50);

    // free space > append size
    assert_always(cra_buffer_append(&buffer, src, 200));
    assert_always(cra_buffer_get_readable_size(&buffer) == 250);

    cra_buffer_reset(&buffer);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    cra_buffer_append_size(&buffer, 100);
    assert_always(cra_buffer_get_readable_size(&buffer) == 100);

    assert_always(cra_buffer_retrieve(&buffer, tmp, 50) == 50);
    assert_always(cra_buffer_get_readable_size(&buffer) == 50);

    // free space == append size
    assert_always(cra_buffer_append(&buffer, src, 206));
    assert_always(cra_buffer_get_readable_size(&buffer) == 256);

    cra_buffer_reset(&buffer);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    cra_buffer_append_size(&buffer, 100);
    assert_always(cra_buffer_get_readable_size(&buffer) == 100);

    assert_always(cra_buffer_retrieve(&buffer, tmp, 50) == 50);
    assert_always(cra_buffer_get_readable_size(&buffer) == 50);

    // free space < append size
    assert_always(cra_buffer_append(&buffer, src, 210));
    assert_always(cra_buffer_get_readable_size(&buffer) == 260);

    assert_always(cra_buffer_retrieve_size(&buffer, 100) == 100);
    assert_always(cra_buffer_get_readable_size(&buffer) == 160);

    assert_always(cra_buffer_retrieve_size(&buffer, 10000) == 160);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    assert_always(cra_buffer_append(&buffer, src, sizeof(src)));
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src));
    assert_always(cra_buffer_append(&buffer, src, sizeof(src)));
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src) * 2);

    assert_always(cra_buffer_append(&buffer, src, 1));
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src) * 2 + 1);

    // test with head length

    void    *data;
    uint32_t len, read_len;
    cra_buffer_reset(&buffer);
    cra_buffer_append_size(&buffer, sizeof(len));
    cra_buffer_append(&buffer, "hello", 5);
    cra_buffer_append(&buffer, " world", 6);
    len = (uint32_t)(cra_buffer_get_readable_size(&buffer) - sizeof(len));
    data = cra_buffer_get_data(&buffer);
    memcpy(data, &len, sizeof(len));
    assert_always(len == 5 + 6);

    assert_always(cra_buffer_retrieve(&buffer, &read_len, sizeof(read_len)) == sizeof(read_len));
    assert_always(read_len == len);
    assert_always(cra_buffer_retrieve(&buffer, tmp, sizeof(tmp)) == (size_t)len);
    assert_always(strncmp(tmp, "hello world", len) == 0);

    cra_buffer_uninit(&buffer);
}

void
test_buf_fixed(void)
{
    CraBufFixed *buf = cra_buf_fixed_new(128);
    assert_always(buf != NULL);
    assert_always(buf->size == 128);
    assert_always(buf->curr == 0);
    assert_always(cra_buf_fixed_get_remaining_size(buf) == 128);

    assert_always(cra_buf_fixed_append(buf, "hello", 5));
    assert_always(buf->curr == 5);
    assert_always(cra_buf_fixed_get_remaining_size(buf) == 123);
    assert_always(strncmp((char *)buf->data, "hello", 5) == 0);
    char buff[1000] = { 0 };
    assert_always(!cra_buf_fixed_append(buf, buff, sizeof(buff)));
    assert_always(buf->curr == 5);
    assert_always(cra_buf_fixed_get_remaining_size(buf) == 123);
    assert_always(strncmp((char *)buf->data, "hello", 5) == 0);

    assert_always(cra_buf_fixed_retrieve(buf, buff, 5) == 5);
    assert_always(strncmp(buff, "hello", 5) == 0);
    assert_always(buf->curr == 0);
    assert_always(cra_buf_fixed_get_remaining_size(buf) == 128);

    assert_always(0 == cra_buf_fixed_retrieve(buf, buff, sizeof(buff)));
    assert_always(0 == cra_buf_fixed_retrieve_size(buf, 100));

    cra_buf_fixed_append_size(buf, 100);
    assert_always(buf->curr == 100);
    assert_always(cra_buf_fixed_get_remaining_size(buf) == 128 - 100);
    assert_always(50 == cra_buf_fixed_retrieve_size(buf, 50));
    assert_always(50 == cra_buf_fixed_retrieve_size(buf, 50));

    cra_buf_fixed_del(buf);
}

int
main(void)
{
    test_buffer();
    test_buf_fixed();
    cra_memory_leak_report();
    return 0;
}
