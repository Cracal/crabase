/**
 * @file cra_json.c
 * @author Cracal
 * @brief JSON
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "crabase/serialize/cra_json.h"
#include "cra_serialize_inner.h"
#include "crabase/cra_malloc.h"
#include <float.h>
#include <math.h>

#define CHECK_WRITE_NULL(szer, p, buff)                  \
    do                                                   \
    {                                                    \
        if (!(p))                                        \
        {                                                \
            CRA_SER_ENSURE2(szer, sizeof("null"), buff); \
            memcpy(buff, "null", sizeof("null"));        \
            return true;                                 \
        }                                                \
    } while (0)

#define CHECK_TYPE(szer, t1, t2)                                \
    do                                                          \
    {                                                           \
        if (t1 != t2)                                           \
        {                                                       \
            cra_ser_error_set(szer, CRA_SER_ERR_TYPE_MISMATCH); \
            return false;                                       \
        }                                                       \
    } while (0)

#define WRITE_OBJECT_BEGIN(szer, buff)                          \
    size_t len1, len2;                                          \
    char   buf2[sizeof(": ") - 1];                              \
    char   buf1[CRA_SER_NESTING_MAX + 1];                       \
    CRA_SER_NESTING_INC(szer);                                  \
    len1 = (szer)->format ? (szer)->nesting + sizeof("\n") : 1; \
    len2 = (szer)->format ? sizeof(": ") : sizeof(":");         \
    buf1[0] = '\n';                                             \
    memset(buf1 + 1, '\t', (szer)->nesting);                    \
    memcpy(buf2, ": ", sizeof(": ") - 1);                       \
    CRA_SER_ENSURE0(szer, sizeof("{}"), buff, sizeof("}"));     \
    *buff = '{'
#define WRITE_OBJECT_END(szer, buff)                          \
    buff = (szer)->buf + (szer)->curr;                        \
    len1 = ((szer)->format && *(buff - 1) != '{') ? len1 : 2; \
    if (!CRA_SER_ENOUGH(szer, len1))                          \
        goto no_buf;                                          \
    (szer)->curr += len1 - 1;                                 \
    if (len1 > 2)                                             \
    {                                                         \
        memcpy(buff, buf1, len1 - 2);                         \
        buff += len1 - 2;                                     \
    }                                                         \
    *buff = '}';                                              \
    CRA_SER_NESTING_DEC(szer)
#define WRITE_OBJECT_BEFORE_KEY(szer, buff, index)                   \
    do                                                               \
    {                                                                \
        if (len1 > 1 || (index) > 0)                                 \
        {                                                            \
            if (!CRA_SER_ENOUGH(szer, len1 + ((index) > 0 ? 1 : 0))) \
                goto no_buf;                                         \
            buff = (szer)->buf + (szer)->curr;                       \
            (szer)->curr += len1 + ((index) > 0 ? 1 : 0) - 1;        \
            if ((index) > 0)                                         \
                *buff++ = ',';                                       \
            if (len1 > 1)                                            \
                memcpy(buff, buf1, len1 - 1);                        \
        }                                                            \
    } while (0)
#define WRITE_OBJECT_AFTER_KEY(szer, buff) \
    if (!CRA_SER_ENOUGH(szer, len2))       \
        goto no_buf;                       \
    buff = (szer)->buf + (szer)->curr;     \
    memcpy(buff, buf2, len2 - 1);          \
    (szer)->curr += len2 - 1
#define READ_OBJECT_BEGIN(szer, buff)                    \
    do                                                   \
    {                                                    \
        CRA_SER_NESTING_INC(szer);                       \
        ++(szer)->curr; /* skip '{' */                   \
        if (!(buff = cra_json_skip_whitespaces(szer)))   \
        {                                                \
            cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF); \
            return false;                                \
        }                                                \
    } while (0)
#define READ_OBJECT_END_(szer, buff, ch) \
    do                                   \
    {                                    \
        if (*buff != ch)                 \
            goto invalid_value;          \
        ++(szer)->curr; /* skip `ch` */  \
        CRA_SER_NESTING_DEC(szer);       \
    } while (0)
#define READ_OBJECT_END(szer, buff) READ_OBJECT_END_(szer, buff, '}')
#define READ_OBJECT_AFTER_KEY(szer, buff)              \
    do                                                 \
    {                                                  \
        if (!(buff = cra_json_skip_whitespaces(szer))) \
            goto no_buf;                               \
        if (*buff != ':')                              \
            goto invalid_value;                        \
        ++(szer)->curr; /*skip ':' */                  \
        if (!(buff = cra_json_skip_whitespaces(szer))) \
            goto no_buf;                               \
    } while (0)
#define READ_OBJECT_AFTER_VALUE_(szer, buff, ch)   \
    if (!(buff = cra_json_skip_whitespaces(szer))) \
        goto no_buf;                               \
    if (*buff == ch)                               \
        break; /* end of object */                 \
    if (*buff != ',')                              \
        goto invalid_value;                        \
    ++(szer)->curr; /*skip ',' */                  \
    if (!(buff = cra_json_skip_whitespaces(szer))) \
    goto no_buf
#define READ_OBJECT_AFTER_VALUE(szer, buff) READ_OBJECT_AFTER_VALUE_(szer, buff, '}')

#define WRITE_ARRAY_BEGIN(szer, buff)                          \
    size_t len1 = (szer)->format ? sizeof(", ") : sizeof(","); \
    char   buf1[sizeof(", ")] = ", ";                          \
    CRA_SER_NESTING_INC(szer);                                 \
    CRA_SER_ENSURE0(szer, sizeof("[]"), buff, sizeof("]"));    \
    *buff = '['
#define WRITE_ARRAY_END(szer, buff)               \
    do                                            \
    {                                             \
        CRA_SER_NESTING_DEC(szer);                \
        CRA_SER_ENSURE2(szer, sizeof("]"), buff); \
        *buff = ']';                              \
    } while (0)
#define WRITE_ARRAY_BEFORE_VALUE(szer, buff, index) \
    do                                              \
    {                                               \
        if ((index) > 0)                            \
        {                                           \
            if (!CRA_SER_ENOUGH(szer, len1))        \
                goto no_buf;                        \
            buff = (szer)->buf + (szer)->curr;      \
            memcpy(buff, buf1, len1 - 1);           \
            (szer)->curr += len1 - 1;               \
        }                                           \
    } while (0)
#define READ_ARRAY_BEGIN(szer, buff)       READ_OBJECT_BEGIN(szer, buff)
#define READ_ARRAY_END(szer, buff)         READ_OBJECT_END_(szer, buff, ']')
#define READ_ARRAY_AFTER_VALUE(szer, buff) READ_OBJECT_AFTER_VALUE_(szer, buff, ']')

static inline unsigned char *
cra_json_skip_whitespaces(CraSerializer *szer)
{
    while (CRA_SER_ENOUGH(szer, 1) && szer->buf[szer->curr] <= 0x20 && szer->buf[szer->curr] != '\0')
        ++szer->curr;
    return CRA_SER_ENOUGH(szer, 1) && szer->buf[szer->curr] != '\0' ? &szer->buf[szer->curr] : NULL;
}

static void
cra_json_skip_value(CraSerializer *szer)
{
    unsigned char *buf = szer->buf + szer->curr;
    unsigned char *end = buf + szer->size;
    unsigned char *old = buf;
    ssize_t        nc = 0;

    while (buf < end && *buf != '\0')
    {
        if ((*buf == ',' || *buf == '}') && nc <= 0)
            break;

        if (*buf == '{' || *buf == '[')
            ++nc;
        else if (*buf == '}' || *buf == ']')
            --nc;

        ++buf;
    }

    szer->curr += (buf - old);
}

static inline int
hex2int(int ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    return -1; // invalid hex character
}

static inline bool
parse_hex4(const unsigned char *buf, uint32_t *retval)
{
    uint32_t cp = 0;
    for (int val, i = 0; i < 4; ++i)
    {
        if ((val = hex2int(buf[i])) < 0)
            return false;
        cp = (cp << 4) | (uint32_t)val;
    }
    *retval = cp;
    return true;
}

static inline bool
is_high_surrogate(uint32_t cp)
{
    return cp >= 0xD800 && cp <= 0xDBFF;
}

static inline bool
is_low_surrogate(uint32_t cp)
{
    return cp >= 0xDC00 && cp <= 0xDFFF;
}

static inline uint32_t
combine_surrogates(uint32_t high, uint32_t low)
{
    return 0x10000 + ((high - 0xd800) << 10) + (low - 0xdc00);
}

static inline int
codepoint2utf8(unsigned char *buff, uint32_t cp)
{
    unsigned char *buf = buff;
    if (cp <= 0x7F)
    {
        buf[0] = (unsigned char)cp;
        return 1;
    }
    if (cp <= 0x7FF)
    {
        buf[0] = (unsigned char)(0xC0 | (cp >> 6));
        buf[1] = (unsigned char)(0x80 | (cp & 0x3F));
        return 2;
    }
    if (cp <= 0xFFFF)
    {
        buf[0] = (unsigned char)(0xE0 | (cp >> 12));
        buf[1] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
        buf[2] = (unsigned char)(0x80 | (cp & 0x3F));
        return 3;
    }
    if (cp <= 0x10FFFF)
    {
        buf[0] = (unsigned char)(0xF0 | (cp >> 18));
        buf[1] = (unsigned char)(0x80 | ((cp >> 12) & 0x3F));
        buf[2] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
        buf[3] = (unsigned char)(0x80 | (cp & 0x3F));
        return 4;
    }

    return -1;
}

static bool
cra_json_string_encode_needed(const unsigned char *str, size_t *len)
{
    const unsigned char *p = str;
    bool                 b = true;
    size_t               i, j, n = *len;

    for (i = 0, j = 0; i < n && *p != '\0'; ++i, ++j, ++p)
    {
        if (*p != '\"' && *p != '\\' && *p >= 0x20)
            continue;

        b = false;
        switch (*p)
        {
            case '\b':
            case '\t':
            case '\n':
            case '\f':
            case '\r':
            case '\"':
            case '\\':
                // \C
                j += 1;
                break;
            default:
                // \uXXXX: U+0000-U+001F
                j += 5;
                break;
        }
    }

    *len = j;
    return b;
}

static bool
cra_json_string_decode_needed(const unsigned char *str, size_t *nsrc, size_t *ndst, bool *normal)
{
    const unsigned char *p = str;
    bool                 b = true;
    size_t               i, j, k, n = *nsrc;
    uint32_t             cp, lo;

    for (i = 0, j = 0; i < n && *p != '\"';)
    {
        if (*p != '\\')
        {
            if (*p < 0x20)
                return false; // illegal character
            ++i, ++j, ++p;
            continue;
        }

        b = false;
        if (n - i < 2)
            return false;
        switch (*(++p)) // skip '\\'
        {
            case 'b':
            case 't':
            case 'n':
            case 'f':
            case 'r':
            case '\"':
            case '/':
            case '\\':
                ++j, ++p, i += 2;
                break;
            case 'u':
                if (n - i < 6 || !parse_hex4(p + 1, &cp))
                    return false; // illegal escape sequence
                if (is_low_surrogate(cp))
                    return false; // low surrogate without high surrogate
                p += 5;
                i += 6;
                if (is_high_surrogate(cp))
                {
                    if (n - i < 6 || p[0] != '\\' || p[1] != 'u' || !parse_hex4(p + 2, &lo))
                        return false; // illegal escape sequence
                    if (!is_low_surrogate(lo))
                        return false; // low surrogate must follow high surrogate
                    p += 6;
                    i += 6;
                    cp = combine_surrogates(cp, lo);
                }

                k = cp <= 0x7F ? 1 : cp <= 0x7FF ? 2 : cp <= 0xFFFF ? 3 : 4;
                j += k;
                break;

            default:
                return false;
        }
    }

    *nsrc = i;
    *ndst = j;
    *normal = b;
    return true;
}

static bool
cra_json_write_string(CraSerializer *szer, const unsigned char *val, size_t len, bool is_ptr)
{
    const unsigned char *p;
    unsigned char       *d;
    unsigned char       *e;
    bool                 normal;

    if (is_ptr)
        memcpy(&p, val, sizeof(unsigned char *)); // p = *(unsigned char**)val;
    else
        p = val;

    CHECK_WRITE_NULL(szer, p, d);

    normal = cra_json_string_encode_needed(p, &len);
    CRA_SER_CHECK_LIMIT(szer, len, >=, CRA_SER_STRING_MAX_LENGTH);

    CRA_SER_ENSURE2(szer, len + sizeof("\"\""), d);
    *d++ = '\"';

    // fast path
    if (normal)
    {
        if (len > 0)
            memcpy(d, p, len);
        d += len;
        goto end;
    }

    for (e = d + len; d < e; ++p)
    {
        if (*p != '\"' && *p != '\\' && *p >= 0x20)
        {
            *d++ = *p;
            continue;
        }

        *d++ = '\\';
        switch (*p)
        {
            case '\b':

                *d++ = 'b';
                break;
            case '\t':
                *d++ = 't';
                break;
            case '\n':
                *d++ = 'n';
                break;
            case '\f':
                *d++ = 'f';
                break;
            case '\r':
                *d++ = 'r';
                break;
            case '\"':
                *d++ = '\"';
                break;
            case '\\':
                *d++ = '\\';
                break;
            default:
                // \uXXXX: U+0000-U+001F
                snprintf((char *)d, 6, "u%04x", *p);
                d += 5;
                break;
        }
    }

end:
    d[0] = '\"';
    return true;
}

static bool
cra_json_read_string(CraSerializer *szer, unsigned char *val, const CraSerInfo *info, size_t *retlen)
{
    bool           b;
    int            l;
    size_t         i;
    size_t         j;
    unsigned char *d;
    unsigned char *p;
    unsigned char *e;
    uint32_t       cp = 0;
    uint32_t       lo = 0;

    CHECK_TYPE(szer, info->type, CRA_SER_TYPE_STRING);

    CRA_SER_ENSURE0(szer, sizeof("\"\""), d, sizeof("\""));
    ++d; // skip '\"'

    // get string length
    i = szer->size - szer->curr - 1;
    if (!cra_json_string_decode_needed(d, &i, &j, &b))
    {
        cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
        return false;
    }
    CRA_SER_CHECK_LIMIT(szer, j, >=, CRA_SER_STRING_MAX_LENGTH);
    szer->curr += i + 1; // skip string data and '\"'
    if (retlen)
        *retlen = j;

    if (d[i] != '\"')
    {
        cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF);
        return false;
    }

    CRA_SER_NEW_STRING(szer, val, p, info, j + 1, j);

    // fast path
    if (b)
    {
        if (j > 0)
            memcpy(p, d, j);
        *(p + j) = '\0';
        return true;
    }

    for (e = p + j; p < e;)
    {
        if (*d != '\\')
        {
            *p++ = *d++;
            continue;
        }

        switch (*(++d)) // skip '\\'
        {
            case 'b':
                *p++ = '\b';
                ++d;
                break;
            case 't':
                *p++ = '\t';
                ++d;
                break;
            case 'n':
                *p++ = '\n';
                ++d;
                break;
            case 'f':
                *p++ = '\f';
                ++d;
                break;
            case 'r':
                *p++ = '\r';
                ++d;
                break;
            case '\"':
                *p++ = '\"';
                ++d;
                break;
            case '/':
                *p++ = '/';
                ++d;
                break;
            case '\\':
                *p++ = '\\';
                ++d;
                break;
            case 'u':
                // 因为`cra_json_string_decode_needed`已经检查过，所以这里直接解析
                parse_hex4(d + 1, &cp);
                d += 5;
                if (is_high_surrogate(cp))
                {
                    parse_hex4(d + 2, &lo);
                    cp = combine_surrogates(cp, lo);
                    d += 6;
                }
                // convert to utf-8
                l = codepoint2utf8(p, cp);
                if (l > 0)
                {
                    p += l;
                    break;
                }
                // invalid_value
                // FALL-THROUGH

            default:
                *p = '\0';
                cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
                return false;
        }
    }

    *p = '\0';
    return true;
}

static inline bool
cra_json_write_bool(CraSerializer *szer, const unsigned char *val)
{
    unsigned char *buf;
    char          *value;
    size_t         value_size;
    if (!!(*val))
    {
        value = "true";
        value_size = sizeof("true");
    }
    else
    {
        value = "false";
        value_size = sizeof("false");
    }
    CRA_SER_ENSURE2(szer, value_size, buf);
    memcpy(buf, value, value_size - 1);
    return true;
}

static inline bool
cra_json_read_null(CraSerializer *szer, unsigned char *val, const CraSerInfo *info, const CraSerMeta *meta)
{
    unsigned char *buff;
    CRA_SER_ENSURE2(szer, sizeof("null"), buff);
    if (!((info->flags & CRA_SER_FLAG_PTR) || (meta && (meta->flags & CRA_SER_FLAG_ARRAY) && meta->count == 0)))
    {
        cra_ser_error_set(szer, CRA_SER_ERR_CANNOT_BE_NULL);
        return false;
    }
    if (!(buff[1] == 'u' && buff[2] == 'l' && buff[3] == 'l'))
    {
        cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
        return false;
    }
    bzero(val, sizeof(void *)); // *(T **)val = NULL;
    return true;
}

static inline bool
cra_json_read_true(CraSerializer *szer, unsigned char *val, const CraSerInfo *info)
{
    unsigned char *buff;

    CHECK_TYPE(szer, info->type, CRA_SER_TYPE_BOOL);

    CRA_SER_ENSURE2(szer, sizeof("true"), buff);
    if (strncmp((char *)buff, "true", sizeof("true") - 1) == 0)
    {
        memset(val, true, sizeof(bool));
        return true;
    }
    cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
    return false;
}

static inline bool
cra_json_read_false(CraSerializer *szer, unsigned char *val, const CraSerInfo *info)
{
    unsigned char *buff;

    CHECK_TYPE(szer, info->type, CRA_SER_TYPE_BOOL);

    CRA_SER_ENSURE2(szer, sizeof("false"), buff);
    if (strncmp((char *)buff, "false", sizeof("false") - 1) == 0)
    {
        memset(val, false, sizeof(bool));
        return true;
    }
    cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
    return false;
}

static inline bool
cra_json_compare_double(double a, double b)
{
    return fabs(a - b) < DBL_EPSILON;
}

static bool
cra_json_write_float(CraSerializer *szer, const unsigned char *val, size_t size)
{
    int            len;
    float          flt;
    double         dbl;
    double         tmp;
    unsigned char *buf;
    char           dblbuf[32];

    switch (size)
    {
        case sizeof(flt):
            memcpy(&flt, val, sizeof(flt));
            dbl = flt;
            break;
        case sizeof(dbl):
            memcpy(&dbl, val, sizeof(dbl));
            break;
        default:
            // unreachable
            abort();
    }

    if (isnan(dbl) || isinf(dbl))
        goto invalid_value;

    // -0.0
    if (dbl == 0.0 && signbit(dbl))
        dbl = 0.0;

    len = snprintf(dblbuf, sizeof(dblbuf), "%.15g", dbl);
    tmp = strtod(dblbuf, NULL);
    if (!cra_json_compare_double(tmp, dbl))
    {
        len = snprintf(dblbuf, sizeof(dblbuf), "%.17g", dbl);
    }

    if (len < 0 || len >= (int)sizeof(dblbuf))
    {
    invalid_value:
        cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
        return false;
    }

    CRA_SER_ENSURE2(szer, len + sizeof(""), buf);
    memcpy(buf, dblbuf, len);
    return true;
}

static bool
cra_json_write_uint(CraSerializer *szer, const unsigned char *val, size_t size)
{
    int            n;
    uint8_t        u8;
    uint16_t       u16;
    uint32_t       u32;
    uint64_t       u64;
    unsigned char *buf;

    // get uint number
    switch (size)
    {
        case sizeof(u8):
            memcpy(&u8, val, sizeof(u8));
            u64 = u8;
            break;
        case sizeof(u16):
            memcpy(&u16, val, sizeof(u16));
            u64 = u16;
            break;
        case sizeof(u32):
            memcpy(&u32, val, sizeof(u32));
            u64 = u32;
            break;
        case sizeof(u64):
            memcpy(&u64, val, sizeof(u64));
            break;
        default:
            // unreachable
            abort();
    }
#ifdef CRA_JSON_SAFE_INT
    u64 = CRA_MIN(u64, CRA_MAX_SAFE_INT);
#endif

    // write value
    buf = szer->buf + szer->curr;
    size = szer->size - szer->curr;
    n = snprintf((char *)buf, size, "%" PRIu64, u64);
    if (n <= 0 || !CRA_SER_ENOUGH(szer, n + sizeof("")))
    {
        cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF);
        return false;
    }
    szer->curr += n;
    return true;
}

static bool
cra_json_write_int(CraSerializer *szer, const unsigned char *val, size_t size)
{
    int            n;
    int8_t         i8;
    int16_t        i16;
    int32_t        i32;
    int64_t        i64;
    unsigned char *buf;

    // get int number
    switch (size)
    {
        case sizeof(i8):
            memcpy(&i8, val, sizeof(i8));
            i64 = i8;
            break;
        case sizeof(i16):
            memcpy(&i16, val, sizeof(i16));
            i64 = i16;
            break;
        case sizeof(i32):
            memcpy(&i32, val, sizeof(i32));
            i64 = i32;
            break;
        case sizeof(i64):
            memcpy(&i64, val, sizeof(i64));
            break;
        default:
            // unreachable
            abort();
    }
#ifdef CRA_JSON_SAFE_INT
    i64 = CRA_CLAMP(i64, CRA_MAX_SAFE_INT, CRA_MIN_SAFE_INT);
#endif

    // write value
    buf = szer->buf + szer->curr;
    size = szer->size - szer->curr;
    n = snprintf((char *)buf, size, "%" PRId64, i64);
    if (n <= 0 || !CRA_SER_ENOUGH(szer, n + sizeof("")))
    {
        cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF);
        return false;
    }
    szer->curr += n;
    return true;
}

static bool
cra_json_write_number(CraSerializer *szer, const unsigned char *val, const CraSerInfo *info)
{
    if (info->flags & CRA_SER_FLAG_FLOAT)
        return cra_json_write_float(szer, val, info->size);
    if (info->flags & CRA_SER_FLAG_UNSIGNED)
        return cra_json_write_uint(szer, val, info->size);
    return cra_json_write_int(szer, val, info->size);
}

static bool
cra_json_read_float(CraSerializer *szer, unsigned char *val, size_t size)
{
    float          flt;
    double         dbl;
    size_t         len;
    unsigned char *end;
    unsigned char *buf;

    end = NULL;
    buf = szer->buf + szer->curr;
    dbl = strtod((char *)buf, (char **)&end);
    if (buf == end || isnan(dbl) || isinf(dbl))
    {
        cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
        return false;
    }

    // enough?
    len = (size_t)(end - buf);
    CRA_SER_ENSURE2(szer, len + sizeof(""), buf);

    switch (size)
    {
        case sizeof(flt):
            flt = (float)dbl;
            memcpy(val, &flt, sizeof(flt));
            break;
        case sizeof(dbl):
            memcpy(val, &dbl, sizeof(dbl));
            break;
        default:
            // unreachable
            abort();
    }

    return true;
}

static bool
cra_json_read_uint(CraSerializer *szer, unsigned char *val, size_t size)
{
    size_t         n;
    uint8_t        u8;
    uint16_t       u16;
    uint32_t       u32;
    uint64_t       u64;
    unsigned char *end;
    unsigned char *buf;

    end = NULL;
    buf = szer->buf + szer->curr;
    u64 = strtoull((char *)buf, (char **)&end, 10);
    if (buf == end)
        goto fail;
    // float?
    if (*end == '.' || *end == 'e' || *end == 'E')
    {
        double dbl;
        if (!cra_json_read_float(szer, (unsigned char *)&dbl, sizeof(dbl)))
            goto fail;

#ifdef CRA_JSON_SAFE_INT
        u64 = (uint64_t)CRA_CLAMP(dbl, (double)CRA_MAX_SAFE_INT, 0);
#else
        u64 = (uint64_t)dbl;
#endif
    }
    else
    {
        n = (size_t)(end - buf);
        CRA_SER_ENSURE2(szer, n + sizeof(""), buf);
#ifdef CRA_JSON_SAFE_INT
        u64 = CRA_MIN(u64, CRA_MAX_SAFE_INT);
#endif
    }

    switch (size)
    {
        case sizeof(u8):
            u8 = (uint8_t)u64;
            memcpy(val, &u8, sizeof(u8));
            break;
        case sizeof(u16):
            u16 = (uint16_t)u64;
            memcpy(val, &u16, sizeof(u16));
            break;
        case sizeof(u32):
            u32 = (uint32_t)u64;
            memcpy(val, &u32, sizeof(u32));
            break;
        case sizeof(u64):
            memcpy(val, &u64, sizeof(u64));
            break;
        default:
            // unreachable
            abort();
    }

    return true;

fail:
    cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
    return false;
}

static bool
cra_json_read_int(CraSerializer *szer, unsigned char *val, size_t size)
{
    size_t         n;
    int8_t         i8;
    int16_t        i16;
    int32_t        i32;
    int64_t        i64;
    unsigned char *end;
    unsigned char *buf;

    end = NULL;
    buf = szer->buf + szer->curr;
    i64 = strtoll((char *)buf, (char **)&end, 10);
    if (buf == end)
        goto fail;
    // float?
    if (*end == '.' || *end == 'e' || *end == 'E')
    {
        double dbl;
        if (!cra_json_read_float(szer, (unsigned char *)&dbl, sizeof(dbl)))
            goto fail;

#ifdef CRA_JSON_SAFE_INT
        i64 = (int64_t)CRA_CLAMP(dbl, (double)CRA_MAX_SAFE_INT, (double)CRA_MIN_SAFE_INT);
#else
        i64 = (int64_t)dbl;
#endif
    }
    else
    {
        n = (size_t)(end - buf);
        CRA_SER_ENSURE2(szer, n + sizeof(""), buf);
#ifdef CRA_JSON_SAFE_INT
        i64 = CRA_CLAMP(i64, CRA_MAX_SAFE_INT, CRA_MIN_SAFE_INT);
#endif
    }

    switch (size)
    {
        case sizeof(i8):
            i8 = (int8_t)i64;
            memcpy(val, &i8, sizeof(i8));
            break;
        case sizeof(i16):
            i16 = (int16_t)i64;
            memcpy(val, &i16, sizeof(i16));
            break;
        case sizeof(i32):
            i32 = (int32_t)i64;
            memcpy(val, &i32, sizeof(i32));
            break;
        case sizeof(i64):
            memcpy(val, &i64, sizeof(i64));
            break;
        default:
            // unreachable
            abort();
    }

    return true;

fail:
    cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
    return false;
}

static bool
cra_json_read_number(CraSerializer *szer, unsigned char *val, const CraSerInfo *info)
{
    if (info->type < CRA_SER_TYPE_VARINT || info->type > CRA_SER_TYPE_64BITS)
    {
        cra_ser_error_set(szer, CRA_SER_ERR_TYPE_MISMATCH);
        return false;
    }

    if (info->flags & CRA_SER_FLAG_FLOAT)
        return cra_json_read_float(szer, val, info->size);
    if (info->flags & CRA_SER_FLAG_UNSIGNED)
        return cra_json_read_uint(szer, val, info->size);
    return cra_json_read_int(szer, val, info->size);
}

static bool
cra_json_write_struct(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta);
static bool
cra_json_read_struct(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta);
static bool
cra_json_write_array(CraSerializer *szer, const unsigned char *val, const CraSerMeta *meta);
static bool
cra_json_read_array(CraSerializer *szer, unsigned char *val, const CraSerMeta *meta);
static bool
cra_json_write_list(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta);
static bool
cra_json_read_list(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta);
static bool
cra_json_write_dict(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta);
static bool
cra_json_read_dict(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta);

static bool
cra_json_write_value(CraSerializer       *szer,
                     const unsigned char *val,
                     const CraSerMeta    *meta,
                     const CraSerInfo    *info,
                     uint16_t             idx)
{
    bool   b;
    size_t s;

    assert(val);
    assert(info);
    assert(meta);
    assert(meta->metas);
    assert(!meta->metas[idx] || info->size == meta->metas[idx]->size);

    switch (info->type)
    {
        case CRA_SER_TYPE_BOOL:
            return cra_json_write_bool(szer, val);
        case CRA_SER_TYPE_VARINT:
        case CRA_SER_TYPE_32BITS:
        case CRA_SER_TYPE_64BITS:
            return cra_json_write_number(szer, val, info);
        case CRA_SER_TYPE_STRING:
            b = info->flags & CRA_SER_FLAG_PTR;
            s = info->size > 0 ? info->size : CRA_SER_STRING_MAX_LENGTH;
            return cra_json_write_string(szer, val, s, b);
        case CRA_SER_TYPE_STRUCT:
            return cra_json_write_struct(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[idx]);
        case CRA_SER_TYPE_LIST:
            if (info->flags & CRA_SER_FLAG_ARRAY)
                return cra_json_write_array(szer, val, meta->metas[idx]);
            return cra_json_write_list(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[idx]);
        case CRA_SER_TYPE_DICT:
            return cra_json_write_dict(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[idx]);

        default:
            // unreachable
            abort();
    }
}

static bool
cra_json_read_value(CraSerializer    *szer,
                    unsigned char    *val,
                    const CraSerMeta *meta,
                    const CraSerInfo *info,
                    uint16_t          idx)
{
    unsigned char *buff;

    assert(val);
    assert(info);
    assert(meta);
    assert(meta->metas);
    assert(!meta->metas[idx] || info->size == meta->metas[idx]->size);

    CRA_SER_ENSURE0(szer, sizeof("X"), buff, sizeof("X"));

    if (*buff == 'n') // null
        return cra_json_read_null(szer, val, info, meta->metas[idx]);
    if (*buff == 't') // bool:true
        return cra_json_read_true(szer, val, info);
    if (*buff == 'f') // bool:false
        return cra_json_read_false(szer, val, info);
    if (*buff == '-' || (*buff >= '0' && *buff <= '9')) // number
        return cra_json_read_number(szer, val, info);
    if (*buff == '\"') // string
        return cra_json_read_string(szer, val, info, NULL);
    if (*buff == '{') // object(struct or dict)
    {
        if (info->type == CRA_SER_TYPE_STRUCT)
            return cra_json_read_struct(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[idx]);
        if (info->type == CRA_SER_TYPE_DICT)
            return cra_json_read_dict(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[idx]);
        goto type_mismatch;
    }
    if (*buff == '[') // array(list and c array)
    {
        if (info->type != CRA_SER_TYPE_LIST)
        {
        type_mismatch:
            cra_ser_error_set(szer, CRA_SER_ERR_TYPE_MISMATCH);
            return false;
        }

        if (info->flags & CRA_SER_FLAG_ARRAY)
            return cra_json_read_array(szer, val, meta->metas[idx]);
        return cra_json_read_list(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[idx]);
    }

    cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
    return false;
}

static bool
cra_json_write_struct(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    CraStringView        sv;
    const unsigned char *stru;
    unsigned char       *buff;
    const CraSerInfo    *info;
    uint16_t             index;

    assert(szer);
    assert(val);
    assert(meta);
    assert(meta->infos);
    assert(meta->count > 0);
    assert(meta->infos[0].offset == 0);

    if (is_ptr)
        memcpy(&stru, val, sizeof(void *)); // stru = *(T **)val;
    else
        stru = val;

    CHECK_WRITE_NULL(szer, stru, buff);

    CRA_SER_CHECK_LIMIT(szer, meta->count, >=, CRA_SER_STRUCT_MAX_FIELDS);

    WRITE_OBJECT_BEGIN(szer, buff);

    for (index = 0; index < meta->count; ++index)
    {
        info = &meta->infos[index];
        sv = cra_ser_get_field_name(meta, index);

        // write "\n\t..\t," or "\n\t..\t" or "," or ""
        WRITE_OBJECT_BEFORE_KEY(szer, buff, index);

        // write key
        if (!cra_json_write_string(szer, (unsigned char *)sv.data, sv.size, false))
            goto fail;

        // write ":" or ": "
        WRITE_OBJECT_AFTER_KEY(szer, buff);

        // write value
        if (!cra_json_write_value(szer, stru + info->offset, meta, info, index))
        {
        fail:
            cra_ser_error_append(szer, &sv);
            return false;
        }
    }

    WRITE_OBJECT_END(szer, buff);
    return true;

no_buf:
    cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF);
    return false;
}

static bool
cra_json_read_struct(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    CraStringView     sv;
    unsigned char    *stru;
    unsigned char    *buff;
    const CraSerInfo *info;
    uint16_t          idx, index, last_index;
    unsigned char     key[CRA_MAX_JSON_KEY_LENGTH];
    size_t            key_len;

    CRA_SER_DEF_IDS_BITMAP(id_bitmap);

    assert(szer);
    assert(val);
    assert(meta);
    assert(meta->infos);
    assert(meta->count > 0);
    assert(meta->infos[0].offset == 0);

    // CHECK_TYPE(szer, type, CRA_SER_TYPE_STRUCT);

    READ_OBJECT_BEGIN(szer, buff);

    CRA_SER_NEW_STRUCT(szer, val, stru, meta, is_ptr, false);

    // check empty
    if (*buff == '}')
        goto end;

    static const CraSerInfo key_info = { CRA_SER_FLAG_ARRAY, CRA_SER_TYPE_STRING, sizeof(key), 0 };
    for (index = 0; CRA_SER_ENOUGH(szer, sizeof("X"));)
    {
        // read key
        if (!cra_json_read_string(szer, key, &key_info, &key_len))
        {
            sv = cra_string_view_from_cstr("<<StructField>>", sizeof("<<StructField>>") - 1);
            cra_ser_error_append(szer, &sv);
            goto fail;
        }

        READ_OBJECT_AFTER_KEY(szer, buff);

        // match key
        for (last_index = index;;)
        {
            sv = cra_ser_get_field_name(meta, index);
            if (key_len == sv.size && strncmp((char *)key, sv.data, key_len) == 0)
            {
                idx = index;
                info = &meta->infos[index];
                if (++index == meta->count)
                    index = 0;
                break;
            }
            // index = (index + 1) % meta->count;
            if (++index == meta->count)
                index = 0;
            if (index == last_index)
            {
                // skip this key-value pair
                cra_json_skip_value(szer);
                goto after_value;
            }
        }

        // check duplicate member
        if (cra_ser_check_id_duplicate(id_bitmap, meta->for_struct.ids[idx]))
        {
            cra_ser_error_set(szer, CRA_SER_ERR_DUPLICATE_MEMBER);
            goto fail2;
        }

        // read value
        if (!cra_json_read_value(szer, stru + info->offset, meta, info, idx))
        {
        fail2:
            cra_ser_error_append(szer, &sv);
            goto fail;
        }

    after_value:
        READ_OBJECT_AFTER_VALUE(szer, buff);
    }

end:
    READ_OBJECT_END(szer, buff);
    return true;

no_buf:
    cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF);
    goto fail;
invalid_value:
    cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
fail:
    CRA_SER_DEL_STRUCT(val, stru, meta, is_ptr);
    return false;
}

static bool
cra_json_write_array(CraSerializer *szer, const unsigned char *val, const CraSerMeta *meta)
{
    uint16_t             index;
    uint16_t             count;
    const unsigned char *array;
    unsigned char       *buff;
    const CraSerInfo    *info;

    assert(val);
    assert(meta);
    assert(meta->infos);
    assert(meta->infos[0].offset == 0);

    // get array count
    memcpy(&count, val + meta->for_array.offsetof_len, sizeof(count));
    CRA_SER_CHECK_LIMIT(szer, count, >=, CRA_SER_LIST_MAX_COUNT);

    if (meta->count == 0)
        memcpy(&array, val, sizeof(void *)); // array = *(T **)val;
    else
        array = val;
    assert(array || (!array && count == 0));

    // check null
    CHECK_WRITE_NULL(szer, array, buff);

    info = &meta->infos[0];
    WRITE_ARRAY_BEGIN(szer, buff);

    for (index = 0; index < count; ++index)
    {
        WRITE_ARRAY_BEFORE_VALUE(szer, buff, index);
        // write value
        if (!cra_json_write_value(szer, array + index * meta->for_array.size_of_slot, meta, info, 0))
        {
            CraStringView sv = CRA_STRING_VIEW_INIT_CONST("<<ArrayItem>>");
            cra_ser_error_append(szer, &sv);
            return false;
        }
    }

    WRITE_ARRAY_END(szer, buff);
    return true;

no_buf:
    cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF);
    return false;
}

static bool
cra_json_read_array(CraSerializer *szer, unsigned char *val, const CraSerMeta *meta)
{
    uint16_t          index;
    size_t            count;
    unsigned char    *array;
    unsigned char    *buff;
    const CraSerInfo *info;

    assert(val);
    assert(meta);
    assert(meta->infos);
    assert(meta->infos[0].offset == 0);

    info = &meta->infos[0];
    READ_ARRAY_BEGIN(szer, buff);

    index = 0;
    count = meta->count == 0 ? 8 : meta->count;
    CRA_SER_NEW_ARRAY(szer, val, array, meta, count + 1, count);

    // check empty
    if (*buff == ']')
        goto end;

    for (; CRA_SER_ENOUGH(szer, sizeof("X"));)
    {
        // check enough
        if ((size_t)index == count)
        {
            if (index >= CRA_SER_LIST_MAX_COUNT)
            {
                cra_ser_error_set(szer, CRA_SER_ERR_LENGTH_MAX);
                goto fail;
            }

            if (meta->count > 0)
            {
                cra_ser_error_set(szer, CRA_SER_ERR_OUT_OF_RANGE);
                goto fail;
            }

            count *= 2;
            unsigned char *p = cra_realloc(array, count * meta->for_array.size_of_slot);
            if (!p)
            {
                cra_ser_error_set(szer, CRA_SER_ERR_NO_MEM);
                goto fail;
            }
            array = p;
            memcpy(val, &array, sizeof(void *)); // *(T **)val = array;
        }

        if (!cra_json_read_value(szer, array + index * meta->for_array.size_of_slot, meta, info, 0))
        {
            CraStringView sv = CRA_STRING_VIEW_INIT_CONST("<<ArrayItem>>");
            cra_ser_error_append(szer, &sv);
            goto fail;
        }

        ++index;
        READ_ARRAY_AFTER_VALUE(szer, buff);
    }

end:
    READ_ARRAY_END(szer, buff);
    // update array count
    memcpy(val + meta->for_array.offsetof_len, &index, sizeof(index));
    return true;

no_buf:
    cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF);
    goto fail;
invalid_value:
    cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
fail:
    // update array count
    memcpy(val + meta->for_array.offsetof_len, &index, sizeof(index));
    CRA_SER_DEL_ARRAY(val, array, meta);
    return false;
}

static bool
cra_json_write_list(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    size_t               index;
    size_t               count;
    size_t               slotsize;
    unsigned char        iter[64] = { 0 };
    const unsigned char *list;
    unsigned char       *buff;
    const CraSerInfo    *info;

    assert(val);
    assert(meta);
    assert(meta->infos);
    assert(meta->for_list.iter_init);
    assert(meta->for_list.iter_next);
    assert(meta->infos[0].offset == 0);

    if (is_ptr)
        memcpy(&list, val, sizeof(void *)); // list = *(T **)val;
    else
        list = val;

    CHECK_WRITE_NULL(szer, list, buff);

    meta->for_list.iter_init(iter, sizeof(iter), (void *)list, &count);
    CRA_SER_CHECK_LIMIT(szer, count, >=, CRA_SER_LIST_MAX_COUNT);

    WRITE_ARRAY_BEGIN(szer, buff);

    info = &meta->infos[0];
    slotsize = (info->flags & CRA_SER_FLAG_PTR) ? sizeof(void *) : info->size;
    CRA_TEMP_NEW(unsigned char, value, slotsize);
    if (!value)
    {
        cra_ser_error_set(szer, CRA_SER_ERR_NO_MEM);
        return false;
    }

    for (index = 0; meta->for_list.iter_next(iter, value); ++index)
    {
        WRITE_ARRAY_BEFORE_VALUE(szer, buff, index);
        // write value
        if (!cra_json_write_value(szer, value, meta, info, 0))
        {
            CraStringView sv = CRA_STRING_VIEW_INIT_CONST("<<ListItem>>");
            cra_ser_error_append(szer, &sv);
            goto fail;
        }
    }

    CRA_TEMP_DEL(unsigned char, value, slotsize);
    WRITE_ARRAY_END(szer, buff);
    return true;

no_buf:
    cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF);
fail:
    CRA_TEMP_DEL(unsigned char, value, slotsize);
    return false;
}

static bool
cra_json_read_list(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    size_t            slotsize;
    size_t            index;
    const CraSerInfo *info;
    unsigned char    *list;
    unsigned char    *buff;
    CraStringView     sv;

    assert(val);
    assert(meta);
    assert(meta->infos);
    assert(meta->for_list.init);
    assert(meta->for_list.append);
    assert(meta->infos[0].offset == 0);

    READ_ARRAY_BEGIN(szer, buff);

    info = &meta->infos[0];
    slotsize = (info->flags & CRA_SER_FLAG_PTR) ? sizeof(void *) : info->size;
    CRA_SER_NEW_LIST(szer, val, list, meta, is_ptr, 8 + 1, 8, slotsize);

    CRA_TEMP_NEW(unsigned char, value, slotsize);
    if (!value)
    {
        cra_ser_error_set(szer, CRA_SER_ERR_NO_MEM);
        goto fail;
    }

    // check empty
    if (*buff == ']')
        goto end;

    for (index = 0; CRA_SER_ENOUGH(szer, sizeof("X")); ++index)
    {
        if (index == CRA_SER_LIST_MAX_COUNT)
        {
            cra_ser_error_set(szer, CRA_SER_ERR_OUT_OF_RANGE);
            goto fail2;
        }

        if (!cra_json_read_value(szer, value, meta, info, 0))
        {
        fail3:
            sv = cra_string_view_from_cstr("<<ListItem>>", sizeof("<<ListItem>>") - 1);
            cra_ser_error_append(szer, &sv);
            goto fail2;
        }
        if (!meta->for_list.append(list, value))
        {
            cra_ser_destroy_object(value, meta->metas[0], info);
            cra_ser_error_set(szer, CRA_SER_ERR_APPEND_FAILED);
            goto fail3;
        }

        READ_ARRAY_AFTER_VALUE(szer, buff);
    }

end:
    READ_ARRAY_END(szer, buff);
    CRA_TEMP_DEL(unsigned char, value, slotsize);
    return true;

no_buf:
    cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF);
    goto fail2;
invalid_value:
    cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
fail2:
    CRA_TEMP_DEL(unsigned char, value, slotsize);
fail:
    CRA_SER_DEL_LIST(val, list, meta, is_ptr);
    return false;
}

static bool
cra_json_write_dict(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    size_t               index;
    size_t               count;
    size_t               keysize;
    size_t               valsize;
    unsigned char        iter[64] = { 0 };
    unsigned char       *buff;
    const unsigned char *dict;
    const CraSerInfo    *kinfo;
    const CraSerInfo    *vinfo;
    CraStringView        sv;

    assert(val);
    assert(meta);
    assert(meta->infos);
    assert(meta->for_dict.iter_init);
    assert(meta->for_dict.iter_next);
    assert(meta->infos[0].offset == 0);
    assert(meta->infos[0].type <= CRA_SER_TYPE_STRING);

    if (is_ptr)
        memcpy(&dict, val, sizeof(void *)); // dict = *(T **)val;
    else
        dict = val;

    CHECK_WRITE_NULL(szer, dict, buff);

    meta->for_dict.iter_init(iter, sizeof(iter), (void *)dict, &count);
    CRA_SER_CHECK_LIMIT(szer, count, >=, CRA_SER_DICT_MAX_COUNT);

    WRITE_OBJECT_BEGIN(szer, buff);

    kinfo = &meta->infos[0];
    vinfo = &meta->infos[1];
    keysize = (kinfo->flags & CRA_SER_FLAG_PTR) ? sizeof(void *) : kinfo->size;
    valsize = (vinfo->flags & CRA_SER_FLAG_PTR) ? sizeof(void *) : vinfo->size;
    CRA_TEMP_NEW(unsigned char, key, keysize + valsize);
    if (!key)
    {
        cra_ser_error_set(szer, CRA_SER_ERR_NO_MEM);
        return false;
    }
    unsigned char *value = key + keysize;

    for (index = 0; meta->for_dict.iter_next(iter, key, value); ++index)
    {
        WRITE_OBJECT_BEFORE_KEY(szer, buff, index);

        if (kinfo->type != CRA_SER_TYPE_STRING)
        {
            if (!CRA_SER_ENOUGH(szer, sizeof("\"\"")))
                goto no_buf;
            szer->buf[szer->curr++] = '\"';
        }
        if (!cra_json_write_value(szer, key, meta, kinfo, 0))
            goto fail2;
        if (kinfo->type != CRA_SER_TYPE_STRING)
        {
            if (!CRA_SER_ENOUGH(szer, sizeof("\"\"")))
                goto no_buf;
            szer->buf[szer->curr++] = '\"';
        }

        WRITE_OBJECT_AFTER_KEY(szer, buff);

        if (!cra_json_write_value(szer, value, meta, vinfo, 1))
        {
        fail2:
            sv = cra_string_view_from_cstr("<<DictItem>>", sizeof("<<DictItem>>") - 1);
            cra_ser_error_append(szer, &sv);
            goto fail;
        }
    }

    WRITE_OBJECT_END(szer, buff); // goto no_buf if fail
    CRA_TEMP_DEL(unsigned char, key, keysize + valsize);
    return true;

no_buf:
    cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF);
fail:
    CRA_TEMP_DEL(unsigned char, key, keysize + valsize);
    return false;
}

static bool
cra_json_read_dict(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    size_t            keysize;
    size_t            valsize;
    const CraSerInfo *kinfo;
    const CraSerInfo *vinfo;
    unsigned char    *dict;
    unsigned char    *buff;
    CraStringView     sv;

    assert(val);
    assert(meta);
    assert(meta->infos);
    assert(meta->for_dict.init);
    assert(meta->for_dict.append);
    assert(meta->infos[0].offset == 0);
    assert(meta->infos[0].type <= CRA_SER_TYPE_STRING);

    READ_OBJECT_BEGIN(szer, buff);

    kinfo = &meta->infos[0];
    vinfo = &meta->infos[1];
    keysize = (kinfo->flags & CRA_SER_FLAG_PTR) ? sizeof(void *) : kinfo->size;
    valsize = (vinfo->flags & CRA_SER_FLAG_PTR) ? sizeof(void *) : vinfo->size;
    CRA_SER_NEW_DICT(szer, val, dict, meta, is_ptr, 8 + 1, 8, keysize, valsize);

    CRA_TEMP_NEW(unsigned char, key, keysize + valsize);
    if (!key)
    {
        cra_ser_error_set(szer, CRA_SER_ERR_NO_MEM);
        goto fail;
    }
    unsigned char *value = key + keysize;

    // check empty
    if (*buff == '}')
        goto end;

    for (; CRA_SER_ENOUGH(szer, sizeof("X"));)
    {
        if (kinfo->type != CRA_SER_TYPE_STRING)
        {
            if (!CRA_SER_ENOUGH(szer, sizeof("\"\"")))
                goto no_buf;
            if (szer->buf[szer->curr++] != '\"')
                goto invalid_value;
        }
        if (!cra_json_read_value(szer, key, meta, kinfo, 0))
            goto fail3;
        if (kinfo->type != CRA_SER_TYPE_STRING)
        {
            if (!CRA_SER_ENOUGH(szer, sizeof("\"")))
                goto no_buf;
            if (szer->buf[szer->curr++] != '\"')
                goto invalid_value;
        }

        READ_OBJECT_AFTER_KEY(szer, buff);

        if (!cra_json_read_value(szer, value, meta, vinfo, 1))
        {
        fail4:
            if (kinfo->flags & CRA_SER_FLAG_PTR)
            {
                unsigned char *p;
                assert(kinfo->type == CRA_SER_TYPE_STRING);
                memcpy(&p, key, sizeof(void *));
                cra_free(p);
            }
        fail3:
            sv = cra_string_view_from_cstr("<<DictItem>>", sizeof("<<DictItem>>") - 1);
            cra_ser_error_append(szer, &sv);
            goto fail2;
        }

        if (!meta->for_dict.append(dict, key, value))
        {
            cra_ser_destroy_object(value, meta->metas[1], vinfo);
            cra_ser_error_set(szer, CRA_SER_ERR_APPEND_FAILED);
            goto fail4;
        }

        READ_OBJECT_AFTER_VALUE(szer, buff);
    }

end:
    READ_OBJECT_END(szer, buff);
    CRA_TEMP_DEL(unsigned char, key, keysize + valsize);
    return true;

no_buf:
    cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF);
    goto fail2;
invalid_value:
    cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
fail2:
    CRA_TEMP_DEL(unsigned char, key, keysize + valsize);
fail:
    CRA_SER_DEL_DICT(val, dict, meta, is_ptr);
    return false;
}

bool
cra_json_stringify_err(char *buf, size_t *len, const void *obj, const CraSerMeta *meta, CraSerErr *error, bool format)
{
    CraSerializer szer;

    assert(buf);
    assert(len);
    assert(obj);
    assert(meta);
    assert(meta->infos);
    assert(meta->infos->offset == 0);

    cra_serializer_init(&szer, buf, *len, format);

    switch (meta->type)
    {
        case CRA_SER_TYPE_STRUCT:
            cra_json_write_struct(&szer, obj, false, meta);
            break;
        case CRA_SER_TYPE_LIST:
            if (meta->flags & CRA_SER_FLAG_ARRAY)
                cra_json_write_array(&szer, obj, meta);
            else
                cra_json_write_list(&szer, obj, false, meta);
            break;
        case CRA_SER_TYPE_DICT:
            cra_json_write_dict(&szer, obj, false, meta);
            break;

        default:
            // unreachable
            abort();
    }

    assert(szer.curr < szer.size);
    szer.buf[szer.curr] = '\0';
    *len = szer.curr + 1;
    if (error)
        memcpy(error, &szer.error, sizeof(CraSerErr));

    assert(szer.error.err || szer.nesting == 0);

    return szer.error.err == CRA_SER_ERR_OK;
}

bool
cra_json_parse_err(const char *buf, size_t len, void *obj, const CraSerMeta *meta, CraSerErr *error)
{
    CraSerializer szer;

    assert(buf);
    assert(len);
    assert(obj);
    assert(meta);
    assert(meta->infos);
    assert(meta->infos->offset == 0);

    CraStringView sv = CRA_STRING_VIEW_INIT((char *)buf, len - 1);
    sv = cra_string_view_trim(&sv);

    cra_serializer_init(&szer, sv.data, sv.size, false);

    switch (meta->type)
    {
        case CRA_SER_TYPE_STRUCT:
            cra_json_read_struct(&szer, obj, false, meta);
            break;
        case CRA_SER_TYPE_LIST:
            if (meta->flags & CRA_SER_FLAG_ARRAY)
                cra_json_read_array(&szer, obj, meta);
            else
                cra_json_read_list(&szer, obj, false, meta);
            break;
        case CRA_SER_TYPE_DICT:
            cra_json_read_dict(&szer, obj, false, meta);
            break;

        default:
            // unreachable
            abort();
    }

    assert(szer.error.err || szer.nesting == 0);

    if (error)
        memcpy(error, &szer.error, sizeof(CraSerErr));

    return szer.error.err == CRA_SER_ERR_OK;
}
