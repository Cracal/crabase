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
#include <math.h>
#include <float.h>
#include "cra_malloc.h"
#include "serialize/cra_json.h"
#include "serialize/cra_ser_inner.h"

#define CRA_JSON_READ_GET_BUF(_ser) ((_ser)->buffer + (_ser)->index)
#define CRA_JSON_READ_ENSURE(_ser, _needed) ((_ser)->index + (_needed) < (_ser)->length)
#define CRA_JSON_READ_BUF(_ser, _buf, _needed) \
    if (!CRA_JSON_READ_ENSURE(_ser, _needed))  \
    {                                          \
        (_ser)->error = CRA_SER_ERROR_NOBUF;   \
        return false;                          \
    }                                          \
    _buf = CRA_JSON_READ_GET_BUF(_ser)

#if 1 // serialize

static inline void cra_json_write_whitespaces(unsigned char *buf, uint16_t nested)
{
    for (uint16_t i = 0; i < nested; ++i)
        *buf++ = '\t';
}

static inline bool cra_json_write_comma(CraSerializer *ser)
{
    size_t len;
    unsigned char *buf;
    len = ser->format ? (3) : 2;
    CRA_SERIALIZER_BUF(ser, buf, len);
    *buf++ = ',';
    if (ser->format)
        *buf++ = ' ';
    *buf = '\0';
    ser->index += len - 1;
    return true;
}

static inline bool cra_json_write_left_brace(CraSerializer *ser)
{
    unsigned char *buf;
    CRA_SERIALIZER_BUF(ser, buf, sizeof("{"));
    *buf++ = '{';
    *buf = '\0';
    ser->index += sizeof("{") - 1;
    return true;
}

static inline bool cra_json_write_right_brace(CraSerializer *ser)
{
    size_t len;
    unsigned char *buf;
    len = ser->format ? (ser->nested + sizeof("\n}")) : sizeof("}");
    CRA_SERIALIZER_BUF(ser, buf, len);
    if (ser->format)
    {
        *buf++ = '\n';
        cra_json_write_whitespaces(buf, ser->nested);
        buf += ser->nested;
    }
    *buf++ = '}';
    *buf = '\0';
    ser->index += len - 1;
    return true;
}

static inline bool cra_json_write_left_bracket(CraSerializer *ser)
{
    unsigned char *buf;
    CRA_SERIALIZER_BUF(ser, buf, sizeof("["));
    *buf++ = '[';
    *buf = '\0';
    ser->index += sizeof("[") - 1;
    return true;
}

static inline bool cra_json_write_right_bracket(CraSerializer *ser)
{
    unsigned char *buf;
    CRA_SERIALIZER_BUF(ser, buf, sizeof("]"));
    *buf++ = ']';
    *buf = '\0';
    ser->index += sizeof("]") - 1;
    return true;
}

static bool cra_json_write_bool(CraSerializer *ser, bool val)
{
    unsigned char *buf;
    size_t needed = val ? sizeof("true") : sizeof("false");
    CRA_SERIALIZER_BUF(ser, buf, needed);
    memcpy(buf, val ? "true" : "false", needed);
    ser->index += needed - 1;
    return true;
}

static inline bool cra_json_compare_double(double a, double b)
{
    return fabs(a - b) < DBL_EPSILON;
}

static bool cra_json_write_double(CraSerializer *ser, double val)
{
    int len;
    double dbl;
    unsigned char *buf;
    char dblstr[32] = {0};

    if (isnan(val) || isinf(val))
    {
        ser->error = CRA_SER_ERROR_FLOAT_NAN_OR_INF;
        return false;
    }

#ifdef CRA_COMPILER_MSVC
    len = sprintf_s(dblstr, sizeof(dblstr), "%1.15g", val);
    if ((sscanf_s(dblstr, "%lg", &dbl) != 1) || !cra_json_compare_double(dbl, val))
    {
        len = sprintf_s(dblstr, sizeof(dblstr), "%1.17g", val);
    }
#else
    len = sprintf(dblstr, "%1.15g", val);
    if ((sscanf(dblstr, "%lg", &dbl) != 1) || !cra_json_compare_double(dbl, val))
    {
        len = sprintf(dblstr, "%1.17g", val);
    }
#endif

    if (len < 0 || len > (int)(sizeof(dblstr) - 1))
    {
        ser->error = CRA_SER_ERROR_FLOAT_NAN_OR_INF;
        return false;
    }

    // write val
    CRA_SERIALIZER_BUF(ser, buf, len + sizeof(""));
    memcpy(buf, dblstr, len);
    buf[len] = '\0';
    ser->index += len;
    return true;
}

#define CRA_CHECK_WRITE_NULL                          \
    if (val == NULL)                                  \
    {                                                 \
        CRA_SERIALIZER_BUF(ser, buf, sizeof("null")); \
        memcpy(buf, "null", sizeof("null"));          \
        ser->index += sizeof("null") - 1;             \
        return true;                                  \
    }

static bool cra_json_write_string(CraSerializer *ser, const char *val)
{
    unsigned char *buf;
    unsigned char *strbuf;
    const unsigned char *str;

    CRA_CHECK_WRITE_NULL

    // 空串
    if (val[0] == '\0')
    {
        CRA_SERIALIZER_BUF(ser, buf, sizeof("\"\""));
        memcpy(buf, "\"\"", sizeof("\"\""));
        ser->index += sizeof("\"\"") - 1;
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

    CRA_SERIALIZER_BUF(ser, buf, needed + sizeof("\"\""));
    ser->index += needed + sizeof("\"\"") - 1;

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

static inline bool cra_json_write_key_front_whitespace(CraSerializer *ser)
{
    unsigned char *buf;
    if (ser->format)
    {
        CRA_SERIALIZER_BUF(ser, buf, sizeof("\n") + ser->nested);
        ser->index += sizeof("\n") - 1 + ser->nested;
        *buf = '\n';
        cra_json_write_whitespaces(buf + 1, ser->nested);
    }
    return true;
}

static inline bool cra_json_write_key_back_colon(CraSerializer *ser)
{
    unsigned char *buf;
    size_t needed = ser->format ? 3 : 2;
    CRA_SERIALIZER_BUF(ser, buf, needed);
    ser->index += needed - 1;
    *buf++ = ':';
    if (ser->format)
        *buf++ = ' ';
    *buf = '\0';
    return true;
}

// [\n[\t*N]]"key":[ ]
static bool cra_json_write_key_and_colon(CraSerializer *ser, const char *key)
{
    if (key == NULL)
    {
        ser->error = CRA_SER_ERROR_JSON_KEY_IS_NULL;
        return false;
    }
    // write whitespace
    if (!cra_json_write_key_front_whitespace(ser))
        return false;
    // write key
    if (!cra_json_write_string(ser, key))
        return false;
    // write colon
    return cra_json_write_key_back_colon(ser);
}

static bool cra_json_write_once(CraSerializer *ser, void *val, size_t offset, const CraTypeMeta *meta);
static bool cra_json_stringify_all(CraSerializer *ser, void *val, const CraTypeMeta *meta);

#define CRA_SER_NESTING_BEGIN                   \
    if (ser->nested++ >= CRA_SER_MAX_NESTING)   \
    {                                           \
        ser->error = CRA_SER_ERROR_MAX_NESTING; \
        return false;                           \
    }
#define CRA_SER_NESTING_END --ser->nested;

static bool cra_json_write_struct(CraSerializer *ser, void *val, const CraTypeMeta *members_meta)
{
    unsigned char *buf;

    CRA_CHECK_WRITE_NULL

    CRA_SER_NESTING_BEGIN

    // write '{'
    if (!cra_json_write_left_brace(ser))
        return false;

    // write members
    if (!cra_json_stringify_all(ser, val, members_meta))
        return false;

    CRA_SER_NESTING_END

    // write '}'
    if (!cra_json_write_right_brace(ser))
        return false;

    return true;
}

static bool __cra_json_write_list(CraSerializer *ser, void *val, const CraTypeMeta *element_meta, const CraTypeIter_i *iter_i, cra_ser_count_t count)
{
    unsigned char *buf;

    assert_always((element_meta + 1)->type == __CRA_TYPE_END_OF_META);

    CRA_CHECK_WRITE_NULL

    CRA_SER_NESTING_BEGIN

    // write '['
    if (!cra_json_write_left_bracket(ser))
        return false;

    // write elements
    if (iter_i)
    {
        void *item = NULL;
        char it[CRA_SERI_ITER_SIZE];
        assert_always(iter_i->list.init && iter_i->list.next);
        iter_i->list.init(val, &it, sizeof(it));
        for (count = 0; iter_i->list.next(&it, &item);)
        {
            // element
            if (!cra_json_write_once(ser, item, 0, element_meta))
                return false;
            // ,[ ]
            if (!cra_json_write_comma(ser))
                return false;
            if (++count == CRA_SER_COUNT_MAX)
            {
                ser->error = CRA_SER_ERROR_MAX_ELEMENTS;
                return false;
            }
        }
        if (count > 0)
            ser->index -= ser->format ? 2 : 1; // 消除最后的“,[ ]”
    }
    else
    {
        if (count > 0)
        {
            size_t slot_size = element_meta->is_ptr ? sizeof(void *) : element_meta->size;
            for (size_t i = 0;;)
            {
                if (!cra_json_write_once(ser, (char *)val + slot_size * i, 0, element_meta))
                    return false;
                if (++i < count)
                {
                    // ,[ ]
                    if (!cra_json_write_comma(ser))
                        return false;
                }
                else
                {
                    break;
                }
            }
        }
    }

    CRA_SER_NESTING_END

    // write ']'
    if (!cra_json_write_right_bracket(ser))
        return false;

    return true;
}

static bool cra_json_write_list(CraSerializer *ser, void *val, const CraTypeMeta *element_meta, const CraTypeIter_i *iter_i)
{
    return __cra_json_write_list(ser, val, element_meta, iter_i, 0);
}

static bool cra_json_write_array(CraSerializer *ser, void *val, cra_ser_count_t count, const CraTypeMeta *element_meta)
{
    return __cra_json_write_list(ser, val, element_meta, NULL, count);
}

static bool cra_json_write_dict(CraSerializer *ser, void *val, const CraTypeMeta *kv_meta, const CraTypeIter_i *iter_i)
{
    unsigned char *buf;

    assert_always((kv_meta + 2)->type == __CRA_TYPE_END_OF_META);
    assert_always(iter_i != NULL && iter_i->dict.init && iter_i->dict.next);

    // check key type
    if (kv_meta->type > CRA_TYPE_STRING)
    {
        ser->error = CRA_SER_ERROR_JSON_CANNOT_BE_KEY;
        return false;
    }

    CRA_CHECK_WRITE_NULL

    CRA_SER_NESTING_BEGIN

    // write '{'
    if (!cra_json_write_left_brace(ser))
        return false;

    // write elements
    void *k = NULL;
    void *v = NULL;
    cra_ser_count_t count = 0;
    char it[CRA_SERI_ITER_SIZE];
    iter_i->dict.init(val, &it, sizeof(it));
    while (iter_i->dict.next(&it, &k, &v))
    {
        if (!cra_json_write_key_front_whitespace(ser))
            return false;

        // "key"
        if (k == NULL)
        {
            ser->error = CRA_SER_ERROR_JSON_KEY_IS_NULL;
            return false;
        }
        CRA_SERIALIZER_BUF(ser, buf, sizeof("\""));
        ++ser->index;
        *buf = '\"';
        if (!cra_json_write_once(ser, k, 0, kv_meta))
            return false;
        CRA_SERIALIZER_BUF(ser, buf, sizeof("\""));
        ++ser->index;
        *buf = '\"';
        // :
        if (!cra_json_write_key_back_colon(ser))
            return false;
        // value
        if (!cra_json_write_once(ser, v, 0, kv_meta + 1))
            return false;
        // ,[ ]
        if (!cra_json_write_comma(ser))
            return false;
        if (++count == CRA_SER_COUNT_MAX)
        {
            ser->error = CRA_SER_ERROR_MAX_ELEMENTS;
            return false;
        }
    }
    if (count > 0)
        ser->index -= ser->format ? 2 : 1; // 消除最后的“,[ ]”

    CRA_SER_NESTING_END

    // write '}'
    if (!cra_json_write_right_brace(ser))
        return false;

    return true;
}

static bool cra_json_write_once(CraSerializer *ser, void *val, size_t offset, const CraTypeMeta *meta)
{
    double dbl;
    void *value = (void *)((char *)val + offset);
    switch (meta->type)
    {
    case CRA_TYPE_FALSE:
    case CRA_TYPE_TRUE:
        if (!cra_json_write_bool(ser, *(bool *)value))
            return false;
        break;
    case CRA_TYPE_INT8:
        dbl = (double)(*(int8_t *)value);
        goto write_dbl;
    case CRA_TYPE_INT16:
        dbl = (double)(*(int16_t *)value);
        goto write_dbl;
    case CRA_TYPE_INT32:
        dbl = (double)(*(int32_t *)value);
        goto write_dbl;
    case CRA_TYPE_INT64:
        dbl = (double)(*(int64_t *)value);
        goto write_dbl;
    case CRA_TYPE_UINT8:
        dbl = (double)(*(uint8_t *)value);
        goto write_dbl;
    case CRA_TYPE_UINT16:
        dbl = (double)(*(uint16_t *)value);
        goto write_dbl;
    case CRA_TYPE_UINT32:
        dbl = (double)(*(uint32_t *)value);
        goto write_dbl;
    case CRA_TYPE_UINT64:
        dbl = (double)(*(uint64_t *)value);
        goto write_dbl;
    case CRA_TYPE_FLOAT:
        dbl = (double)(*(float *)value);
        goto write_dbl;
    case CRA_TYPE_DOUBLE:
        dbl = *(double *)value;
    write_dbl:
        if (!cra_json_write_double(ser, dbl))
            return false;
        break;
    case CRA_TYPE_STRING:
        if (meta->is_ptr)
            value = *(void **)value;
        if (!cra_json_write_string(ser, (char *)value))
            return false;
        break;
    case CRA_TYPE_STRUCT:
        if (meta->is_ptr)
            value = *(void **)value;
        if (!cra_json_write_struct(ser, value, meta->meta))
            return false;
        break;
    case CRA_TYPE_LIST:
        if (meta->is_ptr)
            value = *(void **)value;
        if (!__cra_json_write_list(ser, value, meta->meta, meta->iter_i, *(cra_ser_count_t *)((char *)val + meta->noffset)))
            return false;
        break;
    case CRA_TYPE_DICT:
        if (meta->is_ptr)
            value = *(void **)value;
        if (!cra_json_write_dict(ser, value, meta->meta, meta->iter_i))
            return false;
        break;

    default:
        ser->error = CRA_SER_ERROR_INVALID_VALUE;
        return false;
    }
    return true;
}

// =======================

static void cra_json_stringify_begin(CraSerializer *ser, unsigned char *buffer, size_t buffer_length, bool format)
{
    assert_always(ser != NULL);
    assert_always(buffer_length >= 2);

    ser->error = CRA_SER_ERROR_SUCCESS;
    ser->format = format;
    ser->nested = 0;
    ser->index = 0;
    ser->length = buffer_length;
    if (!!buffer)
    {
        ser->noalloc = true;
        ser->buffer = buffer;
    }
    else
    {
        ser->noalloc = false;
        ser->buffer = (unsigned char *)cra_malloc(buffer_length);
    }
    bzero(&ser->release, sizeof(ser->release));
}

static unsigned char *cra_json_stringify_end(CraSerializer *ser, size_t *buffer_length, CraSerError *error)
{
    assert_always(ser != NULL);
    assert_always(ser->buffer != NULL);
    assert_always(buffer_length != NULL);
    assert_always(ser->index < ser->length);

    *buffer_length = ser->index + 1;
    if (!!error)
        *error = ser->error;
    return ser->error == CRA_SER_ERROR_SUCCESS ? ser->buffer : NULL;
}

static bool cra_json_stringify_all(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    CraTypeMeta *m = (CraTypeMeta *)meta;
    if (m->type == __CRA_TYPE_END_OF_META)
        return true;

    do
    {
        // [\n]"key":[ ]
        if (!cra_json_write_key_and_colon(ser, m->member))
            return false;
        // value
        if (!cra_json_write_once(ser, val, m->offset, m))
            return false;

        m += 1;
        if (m->type != __CRA_TYPE_END_OF_META)
        {
            // ,[ ]
            if (!cra_json_write_comma(ser))
                return false;
        }
        else
        {
            break;
        }
    } while (true);

    return true;
}

#endif // end serialize

#if 1 // deserialize

// 跳过控制字符和空格
static inline unsigned char *cra_json_skip_whitespaces(CraSerializer *ser)
{
    unsigned char *buf;
    buf = ser->buffer + ser->index;
    while (*buf <= 0x20 && *buf++)
        ++ser->index;
    if (ser->index == ser->length)
        --ser->index;
    return ser->buffer + ser->index;
}

static inline bool cra_json_is_ch(CraSerializer *ser, unsigned char ch)
{
    unsigned char *buf = cra_json_skip_whitespaces(ser);
    if (CRA_JSON_READ_ENSURE(ser, 1) && *buf == ch)
        return true;
    return false;
}

static inline bool cra_json_check_symbol(CraSerializer *ser, unsigned char ch)
{
    if (cra_json_is_ch(ser, ch))
    {
        ++ser->index; // 越过ch
        return true;
    }
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    return false;
}

static inline bool cra_json_skip_comma_and_check_quit_loop(CraSerializer *ser, unsigned char quitch, bool *loopflag)
{
    if (cra_json_is_ch(ser, ','))
        ++ser->index;
    // } or ]
    *loopflag = !cra_json_is_ch(ser, quitch);
    return true;
}

static inline void cra_json_skip_value(CraSerializer *ser)
{
    size_t leftcount = 0;
    size_t skipcount = 0;
    unsigned char *buf = CRA_JSON_READ_GET_BUF(ser);
    while (CRA_JSON_READ_ENSURE(ser, 1) && (leftcount > 0 || *buf != ','))
    {
        if (*buf == '{' || *buf == '[')
            ++leftcount;
        else if (*buf == '}' || *buf == ']')
        {
            // leftcount == 0 表示要跳过的value是对象中的最后一个，可以跳出循环
            if (leftcount == 0)
                break;
            --leftcount;
        }
        ++skipcount;
        ++buf;
    }
    ser->index += skipcount;
}

bool cra_json_read_bool(CraSerializer *ser, bool *retval)
{
    size_t len = sizeof("true") - 1;
    unsigned char *buf = CRA_JSON_READ_GET_BUF(ser);
    if (CRA_JSON_READ_ENSURE(ser, len) && strncmp((char *)buf, "true", len) == 0)
    {
        *(bool *)retval = true;
        ser->index += len;
        return true;
    }
    len = sizeof("false") - 1;
    if (CRA_JSON_READ_ENSURE(ser, len) && strncmp((char *)buf, "false", len) == 0)
    {
        *(bool *)retval = false;
        ser->index += len;
        return true;
    }
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    return false;
}

bool cra_json_read_double(CraSerializer *ser, double *retval)
{
    unsigned char *end = NULL;
    unsigned char *buf = CRA_JSON_READ_GET_BUF(ser);
    if (buf[0] == '-' || (buf[0] >= '0' && buf[0] <= '9'))
    {
        *retval = strtod((char *)buf, (char **)&end);
        if (buf != end && !(isnan(*retval) || isinf(*retval)))
        {
            ser->index += (end - buf);
            return true;
        }
    }
    ser->error = CRA_SER_ERROR_FLOAT_NAN_OR_INF;
    return false;
}

static inline int cra_hex2int(unsigned char c)
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
static uint32_t cra_parse_unicodee_escape(const unsigned char *buf)
{
    uint32_t code_point = 0;
    for (int i = 0; i < 4; ++i)
    {
        int val = cra_hex2int(*buf);
        if (val == -1)
            return UINT32_MAX; // error
        code_point = (code_point << 4) | val;
        buf++;
    }
    return code_point;
}

static inline bool cra_is_high_surrogate(uint32_t code_point)
{
    return code_point >= 0xd800 && code_point <= 0xdbff;
}

static inline bool cra_is_low_surrogate(uint32_t code_point)
{
    return code_point >= 0xdc00 && code_point <= 0xdfff;
}

static inline uint32_t cra_combine_surrogates(uint32_t high, uint32_t low)
{
    return 0x10000 + ((high - 0xd800) << 10) + (low - 0xdc00);
}

static int cra_code_point2utf8(CraSerializer *ser, uint32_t code_point, unsigned char *str, size_t remain)
{
#define CHECK(_needed)                                   \
    if (remain < _needed)                                \
    {                                                    \
        ser->error = CRA_SER_ERROR_STRING_BUF_TOO_SMALL; \
        return -1;                                       \
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

static bool cra_json_read_string(CraSerializer *ser, char *retval, cra_ser_count_t max_length,
                                 bool is_char_ptr, bool auto_free_if_fail)
{
    size_t len, length;
    unsigned char *buf, *str;

    CRA_JSON_READ_BUF(ser, buf, sizeof("\"\""));
    if (*buf != '\"')
    {
        // 不是字符串
        ser->error = CRA_SER_ERROR_INVALID_VALUE;
        return false;
    }
    ++buf; // 越过'\"'

    // 检查串中是否有特殊字符
    for (length = 0, str = buf; (size_t)(str - ser->buffer) < ser->length && *str != '\"';)
    {
        if (*str++ == '\\')
        {
            if ((size_t)(str - ser->buffer) >= ser->length)
                goto error_return;
            (void)++length;
            (void)++str;
        }
    }

    // 如果没有闭合的'\"'则表示这不是正确的字符串
    if (*str != '\"')
        goto error_return;

    // get string length
    len = (size_t)(str - buf) - length;
    if (is_char_ptr)
    {
        max_length = (cra_ser_count_t)(len + sizeof(""));
        str = *(unsigned char **)retval = (unsigned char *)cra_malloc(max_length);
        if (auto_free_if_fail)
            cra_ser_release_add(&ser->release, true, str, NULL, NULL);
    }
    else
    {
        str = (unsigned char *)retval;
    }

    // 没有特殊字符时copy string
    if (length == 0)
    {
        // 测试字符数组大小是否能容纳该字符串
        if (len >= max_length)
        {
            ser->error = CRA_SER_ERROR_STRING_BUF_TOO_SMALL;
            return false;
        }

        memcpy(str, buf, len);
        str[len] = '\0';
        goto ok_return;
    }

    // 有特殊字符时copy string
    len += length;
    size_t remaining = max_length;
    for (length = 0; length < len;)
    {
        // 测试字符数组大小是否能容纳该字符串
        if (remaining <= 1)
        {
            ser->error = CRA_SER_ERROR_STRING_BUF_TOO_SMALL;
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
                uint32_t code_pint = cra_parse_unicodee_escape(buf + length);
                if (code_pint == UINT32_MAX)
                    goto error_return;
                length += sizeof("XXXX") - 1;

                // 处理代理对
                if (!cra_is_high_surrogate(code_pint))
                {
                    if (cra_is_low_surrogate(code_pint))
                        goto error_return; // 只有单独的低位代理
                    // ascii
                    goto to_utf8;
                }
                if (buf[0] != '\\' || buf[1] != 'u')
                    goto error_return; // 代理对不完整
                length += sizeof("\\u") - 1;
                uint32_t low_surrogate = cra_parse_unicodee_escape(buf + length);
                if (!cra_is_low_surrogate(low_surrogate))
                    goto error_return;
                length += sizeof("XXXX") - 1;
                code_pint = cra_combine_surrogates(code_pint, low_surrogate);

            to_utf8:
                // to utf-8
                {
                    int retlen = cra_code_point2utf8(ser, code_pint, str, remaining);
                    if (retlen == -1)
                        goto error_return;
                    remaining -= retlen;
                    str += retlen;
                }
                continue; // 跳过下面的"++length"
            }
            break;
            default:
                goto error_return;
            }
        }

        (void)++length;
        (void)--remaining;
    }

    *str = '\0';
ok_return:
    ser->index += len + sizeof("\"\"") - 1;
    return true;

error_return:
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    return false;
}

static inline bool cra_json_check_key_back_colon(CraSerializer *ser)
{
    if (!cra_json_check_symbol(ser, ':'))
        return false;
    cra_json_skip_whitespaces(ser);
    return true;
}

// key & :
static bool cra_json_read_key_and_colon(CraSerializer *ser, char **key)
{
    // read key
    if (!cra_json_read_string(ser, (char *)key, 0, true, false))
    {
        ser->error = CRA_SER_ERROR_INVALID_VALUE;
        return false;
    }
    // read colon
    return cra_json_check_key_back_colon(ser);
}

static bool cra_json_read_once(CraSerializer *ser, void *val, size_t offset, const CraTypeMeta *meta, bool auto_free_if_fail);
static bool cra_json_parse_all(CraSerializer *ser, void *retval, const CraTypeMeta *meta, bool auto_free_if_fail);

static inline void *cra_json_alloc_init(CraSerializer *ser, void *retval, size_t valsize, bool is_ptr,
                                        bool *auto_free_if_fail, const CraTypeInit_i *init_i, void *args4init)
{
    void (*uninit_fn)(void *) = NULL;
    void (*dealloc_fn)(void *) = NULL;
    if (is_ptr)
    {
        if (init_i && init_i->alloc)
        {
            dealloc_fn = init_i->dealloc;
            retval = *(void **)retval = init_i->alloc();
        }
        else
        {
            retval = *(void **)retval = cra_malloc(valsize);
        }
    }
    if (init_i && init_i->init)
    {
        uninit_fn = init_i->uinit;
        init_i->init(retval, args4init);
    }
    else
    {
        bzero(retval, valsize);
    }
    if (*auto_free_if_fail && (is_ptr || uninit_fn))
        cra_ser_release_add(&ser->release, is_ptr, retval, uninit_fn, dealloc_fn);

    *auto_free_if_fail = !uninit_fn;
    return retval;
}

bool cra_json_read_struct(CraSerializer *ser, void *retval, size_t valsize, bool is_ptr, bool auto_free_if_fail,
                          const CraTypeMeta *members_meta, const CraTypeInit_i *init_i, void *args4init)
{
    assert_always(members_meta->type != __CRA_TYPE_END_OF_META);

    CRA_SER_NESTING_BEGIN

    if (!cra_json_check_symbol(ser, '{'))
        return false;

    // alloc & init
    retval = cra_json_alloc_init(ser, retval, valsize, is_ptr, &auto_free_if_fail, init_i, args4init);

    // read members
    if (!cra_json_parse_all(ser, retval, members_meta, auto_free_if_fail))
        return false;

    if (!cra_json_check_symbol(ser, '}'))
        return false;

    CRA_SER_NESTING_END

    return true;
}

bool cra_json_read_list(CraSerializer *ser, void *retval, size_t valsize,
                        bool is_ptr, bool auto_free_if_fail, const CraTypeMeta *element_meta,
                        const CraTypeIter_i *iter_i, const CraTypeInit_i *init_i, void *args4init)
{
    bool loop = true;

    assert_always((element_meta + 1)->type == __CRA_TYPE_END_OF_META);
    assert_always(iter_i != NULL && iter_i->list.append != NULL);

    CRA_SER_NESTING_BEGIN

    if (!cra_json_check_symbol(ser, '['))
        return false;
    // empty array
    if (cra_json_is_ch(ser, ']'))
        loop = false;

    size_t slot_size = element_meta->is_ptr ? sizeof(void *) : element_meta->size;

    // alloc & init
    retval = cra_json_alloc_init(ser, retval, valsize, is_ptr, &auto_free_if_fail, init_i, args4init);

    void *item = cra_malloc(slot_size);
    for (; loop;)
    {
        // read value
        if (!cra_json_read_once(ser, item, 0, element_meta, auto_free_if_fail))
            goto error_return;
        if (!iter_i->list.append(retval, item))
        {
            ser->error = CRA_SER_ERROR_APPEND_ELEMENT_FAILED;
            goto error_return;
        }

        if (!cra_json_skip_comma_and_check_quit_loop(ser, ']', &loop))
            goto error_return;
    }

    if (!cra_json_check_symbol(ser, ']'))
        return false;

    CRA_SER_NESTING_END

    if (item != NULL)
        cra_free(item);
    return true;
error_return:
    if (item != NULL)
        cra_free(item);
    return false;
}

bool cra_json_read_array(CraSerializer *ser, void *retval, size_t valsize,
                         bool is_ptr, cra_ser_count_t *countptr, const CraTypeMeta *element_meta)
{
    bool loop = true;

    assert_always((element_meta + 1)->type == __CRA_TYPE_END_OF_META);
    assert_always(countptr != NULL);

    CRA_SER_NESTING_BEGIN

    if (!cra_json_check_symbol(ser, '['))
        return false;
    // empty array
    if (cra_json_is_ch(ser, ']'))
        loop = false;

    // read elements
    if (!cra_json_skip_whitespaces(ser))
        return false;

    size_t maxcnt, count;
    size_t slot_size;
    void *array;

    slot_size = element_meta->is_ptr ? sizeof(void *) : element_meta->size;
    if (is_ptr)
    {
        maxcnt = 8;
        valsize = slot_size * maxcnt;
        array = cra_malloc(valsize);
    }
    else
    {
        maxcnt = valsize / slot_size;
        array = retval;
    }
    bzero(array, valsize);

    for (count = 0; loop; ++count)
    {
        if (count == maxcnt)
        {
            if (!is_ptr)
            {
                ser->error = CRA_SER_ERROR_ARRAY_TOO_SMALL;
                goto error_return;
            }
            size_t newcnt = maxcnt + (maxcnt >> 1); // * 1.5
            array = cra_realloc(array, slot_size * newcnt);
            bzero((char *)array + maxcnt * slot_size, (newcnt - maxcnt) * slot_size);
        }
        if (count == CRA_SER_COUNT_MAX)
        {
            ser->error = CRA_SER_ERROR_MAX_ELEMENTS;
            goto error_return;
        }

        // read value
        if (!cra_json_read_once(ser, (char *)array + count * slot_size, 0, element_meta, true))
            goto error_return;

        if (!cra_json_skip_comma_and_check_quit_loop(ser, ']', &loop))
            goto error_return;
    }

    if (!cra_json_check_symbol(ser, ']'))
        goto error_return;

    CRA_SER_NESTING_END

    *countptr = (cra_ser_count_t)count;

    if (is_ptr)
    {
        *(void **)retval = array;
        cra_ser_release_add(&ser->release, is_ptr, array, NULL, NULL);
    }
    return true;
error_return:
    if (is_ptr && array)
        cra_free(array);
    return false;
}

bool cra_json_read_dict(CraSerializer *ser, void *retval, size_t valsize,
                        bool is_ptr, bool auto_free_if_fail, const CraTypeMeta *kv_meta,
                        const CraTypeIter_i *iter_i, const CraTypeInit_i *init_i, void *args4init)
{
    bool loop = true;

    assert_always((kv_meta + 2)->type == __CRA_TYPE_END_OF_META);
    assert_always(iter_i != NULL && iter_i->dict.append != NULL);

    CRA_SER_NESTING_BEGIN

    if (!cra_json_check_symbol(ser, '{'))
        return false;
    // empty array
    if (cra_json_is_ch(ser, '}'))
        loop = false;

    size_t key_size = kv_meta->is_ptr ? sizeof(void *) : kv_meta->size;
    size_t val_size = (kv_meta + 1)->is_ptr ? sizeof(void *) : (kv_meta + 1)->size;

    // alloc & init
    retval = cra_json_alloc_init(ser, retval, valsize, is_ptr, &auto_free_if_fail, init_i, args4init);

    // read elements
    void *k = cra_malloc(key_size + val_size);
    void *v = (char *)k + key_size;
    for (; loop;)
    {
#if 1 // "key"
        if (kv_meta->type != CRA_TYPE_STRING)
        {
            // 越过'\"'
            if (!cra_json_check_symbol(ser, '\"'))
                goto error_return;
        }
        // read key
        if (!cra_json_read_once(ser, k, 0, kv_meta, auto_free_if_fail))
            goto error_return;
        if (kv_meta->type != CRA_TYPE_STRING)
        {
            // 越过'\"'
            if (!cra_json_check_symbol(ser, '\"'))
                goto error_return;
        }
#endif
        // colon :
        if (!cra_json_check_key_back_colon(ser))
            goto error_return;
        // value
        if (!cra_json_read_once(ser, v, 0, kv_meta + 1, auto_free_if_fail))
            goto error_return;
        // add k-v to dict
        if (!iter_i->dict.append(retval, k, v))
        {
            ser->error = CRA_SER_ERROR_APPEND_ELEMENT_FAILED;
            goto error_return;
        }

        if (!cra_json_skip_comma_and_check_quit_loop(ser, '}', &loop))
            goto error_return;
    }

    if (!cra_json_check_symbol(ser, '}'))
        goto error_return;

    CRA_SER_NESTING_END

    if (k != NULL)
        cra_free(k);
    return true;
error_return:
    if (k != NULL)
        cra_free(k);
    return false;
}

static bool cra_json_read_once(CraSerializer *ser, void *val, size_t offset, const CraTypeMeta *meta, bool auto_free_if_fail)
{
    void *value = (void *)((char *)val + offset);
    assert_always(value != NULL);

    unsigned char *buf = cra_json_skip_whitespaces(ser);

    // null
    if (CRA_JSON_READ_ENSURE(ser, sizeof("null")) && strncmp((char *)buf, "null", sizeof("null") - 1) == 0)
    {
        if (!meta->is_ptr)
        {
            ser->error = CRA_SER_ERROR_CANNOT_BE_NULL;
            return false;
        }
        *(void **)value = NULL;
        ser->index += sizeof("null") - 1;
        return true;
    }

    double dbl;
    switch (meta->type)
    {
    case CRA_TYPE_FALSE:
    case CRA_TYPE_TRUE:
        if (!cra_json_read_bool(ser, (bool *)value))
            return false;
        break;
    case CRA_TYPE_INT8:
        if (!cra_json_read_double(ser, &dbl))
            return false;
        *(int8_t *)value = (int8_t)dbl;
        break;
    case CRA_TYPE_INT16:
        if (!cra_json_read_double(ser, &dbl))
            return false;
        *(int16_t *)value = (int16_t)dbl;
        break;
    case CRA_TYPE_INT32:
        if (!cra_json_read_double(ser, &dbl))
            return false;
        *(int32_t *)value = (int32_t)dbl;
        break;
    case CRA_TYPE_INT64:
        if (!cra_json_read_double(ser, &dbl))
            return false;
        *(int64_t *)value = (int64_t)dbl;
        break;
    case CRA_TYPE_UINT8:
        if (!cra_json_read_double(ser, &dbl))
            return false;
        *(uint8_t *)value = (uint8_t)dbl;
        break;
    case CRA_TYPE_UINT16:
        if (!cra_json_read_double(ser, &dbl))
            return false;
        *(uint16_t *)value = (uint16_t)dbl;
        break;
    case CRA_TYPE_UINT32:
        if (!cra_json_read_double(ser, &dbl))
            return false;
        *(uint32_t *)value = (uint32_t)dbl;
        break;
    case CRA_TYPE_UINT64:
        if (!cra_json_read_double(ser, &dbl))
            return false;
        *(uint64_t *)value = (uint64_t)dbl;
        break;
    case CRA_TYPE_FLOAT:
        if (!cra_json_read_double(ser, &dbl))
            return false;
        *(float *)value = (float)dbl;
        break;
    case CRA_TYPE_DOUBLE:
        if (!cra_json_read_double(ser, (double *)value))
            return false;
        break;
    case CRA_TYPE_STRING:
        if (!cra_json_read_string(ser, (char *)value, (cra_ser_count_t)meta->size, meta->is_ptr, auto_free_if_fail))
            return false;
        break;
    case CRA_TYPE_STRUCT:
        if (!cra_json_read_struct(ser, value, meta->size, meta->is_ptr, auto_free_if_fail, meta->meta, meta->init_i, meta->args4init))
            return false;
        break;
    case CRA_TYPE_LIST:
        if (meta->nsize == 0)
        {
            // list
            if (!cra_json_read_list(ser, value, meta->size, meta->is_ptr, auto_free_if_fail, meta->meta, meta->iter_i, meta->init_i, meta->args4init))
                return false;
        }
        else
        {
            // c array
            assert_always(meta->nsize == sizeof(cra_ser_count_t));
            if (!cra_json_read_array(ser, value, meta->size, meta->is_ptr, (cra_ser_count_t *)((char *)val + meta->noffset), meta->meta))
                return false;
        }
        break;
    case CRA_TYPE_DICT:
        if (!cra_json_read_dict(ser, value, meta->size, meta->is_ptr, auto_free_if_fail, meta->meta, meta->iter_i, meta->init_i, meta->args4init))
            return false;
        break;

    default:
        ser->error = CRA_SER_ERROR_INVALID_VALUE;
        return false;
    }
    return true;
}

// =====================

static void cra_json_parse_begin(CraSerializer *ser, unsigned char *buffer, size_t buffer_length)
{
    assert_always(ser != NULL);
    assert_always(buffer != NULL);
    assert_always(buffer_length > 0);

    ser->error = CRA_SER_ERROR_SUCCESS;
    ser->nested = 0;
    ser->noalloc = true;
    ser->index = 0;
    ser->length = buffer_length;
    ser->buffer = buffer;
    cra_ser_release_init(&ser->release);
}

static void cra_json_parse_end(CraSerializer *ser, CraSerError *error)
{
    assert_always(ser != NULL);
    assert_always(ser->buffer != NULL);
    assert_always(ser->index <= ser->length);

    if (!!error)
        *error = ser->error;
    cra_ser_release_uninit(&ser->release, ser->error != CRA_SER_ERROR_SUCCESS);
}

static bool cra_json_parse_all(CraSerializer *ser, void *val, const CraTypeMeta *meta, bool auto_free_if_fail)
{
    bool loop;
    char *key;
    CraTypeMeta *last, *m = (CraTypeMeta *)meta;

    cra_json_skip_whitespaces(ser);

    do
    {
        // key & :
        key = NULL;
        if (!cra_json_read_key_and_colon(ser, &key))
            goto error_return;

        // 匹配key val
        last = m;
        while (true)
        {
            if (strcmp(key, m->member) == 0)
            {
                // value
                if (!cra_json_read_once(ser, val, m->offset, m, auto_free_if_fail))
                    goto error_return;

                if ((++m)->type == __CRA_TYPE_END_OF_META)
                    m = (CraTypeMeta *)meta;
                break;
            }
            if ((++m)->type == __CRA_TYPE_END_OF_META)
                m = (CraTypeMeta *)meta;
            if (m == last)
            {
                // skip value
                cra_json_skip_value(ser);
                break;
            }
        }

        cra_free(key);

        if (!cra_json_skip_comma_and_check_quit_loop(ser, '}', &loop))
            return false;
    } while (loop);

    return true;
error_return:
    if (key != NULL)
        cra_free(key);
    return false;
}

#endif // end deserialize

unsigned char *cra_json_stringify_struct0(unsigned char *buffer, size_t *buffer_length, void *val,
                                          const CraTypeMeta *members_meta, bool format,
                                          CraSerError *error)
{
    CraSerializer ser;
    assert_always(buffer_length != NULL);
    cra_json_stringify_begin(&ser, buffer, *buffer_length, format);
    cra_json_write_struct(&ser, val, members_meta);
    return cra_json_stringify_end(&ser, buffer_length, error);
}

void cra_json_parse_struct0(unsigned char *buffer, size_t buffer_length, void *retval,
                            size_t valsize, bool is_ptr, const CraTypeMeta *members_meta,
                            const CraTypeInit_i *init_i, void *args4init, CraSerError *error)
{
    CraSerializer ser;
    cra_json_parse_begin(&ser, buffer, buffer_length);
    cra_json_read_struct(&ser, retval, valsize, is_ptr, true, members_meta, init_i, args4init);
    cra_json_parse_end(&ser, error);
}

unsigned char *cra_json_stringify_list0(unsigned char *buffer, size_t *buffer_length, void *val,
                                        const CraTypeMeta *element_meta, const CraTypeIter_i *iter_i,
                                        bool format, CraSerError *error)
{
    CraSerializer ser;
    assert_always(buffer_length != NULL);
    cra_json_stringify_begin(&ser, buffer, *buffer_length, format);
    cra_json_write_list(&ser, val, element_meta, iter_i);
    return cra_json_stringify_end(&ser, buffer_length, error);
}

void cra_json_parse_list0(unsigned char *buffer, size_t buffer_length, void *retval,
                          size_t valsize, bool is_ptr, const CraTypeMeta *element_meta,
                          const CraTypeIter_i *iter_i, const CraTypeInit_i *init_i,
                          void *args4init, CraSerError *error)
{
    CraSerializer ser;
    cra_json_parse_begin(&ser, buffer, buffer_length);
    cra_json_read_list(&ser, retval, valsize, is_ptr, true, element_meta, iter_i, init_i, args4init);
    cra_json_parse_end(&ser, error);
}

unsigned char *cra_json_stringify_array0(unsigned char *buffer, size_t *buffer_length, void *val,
                                         cra_ser_count_t count, const CraTypeMeta *element_meta,
                                         bool format, CraSerError *error)
{
    CraSerializer ser;
    assert_always(buffer_length != NULL);
    cra_json_stringify_begin(&ser, buffer, *buffer_length, format);
    cra_json_write_array(&ser, val, count, element_meta);
    return cra_json_stringify_end(&ser, buffer_length, error);
}

void cra_json_parse_array0(unsigned char *buffer, size_t buffer_length, void *retval, size_t valsize,
                           bool is_ptr, cra_ser_count_t *countptr, const CraTypeMeta *element_meta,
                           CraSerError *error)
{
    CraSerializer ser;
    cra_json_parse_begin(&ser, buffer, buffer_length);
    cra_json_read_array(&ser, retval, valsize, is_ptr, countptr, element_meta);
    cra_json_parse_end(&ser, error);
}

unsigned char *cra_json_stringify_dict0(unsigned char *buffer, size_t *buffer_length, void *val,
                                        const CraTypeMeta *kv_meta, const CraTypeIter_i *iter_i,
                                        bool format, CraSerError *error)
{
    CraSerializer ser;
    assert_always(buffer_length != NULL);
    cra_json_stringify_begin(&ser, buffer, *buffer_length, format);
    cra_json_write_dict(&ser, val, kv_meta, iter_i);
    return cra_json_stringify_end(&ser, buffer_length, error);
}

void cra_json_parse_dict0(unsigned char *buffer, size_t buffer_length, void *retval, size_t valsize,
                          bool is_ptr, const CraTypeMeta *kv_meta, const CraTypeIter_i *iter_i,
                          const CraTypeInit_i *init_i, void *args4init, CraSerError *error)
{
    CraSerializer ser;
    cra_json_parse_begin(&ser, buffer, buffer_length);
    cra_json_read_dict(&ser, retval, valsize, is_ptr, true, kv_meta, iter_i, init_i, args4init);
    cra_json_parse_end(&ser, error);
}
