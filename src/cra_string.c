#include <stdarg.h>
#include "cra_string.h"
#include "cra_malloc.h"

#if 1 // SSO string

bool
cra_string_init(CraString *str, const char *data, size_t size)
{
    char  *s;
    size_t n;

    assert(str);
    assert(size <= CRA_STRING_MAX_LEN);

    n = data ? size : 0;

    if (size < CRA_STRING_SHORT_MAX)
    {
        s = str->_short.data;
        str->is_short = true;
        str->_short.size = (uint8_t)n;
    }
    else
    {
        s = str->_long.data = cra_malloc(size + 1);
        if (!s)
            return false;
        str->_long.size = n;
        str->is_short = false;
        str->_long.capacity = size;
    }

    if (data)
        memcpy(s, data, n);
    s[n] = '\0';

    return true;
}

bool
cra_string_init_fmt(CraString *str, const char *fmt, ...)
{
    int     n;
    va_list ap;
    char   *data;

    assert(str);
    assert(fmt);

    va_start(ap, fmt);
    n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (n < 0)
        return false;

    if (n < (int)CRA_STRING_SHORT_MAX)
    {
        data = str->_short.data;
        str->is_short = true;
        str->_short.size = (uint8_t)n;
    }
    else
    {
        data = str->_long.data = cra_malloc(n + 1);
        if (!data)
            return false;
        str->_long.size = n;
        str->is_short = false;
        str->_long.capacity = n;
    }

    va_start(ap, fmt);
    vsnprintf(data, n + 1, fmt, ap);
    va_end(ap);

    return true;
}

void
cra_string_uninit(CraString *str)
{
    assert(str);

    if (!cra_string_is_short(str))
    {
        assert(str->_long.data);
        cra_free(str->_long.data);
    }

    bzero(str, sizeof(CraString));
}

static inline bool
cra_string_ensure(CraString *str, size_t needed)
{
    char  *s;
    bool   res;
    size_t len, cap;

    len = cra_string_size(str);
    if (len > CRA_STRING_MAX_LEN - needed)
        return false; // overflow

    cap = len + needed;
    assert((cap & 0x8000000000000000) == 0);

    res = cra_string_is_short(str);
    if (res && cap >= CRA_STRING_SHORT_MAX)
    {
        cap = CRA_MAX(cap, 128);
        s = cra_malloc(cap + 1);
        if (!s)
            return false;

        memcpy(s, str->_short.data, len + 1); // include '\0'
        str->_long.capacity = cap;
        str->_long.size = len;
        str->_long.data = s;
        str->is_short = false;

        return true;
    }

    if (!res && cap >= str->_long.capacity)
    {
        cap = CRA_MAX(cap, str->_long.capacity * 2);
        s = cra_realloc(str->_long.data, cap + 1);
        if (!s)
            return false;

        str->_long.capacity = cap;
        str->_long.data = s;
        return true;
    }

    return true; // no need to realloc
}

static inline void
cra_string_set_size(CraString *str, size_t size)
{
    if (cra_string_is_short(str))
        str->_short.size = (uint8_t)size;
    else
        str->_long.size = size;
}

bool
cra_string_append(CraString *str, const char *data, size_t size)
{
    char  *s;
    size_t n;

    assert(str);
    assert(data);

    if (!cra_string_ensure(str, size))
        return false;

    s = cra_string_data(str);
    n = cra_string_size(str);
    memcpy(s + n, data, size);
    s[n + size] = '\0';
    cra_string_set_size(str, n + size);

    return true;
}

bool
cra_string_append_fmt(CraString *str, const char *fmt, ...)
{
    char   *s;
    int     n;
    size_t  l;
    va_list ap;

    assert(str);
    assert(fmt);

    va_start(ap, fmt);
    n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (n < 0)
        return false;

    if (!cra_string_ensure(str, n))
        return false;

    s = cra_string_data(str);
    l = cra_string_size(str);

    va_start(ap, fmt);
    vsnprintf(s + l, n + 1, fmt, ap);
    va_end(ap);

    cra_string_set_size(str, l + n);

    return true;
}

#endif // end SSO string

#if 1 // string view

bool
cra_string_view_find_cstr(const CraStringView *sv, const char *seq, size_t size, size_t *retindex)
{
    size_t i, j;
    size_t n, m;

    assert(seq);
    assert(retindex);

    if (size == 0)
    {
        *retindex = 0;
        return true;
    }

    n = sv->size;
    m = size;
    if (m > n)
        return false;

    CRA_TEMP_NEW(size_t, next, m);
    if (next == NULL)
        return false; // memory allocation failed

    next[0] = 0;
    for (j = 0, i = 1; i < m; ++i)
    {
        while (j > 0 && seq[j] != seq[i])
            j = next[j - 1];
        if (seq[j] == seq[i])
            ++j;
        next[i] = j;
    }

    for (j = 0, i = 0; i < n; ++i)
    {
        while (j > 0 && seq[j] != sv->data[i])
            j = next[j - 1];
        if (seq[j] == sv->data[i])
            ++j;
        if (j == m)
        {
            CRA_TEMP_DEL(size_t, next, m);
            *retindex = i - m + 1;
            return true;
        }
    }
    CRA_TEMP_DEL(size_t, next, m);
    return false;
}

bool
cra_string_view_rfind_cstr(const CraStringView *sv, const char *seq, size_t size, size_t *retindex)
{
    size_t i, j;
    size_t n, m;

    assert(seq);
    assert(retindex);

    if (size == 0)
    {
        *retindex = 0;
        return true;
    }

    n = sv->size;
    m = size;
    if (m > n)
        return false;

    CRA_TEMP_NEW(size_t, next, m);
    if (next == NULL)
        return false; // memory allocation failed

    next[0] = 0;
    for (j = 0, i = 1; i < m; ++i)
    {
        while (j > 0 && seq[m - 1 - j] != seq[m - 1 - i])
            j = next[j - 1];
        if (seq[m - 1 - j] == seq[m - 1 - i])
            ++j;
        next[i] = j;
    }

    assert(n > 0);
    for (j = 0, i = n; i > 0; --i)
    {
        while (j > 0 && seq[m - 1 - j] != sv->data[i - 1])
            j = next[j - 1];
        if (seq[m - 1 - j] == sv->data[i - 1])
            ++j;
        if (j == m)
        {
            CRA_TEMP_DEL(size_t, next, m);
            *retindex = i - 1;
            return true;
        }
    }
    CRA_TEMP_DEL(size_t, next, m);
    return false;
}

const uint8_t cra_g_string_view_bytes_for_utf8[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6,
};

bool
cra_string_view_is_valid_utf8_cstr(const char *str, size_t size)
{
    size_t               i;
    unsigned char        c;
    const unsigned char *p;

    if (!str)
        return false;

    i = 0;
    p = (const unsigned char *)str;

    while (i < size)
    {
        c = p[i];

        // ASCII:
        //      0xxxxxxx
        if (c <= 0x7F)
        {
            ++i;
            continue;
        }

        // 2bytes sequence:
        //      110xxxxx 10xxxxxx
        if (c >= 0xC2 && c <= 0xDF)
        {
            if (i + 1 >= size)
                return false;

            if ((p[i + 1] & 0xC0) != 0x80)
                return false;

            i += 2;
            continue;
        }

        // 3bytes sequence:
        //      1110xxxx 10xxxxxx 10xxxxxx
        if (c >= 0xE0 && c <= 0xEF)
        {
            if (i + 2 >= size)
                return false;

            if ((p[i + 1] & 0xC0) != 0x80 || (p[i + 2] & 0xC0) != 0x80)
                return false;

            if (c == 0xE0 && p[i + 1] < 0xA0)
                return false;

            if (c == 0xED && p[i + 1] >= 0xA0)
                return false;

            i += 3;
            continue;
        }

        // 4bytes sequence:
        //      11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        if (c >= 0xF0 && c <= 0xF4)
        {
            if (i + 3 >= size)
                return false;

            if ((p[i + 1] & 0xC0) != 0x80 || (p[i + 2] & 0xC0) != 0x80 || (p[i + 3] & 0xC0) != 0x80)
                return false;

            if (c == 0xF0 && p[i + 1] < 0x90)
                return false;

            if (c == 0xF4 && p[i + 1] >= 0x90)
                return false;

            i += 4;
            continue;
        }

        return false;
    }

    return true;
}

size_t
cra_string_view_unicode_escape_encode(const CraStringView *from, CraStringView *to)
{
    assert(from);
    assert(from->data);
    assert(to);
    assert(to->data);
    assert(to->size >= from->size);
    // TODO: implement
    CRA_UNUSED(from);
    CRA_UNUSED(to);
    return 0;
}

size_t
cra_string_view_unicode_escape_decode(const CraStringView *from, CraStringView *to)
{
    assert(from);
    assert(from->data);
    assert(to);
    assert(to->data);
    assert(to->size >= from->size);
    // TODO: implement
    CRA_UNUSED(from);
    CRA_UNUSED(to);
    return 0;
}

#endif // end string view