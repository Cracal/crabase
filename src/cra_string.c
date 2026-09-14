#include "cra_string.h"
#include "cra_malloc.h"

#if 1 // string view

bool
cra_string_view_find_cstr(const CraStringView *sv, const char *seq, size_t size, size_t *retindex)
{
    size_t *next;
    size_t  i, j;
    size_t  n, m;

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

    CRA_TEMP_NEW(temp, m * sizeof(size_t));
    if (temp == NULL)
        return false; // memory allocation failed
    next = (size_t *)temp;

    next[0] = 0;
    for (j = 0, i = 1; i < m;)
    {
        if (seq[j] == seq[i])
            next[i++] = ++j;
        else if (j > 0)
            j = next[j - 1];
        else
            next[i++] = 0;
    }

    for (j = 0, i = 0; i < n;)
    {
        if (seq[j] == sv->data[i])
        {
            ++i;
            ++j;

            if (j == m)
            {
                CRA_TEMP_DEL(temp, m * sizeof(size_t));
                *retindex = i - m;
                return true;
            }
        }
        else if (j > 0)
        {
            j = next[j - 1];
        }
        else
        {
            ++i;
        }
    }
    CRA_TEMP_DEL(temp, m * sizeof(size_t));
    return false;
}

bool
cra_string_view_rfind_cstr(const CraStringView *sv, const char *seq, size_t size, size_t *retindex)
{
    size_t *next;
    size_t  i, j;
    size_t  n, m;

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

    CRA_TEMP_NEW(temp, m * sizeof(size_t));
    if (temp == NULL)
        return false; // memory allocation failed
    next = (size_t *)temp;

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
            CRA_TEMP_DEL(temp, m * sizeof(size_t));
            *retindex = i - 1;
            return true;
        }
    }
    CRA_TEMP_DEL(temp, m * sizeof(size_t));
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

bool
cra_string_view_unicode_escape_bytes(const CraStringView *sv, size_t *retbytes)
{
    assert(sv);
    assert(sv->data);
    assert(retbytes);
    // TODO: implement
    return false;
}

bool
cra_string_view_unicode_escape(const CraStringView *from, CraStringView *to)
{
    assert(from);
    assert(from->data);
    assert(to);
    assert(to->data);
    assert(to->size >= from->size);
    // TODO: implement
    return false;
}

#endif // end string view