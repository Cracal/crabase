/**
 * @file cra_bin-ser.c
 * @author Cracal
 * @brief binary srialization
 * @version 0.1
 * @date 2024-09-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "serialize/cra_bin_ser.h"
#include "cra_assert.h"
#include "cra_endian.h"
#include "cra_malloc.h"
#include "serialize/cra_ser_inner.h"
#include <math.h>

#if CRA_IS_BIG_ENDIAN
#define CRA_SER_SWAP16 CRA_BSWAP_UINT16
#define CRA_SER_SWAP32 CRA_BSWAP_UINT32
#define CRA_SER_SWAP64 CRA_BSWAP_UINT64
#else
#define CRA_SER_SWAP16(x) (x)
#define CRA_SER_SWAP32(x) (x)
#define CRA_SER_SWAP64(x) (x)
#endif

#if 1 // serialize

void
cra_bin_serialize_begin(CraSerializer *ser, unsigned char *buffer, size_t buffer_length)
{
    assert_always(ser != NULL);
    assert_always(buffer_length > 0);

    ser->error = CRA_SER_ERROR_SUCCESS;
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
        ser->buffer = cra_malloc(buffer_length);
    }
    bzero(&ser->release, sizeof(ser->release));
}

unsigned char *
cra_bin_serialize_end(CraSerializer *ser, size_t *buffer_length, CraSerError_e *error)
{
    assert_always(ser != NULL);
    assert_always(ser->buffer != NULL);
    assert_always(buffer_length != NULL);
    assert_always(ser->index < ser->length);

    *buffer_length = ser->index;
    if (!!error)
        *error = ser->error;
    return ser->error == CRA_SER_ERROR_SUCCESS ? ser->buffer : NULL;
}

bool
cra_bin_serialize_bool(CraSerializer *ser, bool val)
{
    unsigned char *buf;
    CRA_SERIALIZER_BUF(ser, buf, sizeof(bool));
    *buf = (unsigned char)(!!val);
    ++ser->index;
    return true;
}

bool
cra_bin_serialize_int8(CraSerializer *ser, int8_t val)
{
    unsigned char *buf;
    CRA_SERIALIZER_BUF(ser, buf, 1 + sizeof(val));
    *buf = (unsigned char)CRA_TYPE_INT8;
    *(buf + 1) = (unsigned char)val;
    ser->index += 1 + sizeof(val);
    return true;
}

#define CRA_SERIALIZE_NUMBER(_TYPE, _val)           \
    unsigned char *buf;                             \
    CRA_SERIALIZER_BUF(ser, buf, 1 + sizeof(_val)); \
    *buf = (unsigned char)_TYPE;                    \
    memcpy(buf + 1, &(_val), sizeof(_val));         \
    ser->index += 1 + sizeof(_val);                 \
    return true

bool
cra_bin_serialize_int16(CraSerializer *ser, int16_t val)
{
    val = (int16_t)CRA_SER_SWAP16((uint16_t)val);
    CRA_SERIALIZE_NUMBER(CRA_TYPE_INT16, val);
}

bool
cra_bin_serialize_int32(CraSerializer *ser, int32_t val)
{
    val = (int32_t)CRA_SER_SWAP32((uint32_t)val);
    CRA_SERIALIZE_NUMBER(CRA_TYPE_INT32, val);
}

bool
cra_bin_serialize_int64(CraSerializer *ser, int64_t val)
{
    val = (int64_t)CRA_SER_SWAP64((uint64_t)val);
    CRA_SERIALIZE_NUMBER(CRA_TYPE_INT64, val);
}

bool
cra_bin_serialize_uint8(CraSerializer *ser, uint8_t val)
{
    unsigned char *buf;
    CRA_SERIALIZER_BUF(ser, buf, 1 + sizeof(val));
    *buf = (unsigned char)CRA_TYPE_UINT8;
    *(buf + 1) = (unsigned char)val;
    ser->index += 1 + sizeof(val);
    return true;
}

bool
cra_bin_serialize_uint16(CraSerializer *ser, uint16_t val)
{
    val = CRA_SER_SWAP16(val);
    CRA_SERIALIZE_NUMBER(CRA_TYPE_UINT16, val);
}

bool
cra_bin_serialize_uint32(CraSerializer *ser, uint32_t val)
{
    val = CRA_SER_SWAP32(val);
    CRA_SERIALIZE_NUMBER(CRA_TYPE_UINT32, val);
}

bool
cra_bin_serialize_uint64(CraSerializer *ser, uint64_t val)
{
    val = CRA_SER_SWAP64(val);
    CRA_SERIALIZE_NUMBER(CRA_TYPE_UINT64, val);
}

bool
cra_bin_serialize_float(CraSerializer *ser, float val)
{
    if (isnan(val) || isinf(val))
    {
        ser->error = CRA_SER_ERROR_FLOAT_NAN_OR_INF;
        return false;
    }
    CRA_SERIALIZE_NUMBER(CRA_TYPE_FLOAT, val);
}

bool
cra_bin_serialize_double(CraSerializer *ser, double val)
{
    if (isnan(val) || isinf(val))
    {
        ser->error = CRA_SER_ERROR_FLOAT_NAN_OR_INF;
        return false;
    }
    CRA_SERIALIZE_NUMBER(CRA_TYPE_DOUBLE, val);
}

#define CRA_CHECK_WRITE_NULL                 \
    if (val == NULL)                         \
    {                                        \
        CRA_SERIALIZER_BUF(ser, buf, 1);     \
        *buf = (unsigned char)CRA_TYPE_NULL; \
        ++ser->index;                        \
        return true;                         \
    }

bool
cra_bin_serialize_string_nz(CraSerializer *ser, const char *val, cra_ser_count_t length)
{
    unsigned char *buf;

    CRA_CHECK_WRITE_NULL

    CRA_SERIALIZER_BUF(ser, buf, 1 + sizeof(length) + length);

    // write type
    *buf = (unsigned char)CRA_TYPE_STRING;

    // write length
    cra_ser_count_t len = CRA_SER_SWAP_COUNT(length);
    memcpy(buf + 1, &len, sizeof(length));

    // write string
    memcpy(buf + 1 + sizeof(uint32_t), val, length);

    ser->index += 1 + sizeof(uint32_t) + length;

    return true;
}

static bool
cra_bin_serialize_once(CraSerializer *ser, void *val, size_t offset, const CraTypeMeta *meta);
static bool
cra_bin_serialize_all(CraSerializer *ser, void *val, const CraTypeMeta *meta);

#define CRA_SER_NESTING_BEGIN                   \
    if (ser->nested++ >= CRA_SER_MAX_NESTING)   \
    {                                           \
        ser->error = CRA_SER_ERROR_MAX_NESTING; \
        return false;                           \
    }
#define CRA_SER_NESTING_END --ser->nested;

bool
cra_bin_serialize_struct(CraSerializer *ser, void *val, const CraTypeMeta *members_meta)
{
    unsigned char *buf;

    CRA_SER_NESTING_BEGIN

    CRA_CHECK_WRITE_NULL

    // write type
    CRA_SERIALIZER_BUF(ser, buf, 1);
    *buf = (unsigned char)CRA_TYPE_STRUCT;
    ++ser->index;

    // write members
    if (!cra_bin_serialize_all(ser, val, members_meta))
        return false;

    // write end flag
    CRA_SERIALIZER_BUF(ser, buf, 1);
    *buf = (unsigned char)__CRA_TYPE_END;
    ++ser->index;

    CRA_SER_NESTING_END

    return true;
}

static bool
__cra_bin_serialize_list(CraSerializer       *ser,
                         void                *val,
                         const CraTypeMeta   *element_meta,
                         const CraTypeIter_i *iter_i,
                         cra_ser_count_t      count)
{
    unsigned char *buf;

    assert_always((element_meta + 1)->type == __CRA_TYPE_END_OF_META);

    CRA_SER_NESTING_BEGIN

    CRA_CHECK_WRITE_NULL

    // write type & remain count
    CRA_SERIALIZER_BUF(ser, buf, 1 + sizeof(count));
    *buf = (unsigned char)CRA_TYPE_LIST;
    ser->index += 1 + sizeof(count);

    // write elements
    if (iter_i)
    {
        void *item = NULL;
        char  it[CRA_SERI_ITER_SIZE];
        assert_always(iter_i->list.init && iter_i->list.next);
        iter_i->list.init(val, &it, sizeof(it));
        for (count = 0; iter_i->list.next(&it, &item);)
        {
            if (!cra_bin_serialize_once(ser, item, 0, element_meta))
                return false;
            if (++count == CRA_SER_COUNT_MAX)
            {
                ser->error = CRA_SER_ERROR_MAX_ELEMENTS;
                return false;
            }
        }
    }
    else
    {
        size_t slot_size = element_meta->is_ptr ? sizeof(void *) : element_meta->size;
        for (cra_ser_count_t i = 0; i < count; ++i)
        {
            if (!cra_bin_serialize_once(ser, (char *)val + slot_size * i, 0, element_meta))
                return false;
        }
    }

    // write count
    count = CRA_SER_SWAP_COUNT(count);
    memcpy(buf + 1, &count, sizeof(count));

    // write end flag
    CRA_SERIALIZER_BUF(ser, buf, 1);
    *buf = (unsigned char)__CRA_TYPE_END;
    ++ser->index;

    CRA_SER_NESTING_END

    return true;
}

bool
cra_bin_serialize_list(CraSerializer *ser, void *val, const CraTypeMeta *element_meta, const CraTypeIter_i *iter_i)
{
    return __cra_bin_serialize_list(ser, val, element_meta, iter_i, 0);
}

bool
cra_bin_serialize_array(CraSerializer *ser, void *val, cra_ser_count_t count, const CraTypeMeta *element_meta)
{
    return __cra_bin_serialize_list(ser, val, element_meta, NULL, count);
}

bool
cra_bin_serialize_dict(CraSerializer *ser, void *val, const CraTypeMeta *kv_meta, const CraTypeIter_i *iter_i)
{
    unsigned char *buf;

    assert_always((kv_meta + 2)->type == __CRA_TYPE_END_OF_META);
    assert_always(iter_i != NULL && iter_i->dict.init && iter_i->dict.next);

    CRA_SER_NESTING_BEGIN

    CRA_CHECK_WRITE_NULL

    // write type & remain count
    cra_ser_count_t count = 0;
    CRA_SERIALIZER_BUF(ser, buf, 1 + sizeof(count));
    *buf = (unsigned char)CRA_TYPE_DICT;
    ser->index += 1 + sizeof(count);

    // write elements
    void *k = NULL;
    void *v = NULL;
    char  it[CRA_SERI_ITER_SIZE];
    iter_i->dict.init(val, &it, sizeof(it));
    while (iter_i->dict.next(&it, &k, &v))
    {
        if (!cra_bin_serialize_once(ser, k, 0, kv_meta))
            return false;
        if (!cra_bin_serialize_once(ser, v, 0, kv_meta + 1))
            return false;
        if (++count == CRA_SER_COUNT_MAX)
        {
            ser->error = CRA_SER_ERROR_MAX_ELEMENTS;
            return false;
        }
    }

    // write count
    count = CRA_SER_SWAP_COUNT(count);
    memcpy(buf + 1, &count, sizeof(count));

    // write end flag
    CRA_SERIALIZER_BUF(ser, buf, 1);
    *buf = (unsigned char)__CRA_TYPE_END;
    ++ser->index;

    CRA_SER_NESTING_END

    return true;
}

static bool
cra_bin_serialize_once(CraSerializer *ser, void *val, size_t offset, const CraTypeMeta *meta)
{
    void *value = (void *)((char *)val + offset);
    switch (meta->type)
    {
        case CRA_TYPE_FALSE:
        case CRA_TYPE_TRUE:
            if (!cra_bin_serialize_bool(ser, *(bool *)value))
                return false;
            break;
        case CRA_TYPE_INT8:
            if (!cra_bin_serialize_int8(ser, *(int8_t *)value))
                return false;
            break;
        case CRA_TYPE_INT16:
            if (!cra_bin_serialize_int16(ser, *(int16_t *)value))
                return false;
            break;
        case CRA_TYPE_INT32:
            if (!cra_bin_serialize_int32(ser, *(int32_t *)value))
                return false;
            break;
        case CRA_TYPE_INT64:
            if (!cra_bin_serialize_int64(ser, *(int64_t *)value))
                return false;
            break;
        case CRA_TYPE_UINT8:
            if (!cra_bin_serialize_uint8(ser, *(uint8_t *)value))
                return false;
            break;
        case CRA_TYPE_UINT16:
            if (!cra_bin_serialize_uint16(ser, *(uint16_t *)value))
                return false;
            break;
        case CRA_TYPE_UINT32:
            if (!cra_bin_serialize_uint32(ser, *(uint32_t *)value))
                return false;
            break;
        case CRA_TYPE_UINT64:
            if (!cra_bin_serialize_uint64(ser, *(uint64_t *)value))
                return false;
            break;
        case CRA_TYPE_FLOAT:
            if (!cra_bin_serialize_float(ser, *(float *)value))
                return false;
            break;
        case CRA_TYPE_DOUBLE:
            if (!cra_bin_serialize_double(ser, *(double *)value))
                return false;
            break;
        case CRA_TYPE_STRING:
            if (meta->is_ptr)
                value = *(void **)value;
            if (meta->nsize > 0)
            {
                assert_always(meta->nsize == sizeof(cra_ser_count_t));
                cra_ser_count_t len = *(cra_ser_count_t *)((char *)val + meta->noffset);
                if (!cra_bin_serialize_string_nz(ser, (char *)value, len))
                    return false;
            }
            else
            {
                if (!cra_bin_serialize_string(ser, (char *)value))
                    return false;
            }
            break;
        case CRA_TYPE_STRUCT:
            if (meta->is_ptr)
                value = *(void **)value;
            if (!cra_bin_serialize_struct(ser, value, meta->meta))
                return false;
            break;
        case CRA_TYPE_LIST:
            if (meta->is_ptr)
                value = *(void **)value;
            if (!__cra_bin_serialize_list(
                  ser, value, meta->meta, meta->iter_i, *(cra_ser_count_t *)((char *)val + meta->noffset)))
                return false;
            break;
        case CRA_TYPE_DICT:
            if (meta->is_ptr)
                value = *(void **)value;
            if (!cra_bin_serialize_dict(ser, value, meta->meta, meta->iter_i))
                return false;
            break;

        default:
            ser->error = CRA_SER_ERROR_NOT_SUPPORT;
            return false;
    }
    return true;
}

static bool
cra_bin_serialize_all(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    CraTypeMeta *m = (CraTypeMeta *)meta;
    while (m->type != __CRA_TYPE_END_OF_META)
    {
        if (!cra_bin_serialize_once(ser, val, m->offset, m))
            return false;
        m += 1;
    }
    return true;
}

#endif // end serialize

#if 1 // deserialize

void
cra_bin_deserialize_begin(CraSerializer *ser, unsigned char *buffer, size_t buffer_length)
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

void
cra_bin_deserialize_end(CraSerializer *ser, CraSerError_e *error)
{
    assert_always(ser != NULL);
    assert_always(ser->buffer != NULL);
    assert_always(ser->index <= ser->length);

    if (!!error)
        *error = ser->error;
    cra_ser_release_uninit(&ser->release, ser->error != CRA_SER_ERROR_SUCCESS);
}

bool
cra_bin_deserialize_bool(CraSerializer *ser, bool *retval)
{
    unsigned char *buf;
    CRA_SERIALIZER_BUF(ser, buf, sizeof(bool));
    if (*buf != true && *buf != false)
    {
        ser->error = CRA_SER_ERROR_TYPE_MISMATCH;
        return false;
    }
    *retval = *(bool *)buf;
    ++ser->index;
    return true;
}

#define CRA_DESERIALIZE_CHECK_TYPE(_ser, _buf, _TYPE) \
    if (_TYPE != (CraType_e) * (_buf))                \
    {                                                 \
        (_ser)->error = CRA_SER_ERROR_TYPE_MISMATCH;  \
        return false;                                 \
    }

bool
cra_bin_deserialize_int8(CraSerializer *ser, int8_t *retval)
{
    unsigned char *buf;
    CRA_SERIALIZER_BUF(ser, buf, 1 + sizeof(*retval));
    CRA_DESERIALIZE_CHECK_TYPE(ser, buf, CRA_TYPE_INT8)
    *retval = *(int8_t *)(buf + 1);
    ser->index += 1 + sizeof(*retval);
    return true;
}

#define CRA_DESERIALIZE_NUMBER(_TYPE, _retval)            \
    unsigned char *buf;                                   \
    CRA_SERIALIZER_BUF(ser, buf, 1 + sizeof(*(_retval))); \
    CRA_DESERIALIZE_CHECK_TYPE(ser, buf, _TYPE)           \
    memcpy(_retval, buf + 1, sizeof(*(_retval)));         \
    ser->index += 1 + sizeof(*(_retval))

bool
cra_bin_deserialize_int16(CraSerializer *ser, int16_t *retval)
{
    CRA_DESERIALIZE_NUMBER(CRA_TYPE_INT16, retval);
    *retval = (int16_t)CRA_SER_SWAP16((uint16_t)*retval);
    return true;
}

bool
cra_bin_deserialize_int32(CraSerializer *ser, int32_t *retval)
{
    CRA_DESERIALIZE_NUMBER(CRA_TYPE_INT32, retval);
    *retval = (int32_t)CRA_SER_SWAP32((uint32_t)*retval);
    return true;
}

bool
cra_bin_deserialize_int64(CraSerializer *ser, int64_t *retval)
{
    CRA_DESERIALIZE_NUMBER(CRA_TYPE_INT64, retval);
    *retval = (int64_t)CRA_SER_SWAP64((uint64_t)*retval);
    return true;
}

bool
cra_bin_deserialize_uint8(CraSerializer *ser, uint8_t *retval)
{
    unsigned char *buf;
    CRA_SERIALIZER_BUF(ser, buf, 1 + sizeof(*retval));
    CRA_DESERIALIZE_CHECK_TYPE(ser, buf, CRA_TYPE_UINT8)
    *retval = *(uint8_t *)(buf + 1);
    ser->index += 1 + sizeof(*retval);
    return true;
}

bool
cra_bin_deserialize_uint16(CraSerializer *ser, uint16_t *retval)
{
    CRA_DESERIALIZE_NUMBER(CRA_TYPE_UINT16, retval);
    *retval = CRA_SER_SWAP16(*retval);
    return true;
}

bool
cra_bin_deserialize_uint32(CraSerializer *ser, uint32_t *retval)
{
    CRA_DESERIALIZE_NUMBER(CRA_TYPE_UINT32, retval);
    *retval = CRA_SER_SWAP32(*retval);
    return true;
}

bool
cra_bin_deserialize_uint64(CraSerializer *ser, uint64_t *retval)
{
    CRA_DESERIALIZE_NUMBER(CRA_TYPE_UINT64, retval);
    *retval = CRA_SER_SWAP64(*retval);
    return true;
}

bool
cra_bin_deserialize_float(CraSerializer *ser, float *retval)
{
    CRA_DESERIALIZE_NUMBER(CRA_TYPE_FLOAT, retval);
    if (isnan(*retval) || isinf(*retval))
    {
        ser->error = CRA_SER_ERROR_FLOAT_NAN_OR_INF;
        return false;
    }
    return true;
}

bool
cra_bin_deserialize_double(CraSerializer *ser, double *retval)
{
    CRA_DESERIALIZE_NUMBER(CRA_TYPE_DOUBLE, retval);
    if (isnan(*retval) || isinf(*retval))
    {
        ser->error = CRA_SER_ERROR_FLOAT_NAN_OR_INF;
        return false;
    }
    return true;
}

#define CRA_READ_CHECK_NULL(_is_ptr)                   \
    CRA_SERIALIZER_BUF(ser, buf, 1);                   \
    if ((CraType_e) * buf == CRA_TYPE_NULL)            \
    {                                                  \
        if (!_is_ptr)                                  \
        {                                              \
            ser->error = CRA_SER_ERROR_CANNOT_BE_NULL; \
            return false;                              \
        }                                              \
        *(void **)retval = NULL;                       \
        ++ser->index;                                  \
        return true;                                   \
    }

// `retlength`: for string_nz
static bool
__cra_bin_deserialize_string_nz(CraSerializer   *ser,
                                char            *retval,
                                cra_ser_count_t  max_length,
                                bool             is_char_ptr,
                                bool             auto_free_if_fail,
                                cra_ser_count_t *retlength)
{
    cra_ser_count_t len;
    unsigned char  *buf;

    if (retlength)
        *retlength = 0;

    CRA_READ_CHECK_NULL(is_char_ptr)

    CRA_SERIALIZER_BUF(ser, buf, 1 + sizeof(len));

    // read & check type
    CRA_DESERIALIZE_CHECK_TYPE(ser, buf, CRA_TYPE_STRING)

    // read & check length
    memcpy(&len, buf + 1, sizeof(len));
    ser->index += 1 + sizeof(len);
    len = CRA_SER_SWAP_COUNT(len);
    if (len == CRA_SER_COUNT_MAX || len > ser->length - ser->index)
    {
        ser->error = CRA_SER_ERROR_STRING_TOO_LONG;
        return false;
    }

    // read string
    if (is_char_ptr)
    {
        retval = *(char **)retval = cra_malloc(len + ((!retlength || len == 0) ? 1 : 0));
        if (auto_free_if_fail)
            cra_ser_release_add(&ser->release, true, retval, NULL, NULL);
    }
    else
    {
        if (max_length <= len)
        {
            ser->error = CRA_SER_ERROR_STRING_BUF_TOO_SMALL;
            return false;
        }
    }
    if (len > 0)
        memcpy(retval, buf + 1 + sizeof(len), len);
    if (!retlength)
        retval[len] = '\0';
    else
        *retlength = len;
    ser->index += len;

    return true;
}

bool
cra_bin_deserialize_string(CraSerializer  *ser,
                           char           *retval,
                           cra_ser_count_t max_length,
                           bool            is_char_ptr,
                           bool            auto_free_if_fail)
{
    return __cra_bin_deserialize_string_nz(ser, retval, max_length, is_char_ptr, auto_free_if_fail, NULL);
}

bool
cra_bin_deserialize_string_nz(CraSerializer   *ser,
                              char            *retval,
                              cra_ser_count_t *length,
                              bool             is_char_ptr,
                              bool             auto_free_if_fail)
{
    return __cra_bin_deserialize_string_nz(ser, retval, *length, is_char_ptr, auto_free_if_fail, length);
}

static bool
cra_bin_deserialize_once(CraSerializer *ser, void *val, size_t offset, const CraTypeMeta *meta, bool auto_free_if_fail);
static bool
cra_bin_deserialize_all(CraSerializer *ser, void *retval, const CraTypeMeta *meta, bool auto_free_if_fail);

static inline void *
cra_json_alloc_init(CraSerializer       *ser,
                    void                *retval,
                    size_t               valsize,
                    bool                 is_ptr,
                    bool                *auto_free_if_fail,
                    const CraTypeInit_i *init_i,
                    void                *args4init)
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

#define CRA_BUF_HAS_CHAR(_ser)   ((_ser)->index + 1 < (_ser)->length)
#define CRA_BUF_IS_OBJ_END(_ser) ((CraType_e)(*((_ser)->buffer + (_ser)->index)) == __CRA_TYPE_END)

static inline bool
cra_bin_skip_values_and_check_flag(CraSerializer *ser)
{
    while (CRA_BUF_HAS_CHAR(ser) && !CRA_BUF_IS_OBJ_END(ser))
        ++ser->index;
    return CRA_BUF_IS_OBJ_END(ser);
}

bool
cra_bin_deserialize_struct(CraSerializer       *ser,
                           void                *retval,
                           size_t               valsize,
                           bool                 is_ptr,
                           bool                 auto_free_if_fail,
                           const CraTypeMeta   *members_meta,
                           const CraTypeInit_i *init_i,
                           void                *args4init)
{
    unsigned char *buf;

    assert_always(members_meta->type != __CRA_TYPE_END_OF_META);

    CRA_SER_NESTING_BEGIN

    CRA_READ_CHECK_NULL(is_ptr)

    // read & check type
    CRA_DESERIALIZE_CHECK_TYPE(ser, buf, CRA_TYPE_STRUCT)
    ++ser->index;

    // alloc & init
    retval = cra_json_alloc_init(ser, retval, valsize, is_ptr, &auto_free_if_fail, init_i, args4init);

    // read members
    if (!cra_bin_deserialize_all(ser, retval, members_meta, auto_free_if_fail))
        return false;

    // read end flag
    CRA_SERIALIZER_BUF(ser, buf, 1);
    if (__CRA_TYPE_END != (CraType_e)*buf)
    {
        // skip the rest of values
        if (!cra_bin_skip_values_and_check_flag(ser))
        {
            (ser)->error = CRA_SER_ERROR_INVALID_VALUE;
            return false;
        }
    }
    ++ser->index;

    CRA_SER_NESTING_END

    return true;
}

bool
cra_bin_deserialize_list(CraSerializer       *ser,
                         void                *retval,
                         size_t               valsize,
                         bool                 is_ptr,
                         bool                 auto_free_if_fail,
                         const CraTypeMeta   *element_meta,
                         const CraTypeIter_i *iter_i,
                         const CraTypeInit_i *init_i,
                         void                *args4init)
{
    cra_ser_count_t count;
    unsigned char  *buf;

    assert_always((element_meta + 1)->type == __CRA_TYPE_END_OF_META);
    assert_always(iter_i != NULL && iter_i->list.append != NULL);

    CRA_SER_NESTING_BEGIN

    CRA_READ_CHECK_NULL(is_ptr)

    // read & check type
    CRA_DESERIALIZE_CHECK_TYPE(ser, buf, CRA_TYPE_LIST)
    ++ser->index;

    // read count
    CRA_SERIALIZER_BUF(ser, buf, sizeof(count));
    memcpy(&count, buf, sizeof(count));
    count = CRA_SER_SWAP_COUNT(count);
    ser->index += sizeof(count);

    size_t slot_size = element_meta->is_ptr ? sizeof(void *) : element_meta->size;

    // alloc & init
    retval = cra_json_alloc_init(ser, retval, valsize, is_ptr, &auto_free_if_fail, init_i, args4init);

    // read elements
    void *item = cra_malloc(slot_size);
    for (cra_ser_count_t i = 0; i < count; ++i)
    {
        if (!cra_bin_deserialize_once(ser, item, 0, element_meta, auto_free_if_fail))
            goto error_return;
        if (!iter_i->list.append(retval, item))
        {
            ser->error = CRA_SER_ERROR_APPEND_ELEMENT_FAILED;
            goto error_return;
        }
    }

    // read end flag
    CRA_SERIALIZER_BUF(ser, buf, 1);
    if (__CRA_TYPE_END != (CraType_e)*buf)
    {
        (ser)->error = CRA_SER_ERROR_INVALID_VALUE;
        goto error_return;
    }
    ++ser->index;

    CRA_SER_NESTING_END

    if (item != NULL)
        cra_free(item);
    return true;
error_return:
    if (item != NULL)
        cra_free(item);
    return false;
}

bool
cra_bin_deserialize_array(CraSerializer     *ser,
                          void              *retval,
                          size_t             valsize,
                          bool               is_ptr,
                          cra_ser_count_t   *countptr,
                          const CraTypeMeta *element_meta)
{
    cra_ser_count_t count;
    unsigned char  *buf;

    assert_always((element_meta + 1)->type == __CRA_TYPE_END_OF_META);
    assert_always(countptr != NULL);

    CRA_SER_NESTING_BEGIN

    CRA_READ_CHECK_NULL(is_ptr)

    // read & check type
    CRA_DESERIALIZE_CHECK_TYPE(ser, buf, CRA_TYPE_LIST)
    ++ser->index;

    // read count
    CRA_SERIALIZER_BUF(ser, buf, sizeof(count));
    memcpy(&count, buf, sizeof(count));
    count = CRA_SER_SWAP_COUNT(count);
    ser->index += sizeof(count);

    size_t slot_size = element_meta->is_ptr ? sizeof(void *) : element_meta->size;
    size_t list_size = is_ptr ? (slot_size * (count == 0 ? 1 : count)) : valsize;

    // alloc & init
    if (is_ptr)
    {
        retval = *(void **)retval = cra_malloc(list_size);
        cra_ser_release_add(&ser->release, is_ptr, retval, NULL, NULL);
    }
    bzero(retval, list_size);

    // enough?
    if (!is_ptr && valsize / slot_size < count)
    {
        ser->error = CRA_SER_ERROR_ARRAY_TOO_SMALL;
        goto error_return;
    }
    // read elements
    for (cra_ser_count_t i = 0; i < count; ++i)
    {
        if (!cra_bin_deserialize_once(ser, (char *)retval + i * slot_size, 0, element_meta, true))
            goto error_return;
    }
    *countptr = count;

    // read end flag
    CRA_SERIALIZER_BUF(ser, buf, 1);
    if (__CRA_TYPE_END != (CraType_e)*buf)
    {
        (ser)->error = CRA_SER_ERROR_INVALID_VALUE;
        goto error_return;
    }
    ++ser->index;

    CRA_SER_NESTING_END

    return true;
error_return:
    return false;
}

bool
cra_bin_deserialize_dict(CraSerializer       *ser,
                         void                *retval,
                         size_t               valsize,
                         bool                 is_ptr,
                         bool                 auto_free_if_fail,
                         const CraTypeMeta   *kv_meta,
                         const CraTypeIter_i *iter_i,
                         const CraTypeInit_i *init_i,
                         void                *args4init)
{
    cra_ser_count_t count;
    unsigned char  *buf;

    assert_always((kv_meta + 2)->type == __CRA_TYPE_END_OF_META);
    assert_always(iter_i != NULL && iter_i->dict.append != NULL);

    CRA_SER_NESTING_BEGIN

    CRA_READ_CHECK_NULL(is_ptr)

    // read & check type
    CRA_DESERIALIZE_CHECK_TYPE(ser, buf, CRA_TYPE_DICT)
    ++ser->index;

    // read count
    CRA_SERIALIZER_BUF(ser, buf, sizeof(count));
    memcpy(&count, buf, sizeof(count));
    count = CRA_SER_SWAP_COUNT(count);
    ser->index += sizeof(count);

    size_t key_size = kv_meta->is_ptr ? sizeof(void *) : kv_meta->size;
    size_t val_size = (kv_meta + 1)->is_ptr ? sizeof(void *) : (kv_meta + 1)->size;

    // alloc & init
    retval = cra_json_alloc_init(ser, retval, valsize, is_ptr, &auto_free_if_fail, init_i, args4init);

    // read elements
    void *k = cra_malloc(key_size + val_size);
    void *v = (char *)k + key_size;
    for (cra_ser_count_t i = 0; i < count; ++i)
    {
        if (!cra_bin_deserialize_once(ser, k, 0, kv_meta, auto_free_if_fail))
            goto error_return;
        if (!cra_bin_deserialize_once(ser, v, 0, kv_meta + 1, auto_free_if_fail))
            goto error_return;
        if (!iter_i->dict.append(retval, k, v))
        {
            ser->error = CRA_SER_ERROR_APPEND_ELEMENT_FAILED;
            goto error_return;
        }
    }

    // read end flag
    CRA_SERIALIZER_BUF(ser, buf, 1);
    if (__CRA_TYPE_END != (CraType_e)*buf)
    {
        (ser)->error = CRA_SER_ERROR_INVALID_VALUE;
        goto error_return;
    }
    ++ser->index;

    CRA_SER_NESTING_END

    if (k != NULL)
        cra_free(k);
    return true;
error_return:
    if (k != NULL)
        cra_free(k);
    return false;
}

static bool
cra_bin_deserialize_once(CraSerializer *ser, void *val, size_t offset, const CraTypeMeta *meta, bool auto_free_if_fail)
{
    void *value = (void *)((char *)val + offset);
    assert_always(value != NULL);
    switch (meta->type)
    {
        case CRA_TYPE_FALSE:
        case CRA_TYPE_TRUE:
            if (!cra_bin_deserialize_bool(ser, (bool *)value))
                return false;
            break;
        case CRA_TYPE_INT8:
            if (!cra_bin_deserialize_int8(ser, (int8_t *)value))
                return false;
            break;
        case CRA_TYPE_INT16:
            if (!cra_bin_deserialize_int16(ser, (int16_t *)value))
                return false;
            break;
        case CRA_TYPE_INT32:
            if (!cra_bin_deserialize_int32(ser, (int32_t *)value))
                return false;
            break;
        case CRA_TYPE_INT64:
            if (!cra_bin_deserialize_int64(ser, (int64_t *)value))
                return false;
            break;
        case CRA_TYPE_UINT8:
            if (!cra_bin_deserialize_uint8(ser, (uint8_t *)value))
                return false;
            break;
        case CRA_TYPE_UINT16:
            if (!cra_bin_deserialize_uint16(ser, (uint16_t *)value))
                return false;
            break;
        case CRA_TYPE_UINT32:
            if (!cra_bin_deserialize_uint32(ser, (uint32_t *)value))
                return false;
            break;
        case CRA_TYPE_UINT64:
            if (!cra_bin_deserialize_uint64(ser, (uint64_t *)value))
                return false;
            break;
        case CRA_TYPE_FLOAT:
            if (!cra_bin_deserialize_float(ser, (float *)value))
                return false;
            break;
        case CRA_TYPE_DOUBLE:
            if (!cra_bin_deserialize_double(ser, (double *)value))
                return false;
            break;
        case CRA_TYPE_STRING:
            if (meta->nsize == 0)
            {
                if (!cra_bin_deserialize_string(
                      ser, (char *)value, (cra_ser_count_t)meta->size, meta->is_ptr, auto_free_if_fail))
                    return false;
            }
            else
            {
                assert_always(meta->nsize == sizeof(cra_ser_count_t));
                cra_ser_count_t *nsize = (cra_ser_count_t *)((char *)val + meta->noffset);
                *nsize = (cra_ser_count_t)meta->size;
                if (!cra_bin_deserialize_string_nz(ser, (char *)value, nsize, meta->is_ptr, auto_free_if_fail))
                    return false;
            }
            break;
        case CRA_TYPE_STRUCT:
            if (!cra_bin_deserialize_struct(
                  ser, value, meta->size, meta->is_ptr, auto_free_if_fail, meta->meta, meta->init_i, meta->args4init))
                return false;
            break;
        case CRA_TYPE_LIST:
            if (meta->nsize == 0)
            {
                // list
                if (!cra_bin_deserialize_list(ser,
                                              value,
                                              meta->size,
                                              meta->is_ptr,
                                              auto_free_if_fail,
                                              meta->meta,
                                              meta->iter_i,
                                              meta->init_i,
                                              meta->args4init))
                    return false;
            }
            else
            {
                // c array
                assert_always(meta->nsize == sizeof(cra_ser_count_t));
                if (!cra_bin_deserialize_array(ser,
                                               value,
                                               meta->size,
                                               meta->is_ptr,
                                               (cra_ser_count_t *)((char *)val + meta->noffset),
                                               meta->meta))
                    return false;
            }
            break;
        case CRA_TYPE_DICT:
            if (!cra_bin_deserialize_dict(ser,
                                          value,
                                          meta->size,
                                          meta->is_ptr,
                                          auto_free_if_fail,
                                          meta->meta,
                                          meta->iter_i,
                                          meta->init_i,
                                          meta->args4init))
                return false;
            break;

        default:
            ser->error = CRA_SER_ERROR_INVALID_VALUE;
            return false;
    }
    return true;
}

static bool
cra_bin_deserialize_all(CraSerializer *ser, void *val, const CraTypeMeta *meta, bool auto_free_if_fail)
{
    CraTypeMeta *m = (CraTypeMeta *)meta;
    while (m->type != __CRA_TYPE_END_OF_META && (CRA_BUF_HAS_CHAR(ser) && !CRA_BUF_IS_OBJ_END(ser)))
    {
        if (!cra_bin_deserialize_once(ser, val, m->offset, m, auto_free_if_fail))
            return false;
        m += 1;
    }
    return true;
}

#endif // end deserialize

unsigned char *
cra_bin_serialize_struct0(unsigned char     *buffer,
                          size_t            *buffer_length,
                          void              *val,
                          const CraTypeMeta *members_meta,
                          CraSerError_e     *error)
{
    CraSerializer ser;
    cra_bin_serialize_begin(&ser, buffer, *buffer_length);
    cra_bin_serialize_struct(&ser, val, members_meta);
    return cra_bin_serialize_end(&ser, buffer_length, error);
}

void
cra_bin_deserialize_struct0(unsigned char       *buffer,
                            size_t               buffer_length,
                            void                *retval,
                            size_t               valsize,
                            bool                 is_ptr,
                            const CraTypeMeta   *members_meta,
                            const CraTypeInit_i *init_i,
                            void                *args4init,
                            CraSerError_e       *error)
{
    CraSerializer ser;
    cra_bin_deserialize_begin(&ser, buffer, buffer_length);
    cra_bin_deserialize_struct(&ser, retval, valsize, is_ptr, true, members_meta, init_i, args4init);
    cra_bin_deserialize_end(&ser, error);
}

unsigned char *
cra_bin_serialize_list0(unsigned char       *buffer,
                        size_t              *buffer_length,
                        void                *val,
                        const CraTypeMeta   *element_meta,
                        const CraTypeIter_i *iter_i,
                        CraSerError_e       *error)
{
    CraSerializer ser;
    cra_bin_serialize_begin(&ser, buffer, *buffer_length);
    cra_bin_serialize_list(&ser, val, element_meta, iter_i);
    return cra_bin_serialize_end(&ser, buffer_length, error);
}

void
cra_bin_deserialize_list0(unsigned char       *buffer,
                          size_t               buffer_length,
                          void                *retval,
                          size_t               valsize,
                          bool                 is_ptr,
                          const CraTypeMeta   *element_meta,
                          const CraTypeIter_i *iter_i,
                          const CraTypeInit_i *init_i,
                          void                *args4init,
                          CraSerError_e       *error)
{
    CraSerializer ser;
    cra_bin_deserialize_begin(&ser, buffer, buffer_length);
    cra_bin_deserialize_list(&ser, retval, valsize, is_ptr, true, element_meta, iter_i, init_i, args4init);
    cra_bin_deserialize_end(&ser, error);
}

unsigned char *
cra_bin_serialize_array0(unsigned char     *buffer,
                         size_t            *buffer_length,
                         void              *val,
                         cra_ser_count_t    count,
                         const CraTypeMeta *element_meta,
                         CraSerError_e     *error)
{
    CraSerializer ser;
    cra_bin_serialize_begin(&ser, buffer, *buffer_length);
    cra_bin_serialize_array(&ser, val, count, element_meta);
    return cra_bin_serialize_end(&ser, buffer_length, error);
}

void
cra_bin_deserialize_array0(unsigned char     *buffer,
                           size_t             buffer_length,
                           void              *retval,
                           size_t             valsize,
                           bool               is_ptr,
                           cra_ser_count_t   *countptr,
                           const CraTypeMeta *element_meta,
                           CraSerError_e     *error)
{
    CraSerializer ser;
    cra_bin_deserialize_begin(&ser, buffer, buffer_length);
    cra_bin_deserialize_array(&ser, retval, valsize, is_ptr, countptr, element_meta);
    cra_bin_deserialize_end(&ser, error);
}

unsigned char *
cra_bin_serialize_dict0(unsigned char       *buffer,
                        size_t              *buffer_length,
                        void                *val,
                        const CraTypeMeta   *kv_meta,
                        const CraTypeIter_i *iter_i,
                        CraSerError_e       *error)
{
    CraSerializer ser;
    cra_bin_serialize_begin(&ser, buffer, *buffer_length);
    cra_bin_serialize_dict(&ser, val, kv_meta, iter_i);
    return cra_bin_serialize_end(&ser, buffer_length, error);
}

void
cra_bin_deserialize_dict0(unsigned char       *buffer,
                          size_t               buffer_length,
                          void                *retval,
                          size_t               valsize,
                          bool                 is_ptr,
                          const CraTypeMeta   *kv_meta,
                          const CraTypeIter_i *iter_i,
                          const CraTypeInit_i *init_i,
                          void                *args4init,
                          CraSerError_e       *error)
{
    CraSerializer ser;
    cra_bin_deserialize_begin(&ser, buffer, buffer_length);
    cra_bin_deserialize_dict(&ser, retval, valsize, is_ptr, true, kv_meta, iter_i, init_i, args4init);
    cra_bin_deserialize_end(&ser, error);
}

void
cra_bin_serialize_print(const unsigned char *buffer, size_t buffer_length)
{
    int            nesting = 0;
    unsigned char *current = (unsigned char *)buffer;
    unsigned char *end = (unsigned char *)buffer + buffer_length;

    printf("\ntips: T: type. C: count. L: length. K: key. V: value. F: flag.\n");
    printf("buffer length: %zu byte[s].\n", buffer_length);
    while (current < end)
    {
        for (int i = 0; i < nesting; i++)
            printf("   ");

        switch (*current++)
        {
            case CRA_TYPE_FALSE:
            case CRA_TYPE_TRUE:
                printf("[T&V] -------- bool(1): %s\n", *(current - 1) ? "true" : "false");
                break;
            case CRA_TYPE_INT8:
                printf("[T][V] ------- int[8](1 + 1): %d\n", *(int8_t *)current);
                current++;
                break;
            case CRA_TYPE_INT16:
            {
                int16_t i = (int16_t)CRA_SER_SWAP16(*(uint16_t *)current);
                printf("[T][V] ------- int[16](1 + 2): %d\n", i);
                current += sizeof(int16_t);
                break;
            }
            case CRA_TYPE_INT32:
            {
                int32_t i = (int32_t)CRA_SER_SWAP32(*(uint32_t *)current);
                printf("[T][V] ------- int[32](1 + 4): %d\n", i);
                current += sizeof(int32_t);
                break;
            }
            case CRA_TYPE_INT64:
            {
                int64_t i = (int64_t)CRA_SER_SWAP64(*(uint64_t *)current);
                printf("[T][V] ------- int[64](1 + 8): %zd\n", i);
                current += sizeof(int64_t);
                break;
            }
            case CRA_TYPE_UINT8:
                printf("[T][V] ------- uint[8](1 + 1): %u\n", *(uint8_t *)current);
                current++;
                break;
            case CRA_TYPE_UINT16:
            {
                uint16_t i = CRA_SER_SWAP16(*(uint16_t *)current);
                printf("[T][V] ------- uint[16](1 + 2): %u\n", i);
                current += sizeof(uint16_t);
                break;
            }
            case CRA_TYPE_UINT32:
            {
                uint32_t i = CRA_SER_SWAP32(*(uint32_t *)current);
                printf("[T][V] ------- uint[32](1 + 4): %u\n", i);
                current += sizeof(uint32_t);
                break;
            }
            case CRA_TYPE_UINT64:
            {
                uint64_t i = CRA_SER_SWAP64(*(uint64_t *)current);
                printf("[T][V] ------- uint[64](1 + 8): %zu\n", i);
                current += sizeof(uint64_t);
                break;
            }
            case CRA_TYPE_FLOAT:
            {
                float f = *(float *)current;
                printf("[T][V] ------- float(1 + 4): %g\n", f);
                current += sizeof(float);
                break;
            }
            case CRA_TYPE_DOUBLE:
            {
                double d = *(double *)current;
                printf("[T][V] ------- double(1 + 8): %g\n", d);
                current += sizeof(double);
                break;
            }
            case CRA_TYPE_STRING:
            {
                cra_ser_count_t len = CRA_SER_SWAP_COUNT(*(cra_ser_count_t *)current);
                current += sizeof(len);
                printf("[T][L][V] ---- string(1 + 4 + %u): \"", len);
                for (cra_ser_count_t i = 0; i < len; i++)
                    putc(*current++, stdout);
                printf("\"\n");
                break;
            }
            case CRA_TYPE_STRUCT:
            {
                nesting++;
                printf("[T][V][F] ---- struct(1 + N + 1):\n");
                break;
            }
            case CRA_TYPE_LIST:
            {
                nesting++;
                cra_ser_count_t count = CRA_SER_SWAP_COUNT(*(cra_ser_count_t *)current);
                current += sizeof(count);
                printf("[T][C][V][F] - list(1 + 4 + N * %u + 1):\n", count);
                break;
            }
            case CRA_TYPE_DICT:
            {
                nesting++;
                cra_ser_count_t count = CRA_SER_SWAP_COUNT(*(cra_ser_count_t *)current);
                current += sizeof(count);
                printf("[T][C][KV][F]  dict(1 + 4 + N * %u + 1):\n", count);
                break;
            }
            case __CRA_TYPE_END:
                nesting--;
                printf("\r");
                break;

            case CRA_TYPE_NULL:
                printf("[T&V] -------- pointer(1): null\n");
                break;

            default:
                printf("invalid type: %d\n", *current);
        }
    }
    printf("\n");
}
