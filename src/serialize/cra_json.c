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
#define __CRA_SER_INNER
#include "serialize/cra_json.h"
#include "cra_malloc.h"
#include "serialize/cra_serialize.h"
#include <float.h>
#include <math.h>

static inline int
__cra_hex2int(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';
    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';
    return -1; // 非法字符
}

// 解析"\uXXXX"序列，返回unicode代码点
static uint32_t
__cra_parse_unicodee_escape(const char *buf)
{
    uint32_t code_point = 0;
    for (int i = 0; i < 4; ++i)
    {
        int val = __cra_hex2int(*buf);
        if (val == -1)
            return UINT32_MAX; // error
        code_point = (code_point << 4) | val;
        buf++;
    }
    return code_point;
}

static inline bool
__cra_is_high_surrogate(uint32_t code_point)
{
    return code_point >= 0xd800 && code_point <= 0xdbff;
}

static inline bool
__cra_is_low_surrogate(uint32_t code_point)
{
    return code_point >= 0xdc00 && code_point <= 0xdfff;
}

static inline uint32_t
__cra_combine_surrogates(uint32_t high, uint32_t low)
{
    return 0x10000 + ((high - 0xd800) << 10) + (low - 0xdc00);
}

static int32_t
__cra_code_point2utf8(uint32_t code_point, unsigned char *str, size_t remain)
{
#define CHECK(_needed)    \
    if (remain < _needed) \
    {                     \
        return -1;        \
    }

    if (code_point <= 0x7f)
    {
        CHECK(1)
        str[0] = (unsigned char)(code_point);
        return 1;
    }
    else if (code_point <= 0x7ff)
    {
        CHECK(2)
        str[0] = (unsigned char)(0xc0 | (code_point >> 6));
        str[1] = (unsigned char)(0x80 | (code_point & 0x3f));
        return 2;
    }
    else if (code_point <= 0xffff)
    {
        CHECK(3)
        str[0] = (unsigned char)(0xe0 | (code_point >> 12));
        str[1] = (unsigned char)(0x80 | ((code_point >> 6) & 0x3f));
        str[2] = (unsigned char)(0x80 | (code_point & 0x3f));
        return 3;
    }
    else if (code_point <= 0x10ffff)
    {
        CHECK(4)
        str[0] = (unsigned char)(0xf0 | (code_point >> 18));
        str[1] = (unsigned char)(0x80 | ((code_point >> 12) & 0x3f));
        str[2] = (unsigned char)(0x80 | ((code_point >> 6) & 0x3f));
        str[3] = (unsigned char)(0x80 | (code_point & 0x3f));
        return 4;
    }
    return -1; // 无效代码点

#undef CHECK
}

static bool
cra_json_write_string(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    char *buf;
    char *str;
    char *src;
    char *strbuf;

    src = meta->is_ptr ? *(char **)val : (char *)val;

    // 空串
    if (src[0] == '\0')
    {
        CRA_SERIALIZER_ENSURE_(ser, buf, sizeof("\"\""), sizeof(""));
        memcpy(buf, "\"\"", sizeof("\"\""));
        return true;
    }

    // 计算字符串长度
    size_t needed = 0;
    size_t strlength = 0;
    for (str = src; *str; ++str)
    {
        ++strlength;
        switch (*str)
        {
            case '\b':
            case '\t':
            case '\n':
            case '\f':
            case '\r':
            case '\"':
            case '\\':
                needed += 2;
                break;

            default:
                if (*str < 0x20)
                    needed += 6; // 控制字符 -> utf-16 \uXXXX
                else
                    ++needed;
                break;
        }
    }

    CRA_SERIALIZER_ENSURE_(ser, buf, needed + sizeof("\"\""), sizeof(""));

    // write string

    *buf = '\"';

    if (needed == strlength)
    {
        memcpy(buf + 1, src, needed);
    }
    else
    {
        for (str = src, strbuf = buf + 1; *str; ++str, ++strbuf)
        {
            switch (*str)
            {
                case '\b':
                    *strbuf++ = '\\';
                    *strbuf = 'b';
                    break;
                case '\t':
                    *strbuf++ = '\\';
                    *strbuf = 't';
                    break;
                case '\n':
                    *strbuf++ = '\\';
                    *strbuf = 'n';
                    break;
                case '\f':
                    *strbuf++ = '\\';
                    *strbuf = 'f';
                    break;
                case '\r':
                    *strbuf++ = '\\';
                    *strbuf = 'r';
                    break;
                case '\"':
                    *strbuf++ = '\\';
                    *strbuf = '\"';
                    break;
                case '\\':
                    *strbuf++ = '\\';
                    *strbuf = '\\';
                    break;

                default:
                    if (*str < 0x20)
                    {
                        snprintf((char *)strbuf, 10, "\\u%04x", *str);
                        strbuf += 5;
                    }
                    else
                    {
                        *strbuf = *str;
                    }
                    break;
            }
        }
    }

    *(buf + needed + 1) = '\"';
    *(buf + needed + 2) = '\0';

    return true;
}

static bool
cra_json_read_string(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    char  *buf, *str;
    size_t l, len, length, max_length;

    CRA_SERIALIZER_ENSURE_(ser, buf, sizeof("\"\""), sizeof("\"\""));
    if (*buf != '\"')
    {
        // 不是字符串
        goto fail;
    }

    ++buf; // 越过'\"'

    // 检查串中是否有特殊字符
    for (length = 0, str = buf; (size_t)(str - ser->buffer) < ser->maxlen && *str != '\"'; ++str)
    {
        if (*str == '\\')
        {
            (void)++length;
            (void)++str;
        }
    }

    // 如果没有闭合的'\"'则表示这不是正确的字符串
    if (*str != '\"')
        goto fail;

    // get string length
    l = len = (size_t)(str - buf) - length;
    if (meta->is_ptr)
    {
        max_length = len + sizeof("");
        str = *(char **)retval = (char *)cra_malloc(max_length);
        cra_release_mgr_add(&ser->release, str, meta);
    }
    else
    {
        max_length = meta->size;
        str = (char *)retval;
    }

    // 没有特殊字符时copy string
    if (length == 0)
    {
        // 测试字符数组大小是否能容纳该字符串
        if (max_length <= len)
        {
        too_small:
            CRA_SERIALIZER_ERROR(
              ser, meta, CRA_SER_ERR_TOO_SMALL, "string size too small(%zu < %zu)", meta->size, l + 1);
            return false;
        }

        memcpy(str, buf, len);
        str[len] = '\0';
        goto ok;
    }

    // 有特殊字符时copy string
    len += length;
    size_t remaining = max_length;
    for (length = 0; length < len;)
    {
        // 测试字符数组大小是否能容纳该字符串
        if (remaining <= 1)
            goto too_small;

        if (buf[length] != '\\')
        {
            *str++ = buf[length];
        }
        else
        {
            switch (buf[++length])
            {
                case 'b':
                    *str++ = '\b';
                    break;
                case 't':
                    *str++ = '\t';
                    break;
                case 'n':
                    *str++ = '\n';
                    break;
                case 'f':
                    *str++ = '\f';
                    break;
                case 'r':
                    *str++ = '\r';
                    break;
                case '\"':
                    *str++ = '\"';
                    break;
                case '/':
                    *str++ = '/';
                    break;
                case '\\':
                    *str++ = '\\';
                    break;
                case 'u':
                {
                    (void)++length; // 跳过'u'
                    uint32_t code_point = __cra_parse_unicodee_escape(buf + length);
                    if (code_point == UINT32_MAX)
                        goto fail;
                    length += sizeof("XXXX") - 1;

                    // 处理代理对
                    if (!__cra_is_high_surrogate(code_point))
                    {
                        if (__cra_is_low_surrogate(code_point))
                            goto fail; // 只有单独的低位代理
                        // ascii
                        goto to_utf8;
                    }
                    if (buf[0] != '\\' || buf[1] != 'u')
                        goto fail; // 代理对不完整
                    length += sizeof("\\u") - 1;
                    uint32_t low_surrogate = __cra_parse_unicodee_escape(buf + length);
                    if (!__cra_is_low_surrogate(low_surrogate))
                        goto fail;
                    length += sizeof("XXXX") - 1;
                    code_point = __cra_combine_surrogates(code_point, low_surrogate);

                to_utf8:
                    // to utf-8
                    {
                        int retlen = __cra_code_point2utf8(code_point, (unsigned char *)str, remaining);
                        if (retlen == -1)
                            goto fail;
                        remaining -= retlen;
                        str += retlen;
                    }
                    continue; // 跳过下面的"++length"
                }
                break;
                default:
                    goto fail;
            }
        }

        (void)++length;
        (void)--remaining;
    }

    *str = '\0';
ok:
    ser->index += len + sizeof("\"\"") - 1;
    return true;

fail:
    CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid string value");
    return false;
}

static inline bool
cra_json_write_bool(CraSerializer *ser, void *val)
{
    char *buf;
    if (!!(*(bool *)val))
    {
        CRA_SERIALIZER_ENSURE_(ser, buf, sizeof("true"), sizeof(""));
        memcpy(buf, "true", sizeof("true"));
    }
    else
    {
        CRA_SERIALIZER_ENSURE_(ser, buf, sizeof("false"), sizeof(""));
        memcpy(buf, "false", sizeof("false"));
    }
    return true;
}

static inline bool
cra_json_read_bool(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    char *buf = CRA_SERIALIZER_GET_BUF(ser);
    if (CRA_SERIALIZER_IS_ENOUGH(ser, sizeof("true")) && strncmp((char *)buf, "true", sizeof("true") - 1) == 0)
    {
        ser->index += sizeof("true") - 1;
        *(bool *)retval = true;
        return true;
    }
    else if (CRA_SERIALIZER_IS_ENOUGH(ser, sizeof("false")) && strncmp((char *)buf, "false", sizeof("false") - 1) == 0)
    {
        ser->index += sizeof("false") - 1;
        *(bool *)retval = false;
        return true;
    }
    CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid bool value");
    return false;
}

static bool
__cra_json_read_float(CraSerializer *ser, double *retval)
{
    double dbl;
    size_t len;
    char  *end;
    char  *buf;

    end = NULL;
    buf = CRA_SERIALIZER_GET_BUF(ser);
    dbl = strtod((char *)buf, (char **)&end);
    if (buf == end || isnan(dbl) || isinf(dbl))
        return false;

    // enough?
    len = (size_t)(end - buf);
    CRA_SERIALIZER_ENSURE_(ser, buf, len + sizeof(""), sizeof(""));

    *retval = dbl;
    return true;
}

static bool
cra_json_write_int(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    int     len;
    int64_t i64;
    char   *buf;

    // get int number
    cra_serializer_p2i(val, &i64, meta);
#ifdef CRA_JSON_SAFE_INT
    i64 = CRA_CLAMP(i64, CRA_MAX_SAFE_INT, CRA_MIN_SAFE_INT);
#endif

    // get length
    len = snprintf(NULL, 0, "%zd", i64);
    assert(len > 0);

    // write value
    CRA_SERIALIZER_ENSURE_(ser, buf, len + sizeof(""), sizeof(""));
    snprintf(buf, len + sizeof(""), "%zd", i64);
    return true;
}

static bool
cra_json_read_int(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    size_t  len;
    int64_t i64;
    char   *end;
    char   *buf;

    end = NULL;
    buf = CRA_SERIALIZER_GET_BUF(ser);
    i64 = strtoll((char *)buf, (char **)&end, 10); // buf在输入时保证以null结尾，所以strtoXX是安全的
    if (buf == end)
        goto fail;
    // float?
    if (*end == '.' || *end == 'e' || *end == 'E')
    {
        double dbl;
        if (!__cra_json_read_float(ser, &dbl))
            goto fail;

#ifdef CRA_JSON_SAFE_INT
        i64 = (int64_t)CRA_CLAMP(dbl, (double)CRA_MAX_SAFE_INT, (double)CRA_MIN_SAFE_INT);
#else
        i64 = (int64_t)dbl;
#endif
    }
    else
    {
        len = (size_t)(end - buf);
        CRA_SERIALIZER_ENSURE_(ser, buf, len + sizeof(""), sizeof(""));
#ifdef CRA_JSON_SAFE_INT
        i64 = CRA_CLAMP(i64, CRA_MAX_SAFE_INT, CRA_MIN_SAFE_INT);
#endif
    }

    cra_serializer_i2p(i64, retval, meta);
    return true;

fail:
    if (!ser->error.err)
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid int value");
    return false;
}

static bool
cra_json_write_uint(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    int      len;
    uint64_t u64;
    char    *buf;

    // get uint number
    cra_serializer_p2u(val, &u64, meta);
#ifdef CRA_JSON_SAFE_INT
    u64 = CRA_MIN(u64, CRA_MAX_SAFE_INT);
#endif

    // get length
    len = snprintf(NULL, 0, "%zu", u64);
    assert(len > 0);

    // write value
    CRA_SERIALIZER_ENSURE_(ser, buf, len + sizeof(""), sizeof(""));
    snprintf(buf, len + sizeof(""), "%zu", u64);
    return true;
}

static bool
cra_json_read_uint(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    size_t   len;
    uint64_t u64;
    char    *end;
    char    *buf;

    end = NULL;
    buf = CRA_SERIALIZER_GET_BUF(ser);
    u64 = strtoull((char *)buf, (char **)&end, 10);
    if (buf == end)
        goto fail;
    // float?
    if (*end == '.' || *end == 'e' || *end == 'E')
    {
        double dbl;
        if (!__cra_json_read_float(ser, &dbl))
            goto fail;

#ifdef CRA_JSON_SAFE_INT
        u64 = (uint64_t)CRA_CLAMP(dbl, (double)CRA_MAX_SAFE_INT, 0);
#else
        u64 = (uint64_t)dbl;
#endif
    }
    else
    {
        len = (size_t)(end - buf);
        CRA_SERIALIZER_ENSURE_(ser, buf, len + sizeof(""), sizeof(""));
#ifdef CRA_JSON_SAFE_INT
        u64 = CRA_MIN(u64, CRA_MAX_SAFE_INT);
#endif
    }

    cra_serializer_u2p(u64, retval, meta);
    return true;

fail:
    if (!ser->error.err)
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid uint value");
    return false;
}

static inline bool
cra_json_compare_double(double a, double b)
{
    return fabs(a - b) < DBL_EPSILON;
}

static bool
cra_json_write_float(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    int    len;
    double dbl;
    double tmp;
    char  *buf;
    char   dblbuf[32];

    switch (meta->size)
    {
        case sizeof(float):
            dbl = *(float *)val;
            break;
        case sizeof(double):
            dbl = *(double *)val;
            break;
        default:
            assert_always(false);
    }

    if (isnan(dbl) || isinf(dbl))
    {
    invalid_value:
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid float value");
        return false;
    }

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
        goto invalid_value;

    CRA_SERIALIZER_ENSURE_(ser, buf, len + sizeof(""), sizeof(""));
    memcpy(buf, dblbuf, len);
    buf[len] = '\0';
    return true;
}

static bool
cra_json_read_float(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    double dbl;

    if (__cra_json_read_float(ser, &dbl))
    {
        switch (meta->size)
        {
            case sizeof(float):
                *(float *)retval = (float)dbl;
                break;
            case sizeof(double):
                *(double *)retval = dbl;
                break;
            default:
                assert_always(false);
        }
        return true;
    }
    if (!ser->error.err)
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid float value");
    return false;
}

static bool
cra_json_write_struct(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
cra_json_read_struct(CraSerializer *ser, void *retval, const CraTypeMeta *meta);
static bool
cra_json_write_array(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
cra_json_read_array(CraSerializer *ser, void *retval, const CraTypeMeta *meta);
static bool
cra_json_write_list(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
cra_json_read_list(CraSerializer *ser, void *retval, const CraTypeMeta *meta);
static bool
cra_json_write_dict(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
cra_json_read_dict(CraSerializer *ser, void *retval, const CraTypeMeta *meta);

static bool
cra_json_write_value(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    char *buf;

    assert(val);
    assert(meta);
    assert(meta->name);
    assert(meta->is_not_end);

    // null?
    if (meta->is_ptr && !(*(void **)val))
    {
        CRA_SERIALIZER_CHECK_NULL(ser, meta);
        CRA_SERIALIZER_ENSURE_(ser, buf, sizeof("null"), sizeof(""));
        memcpy(buf, "null", sizeof("null"));
        return true;
    }

    switch (meta->type)
    {
        case CRA_TYPE_BOOL:
            return cra_json_write_bool(ser, val);
        case CRA_TYPE_INT:
        case CRA_TYPE_VARINT:
            return cra_json_write_int(ser, val, meta);
        case CRA_TYPE_UINT:
        case CRA_TYPE_VARUINT:
            return cra_json_write_uint(ser, val, meta);
        case CRA_TYPE_FLOAT:
            return cra_json_write_float(ser, val, meta);
        case CRA_TYPE_STRING:
            return cra_json_write_string(ser, val, meta);
        case CRA_TYPE_STRUCT:
            return cra_json_write_struct(ser, val, meta);
        case CRA_TYPE_LIST:
            if (meta->szer_i)
                return cra_json_write_list(ser, val, meta);
            else
                return cra_json_write_array(ser, val, meta);
        case CRA_TYPE_DICT:
            return cra_json_write_dict(ser, val, meta);

        default:
            assert_always(false);
    }
}

static bool
cra_json_read_value(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    char     *buf;
    CraType_e type;

    assert(retval);
    assert(meta);
    assert(meta->name);
    assert(meta->is_not_end);

    CRA_SERIALIZER_ENSURE_(ser, buf, sizeof("X"), sizeof("X"));

#define CHECK_TYPE(_TYPE)        \
    do                           \
    {                            \
        if (meta->type != _TYPE) \
        {                        \
            type = _TYPE;        \
            goto type_mismatch;  \
        }                        \
    } while (0)

    // check null
    if (CRA_SERIALIZER_IS_ENOUGH(ser, sizeof("null")) && strncmp((char *)buf, "null", sizeof("null") - 1) == 0)
    {
        CRA_SERIALIZER_CHECK_NULL(ser, meta);
        ser->index += sizeof("null") - 1;
        *(void **)retval = NULL;
        return true;
    }

    // bool
    if (*buf == 't' || *buf == 'f')
    {
        CHECK_TYPE(CRA_TYPE_BOOL);
        return cra_json_read_bool(ser, retval, meta);
    }
    // number
    else if (*buf == '-' || (*buf >= '0' && *buf <= '9'))
    {
        switch (meta->type)
        {
            case CRA_TYPE_INT:
            case CRA_TYPE_VARINT:
                return cra_json_read_int(ser, retval, meta);
            case CRA_TYPE_UINT:
            case CRA_TYPE_VARUINT:
                return cra_json_read_uint(ser, retval, meta);
            case CRA_TYPE_FLOAT:
                return cra_json_read_float(ser, retval, meta);
            default:
                type = CRA_TYPE_INT;
                goto type_mismatch;
        }
    }
    // string
    else if (*buf == '\"')
    {
        CHECK_TYPE(CRA_TYPE_STRING);
        return cra_json_read_string(ser, retval, meta);
    }
    // struct/dict
    else if (*buf == '{')
    {
        if (meta->type == CRA_TYPE_STRUCT)
        {
            return cra_json_read_struct(ser, retval, meta);
        }
        else if (meta->type == CRA_TYPE_DICT)
        {
            return cra_json_read_dict(ser, retval, meta);
        }
        else
        {
            type = CRA_TYPE_STRUCT;
            goto type_mismatch;
        }
    }
    // array/list
    else if (*buf == '[')
    {
        CHECK_TYPE(CRA_TYPE_LIST);
        if (meta->szer_i)
            return cra_json_read_list(ser, retval, meta);
        else
            return cra_json_read_array(ser, retval, meta);
    }
    // error value
    else
    {
        CRA_SERIALIZER_ERROR1(ser, CRA_SER_ERR_INVALID_VAL, "invalid value");
        return false;
    }

type_mismatch:
    CRA_SERIALIZER_CHECK_TYPE(ser, meta, type);
    return false;

#undef CHECK_TYPE
}

static inline void
cra_json_skip_whitespaces(CraSerializer *ser)
{
    while (CRA_SERIALIZER_IS_ENOUGH(ser, 1) && ser->buffer[ser->index] <= 0x20 && ser->buffer[ser->index] != '\0')
        ++ser->index;
}

static inline bool
cra_json_write_ch(CraSerializer *ser, char ch)
{
    char *buf;
    CRA_SERIALIZER_ENSURE_(ser, buf, sizeof("X"), sizeof(""));
    *buf++ = ch;
    *buf = '\0';
    return true;
}

static inline bool
cra_json_check_ch(CraSerializer *ser, char ch)
{
    char *buf;
    cra_json_skip_whitespaces(ser);
    CRA_SERIALIZER_ENSURE_(ser, buf, sizeof("X"), sizeof("X"));
    if (*buf == ch)
    {
        ++ser->index;
        return true;
    }
    return false;
}

static void
cra_json_skip_value(CraSerializer *ser)
{
    ssize_t leftcount = 0;
    while (CRA_SERIALIZER_IS_ENOUGH(ser, sizeof("X")) && (ser->buffer[ser->index] != ',' || leftcount > 0))
    {
        if (ser->buffer[ser->index] == '{' || ser->buffer[ser->index] == '[')
        {
            ++leftcount;
        }
        else if (ser->buffer[ser->index] == '}' || ser->buffer[ser->index] == ']')
        {
            if (--leftcount <= 0)
            {
                ++ser->index; // skip '}' or ']'
                break;
            }
        }
        ++ser->index;
    }
}

static bool
cra_json_write_key(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    // check key type
    CRA_SERIALIZER_CHECK_KEY(ser, meta);
    // not string
    if (meta->type != CRA_TYPE_STRING)
    {
        if (!cra_json_write_ch(ser, '\"'))
            return false;
    }
    // write key
    if (!cra_json_write_value(ser, val, meta))
    {
        CRA_SERIALIZER_ERROR(ser, meta, ser->error.err, "failed to write a JSON key");
        return false;
    }
    // not string
    if (meta->type != CRA_TYPE_STRING)
    {
        if (!cra_json_write_ch(ser, '\"'))
            return false;
    }
    return true;
}

static bool
cra_json_read_key(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    char *buf;
    CRA_UNUSED_VALUE(buf);

    // check key type
    CRA_SERIALIZER_CHECK_KEY(ser, meta);

    cra_json_skip_whitespaces(ser);

    // not string
    if (meta->type != CRA_TYPE_STRING)
    {
        if (!cra_json_check_ch(ser, '\"'))
        {
        error_key:
            if (!ser->error.err)
                CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid JSON key");
            return false;
        }
    }

    // write key
    if (!cra_json_read_value(ser, retval, meta))
    {
        CRA_SERIALIZER_ERROR(ser, meta, ser->error.err, "failed to write a JSON key");
        return false;
    }

    // not string
    if (meta->type != CRA_TYPE_STRING)
    {
        if (!cra_json_check_ch(ser, '\"'))
            goto error_key;
    }
    return true;
}

static bool
cra_json_write_comma(CraSerializer *ser, size_t needed, bool is_first)
{
    char *buf;
    CRA_SERIALIZER_ENSURE_(ser, buf, needed + (size_t)!is_first, sizeof(""));
    if (!is_first)
        *buf++ = ',';
    if (ser->format)
    {
        *buf++ = '\n';
        for (uint32_t j = 0; j < ser->nesting; ++j)
            *buf++ = '\t';
    }
    *buf = '\0';
    return true;
}

static bool
cra_json_write_finish(CraSerializer *ser, unsigned char finish_ch)
{
    size_t needed;
    char  *buf;

    // write "F" or "\n\t..\tF"
    needed = ser->format ? ser->nesting + sizeof("\nF") : sizeof("F");
    CRA_SERIALIZER_ENSURE_(ser, buf, needed, sizeof(""));
    if (ser->format)
    {
        *buf++ = '\n';
        for (uint32_t i = 0; i < ser->nesting; ++i)
            *buf++ = '\t';
    }
    *buf++ = finish_ch;
    *buf = '\0';
    return true;
}

// return: 0 -> finished
//         1 -> not finished
//        -1 -> error
static int
cra_json_is_finish(CraSerializer *ser, unsigned char finish_ch, const CraTypeMeta *meta)
{
    cra_json_skip_whitespaces(ser);
    if (CRA_SERIALIZER_IS_ENOUGH(ser, sizeof("X")))
    {
        if (ser->buffer[ser->index] == ',')
        {
            ++ser->index; // skip ','
            return 1;
        }
        // finish
        else if (ser->buffer[ser->index] == finish_ch)
        {
            ++ser->index;
            return 0;
        }
        // error
        else
        {
            CRA_SERIALIZER_ERROR(ser,
                                 meta,
                                 CRA_SER_ERR_INVALID_VAL,
                                 "invalid value. expected a '%c' or ',', but got a '%c'",
                                 finish_ch,
                                 ser->buffer[ser->index]);
            return -1;
        }
    }
    // no more buffer
    else
    {
        CRA_SERIALIZER_ERROR1(ser, CRA_SER_ERR_NOBUF, "buffer size too small");
        return -1;
    }
}

static const CraTypeMeta s_keymeta = {
    true, false, false, 0, CRA_TYPE_STRING, "<<JSON_KEY>>", CRA_MAX_JSON_KEY_LENGTH, 0, NULL, { NULL }, NULL
};

static bool
cra_json_write_struct(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    char              *buf;
    char              *stru;
    int                nfields;
    size_t             needed;
    const CraTypeMeta *m;

    assert(meta->submeta && meta->submeta->is_not_end);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // write "{"
    if (!cra_json_write_ch(ser, '{'))
        return false;

    stru = meta->is_ptr ? *(char **)val : (char *)val;
    needed = ser->format ? ser->nesting + sizeof("\n") : sizeof("");
    for (nfields = 0, m = meta->submeta; m->is_not_end; ++m)
    {
        if (m->is_len)
            continue;

        assert(m->name);
        // assert(nfields <= UINT8_MAX);
        assert(meta->size > m->offset);

        // write "," or ",\n\t..\t"
        if (!cra_json_write_comma(ser, needed, nfields++ == 0))
            return false;

        // write key
        if (!cra_json_write_key(ser, (void *)m->name, &s_keymeta))
            return false;

        // write ":" or ": "
        CRA_SERIALIZER_ENSURE_(ser, buf, ser->format ? sizeof(": ") : sizeof(":"), sizeof(""));
        *buf++ = ':';
        if (ser->format)
            *buf++ = ' ';
        *buf = '\0';

        // write value
        if (!cra_json_write_value(ser, stru + m->offset, m))
            return false;
    }

    CRA_SERIALIZER_NESTING_DEC(ser);

    // write "}" or "\n\t..\t}"
    return cra_json_write_finish(ser, '}');
}

static bool
cra_json_read_struct(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    char               ch;
    int                res;
    bool               ret;
    char              *stru;
    const CraTypeMeta *m, *last;

    assert(meta->submeta && meta->submeta->is_not_end);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    if (!cra_json_check_ch(ser, '{'))
    {
        ch = '{';
    invalid_value:
        if (!ser->error.err)
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid struct value. expected a '%c'", ch);
        return false;
    }

    void (*uninit)(void *, bool) = meta->init_i ? meta->init_i->uninit : NULL;

    // alloc
    if (meta->is_ptr)
        stru = *(char **)retval = (char *)cra_malloc(meta->size);
    else
        stru = (char *)retval;

    if (meta->is_ptr || uninit)
        cra_release_mgr_add(&ser->release, stru, meta);

    // init
    if (meta->init_i && meta->init_i->init)
    {
        CraInitArgs da = { .size = meta->size, .length = 0, .val1size = 0, .val2size = 0, .arg = meta->arg };
        meta->init_i->init(stru, &da);
    }
    else
    {
        bzero(stru, meta->size);
    }

    ret = true;

    // check empty
    if (cra_json_check_ch(ser, '}'))
        goto end;

    char temp[CRA_MAX_JSON_KEY_LENGTH];
    for (m = meta->submeta;;)
    {
        last = m;

        // read key
        if (!(ret = cra_json_read_key(ser, temp, &s_keymeta)))
            break;

        // check ':'
        if (!(ret = cra_json_check_ch(ser, ':')))
        {
            ch = ':';
            goto invalid_value;
        }

        // skip white
        cra_json_skip_whitespaces(ser);

        // match key string
        while (true)
        {
            if (m->is_len)
            {
                if (!(++m)->is_not_end)
                    m = meta->submeta;
            }

            // compare key
            if (strcmp((char *)temp, m->name) == 0)
            {
                // read member
                if (!(ret = cra_json_read_value(ser, stru + m->offset, m)))
                    goto end;

                if (!(++m)->is_not_end)
                    m = meta->submeta;
                break;
            }

            if (!(++m)->is_not_end)
                m = meta->submeta;

            // skip this member
            if (m == last)
            {
                cra_json_skip_value(ser);
                break;
            }
        }

        if ((res = cra_json_is_finish(ser, '}', meta)) != 1)
        {
            if (res == -1)
                ret = false;
            break;
        }
    }

end:
    CRA_SERIALIZER_NESTING_DEC(ser);

    return ret;
}

static bool
cra_json_write_array(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    char              *arr;
    size_t             slot;
    uint64_t           count;
    void              *pcount;
    const CraTypeMeta *metacnt;
    size_t             needed;

    assert(!meta->szer_i);
    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert((meta + 1)->is_len && (meta + 1)->type == CRA_TYPE_UINT);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // write "["
    if (!cra_json_write_ch(ser, '['))
        return false;

    count = 0;
    metacnt = meta + 1;
    arr = meta->is_ptr ? *(char **)val : (char *)val;
    pcount = !metacnt->arg ? (void *)((char *)val - meta->offset + metacnt->offset) : metacnt->arg;
    assert(pcount);

    // get count
    cra_serializer_p2u(pcount, &count, metacnt);

    // empty?
    if (count == 0)
    {
        if (!cra_json_write_ch(ser, ']'))
            return false;

        CRA_SERIALIZER_NESTING_DEC(ser);
        return true;
    }

    // write elements
    slot = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;
    needed = ser->format ? ser->nesting + sizeof("\n") : sizeof("");
    for (uint64_t i = 0; i < count; ++i)
    {
        // write "," or ",\n\t..\t"
        if (!cra_json_write_comma(ser, needed, i == 0))
            return false;

        // write element
        if (!cra_json_write_value(ser, arr + (i * slot), meta->submeta))
            return false;
    }

    CRA_SERIALIZER_NESTING_DEC(ser);

    return cra_json_write_finish(ser, ']');
}

static bool
cra_json_read_array(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    int                res;
    bool               ret;
    char              *arr;
    size_t             size;
    size_t             slot;
    uint64_t           index;
    uint64_t           maxcnt;
    void              *pcount;
    const CraTypeMeta *metacnt;

    assert(!meta->szer_i);
    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert((meta + 1)->is_len && (meta + 1)->type == CRA_TYPE_UINT);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // check '['
    if (!cra_json_check_ch(ser, '['))
    {
        if (!ser->error.err)
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid array value. expected a '['");
        return false;
    }

    metacnt = meta + 1;
    slot = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;
    pcount = !metacnt->arg ? (void *)((char *)retval - meta->offset + metacnt->offset) : metacnt->arg;
    assert(pcount);

    if (meta->is_ptr)
    {
        maxcnt = 8;
        size = maxcnt * slot;
        arr = (char *)cra_malloc(size);
    }
    else
    {
        arr = (char *)retval;
        size = meta->size;
        maxcnt = size / slot;
    }
    bzero(arr, size);
    index = 0;

    // check empty
    if ((ret = cra_json_check_ch(ser, ']')))
        goto end;

    // read elements
    for (; CRA_SERIALIZER_IS_ENOUGH(ser, sizeof("x")); ++index)
    {
        // enough?
        if (index >= maxcnt)
        {
            if (!meta->is_ptr)
            {
                CRA_SERIALIZER_ERROR(
                  ser, meta, CRA_SER_ERR_TOO_SMALL, "array size too small(%zu < %zu)", maxcnt, index + 1);
                ret = false;
                break;
            }
            maxcnt *= 1.5;
            arr = (char *)cra_realloc(arr, maxcnt * slot);
        }

        cra_json_skip_whitespaces(ser);

        // read element
        if (!(ret = cra_json_read_value(ser, arr + index * slot, meta->submeta)))
            goto end;

        res = cra_json_is_finish(ser, ']', meta);
        if (res != 1)
        {
            if (res == -1)
                ret = false;
            ++index;
            break;
        }
    }

end:
    if (meta->is_ptr)
    {
        cra_release_mgr_add(&ser->release, arr, meta);
        *(void **)retval = (void *)arr;
    }
    cra_serializer_u2p(index, pcount, metacnt);
    return ret;
}

static bool
cra_json_write_list(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    char    *list;
    uint64_t i, count;
    void    *value = NULL;
    char     it[64] = { 0 };
    size_t   needed;

    assert(meta->szer_i);
    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert(meta->szer_i->iter_init && meta->szer_i->iter_next);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // write '['
    if (!cra_json_write_ch(ser, '['))
        return false;

    list = meta->is_ptr ? *(char **)val : (char *)val;

    // init it & get count
    meta->szer_i->iter_init(list, &count, it, sizeof(it));

    // empty?
    if (count == 0)
    {
        if (!cra_json_write_ch(ser, ']'))
            return false;

        CRA_SERIALIZER_NESTING_DEC(ser);
        return true;
    }

    // write elements
    needed = ser->format ? ser->nesting + sizeof("\n") : sizeof("");
    for (i = 0; meta->szer_i->iter_next(it, (void **)&value, NULL); ++i)
    {
        // write "," or ",\n\t..\t"
        if (!cra_json_write_comma(ser, needed, i == 0))
            return false;

        // write element
        if (!cra_json_write_value(ser, value, meta->submeta))
            return false;
    }

    assert(i == count);

    CRA_SERIALIZER_NESTING_DEC(ser);

    // write "]" or "\n\t..\t]"
    return cra_json_write_finish(ser, ']');
}

static bool
cra_json_read_list(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    int      res;
    bool     ret;
    size_t   slot;
    char    *list;
    uint64_t count;

    assert(meta->szer_i);
    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert(meta->szer_i->add && meta->init_i->init);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // check '['
    if (!cra_json_check_ch(ser, '['))
    {
        if (!ser->error.err)
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid list value. expected a '['");
        return false;
    }

    slot = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;

    // alloc
    if (meta->is_ptr)
        list = *(char **)retval = (char *)cra_malloc(meta->size);
    else
        list = (char *)retval;

    if (meta->is_ptr || meta->init_i->uninit)
        cra_release_mgr_add(&ser->release, list, meta);

    // init
    CraInitArgs da = { .size = meta->size, .length = 8, .val1size = slot, .val2size = 0, .arg = meta->arg };
    meta->init_i->init(list, &da);

    // check empty
    if (cra_json_check_ch(ser, ']'))
    {
        CRA_SERIALIZER_NESTING_DEC(ser);
        return true;
    }

#ifdef __STDC_NO_VLA__
    char *element = (char *)cra_malloc(slot);
#else
    char element[slot];
#endif

    // read elements
    ret = false;
    for (count = 0; CRA_SERIALIZER_IS_ENOUGH(ser, sizeof("X")); ++count)
    {
        cra_json_skip_whitespaces(ser);

        // read element
        if (!(ret = cra_json_read_value(ser, element, meta->submeta)))
            break;

        // append
        if (!(ret = meta->szer_i->add(list, element, NULL)))
        {
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_ADD_FAILED, "failed to add a list element");
            break;
        }

        res = cra_json_is_finish(ser, ']', meta);
        if (res != 1)
        {
            if (res == -1)
                ret = false;
            break;
        }
    }

#ifdef __STDC_NO_VLA__
    cra_free(element);
#endif
    CRA_SERIALIZER_NESTING_DEC(ser);
    return ret;
}

static bool
cra_json_write_dict(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    char              *dict;
    uint64_t           i, count;
    char               it[64] = { 0 };
    const CraTypeMeta *keymeta, *valmeta;
    void              *key, *value;
    size_t             needed;
    char              *buf;

    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert((meta->submeta + 1)->is_not_end);
    assert(meta->szer_i && meta->szer_i->iter_init && meta->szer_i->iter_next);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // write "{"
    if (!cra_json_write_ch(ser, '{'))
        return false;

    keymeta = meta->submeta;
    valmeta = meta->submeta + 1;
    dict = meta->is_ptr ? *(char **)val : (char *)val;
    needed = ser->format ? ser->nesting + sizeof("\n") : sizeof("");

    // iter_init
    meta->szer_i->iter_init(dict, &count, it, sizeof(it));

    // empty?
    if (count == 0)
    {
        if (!cra_json_write_ch(ser, '}'))
            return false;

        CRA_SERIALIZER_NESTING_DEC(ser);
        return true;
    }

    // write k-v pairs
    for (i = 0; meta->szer_i->iter_next(it, (void **)&key, (void **)&value); ++i)
    {
        // write "," or ",\n\t..\t"
        if (!cra_json_write_comma(ser, needed, i == 0))
            return false;

        // write key
        if (!cra_json_write_key(ser, key, keymeta))
            return false;

        // write ":" or ": "
        CRA_SERIALIZER_ENSURE_(ser, buf, ser->format ? sizeof(": ") : sizeof(":"), sizeof(""));
        *buf++ = ':';
        if (ser->format)
            *buf++ = ' ';
        *buf = '\0';

        // write value
        if (!cra_json_write_value(ser, value, valmeta))
            return false;
    }

    assert(i == count);

    CRA_SERIALIZER_NESTING_DEC(ser);

    // write "}" or "\n\t..\t}"
    return cra_json_write_finish(ser, '}');
}

static bool
cra_json_read_dict(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    char               ch;
    int                res;
    bool               ret;
    char              *dict;
    size_t             key_size, val_size;
    const CraTypeMeta *keymeta, *valmeta;

    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert((meta->submeta + 1)->is_not_end);
    assert(meta->szer_i && meta->szer_i->add && meta->init_i->init);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // check '{'
    if (!(ret = cra_json_check_ch(ser, '{')))
    {
        ch = '{';
    invalid_value:
        if (!ser->error.err)
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid dict value. expected a '%c'", ch);
        return false;
    }

    keymeta = meta->submeta;
    valmeta = meta->submeta + 1;
    key_size = keymeta->is_ptr ? sizeof(void *) : keymeta->size;
    val_size = valmeta->is_ptr ? sizeof(void *) : valmeta->size;

    // alloc
    if (meta->is_ptr)
        dict = *(char **)retval = (char *)cra_malloc(meta->size);
    else
        dict = (char *)retval;

    if (meta->is_ptr || meta->init_i->uninit)
        cra_release_mgr_add(&ser->release, dict, meta);

    // init
    CraInitArgs da = {
        .size = meta->size,
        .length = 7,
        .val1size = key_size,
        .val2size = val_size,
        .arg = meta->arg,
    };
    meta->init_i->init(dict, &da);

    // check empty
    if (cra_json_check_ch(ser, '}'))
    {
        CRA_SERIALIZER_NESTING_DEC(ser);
        return true;
    }

#ifdef __STDC_NO_VLA__
    char *key = cra_malloc(key_size + val_size);
#else
    char key[key_size + val_size];
#endif
    char *value = key + key_size;

    // read elements
    ret = false;
    while (true)
    {
        // read key
        if (!(ret = cra_json_read_key(ser, key, keymeta)))
            break;

        // check ':'
        if (!(ret = cra_json_check_ch(ser, ':')))
        {
            ch = ':';
            goto invalid_value;
        }

        // skip white
        cra_json_skip_whitespaces(ser);

        // read value
        if (!(ret = cra_json_read_value(ser, value, valmeta)))
            break;

        // append
        if (!(ret = meta->szer_i->add(dict, key, value)))
        {
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_ADD_FAILED, "failed to insert a key-value pair into dict");
            break;
        }

        res = cra_json_is_finish(ser, '}', meta);
        if (res != 1)
        {
            if (res == -1)
                ret = false;
            break;
        }
    }

#ifdef __STDC_NO_VLA__
    cra_free(key);
#endif
    CRA_SERIALIZER_NESTING_DEC(ser);

    return ret;
}

CRA_API bool
cra_json_stringify_err(char *buf, size_t *len, bool format, CraSeriObject *obj, CraSerErr *err)
{
    bool          ret;
    CraSerializer ser;

    assert(buf);
    assert(len);
    assert(obj);
    assert(obj->objptr);

    cra_serializer_init(&ser, buf, *len, format);
    ret = cra_json_write_value(&ser, obj->objptr, obj->meta);
    cra_serializer_check_err(ser, err, ret);
    assert(ser.buffer[ser.index] == '\0');
    *len = ser.index + 1;
    cra_serializer_uninit(&ser, ret);
    return ret;
}

CRA_API bool
cra_json_parse_err(char *buf, size_t len, CraSeriObject *retobj, CraSerErr *err)
{
    assert(buf);
    assert(len > 0);
    assert(retobj);
    assert(retobj->objptr);

    bool          ret;
    CraSerializer ser;

    // check '\0'
    if (buf[len - 1] != '\0')
    {
        ser.index = 0;
        CRA_SERIALIZER_ERROR1(&ser, CRA_SER_ERR_INVALID_VAL, "'buf' is not null-terminated");
        return false;
    }

    cra_serializer_init(&ser, buf, len, false);
    cra_json_skip_whitespaces(&ser);
    ret = cra_json_read_value(&ser, retobj->objptr, retobj->meta);
    cra_serializer_check_err(ser, err, ret);
    cra_serializer_uninit(&ser, ret);
    return ret;
}
