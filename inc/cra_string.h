/**
 * @file cra_string.h
 * @author Cracal
 * @brief string utils
 * @version 0.1
 * @date 2026-09-10
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __CRA_STRING_H__
#define __CRA_STRING_H__
#include "cra_defs.h"

typedef struct CraStringView CraStringView;
typedef struct CraString     CraString;

#if 1 // SSO string

struct CraString
{
    union
    {
        struct
        {
            uint8_t size;
            char    data[sizeof(void *) * 3 - 1];
        } _short;
        struct
        {
            size_t capacity;
            size_t size;
            char  *data;
        } _long;
    };
};

static inline size_t
cra_string_size(CraString *str)
{
    return (str->_short.size & 0x80) ? (str->_short.size & 0x7F) : str->_long.size;
}

static inline char *
cra_string_data(CraString *str)
{
    return (str->_short.size & 0x80) ? str->_short.data : str->_long.data;
}

static inline const char *
cra_string_cstr(CraString *str)
{
    return cra_string_data(str);
}

CRA_API bool
cra_string_init(CraString *str, const char *data, size_t size);

CRA_API bool
cra_string_init_fmt(CraString *str, const char *fmt, ...);

CRA_API void
cra_string_uninit(CraString *str);

#endif // end SSO string

#if 1 // string view

struct CraStringView
{
    size_t size;
    char  *data;
};

#define CRA_STRING_VIEW_INIT(cstr, size) { size, cstr }
#define CRA_STRING_VIEW_INIT_CONST(cstr) CRA_STRING_VIEW_INIT(cstr, sizeof(cstr) - 1)
#define CRA_STRING_VIEW_FMT              "%.*s"
#define CRA_STRING_VIEW_ARG(sv)          (int)(sv).size, (sv).data
#define CRA_SV_FMT                       CRA_STRING_VIEW_FMT
#define CRA_SV_ARG                       CRA_STRING_VIEW_ARG

static inline CraStringView
cra_string_view_from_cstr(char *cstr, size_t size)
{
    return (CraStringView)CRA_STRING_VIEW_INIT(cstr, size);
}

static inline CraStringView
cra_string_view_from_str(CraString *str)
{
    return cra_string_view_from_cstr(cra_string_data(str), cra_string_size(str));
}

static inline bool
cra_string_view_equals_cstr(const CraStringView *sv, const char *cstr, size_t size)
{
    if (sv->size != size)
        return false;
    return memcmp(sv->data, cstr, size) == 0;
}

static inline bool
cra_string_view_equals(const CraStringView *sv, const CraStringView *sw)
{
    return cra_string_view_equals_cstr(sv, sw->data, sw->size);
}

static inline CraStringView
cra_string_view_slice(const CraStringView *sv, size_t start, size_t end)
{
    end = CRA_MIN(end, sv->size);
    start = CRA_MIN(start, end);
    return (CraStringView)CRA_STRING_VIEW_INIT(sv->data + start, end - start);
}

static inline CraStringView
cra_string_view_slice_left(const CraStringView *sv, size_t n)
{
    return cra_string_view_slice(sv, 0, n);
}

static inline CraStringView
cra_string_view_slice_right(const CraStringView *sv, size_t n)
{
    n = sv->size > n ? sv->size - n : 0;
    return cra_string_view_slice(sv, n, sv->size);
}

static inline CraStringView
cra_string_view_trim_left(CraStringView *sv)
{
    size_t n = 0;
    while (n < sv->size && isspace(sv->data[n]))
        ++n;
    return (CraStringView)CRA_STRING_VIEW_INIT(sv->data + n, sv->size - n);
}

static inline CraStringView
cra_string_view_trim_right(CraStringView *sv)
{
    size_t n = sv->size;
    while (n > 0 && isspace(sv->data[n - 1]))
        --n;
    return (CraStringView)CRA_STRING_VIEW_INIT(sv->data, n);
}

static inline CraStringView
cra_string_view_trim(CraStringView *sv)
{
    CraStringView sw = cra_string_view_trim_left(sv);
    return cra_string_view_trim_right(&sw);
}

static inline bool
cra_string_view_starts_with_cstr(const CraStringView *sv, const char *cstr, size_t size)
{
    if (sv->size < size)
        return false;
    return memcmp(sv->data, cstr, size) == 0;
}

static inline bool
cra_string_view_starts_with(const CraStringView *sv, const CraStringView *sw)
{
    return cra_string_view_starts_with_cstr(sv, sw->data, sw->size);
}

static inline bool
cra_string_view_ends_with_cstr(const CraStringView *sv, const char *cstr, size_t size)
{
    if (sv->size < size)
        return false;
    return memcmp(sv->data + sv->size - size, cstr, size) == 0;
}

static inline bool
cra_string_view_ends_with(const CraStringView *sv, const CraStringView *sw)
{
    return cra_string_view_ends_with_cstr(sv, sw->data, sw->size);
}

CRA_API bool
cra_string_view_find_cstr(const CraStringView *sv, const char *seq, size_t size, size_t *retindex);

static inline bool
cra_string_view_find(const CraStringView *sv, const CraStringView *seq, size_t *retindex)
{
    return cra_string_view_find_cstr(sv, seq->data, seq->size, retindex);
}

CRA_API bool
cra_string_view_rfind_cstr(const CraStringView *sv, const char *seq, size_t size, size_t *retindex);

static inline bool
cra_string_view_rfind(const CraStringView *sv, const CraStringView *seq, size_t *retindex)
{
    return cra_string_view_rfind_cstr(sv, seq->data, seq->size, retindex);
}

static inline bool
cra_string_view_contains_cstr(const CraStringView *sv, const char *cstr, size_t size)
{
    size_t index;
    return cra_string_view_find_cstr(sv, cstr, size, &index);
}

static inline bool
cra_string_view_contains(const CraStringView *sv, const CraStringView *sw)
{
    return cra_string_view_contains_cstr(sv, sw->data, sw->size);
}

static inline CraStringView
cra_string_view_split_cstr(CraStringView *sv, const char *sep, size_t size)
{
    CraStringView sw;
    size_t        index;
    if (cra_string_view_find_cstr(sv, sep, size, &index))
    {
        sw = cra_string_view_slice_left(sv, index);
        sv->data += index + size;
        sv->size -= index + size;
        return sw;
    }
    return (CraStringView)CRA_STRING_VIEW_INIT(NULL, 0);
}

static inline CraStringView
cra_string_view_split(CraStringView *sv, const CraStringView *sep)
{
    return cra_string_view_split_cstr(sv, sep->data, sep->size);
}

static inline CraStringView
cra_string_view_split_left(CraStringView *sv, size_t n)
{
    n = CRA_MIN(n, sv->size);
    CraStringView sw = CRA_STRING_VIEW_INIT(sv->data, n);
    sv->data += n;
    sv->size -= n;
    return sw;
}

static inline CraStringView
cra_string_view_split_right(CraStringView *sv, size_t n)
{
    n = CRA_MIN(n, sv->size);
    CraStringView sw = CRA_STRING_VIEW_INIT(sv->data + sv->size - n, n);
    sv->size -= n;
    return sw;
}

// UTF-8 related

CRA_API const uint8_t cra_g_string_view_bytes_for_utf8[256];
#define CRA_SV_BYTES_FOR_UTF8 cra_g_string_view_bytes_for_utf8

CRA_API bool
cra_string_view_is_valid_utf8_cstr(const char *cstr, size_t size);

static inline bool
cra_string_view_is_valid_utf8(const CraStringView *sv)
{
    return cra_string_view_is_valid_utf8_cstr(sv->data, sv->size);
}

static inline size_t
cra_string_view_utf8_len(const CraStringView *sv, size_t *bytes_overrun)
{
    size_t i = 0;
    size_t n = 0;
    while (true)
    {
        if (i >= sv->size)
        {
            if (bytes_overrun)
                *bytes_overrun = i - sv->size;
            return n;
        }
        i += CRA_SV_BYTES_FOR_UTF8[(uint8_t)sv->data[i]];
        n += 1;
    }
    // unreachable
    return 0;
}

CRA_API bool
cra_string_view_unicode_escape_bytes(const CraStringView *sv, size_t *retbytes);

CRA_API bool
cra_string_view_unicode_escape(const CraStringView *from, CraStringView *to);

#endif // end string view

#endif