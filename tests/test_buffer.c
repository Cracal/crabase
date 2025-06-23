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
#include "cra_malloc.h"
#include "cra_assert.h"
#include "cra_buffer.h"

static void test_new_delete(void)
{
    CraBuffer *buf;

    // buf = cra_alloc(CraBuffer);
    // cra_buffer_init(NULL, 10);
    // cra_buffer_init(buf, 0);

    buf = cra_alloc(CraBuffer);
    cra_buffer_init(buf, 1024);
    cra_buffer_uninit(buf);
    cra_dealloc(buf);
}

static void test_append(void)
{
    CraBuffer *buf = cra_alloc(CraBuffer);
    cra_buffer_init(buf, 10);
    assert_always(cra_buffer_writable(buf) == cra_buffer_size(buf));

    cra_buffer_append(buf, "hello", sizeof("hello"));
    assert_always(cra_buffer_readable(buf) == sizeof("hello"));
    assert_always(cra_buffer_writable(buf) == cra_buffer_size(buf) - sizeof("hello"));

    cra_buffer_append(buf, "world", sizeof("world"));
    assert_always(cra_buffer_readable(buf) == sizeof("hello") + sizeof("world"));
    assert_always(cra_buffer_writable(buf) == cra_buffer_size(buf) - (sizeof("hello") + sizeof("world")));

    // 取后再加
    char buff[100];
    assert_always(cra_buffer_retrieve(buf, buff, sizeof("hello")) == sizeof("hello"));
    assert_always(strcmp(buff, "hello") == 0);
    cra_buffer_append(buf, "after", sizeof("after"));
    assert_always(cra_buffer_readable(buf) == sizeof("after") + sizeof("world"));
    assert_always(cra_buffer_writable(buf) == cra_buffer_size(buf) - (sizeof("after") + sizeof("world")));

    cra_buffer_uninit(buf);
    cra_dealloc(buf);
}

static void test_expand(void)
{
    CraBuffer *buffer = cra_alloc(CraBuffer);
    cra_buffer_init(buffer, 10);

    cra_buffer_append(buffer, "123456789", 9);
    assert_always(cra_buffer_size(buffer) == 10);
    assert_always(cra_buffer_readable(buffer) == 9);

    cra_buffer_append(buffer, "A", 1);
    assert_always(cra_buffer_size(buffer) == 10);
    assert_always(cra_buffer_readable(buffer) == 10);

    cra_buffer_append(buffer, "B", 1); // expand
    assert_always(cra_buffer_size(buffer) > 10);
    assert_always(cra_buffer_readable(buffer) == 11);

    size_t size;
    size = cra_buffer_resize(buffer, 100); // expand
    assert_always(size == 100);
    assert_always(cra_buffer_size(buffer) == 100);

    size = cra_buffer_resize(buffer, 50); // downscale
    assert_always(size == 50);
    assert_always(cra_buffer_size(buffer) == 50);

    size = cra_buffer_resize(buffer, 11); // downscale to length of buffer
    assert_always(size == 11);
    assert_always(cra_buffer_size(buffer) == 11);

    size = cra_buffer_resize(buffer, 8); // mustn't less than readable
    assert_always(size == 11);
    assert_always(cra_buffer_size(buffer) == 11);

    size = cra_buffer_resize(buffer, 0);
    assert_always(size == cra_buffer_size(buffer));

    cra_buffer_retrieve_size(buffer, 1);
    assert_always(buffer->read_idx > 0);
    size = cra_buffer_resize(buffer, 100);
    assert_always(size == 100);
    assert_always(buffer->read_idx == 0);

    cra_buffer_retrieve_size(buffer, 1);
    assert_always(buffer->read_idx > 0);
    size = cra_buffer_resize(buffer, 50);
    assert_always(size == 50);
    assert_always(buffer->read_idx == 0);

    cra_buffer_retrieve_size(buffer, 1);
    assert_always(buffer->read_idx > 0);
    size = cra_buffer_resize(buffer, 3);
    assert_always(size == 8);
    assert_always(buffer->read_idx == 0);

    cra_buffer_retrieve_size(buffer, 1);
    assert_always(buffer->read_idx > 0);
    size = cra_buffer_resize(buffer, 0);
    assert_always(size == 8);
    assert_always(buffer->read_idx == 0);

    cra_buffer_uninit(buffer);
    cra_dealloc(buffer);
}

static void test_retrieve(void)
{
    CraBuffer buf;
    char buff[100];

    cra_buffer_init(&buf, 10);

    assert_always(cra_buffer_retrieve(&buf, buff, 10) == 0);

    cra_buffer_append(&buf, "hello world", sizeof("hello world"));
    assert_always(cra_buffer_readable(&buf) == sizeof("hello world"));

    size_t len = cra_buffer_retrieve(&buf, buff, sizeof("hello") - 1);
    assert_always(len == sizeof("hello") - 1);
    assert_always(strncmp(buff, "hello", sizeof("hello") - 1) == 0);
    assert_always(cra_buffer_readable(&buf) == sizeof("hello world") - (sizeof("hello") - 1));

    len = cra_buffer_retrieve(&buf, buff, sizeof(" world"));
    assert_always(len == sizeof(" world"));
    assert_always(strcmp(buff, " world") == 0);
    assert_always(cra_buffer_readable(&buf) == 0);
    assert_always(cra_buffer_writable(&buf) == cra_buffer_size(&buf));

    cra_buffer_uninit(&buf);
}

static void test_append_retrieve_size(void)
{
    CraBuffer *buf = cra_alloc(CraBuffer);
    cra_buffer_init(buf, 100);
    assert_always(cra_buffer_readable(buf) == 0);
    assert_always(cra_buffer_writable(buf) == cra_buffer_size(buf));

    size_t res = cra_buffer_retrieve_size(buf, 10);
    assert_always(res == 0);

    res = cra_buffer_append_size(buf, 1000);
    assert_always(res == 100);
    assert_always(cra_buffer_readable(buf) == 100);

    res = cra_buffer_retrieve_size(buf, 100);
    assert_always(res == 100);
    assert_always(cra_buffer_readable(buf) == 0);

    res = cra_buffer_append_size(buf, 20);
    assert_always(res == 20);
    assert_always(cra_buffer_readable(buf) == 20);

    res = cra_buffer_retrieve_size(buf, 5);
    assert_always(res == 5);
    assert_always(cra_buffer_readable(buf) == 20 - 5);

    res = cra_buffer_append_size(buf, 50);
    assert_always(res == 50);
    assert_always(cra_buffer_readable(buf) == 50 + (20 - 5));

    res = cra_buffer_retrieve_size(buf, 1000);
    assert_always(res == 65);
    assert_always(cra_buffer_readable(buf) == 0);

    cra_buffer_uninit(buf);
    cra_dealloc(buf);
}

int main(void)
{
    test_new_delete();
    test_append();
    test_expand();
    test_retrieve();
    test_append_retrieve_size();

    cra_memory_leak_report(stdout);
    return 0;
}
