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
#include "serialize/cra_json.h"
#include "cra_assert.h"
#include "cra_malloc.h"
#include "serialize/cra_ser_inner.h"
#include <float.h>
#include <math.h>

static bool
cra_json_stringify_begin(CraSerializer *ser, unsigned char *buffer, size_t buffsize, bool format)
{
    bzero(ser, sizeof(*ser));
    if (buffsize >= 2)
    {
        ser->format = format;
        ser->noalloc = !!buffer;
        ser->length = buffsize;
        ser->buffer = !!buffer ? buffer : cra_malloc(ser->length);
        return true;
    }
    else
    {
        ser->error = CRA_SER_ERROR_NOBUF;
        return false;
    }
}

static unsigned char *
cra_json_stringify_end(CraSerializer *ser, size_t *retbuffsize, CraSerError_e *reterror)
{
    assert_always(retbuffsize);
    assert(ser->index < ser->length);
    if (reterror)
        *reterror = ser->error;
    if (!ser->error)
    {
        *retbuffsize = ser->index + 1;
        return ser->buffer;
    }
    else
    {
        if (!ser->noalloc)
            cra_free(ser->buffer);
        return NULL;
    }
}

static bool
cra_json_parse_begin(CraSerializer *ser, unsigned char *buffer, size_t buffsize)
{
    assert(buffer);

    bzero(ser, sizeof(*ser));
    if (buffsize >= 2)
    {
        ser->buffer = buffer;
        ser->length = buffsize;
        cra_ser_release_init(&ser->release);
        return true;
    }
    else
    {
        ser->error = CRA_SER_ERROR_NOBUF;
        return false;
    }
}

static bool
cra_json_parse_end(CraSerializer *ser, CraSerError_e *reterror)
{
    if (reterror)
        *reterror = ser->error;
    cra_ser_release_uninit(&ser->release, !!ser->error);
    return !ser->error;
}

static bool
__cra_json_stringify_bool(CraSerializer *ser, bool val)
{
    unsigned char *buf;
    if (val)
    {
        CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, sizeof("true"), false);
        ser->index -= sizeof("");
        memcpy(buf, "true", sizeof("true"));
    }
    else
    {
        CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, sizeof("false"), false);
        ser->index -= sizeof("");
        memcpy(buf, "false", sizeof("false"));
    }
    return true;
}

static bool
__cra_json_parse_bool(CraSerializer *ser, bool *retval)
{
    unsigned char *buf = CRA_SERIALIZER_GET_BUF(ser);
    if (CRA_SERIALIZER_ENOUGH(ser, sizeof("true")) && strncmp((char *)buf, "true", sizeof("true") - 1) == 0)
    {
        ser->index += sizeof("true") - 1;
        *retval = true;
        return true;
    }
    else if (CRA_SERIALIZER_ENOUGH(ser, sizeof("false")) && strncmp((char *)buf, "false", sizeof("false") - 1) == 0)
    {
        ser->index += sizeof("false") - 1;
        *retval = false;
        return true;
    }
    else
    {
        ser->error = CRA_SER_ERROR_TYPE_MISMATCH;
        return false;
    }
}

static bool
__cra_json_stringify_int(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    int            len;
    int64_t        i64;
    unsigned char *buf;
    char           intstr[32] = { 0 };

    i64 = __cra_ptr_to_i64(val, meta->size);

#ifdef CRA_COMPILER_MSVC
    len = sprintf_s(intstr, sizeof(intstr), "%zd", i64);
#else
    len = sprintf(intstr, "%zd", i64);
#endif

    if (len < 0 || len > (int)(sizeof(intstr) - 1))
    {
        ser->error = CRA_SER_ERROR_INVALID_VALUE;
        return false;
    }

    // write val
    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, len + sizeof(""), false);
    ser->index -= sizeof("");
    memcpy(buf, intstr, len);
    buf[len] = '\0';
    return true;
}

static bool
__cra_json_parse_int(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    int64_t        i64;
    unsigned char *end = NULL;
    unsigned char *buf = CRA_SERIALIZER_GET_BUF(ser);
    if (buf[0] == '-' || (buf[0] >= '0' && buf[0] <= '9'))
    {
        i64 = strtoll((char *)buf, (char **)&end, 10);
        if (buf == end)
            goto fail;
        // 是浮点数时使用strtod保证取走完整的val
        if (*end == '.' || *end == 'e' || *end == 'E')
        {
            end = NULL;
            double dbl = strtod((char *)buf, (char **)&end);
            if (buf == end || (isnan(dbl) || isinf(dbl)))
                goto fail;
            i64 = (int64_t)CRA_CLAMP(dbl, (double)CRA_MAX_SAFE_INT, (double)CRA_MIN_SAFE_INT);
        }
        __cra_i64_to_ptr(i64, retval, meta->size);
    }
    ser->index += (end - buf);
    if (ser->index < ser->length)
        return true;

fail:
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    return false;
}

static bool
__cra_json_stringify_uint(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    int            len;
    uint64_t       u64;
    unsigned char *buf;
    char           intstr[32] = { 0 };

    u64 = __cra_ptr_to_u64(val, meta->size);

#ifdef CRA_COMPILER_MSVC
    len = sprintf_s(intstr, sizeof(intstr), "%zu", u64);
#else
    len = sprintf(intstr, "%zu", u64);
#endif

    if (len < 0 || len > (int)(sizeof(intstr) - 1))
    {
        ser->error = CRA_SER_ERROR_INVALID_VALUE;
        return false;
    }

    // write val
    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, len + sizeof(""), false);
    ser->index -= sizeof("");
    memcpy(buf, intstr, len);
    buf[len] = '\0';
    return true;
}

static bool
__cra_json_parse_uint(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    uint64_t       u64;
    unsigned char *end = NULL;
    unsigned char *buf = CRA_SERIALIZER_GET_BUF(ser);
    if (buf[0] == '-' || (buf[0] >= '0' && buf[0] <= '9'))
    {
        u64 = strtoull((char *)buf, (char **)&end, 10);
        if (buf == end)
            goto fail;
        // 是浮点数时使用strtod保证取走完整的val
        if (*end == '.' || *end == 'e' || *end == 'E')
        {
            end = NULL;
            double dbl = strtod((char *)buf, (char **)&end);
            if (buf == end || (isnan(dbl) || isinf(dbl)))
                goto fail;
            u64 = (uint64_t)CRA_CLAMP(dbl, (double)CRA_MAX_SAFE_INT, (double)0);
        }
        __cra_u64_to_ptr(u64, retval, meta->size);
    }
    ser->index += (end - buf);
    if (ser->index < ser->length)
        return true;

fail:
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    return false;
}

static bool
__cra_json_stringify_float(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    int            len;
    double         dbl;
    unsigned char *buf;
    char           dblstr[32] = { 0 };

    switch (meta->size)
    {
        case sizeof(float):
            dbl = *(float *)val;
            break;
        case sizeof(double):
            dbl = *(double *)val;
            break;
        default:
            assert_always(false && "error float size.");
            break;
    }

    if (isnan(dbl) || isinf(dbl))
    {
        // FIXME: double of c cannot be null
        ser->error = CRA_SER_ERROR_FLOAT_NAN_OR_INF;
        return false;
    }

    // -0.0
    if (dbl == 0.0 && signbit(dbl))
        dbl = 0.0;

#ifdef CRA_COMPILER_MSVC
    len = sprintf_s(dblstr, sizeof(dblstr), "%.17g", dbl);
#else
    len = sprintf(dblstr, "%.17g", dbl);
#endif

    if (len < 0 || len > (int)(sizeof(dblstr) - 1))
    {
        ser->error = CRA_SER_ERROR_FLOAT_NAN_OR_INF;
        return false;
    }

    // write val
    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, len + sizeof(""), false);
    ser->index -= sizeof("");
    memcpy(buf, dblstr, len);
    buf[len] = '\0';
    return true;
}

static bool
__cra_json_parse_float(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    double         dbl;
    unsigned char *end = NULL;
    unsigned char *buf = CRA_SERIALIZER_GET_BUF(ser);
    if (buf[0] == '-' || (buf[0] >= '0' && buf[0] <= '9'))
    {
        dbl = strtod((char *)buf, (char **)&end);
        if (buf != end && !(isnan(dbl) || isinf(dbl)))
        {
            ser->index += (end - buf);
            if (ser->index < ser->length)
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
                        assert_always(false && "error float size.");
                        break;
                }
                return true;
            }
        }
    }
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    return false;
}

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
__cra_parse_unicodee_escape(const unsigned char *buf)
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
__cra_code_point2utf8(CraSerializer *ser, uint32_t code_point, unsigned char *str, size_t remain)
{
#define CHECK(_needed)                            \
    if (remain < _needed)                         \
    {                                             \
        ser->error = CRA_SER_ERROR_STRING_LENGTH; \
        return -1;                                \
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
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    return -1; // 无效代码点

#undef CHECK
}

static bool
__cra_json_stringify_string(CraSerializer *ser, const char *val)
{
    unsigned char *buf;
    unsigned char *str;
    unsigned char *strbuf;

    // 空串
    if (val[0] == '\0')
    {
        CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, sizeof("\"\""), false);
        ser->index -= sizeof("");
        memcpy(buf, "\"\"", sizeof("\"\""));
        return true;
    }

    // 计算字符串长度
    size_t needed = 0;
    size_t strlength = 0;
    for (str = (unsigned char *)val; *str; ++str)
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

    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, needed + sizeof("\"\""), false);
    ser->index -= sizeof("");

    // write string

    *buf = '\"';

    if (needed == strlength)
    {
        // no chars have to be escaped
        memcpy(buf + 1, val, needed);
    }
    else
    {
        for (str = (unsigned char *)val, strbuf = buf + 1; *str; (void)++str, ++strbuf)
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
#ifdef CRA_COMPILER_MSVC
                        sprintf_s((char *)strbuf, 10, "\\u%04x", *str);
#else
                        sprintf((char *)strbuf, "\\u%04x", *str);
#endif
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
__cra_json_parse_string(CraSerializer *ser, char *retval, const CraTypeMeta *meta)
{
    unsigned char *buf, *str;
    size_t         len, length, max_length;

    CRA_SERIALIZER_ENOUGH_AND_RETURN(ser, buf, sizeof("\"\""), false);
    ser->index -= sizeof("\"\"");
    if (*buf != '\"')
    {
        // 不是字符串
        ser->error = CRA_SER_ERROR_INVALID_VALUE;
        return false;
    }
    ++buf; // 越过'\"'

    // 检查串中是否有特殊字符
    for (length = 0, str = buf; (size_t)(str - ser->buffer) < ser->length && *str != '\"'; ++str)
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
    len = (size_t)(str - buf) - length;
    if (meta->is_ptr)
    {
        max_length = len + sizeof("");
        str = *(unsigned char **)retval = cra_malloc(max_length);
        cra_ser_release_add(&ser->release, str, NULL, cra_free);
    }
    else
    {
        max_length = meta->size;
        str = (unsigned char *)retval;
    }

    // 没有特殊字符时copy string
    if (length == 0)
    {
        // 测试字符数组大小是否能容纳该字符串
        if (len >= max_length)
        {
            ser->error = CRA_SER_ERROR_CHARARR_TOO_SMALL;
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
        {
            ser->error = CRA_SER_ERROR_CHARARR_TOO_SMALL;
            return false;
        }

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
                    uint32_t code_pint = __cra_parse_unicodee_escape(buf + length);
                    if (code_pint == UINT32_MAX)
                        goto fail;
                    length += sizeof("XXXX") - 1;

                    // 处理代理对
                    if (!__cra_is_high_surrogate(code_pint))
                    {
                        if (__cra_is_low_surrogate(code_pint))
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
                    code_pint = __cra_combine_surrogates(code_pint, low_surrogate);

                to_utf8:
                    // to utf-8
                    {
                        int retlen = __cra_code_point2utf8(ser, code_pint, str, remaining);
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
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    return false;
}

static bool
__cra_json_stringify_struct(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
__cra_json_parse_struct(CraSerializer *ser, void *retval, const CraTypeMeta *meta);

static bool
__cra_json_stringify_list(CraSerializer *ser, void *val, void *origin, uint64_t incnt, const CraTypeMeta *meta);
static bool
__cra_json_parse_list(CraSerializer *ser, void *retval, uint64_t *outcount, const CraTypeMeta *meta);
static bool
__cra_json_stringify_dict(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
__cra_json_parse_dict(CraSerializer *ser, void *retval, const CraTypeMeta *meta);

static bool
__cra_json_stringify_value(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    assert(val);
    assert(meta);

    void *origin_val = val;

    if (meta->is_ptr)
    {
        val = *(void **)val;
        // check null
        if (!val)
        {
            unsigned char *buf;
            CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, sizeof("null"), false);
            ser->index -= sizeof("");
            memcpy(buf, "null", sizeof("null"));
            return true;
        }
    }

    switch (meta->type)
    {
        case CRA_TYPE_BOOL:
            return __cra_json_stringify_bool(ser, *(bool *)val);
        case CRA_TYPE_INT:
        case CRA_TYPE_VARINT:
            return __cra_json_stringify_int(ser, val, meta);
        case CRA_TYPE_UINT:
        case CRA_TYPE_VARUINT:
            return __cra_json_stringify_uint(ser, val, meta);
        case CRA_TYPE_FLOAT:
            return __cra_json_stringify_float(ser, val, meta);
        case CRA_TYPE_STRING:
            assert(meta->nsize == 0);
            return __cra_json_stringify_string(ser, val);
        case CRA_TYPE_STRUCT:
            return __cra_json_stringify_struct(ser, val, meta);
        case CRA_TYPE_LIST:
            return __cra_json_stringify_list(ser, val, origin_val, 0, meta);
        case CRA_TYPE_DICT:
            return __cra_json_stringify_dict(ser, val, meta);

        default:
            ser->error = CRA_SER_ERROR_INVALID_TYPE;
            return false;
    }
}

static bool
__cra_json_parse_value(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    assert(retval);
    assert(meta);

    unsigned char *buf = CRA_SERIALIZER_GET_BUF(ser);
    // check & read null
    if (CRA_SERIALIZER_ENOUGH(ser, sizeof("null")) && strncmp((char *)buf, "null", sizeof("null") - 1) == 0)
    {
        if (!meta->is_ptr)
        {
            ser->error = CRA_SER_ERROR_CANNOT_BE_NULL;
            return false;
        }
        ser->index += sizeof("null") - 1;
        *(void **)retval = NULL;
        return true;
    }

    switch (meta->type)
    {
        case CRA_TYPE_BOOL:
            return __cra_json_parse_bool(ser, (bool *)retval);
        case CRA_TYPE_INT:
        case CRA_TYPE_VARINT:
            return __cra_json_parse_int(ser, retval, meta);
        case CRA_TYPE_UINT:
        case CRA_TYPE_VARUINT:
            return __cra_json_parse_uint(ser, retval, meta);
        case CRA_TYPE_FLOAT:
            return __cra_json_parse_float(ser, retval, meta);
        case CRA_TYPE_STRING:
            assert(meta->nsize == 0);
            return __cra_json_parse_string(ser, retval, meta);
        case CRA_TYPE_STRUCT:
            return __cra_json_parse_struct(ser, retval, meta);
        case CRA_TYPE_LIST:
            return __cra_json_parse_list(ser, retval, NULL, meta);
        case CRA_TYPE_DICT:
            return __cra_json_parse_dict(ser, retval, meta);

        default:
            ser->error = CRA_SER_ERROR_INVALID_TYPE;
            return false;
    }
}

// write whitespaces
static inline bool
__cra_json_write_whitespaces(CraSerializer *ser)
{
    unsigned char *buf;
    size_t         len;

    if (ser->format)
    {
        len = ser->nesting + sizeof("\n");
        CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, len, false);
        ser->index -= sizeof("");
        *buf++ = '\n';
        for (uint32_t i = 0; i < ser->nesting; ++i)
            *buf++ = '\t';
    }
    return true;
}

// skip whitespaces
static inline void
__cra_json_skip_whitespaces(CraSerializer *ser)
{
    while (ser->index + 1 < ser->length && ser->buffer[ser->index] <= 0x20 && ser->buffer[ser->index] != '\0')
        ++ser->index;
}

// write char
static inline bool
__cra_json_write_ch(CraSerializer *ser, unsigned char ch, unsigned char after_ch)
{
    size_t         len;
    unsigned char *buf;
    len = (ser->format && after_ch != '\0') ? sizeof("Xx") : sizeof("X");
    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, len, false);
    ser->index -= sizeof("");
    *buf++ = ch;
    if (ser->format && after_ch != '\0')
        *buf++ = after_ch;
    *buf = '\0';
    return true;
}

// check char
static inline bool
__cra_json_check_ch(CraSerializer *ser, unsigned char ch)
{
    __cra_json_skip_whitespaces(ser);
    if (CRA_SERIALIZER_ENOUGH(ser, sizeof("X")) && ser->buffer[ser->index] == ch)
    {
        ++ser->index;
        return true;
    }
    return false;
}

static inline void
__cra_json_skip_value(CraSerializer *ser)
{
    size_t leftcount = 0;
    while (CRA_SERIALIZER_ENOUGH(ser, sizeof("X")) && (ser->buffer[ser->index] != ',' || leftcount > 0))
    {
        if (ser->buffer[ser->index] == '{' || ser->buffer[ser->index] == '[')
        {
            ++leftcount;
        }
        else if (ser->buffer[ser->index] == '}' || ser->buffer[ser->index] == ']')
        {
            // leftcount == 0 表示要跳过的value是对象中的最后一个，可以跳出循环
            if (leftcount == 0)
                break;
            --leftcount;
        }
        ++ser->index;
    }
}

static inline bool
__cra_compare_key(CraSerializer *ser, const char *key_from_meta)
{
    unsigned char *key = (unsigned char *)key_from_meta;
    __cra_json_skip_whitespaces(ser);
    if (ser->buffer[ser->index++] == '\"')
    {
        while (CRA_SERIALIZER_ENOUGH(ser, sizeof("X")) && ser->buffer[ser->index] == *key)
        {
            ++key;
            ++ser->index;
        }
        if (ser->buffer[ser->index] == '\"' && *key == '\0')
        {
            // match
            ++ser->index; // skip '\"'
            return true;
        }
    }
    // not match
    return false;
}

static bool
__cra_json_stringify_struct(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    assert(meta->submeta);

    CRA_SER_NESTING_INC_AND_CHECK(ser, false);

    // write '{'
    if (!__cra_json_write_ch(ser, '{', '\0'))
        return false;

    for (CraTypeMeta *m = (CraTypeMeta *)meta->submeta; true;)
    {
        // write spaces
        if (!__cra_json_write_whitespaces(ser))
            return false;
        // write key
        assert(m->member);
        if (!__cra_json_stringify_string(ser, (char *)m->member))
        {
            ser->error = CRA_SER_ERROR_WRITE_KEY_FAILED;
            return false;
        }
        // write ':'
        if (!__cra_json_write_ch(ser, ':', ' '))
            return false;
        // write val
        if (!__cra_json_stringify_value(ser, (char *)val + m->offset, m))
            return false;

        ++m;
        if (!m->type)
            break;
        // write ','
        if (!__cra_json_write_ch(ser, ',', '\0'))
            return false;
    }

    CRA_SER_NESTING_DEC(ser);

    // write whitespaces
    if (!__cra_json_write_whitespaces(ser))
        return false;
    // write '}'
    if (!__cra_json_write_ch(ser, '}', '\0'))
        return false;

    return true;
}

static bool
__cra_json_parse_struct(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    assert(meta->submeta);

    CRA_SER_NESTING_INC_AND_CHECK(ser, false);

    // check '{'
    if (!__cra_json_check_ch(ser, '{'))
    {
        ser->error = CRA_SER_ERROR_INVALID_VALUE;
        return false;
    }

    // alloc
    if (meta->is_ptr)
        retval = *(void **)retval = cra_malloc(meta->size);
    // init
    if (meta->dzer_i)
        meta->dzer_i->init1(retval, 0, meta->size, meta->arg4dzer);
    else
        bzero(retval, meta->size);
    // record
    if (meta->is_ptr || (meta->dzer_i && meta->dzer_i->uninit))
    {
        cra_ser_release_add(
          &ser->release, retval, meta->dzer_i ? meta->dzer_i->uninit : NULL, meta->is_ptr ? cra_free : NULL);
    }

    // check empty
    if (__cra_json_check_ch(ser, '}'))
        return true;

    // read items
    for (CraTypeMeta *m = (CraTypeMeta *)meta->submeta, *last = m; true;)
    {
        // match key
        while (true)
        {
            // compare
            assert(m->member);
            if (__cra_compare_key(ser, m->member))
            {
                // check ':'
                if (!__cra_json_check_ch(ser, ':'))
                    goto fail;
                __cra_json_skip_whitespaces(ser);
                // read val
                if (!__cra_json_parse_value(ser, (char *)retval + m->offset, m))
                    return false;
                last = m; // update last
                if (!(++m)->type)
                    m = (CraTypeMeta *)meta->submeta;
                break; // quit match loop
            }
            if (!(++m)->type)
                m = (CraTypeMeta *)meta->submeta;

            // no member match
            if (m == last)
            {
                // skip this item and start to match next item
                __cra_json_skip_value(ser);
                break; // quit match loop
            }
        }

        __cra_json_skip_whitespaces(ser);
        if (CRA_SERIALIZER_ENOUGH(ser, sizeof("X")))
        {
            // has next?
            if (ser->buffer[ser->index] == ',')
            {
                ++ser->index; // not finish
            }
            // ending?
            else if (ser->buffer[ser->index] == '}')
            {
                CRA_SER_NESTING_DEC(ser);
                ++ser->index;
                return true; // finish
            }
        }
        else
        {
            break;
        }
    }

fail:
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    return false;
}

static bool
__cra_json_stringify_list(CraSerializer *ser, void *val, void *origin, uint64_t incount, const CraTypeMeta *meta)
{
    uint64_t count;
    bool     write_comma;

    assert(val);
    assert(meta->submeta);
    assert(meta->submeta[0].type != 0 && meta->submeta[1].type == 0); // element's meta必须只有一项

    CRA_SER_NESTING_INC_AND_CHECK(ser, false);

    // write '['
    if (!__cra_json_write_ch(ser, '[', '\0'))
        return false;

    // get count
    count = __cra_get_n(origin, val, meta, incount);
    write_comma = false;
    // write elements
    if (meta->szer_i)
    {
        // list

        assert(meta->szer_i->iter_init && meta->szer_i->iter_next1);

        char  itbuf[64];
        void *elementptr;
        for (meta->szer_i->iter_init(val, itbuf, sizeof(itbuf)); meta->szer_i->iter_next1(itbuf, &elementptr);)
        {
            if (write_comma)
            {
                if (!__cra_json_write_ch(ser, ',', ' '))
                    return false;
            }
            else
            {
                write_comma = true;
            }
            if (!__cra_json_stringify_value(ser, elementptr, meta->submeta))
                return false;
            --count;
        }
        assert(count == 0);
    }
    else
    {
        // array

        assert(meta->nsize > 0);

        size_t slot_size = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;
        for (uint64_t i = 0; i < count; ++i)
        {
            if (write_comma)
            {
                if (!__cra_json_write_ch(ser, ',', ' '))
                    return false;
            }
            else
            {
                write_comma = true;
            }
            if (!__cra_json_stringify_value(ser, (char *)val + slot_size * i, meta->submeta))
                return false;
        }
    }

    CRA_SER_NESTING_DEC(ser);

    // write ']'
    if (!__cra_json_write_ch(ser, ']', '\0'))
        return false;

    return true;
}

static bool
__cra_json_parse_list(CraSerializer *ser, void *retval, uint64_t *outcount, const CraTypeMeta *meta)
{
    void    *list;
    size_t   obj_size;
    size_t   slot_size;
    uint64_t maxcount;
    size_t   i = 0;

    assert(retval);
    assert(meta->submeta);
    assert(meta->size > 0);
    assert(meta->submeta[0].type != 0 && meta->submeta[1].type == 0); // element's meta必须只有一项
    assert((meta->nsize > 0 && !meta->dzer_i) ||
           (meta->dzer_i && meta->dzer_i->init1 && meta->dzer_i->uninit && meta->dzer_i->append1));

    CRA_SER_NESTING_INC_AND_CHECK(ser, false);

    // check '['
    if (!__cra_json_check_ch(ser, '['))
    {
        ser->error = CRA_SER_ERROR_INVALID_VALUE;
        return false;
    }

    maxcount = 8;
    slot_size = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;
    if (meta->is_ptr)
    {
        obj_size = meta->dzer_i ? meta->size : slot_size * maxcount;
        list = cra_malloc(obj_size);
    }
    else
    {
        list = retval;
        obj_size = meta->size;
        maxcount = obj_size / slot_size;
    }
    // init
    if (meta->dzer_i)
        meta->dzer_i->init1(list, maxcount, slot_size, meta->arg4dzer);
    else
        bzero(list, obj_size);

    // check empty
    if (__cra_json_check_ch(ser, ']'))
        goto ok;

    // read elements
    char *element = cra_malloc(slot_size);
    for (; CRA_SERIALIZER_ENOUGH(ser, sizeof("X")); ++i)
    {
        __cra_json_skip_whitespaces(ser);
        // read element
        if (!__cra_json_parse_value(ser, element, meta->submeta))
            goto fail;
        // list
        if (meta->dzer_i)
        {
            if (!meta->dzer_i->append1(list, element))
            {
                ser->error = CRA_SER_ERROR_APPEND_FAILED;
                goto fail;
            }
        }
        // array
        else
        {
            if (i >= maxcount)
            {
                if (!meta->is_ptr)
                {
                    ser->error = CRA_SER_ERROR_ARRAY_TOO_SMALL;
                    goto fail;
                }
                maxcount *= 2;
                list = cra_realloc(list, maxcount);
            }
            memcpy((char *)list + slot_size * i, element, slot_size);
        }

        __cra_json_skip_whitespaces(ser);
        if (CRA_SERIALIZER_ENOUGH(ser, sizeof("X")))
        {
            // has next?
            if (ser->buffer[ser->index] == ',')
            {
                ++ser->index; // not finish
            }
            // ending?
            else if (ser->buffer[ser->index] == ']')
            {
                CRA_SER_NESTING_DEC(ser);
                cra_free(element);
                ++ser->index;
                ++i;

            ok:
                if (meta->nsize > 0)
                    __cra_set_n(retval, meta, i, outcount);

                if (meta->is_ptr)
                    *(void **)retval = list;
                if (meta->is_ptr || meta->dzer_i)
                {
                    cra_ser_release_add(
                      &ser->release, list, meta->dzer_i ? meta->dzer_i->uninit : NULL, meta->is_ptr ? cra_free : NULL);
                }
                return true; // finish
            }
        }
        else
        {
            break;
        }
    }

    ser->error = CRA_SER_ERROR_INVALID_VALUE;
fail:
    if (meta->dzer_i)
        meta->dzer_i->uninit(list);
    if (meta->is_ptr)
        cra_free(list);
    cra_free(element);
    return false;
}

static inline bool
__cra_json_stringify_dict_key(CraSerializer *ser, void *key, const CraTypeMeta *meta)
{
    unsigned char *buf;
    if (meta->type != CRA_TYPE_STRING)
    {
        CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, sizeof("\"\""), false);
        ser->index -= sizeof("\"");
        *buf = '\"';
    }
    if (!__cra_json_stringify_value(ser, key, meta))
        return false;
    if (meta->type != CRA_TYPE_STRING)
    {
        CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, sizeof("\""), false);
        ser->index -= sizeof("");
        *buf = '\"';
    }
    return true;
}

static inline bool
__cra_json_parse_dict_key(CraSerializer *ser, void *retkey, const CraTypeMeta *meta)
{
    unsigned char *buf;
    CRA_UNUSED_VALUE(buf);
    // skip '\"'
    if (meta->type != CRA_TYPE_STRING)
    {
        CRA_SERIALIZER_ENOUGH_AND_RETURN(ser, buf, sizeof("\"\""), false);
        ser->index -= sizeof("\"");
    }
    if (!__cra_json_parse_value(ser, retkey, meta))
        return false;
    // skip '\"'
    if (meta->type != CRA_TYPE_STRING)
    {
        CRA_SERIALIZER_ENOUGH_AND_RETURN(ser, buf, sizeof("\""), false);
        ser->index -= sizeof("");
    }
    return true;
}

static bool
__cra_json_stringify_dict(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    CraTypeMeta *key_meta;
    CraTypeMeta *val_meta;

    assert(val);
    assert(meta->submeta);
    // element's meta必须只有两项(key_meta, val_meta)
    assert(meta->submeta[0].type != 0 && meta->submeta[1].type != 0 && meta->submeta[2].type == 0);
    assert(meta->szer_i && meta->szer_i->get_count && meta->szer_i->iter_init && meta->szer_i->iter_next2);

    CRA_SER_NESTING_INC_AND_CHECK(ser, false);

    key_meta = (CraTypeMeta *)meta->submeta;
    val_meta = (CraTypeMeta *)meta->submeta + 1;
    // check key type
    if (key_meta->type == CRA_TYPE_NULL || (key_meta->type >= CRA_TYPE_STRUCT && key_meta->type <= CRA_TYPE_DICT))
    {
        ser->error = CRA_SER_ERROR_CANNOT_BE_KEY;
        return false;
    }

    // write '{'
    if (!__cra_json_write_ch(ser, '{', '\0'))
        return false;

    // write k-v pair
    char  itbuf[64];
    void *keyptr, *valptr;
    for (meta->szer_i->iter_init(val, itbuf, sizeof(itbuf)); meta->szer_i->iter_next2(itbuf, &keyptr, &valptr);)
    {
        // write spaces
        if (!__cra_json_write_whitespaces(ser))
            return false;
        // write key
        if (!__cra_json_stringify_dict_key(ser, keyptr, key_meta))
            return false;
        // write ':'
        if (!__cra_json_write_ch(ser, ':', ' '))
            return false;
        // write val
        if (!__cra_json_stringify_value(ser, valptr, val_meta))
            return false;

        // write ','
        if (!__cra_json_write_ch(ser, ',', '\0'))
            return false;
    }

    CRA_SER_NESTING_DEC(ser);

    // not empty, delete last ','
    if (ser->buffer[ser->index - 1] == ',')
    {
        --ser->index;
        // write whitespaces
        if (!__cra_json_write_whitespaces(ser))
            return false;
    }
    // else: empty dict, no need whitespaces

    // write '}'
    if (!__cra_json_write_ch(ser, '}', '\0'))
        return false;

    return true;
}

static bool
__cra_json_parse_dict(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    size_t       key_size;
    size_t       val_size;
    void        *key, *val;
    CraTypeMeta *key_meta;
    CraTypeMeta *val_meta;

    assert(retval);
    assert(meta->submeta);
    assert(meta->size > 0);
    // element's meta必须只有两项(key_meta, val_meta)
    assert(meta->submeta[0].type != 0 && meta->submeta[1].type != 0 && meta->submeta[2].type == 0);
    assert(meta->dzer_i && meta->dzer_i->init2 && meta->dzer_i->uninit && meta->dzer_i->append2);

    CRA_SER_NESTING_INC_AND_CHECK(ser, false);

    key_meta = (CraTypeMeta *)meta->submeta;
    val_meta = (CraTypeMeta *)meta->submeta + 1;

    // check '{'
    if (!__cra_json_check_ch(ser, '{'))
    {
        ser->error = CRA_SER_ERROR_INVALID_VALUE;
        return false;
    }

    if (meta->is_ptr)
        retval = *(void **)retval = cra_malloc(meta->size);
    if (meta->is_ptr || meta->dzer_i)
    {
        cra_ser_release_add(
          &ser->release, retval, meta->dzer_i ? meta->dzer_i->uninit : NULL, meta->is_ptr ? cra_free : NULL);
    }
    key_size = key_meta->is_ptr ? sizeof(void *) : key_meta->size;
    val_size = val_meta->is_ptr ? sizeof(void *) : val_meta->size;
    // init
    meta->dzer_i->init2(retval, 8, key_size, val_size, meta->arg4dzer);

    // check empty
    if (__cra_json_check_ch(ser, '}'))
        return true;

    // read k-v pair
    key = cra_malloc(key_size + val_size);
    val = (char *)key + val_size;
    while (CRA_SERIALIZER_ENOUGH(ser, sizeof("X")))
    {
        __cra_json_skip_whitespaces(ser);
        // read key
        if (!__cra_json_parse_dict_key(ser, key, key_meta))
            goto fail;
        // check ':'
        if (!__cra_json_check_ch(ser, ':'))
            goto fail;
        __cra_json_skip_whitespaces(ser);
        // read val
        if (!__cra_json_parse_value(ser, val, val_meta))
            goto fail;
        // add to dict
        if (!meta->dzer_i->append2(retval, key, val))
        {
            ser->error = CRA_SER_ERROR_APPEND_FAILED;
            goto fail;
        }

        __cra_json_skip_whitespaces(ser);
        if (CRA_SERIALIZER_ENOUGH(ser, sizeof("X")))
        {
            // has next?
            if (ser->buffer[ser->index] == ',')
            {
                ++ser->index; // not finish
            }
            // ending?
            else if (ser->buffer[ser->index] == '}')
            {
                CRA_SER_NESTING_DEC(ser);
                cra_free(key);
                ++ser->index;
                return true; // finish
            }
        }
        else
        {
            break;
        }
    }

fail:
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    cra_free(key);
    return false;
}

unsigned char *
cra_json_stringify_struct(unsigned char    *buf,
                          size_t           *bufsize,
                          void             *val,
                          const CraTypeMeta members_meta[],
                          CraSerError_e    *reterror,
                          bool              format)
{
    assert(bufsize);
    CraSerializer ser;
    if (cra_json_stringify_begin(&ser, buf, *bufsize, format))
    {
        const CraTypeMeta meta = { false, CRA_TYPE_STRUCT, 0, 0, 0, 0, 0, members_meta, 0, 0, 0 };
        bool              b = __cra_json_stringify_struct(&ser, val, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_json_stringify_end(&ser, bufsize, reterror);
}

bool
cra_json_parse_struct_with_dzer_i(unsigned char    *buf,
                                  size_t            bufsize,
                                  void             *retval,
                                  size_t            valsize,
                                  bool              is_ptr,
                                  const CraTypeMeta members_meta[],
                                  const CraDzer_i  *dzer_i,
                                  const void       *arg4dzer,
                                  CraSerError_e    *reterror)
{
    CraSerializer ser;
    if (cra_json_parse_begin(&ser, buf, bufsize))
    {
        const CraTypeMeta meta = { is_ptr, CRA_TYPE_STRUCT, 0, valsize, 0, 0, 0, members_meta, 0, dzer_i, arg4dzer };
        bool              b = __cra_json_parse_struct(&ser, retval, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_json_parse_end(&ser, reterror);
}

static inline unsigned char *
__cra_json_stringify_list_array(unsigned char    *buf,
                                size_t           *bufsize,
                                void             *val,
                                size_t            arraycount,
                                const CraTypeMeta element_meta[],
                                const CraSzer_i  *szer_i,
                                CraSerError_e    *reterror,
                                bool              format)
{
    assert(bufsize);
    CraSerializer ser;
    if (cra_json_stringify_begin(&ser, buf, *bufsize, format))
    {
        const CraTypeMeta meta = { false, CRA_TYPE_LIST, 0, 0, 0, 0, 0, element_meta, szer_i, 0, 0 };
        bool              b = __cra_json_stringify_list(&ser, val, val, arraycount, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_json_stringify_end(&ser, bufsize, reterror);
}

static inline bool
__cra_json_parse_list_array(unsigned char    *buf,
                            size_t            bufsize,
                            void             *retval,
                            size_t            valsize,
                            bool              is_ptr,
                            size_t           *retarraycount,
                            const CraTypeMeta element_meta[],
                            const CraDzer_i  *dzer_i,
                            const void       *arg4dzer,
                            CraSerError_e    *reterror)
{
    CraSerializer ser;
    if (cra_json_parse_begin(&ser, buf, bufsize))
    {
        const CraTypeMeta meta = { is_ptr, CRA_TYPE_LIST, 0, valsize, 0, 0, 0, element_meta, 0, dzer_i, arg4dzer };
        bool              b = __cra_json_parse_list(&ser, retval, retarraycount, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_json_parse_end(&ser, reterror);
}

unsigned char *
cra_json_stringify_array(unsigned char    *buf,
                         size_t           *bufsize,
                         void             *val,
                         size_t            arraycount,
                         const CraTypeMeta element_meta[],
                         CraSerError_e    *reterror,
                         bool              format)
{
    return __cra_json_stringify_list_array(buf, bufsize, val, arraycount, element_meta, NULL, reterror, format);
}

bool
cra_json_parse_array(unsigned char    *buf,
                     size_t            bufsize,
                     void             *retval,
                     size_t            valsize,
                     bool              is_ptr,
                     size_t           *retarraycount,
                     const CraTypeMeta element_meta[],
                     CraSerError_e    *reterror)
{
    return __cra_json_parse_list_array(
      buf, bufsize, retval, valsize, is_ptr, retarraycount, element_meta, NULL, NULL, reterror);
}

unsigned char *
cra_json_stringify_list(unsigned char    *buf,
                        size_t           *bufsize,
                        void             *val,
                        const CraTypeMeta element_meta[],
                        const CraSzer_i  *szer_i,
                        CraSerError_e    *reterror,
                        bool              format)
{
    return __cra_json_stringify_list_array(buf, bufsize, val, 0, element_meta, szer_i, reterror, format);
}

bool
cra_json_parse_list(unsigned char    *buf,
                    size_t            bufsize,
                    void             *retval,
                    size_t            valsize,
                    bool              is_ptr,
                    const CraTypeMeta element_meta[],
                    const CraDzer_i  *dzer_i,
                    const void       *arg4dzer,
                    CraSerError_e    *reterror)
{
    return __cra_json_parse_list_array(
      buf, bufsize, retval, valsize, is_ptr, NULL, element_meta, dzer_i, arg4dzer, reterror);
}

unsigned char *
cra_json_stringify_dict(unsigned char    *buf,
                        size_t           *bufsize,
                        void             *val,
                        const CraTypeMeta kv_meta[],
                        const CraSzer_i  *szer_i,
                        CraSerError_e    *reterror,
                        bool              format)
{
    assert(bufsize);
    CraSerializer ser;
    if (cra_json_stringify_begin(&ser, buf, *bufsize, format))
    {
        const CraTypeMeta meta = { false, CRA_TYPE_LIST, 0, 0, 0, 0, 0, kv_meta, szer_i, 0, 0 };
        bool              b = __cra_json_stringify_dict(&ser, val, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_json_stringify_end(&ser, bufsize, reterror);
}

bool
cra_json_parse_dict(unsigned char    *buf,
                    size_t            bufsize,
                    void             *retval,
                    size_t            valsize,
                    bool              is_ptr,
                    const CraTypeMeta kv_meta[],
                    const CraDzer_i  *dzer_i,
                    const void       *arg4dzer,
                    CraSerError_e    *reterror)
{
    CraSerializer ser;
    if (cra_json_parse_begin(&ser, buf, bufsize))
    {
        const CraTypeMeta meta = { is_ptr, CRA_TYPE_LIST, 0, valsize, 0, 0, 0, kv_meta, 0, dzer_i, arg4dzer };
        bool              b = __cra_json_parse_dict(&ser, retval, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_json_parse_end(&ser, reterror);
}
