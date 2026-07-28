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
#include "cra_assert.h"
#include "cra_buffer.h"
#include "cra_malloc.h"

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

    assert_always(cra_buffer_init(&buffer, 128, 0));
    assert_always(cra_buffer_get_size(&buffer) == 128);
    assert_always(cra_buffer_get_head_size(&buffer) == 0);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    assert_always(cra_buffer_retrieve(&buffer, tmp, 10) == 0);
    assert_always(cra_buffer_retrieve_size(&buffer, 10) == 0);

    assert_always(cra_buffer_append(&buffer, src, sizeof(src)));
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src));
    assert_always(cra_buffer_get_head_size(&buffer) == 0);

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
    assert_always(cra_buffer_get_size(&buffer) == 256);

    // free space > append size
    assert_always(cra_buffer_append(&buffer, src, 200));
    assert_always(cra_buffer_get_size(&buffer) == 256);
    assert_always(cra_buffer_get_readable_size(&buffer) == 250);

    cra_buffer_reset(&buffer);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    cra_buffer_append_size(&buffer, 100);
    assert_always(cra_buffer_get_readable_size(&buffer) == 100);

    assert_always(cra_buffer_retrieve(&buffer, tmp, 50) == 50);
    assert_always(cra_buffer_get_readable_size(&buffer) == 50);
    assert_always(cra_buffer_get_size(&buffer) == 256);

    // free space == append size
    assert_always(cra_buffer_append(&buffer, src, 206));
    assert_always(cra_buffer_get_size(&buffer) == 256);
    assert_always(cra_buffer_get_readable_size(&buffer) == 256);

    cra_buffer_reset(&buffer);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    cra_buffer_append_size(&buffer, 100);
    assert_always(cra_buffer_get_readable_size(&buffer) == 100);

    assert_always(cra_buffer_retrieve(&buffer, tmp, 50) == 50);
    assert_always(cra_buffer_get_readable_size(&buffer) == 50);
    assert_always(cra_buffer_get_size(&buffer) == 256);

    // free space < append size
    assert_always(cra_buffer_append(&buffer, src, 210));
    assert_always(cra_buffer_get_size(&buffer) == 512);
    assert_always(cra_buffer_get_readable_size(&buffer) == 260);

    assert_always(cra_buffer_retrieve_size(&buffer, 100) == 100);
    assert_always(cra_buffer_get_readable_size(&buffer) == 160);

    assert_always(cra_buffer_retrieve_size(&buffer, 10000) == 160);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);
    assert_always(cra_buffer_get_size(&buffer) == 512);

    assert_always(cra_buffer_append(&buffer, src, sizeof(src)));
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src));
    assert_always(cra_buffer_append(&buffer, src, sizeof(src)));
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src) * 2);

    assert_always(cra_buffer_append(&buffer, src, 1));
    assert_always(cra_buffer_get_size(&buffer) == 1024);
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src) * 2 + 1);

    assert_always(cra_buffer_get_head_size(&buffer) == 0);
    assert_always(!cra_buffer_write_head(&buffer, &(int64_t){ 120098 }));

    cra_buffer_uninit(&buffer);
}

void
test_buffer_head(void)
{
    CraBuffer buffer;
    char      src[256];
    char      tmp[256];

    random_fill(src, sizeof(src) - 1);
    src[sizeof(src) - 1] = '\0';

    assert_always(cra_buffer_init(&buffer, 128, 8));
    assert_always(cra_buffer_get_size(&buffer) == 128);
    assert_always(cra_buffer_get_head_size(&buffer) == 8);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    assert_always(cra_buffer_retrieve(&buffer, tmp, 10) == 0);
    assert_always(cra_buffer_retrieve_size(&buffer, 10) == 0);

    assert_always(cra_buffer_append(&buffer, src, sizeof(src)));
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src));
    assert_always(cra_buffer_get_head_size(&buffer) == 8);

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
    assert_always(cra_buffer_get_size(&buffer) == 256);

    // free space > append size
    assert_always(cra_buffer_append(&buffer, src, 200));
    assert_always(cra_buffer_get_size(&buffer) == 256);
    assert_always(cra_buffer_get_readable_size(&buffer) == 250);

    cra_buffer_reset(&buffer);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    cra_buffer_append_size(&buffer, 100);
    assert_always(cra_buffer_get_readable_size(&buffer) == 100);

    assert_always(cra_buffer_retrieve(&buffer, tmp, 50) == 50);
    assert_always(cra_buffer_get_readable_size(&buffer) == 50);
    assert_always(cra_buffer_get_size(&buffer) == 256);

    // free space == append size
    assert_always(cra_buffer_append(&buffer, src, 206));
    assert_always(cra_buffer_get_size(&buffer) == 256);
    assert_always(cra_buffer_get_readable_size(&buffer) == 256);

    cra_buffer_reset(&buffer);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);

    cra_buffer_append_size(&buffer, 100);
    assert_always(cra_buffer_get_readable_size(&buffer) == 100);

    assert_always(cra_buffer_retrieve(&buffer, tmp, 50) == 50);
    assert_always(cra_buffer_get_readable_size(&buffer) == 50);
    assert_always(cra_buffer_get_size(&buffer) == 256);

    // free space < append size
    assert_always(cra_buffer_append(&buffer, src, 210));
    assert_always(cra_buffer_get_size(&buffer) == 512);
    assert_always(cra_buffer_get_readable_size(&buffer) == 260);

    assert_always(cra_buffer_retrieve_size(&buffer, 100) == 100);
    assert_always(cra_buffer_get_readable_size(&buffer) == 160);

    assert_always(cra_buffer_retrieve_size(&buffer, 10000) == 160);
    assert_always(cra_buffer_get_readable_size(&buffer) == 0);
    assert_always(cra_buffer_get_size(&buffer) == 512);

    assert_always(cra_buffer_append(&buffer, src, sizeof(src)));
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src));
    assert_always(cra_buffer_append(&buffer, src, sizeof(src)));
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src) * 2);

    assert_always(cra_buffer_append(&buffer, src, 1));
    assert_always(cra_buffer_get_size(&buffer) == 1024);
    assert_always(cra_buffer_get_readable_size(&buffer) == sizeof(src) * 2 + 1);

    assert_always(cra_buffer_get_head_size(&buffer) == 8);
    assert_always(cra_buffer_write_head(&buffer, &(int64_t){ 120098 }));
    char    *data = (char *)cra_buffer_get_read_start_with_head(&buffer);
    char    *str = data + sizeof(int64_t);
    int64_t *pi = (int64_t *)data;
    assert_always(*pi == 120098);
    assert_always(strncmp(str, src, 30) == 0);

    cra_buffer_uninit(&buffer);
}

int
main(void)
{
    test_buffer();
    test_buffer_head();

    cra_memory_leak_report();
    return 0;
}
