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

#define CRA_SER_MAKE_TYPE(_TYPE, _low) (((uint8_t)(_TYPE) << 4) | ((uint8_t)(_low) & 0x0f))
#define CRA_SER_GET_TYPE(_byte)        (CraType_e)((_byte) >> 4)
#define CRA_SER_GET_LOW(_byte)         (uint8_t)((_byte) & 0x0f)
#define CRA_SER_STRUCT_FLAG            0xd

#define CRA_SER_PTR_NULL_AND_RETURN(_val, _is_ptr, _buf)       \
    do                                                         \
    {                                                          \
        /* is ptr? */                                          \
        if (_is_ptr)                                           \
        {                                                      \
            _val = *(char **)(_val);                           \
            /* is null? */                                     \
            if (!(_val))                                       \
            {                                                  \
                *(_buf) = CRA_SER_MAKE_TYPE(CRA_TYPE_NULL, 0); \
                return true;                                   \
            }                                                  \
        }                                                      \
    } while (0)

#define CRA_SER_CHECK_TYPE_AND_NULL(_ser, _buf, _meta, _retval)   \
    do                                                            \
    {                                                             \
        CRA_SERIALIZER_ENOUGH_AND_RETURN(_ser, _buf, 1);          \
        /* check type */                                          \
        if (*(_buf) != CRA_SER_MAKE_TYPE((_meta)->type, 0))       \
        {                                                         \
            /* is null? */                                        \
            if (*(_buf) == CRA_SER_MAKE_TYPE(CRA_TYPE_NULL, 0))   \
            {                                                     \
                if ((_meta)->is_ptr)                              \
                {                                                 \
                    *(void **)(_retval) = NULL;                   \
                    return true;                                  \
                }                                                 \
                else                                              \
                {                                                 \
                    (_ser)->error = CRA_SER_ERROR_CANNOT_BE_NULL; \
                    return false;                                 \
                }                                                 \
            }                                                     \
            (_ser)->error = CRA_SER_ERROR_TYPE_MISMATCH;          \
            return false;                                         \
        }                                                         \
    } while (0)

bool
cra_bin_serialize_begin(CraSerializer *ser, unsigned char *buffer, size_t buffsize, uint32_t code)
{
    bzero(ser, sizeof(*ser));
    if (buffsize > sizeof(code))
    {
        ser->code = code;
        ser->noalloc = !!buffer;
        ser->length = buffsize;
        ser->buffer = !!buffer ? buffer : (unsigned char *)cra_malloc(ser->length);
        // write code
        ser->index = sizeof(code);
        code = CRA_SER_SWAP32(code);
        memcpy(ser->buffer, &code, sizeof(code));
        return true;
    }
    else
    {
        ser->error = CRA_SER_ERROR_NOBUF;
        return false;
    }
}

unsigned char *
cra_bin_serialize_end(CraSerializer *ser, size_t *retbuffsize, CraSerError_e *reterror)
{
    assert(retbuffsize);
    assert(ser->index <= ser->length);
    if (reterror)
        *reterror = ser->error;
    if (!ser->error)
    {
        *retbuffsize = ser->index;
        return ser->buffer;
    }
    else
    {
        if (!ser->noalloc)
            cra_free(ser->buffer);
        return NULL;
    }
}

bool
cra_bin_deserialize_begin(CraSerializer *ser, unsigned char *buffer, size_t buffsize, uint32_t code)
{
    assert(buffer);

    bzero(ser, sizeof(*ser));
    if (buffsize > sizeof(code))
    {
        ser->buffer = buffer;
        ser->length = buffsize;
        ser->code = code;
        // read code
        memcpy(&code, ser->buffer, sizeof(code));
        code = CRA_SER_SWAP32(code);
        ser->index = sizeof(code);
        if (ser->code == code)
        {
            cra_ser_release_init(&ser->release);
            return true;
        }
        else
        {
            ser->error = CRA_SER_ERROR_CODE_MISMATCH;
            return false;
        }
    }
    else
    {
        ser->error = CRA_SER_ERROR_NOBUF;
        return false;
    }
}

bool
cra_bin_deserialize_end(CraSerializer *ser, CraSerError_e *reterror)
{
    if (reterror)
        *reterror = ser->error;
    cra_ser_release_uninit(&ser->release, !!ser->error);
    if (!ser->error)
        return true;
    return false;
}

bool
cra_bin_serialize_bool(CraSerializer *ser, bool val)
{
    unsigned char *buf;
    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, 1);
    *buf = CRA_SER_MAKE_TYPE(CRA_TYPE_BOOL, !!val);
    return true;
}

bool
cra_bin_deserialize_bool(CraSerializer *ser, bool *retval)
{
    assert(retval);
    unsigned char *buf;
    CRA_SERIALIZER_ENOUGH_AND_RETURN(ser, buf, 1);
    if (CRA_SER_GET_TYPE(*buf) == CRA_TYPE_BOOL)
    {
        *retval = CRA_SER_GET_LOW(*buf);
        return true;
    }
    ser->error = CRA_SER_ERROR_TYPE_MISMATCH;
    return false;
}

static inline bool
__cra_bin_serialize_uint(CraSerializer *ser, void *val, size_t size, CraType_e type)
{
    uint16_t       u16;
    uint32_t       u32;
    uint64_t       u64;
    unsigned char *buf;

    assert(size == 1 || size == 2 || size == 4 || size == 8);

    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, 1 + size);
    *buf = CRA_SER_MAKE_TYPE(type, size);
    ++buf;
    switch (size)
    {
        case sizeof(uint8_t):
            *buf = *(uint8_t *)val;
            break;
        case sizeof(uint16_t):
            u16 = CRA_SER_SWAP16(*(uint16_t *)val);
            memcpy(buf, &u16, sizeof(u16));
            break;
        case sizeof(uint32_t):
            u32 = CRA_SER_SWAP32(*(uint32_t *)val);
            memcpy(buf, &u32, sizeof(u32));
            break;
        case sizeof(uint64_t):
            u64 = CRA_SER_SWAP64(*(uint64_t *)val);
            memcpy(buf, &u64, sizeof(u64));
            break;
        default:
            ser->error = CRA_SER_ERROR_INVALID_SIZE;
            return false;
    }
    return true;
}

static inline bool
__cra_bin_deserialize_uint(CraSerializer *ser, void *retval, size_t size, CraType_e type)
{
    uint16_t       u16;
    uint32_t       u32;
    uint64_t       u64;
    unsigned char *buf;

    assert(retval);
    assert(size == 1 || size == 2 || size == 4 || size == 8);

    CRA_SERIALIZER_ENOUGH_AND_RETURN(ser, buf, 1 + size);
    if (CRA_SER_GET_TYPE(*buf) != type)
    {
        ser->error = CRA_SER_ERROR_TYPE_MISMATCH;
        return false;
    }
    if (CRA_SER_GET_LOW(*buf) != size)
    {
        ser->error = CRA_SER_ERROR_SIZE_MISMATCH;
        return false;
    }

    ++buf;
    switch (size)
    {
        case sizeof(uint8_t):
            *(uint8_t *)retval = *buf;
            break;
        case sizeof(uint16_t):
            memcpy(&u16, buf, sizeof(u16));
            *(uint16_t *)retval = CRA_SER_SWAP16(u16);
            break;
        case sizeof(uint32_t):
            memcpy(&u32, buf, sizeof(u32));
            *(uint32_t *)retval = CRA_SER_SWAP32(u32);
            break;
        case sizeof(uint64_t):
            memcpy(&u64, buf, sizeof(u64));
            *(uint64_t *)retval = CRA_SER_SWAP64(u64);
            break;
        default:
            ser->error = CRA_SER_ERROR_INVALID_SIZE;
            return false;
    }
    return true;
}

bool
cra_bin_serialize_int(CraSerializer *ser, void *val, size_t size)
{
    return __cra_bin_serialize_uint(ser, val, size, CRA_TYPE_INT);
}

bool
cra_bin_deserialize_int(CraSerializer *ser, void *retval, size_t size)
{
    return __cra_bin_deserialize_uint(ser, retval, size, CRA_TYPE_INT);
}

bool
cra_bin_serialize_uint(CraSerializer *ser, void *val, size_t size)
{
    return __cra_bin_serialize_uint(ser, val, size, CRA_TYPE_UINT);
}

bool
cra_bin_deserialize_uint(CraSerializer *ser, void *retval, size_t size)
{
    return __cra_bin_deserialize_uint(ser, retval, size, CRA_TYPE_UINT);
}

static bool
__cra_bin_serialize_varuint__(CraSerializer *ser, uint64_t val)
{
    unsigned char *buf;
    while (true)
    {
        CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, 1);
        if (val > 0x7f)
        {
            *buf = (uint8_t)val | 0x80;
            val >>= 7;
        }
        else
        {
            *buf = (uint8_t)val;
            return true;
        }
    }
}

static bool
__cra_bin_deserialize_varuint__(CraSerializer *ser, uint64_t *retval)
{
    unsigned char *buf;
    uint8_t        shift;

    shift = 0;
    *retval = 0;
    while (shift < 64)
    {
        CRA_SERIALIZER_ENOUGH_AND_RETURN(ser, buf, 1);
        if (*buf & 0x80)
        {
            *retval |= (uint64_t)(*buf & 0x7f) << shift;
            shift += 7;
        }
        else
        {
            *retval |= (uint64_t)*buf << shift;
            return true;
        }
    }
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    return false;
}

static inline bool
__cra_bin_serialize_varuint(CraSerializer *ser, uint64_t val, CraType_e type)
{
    size_t         idx;
    unsigned char *buf;

    // write type
    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, 1);
    *buf = CRA_SER_MAKE_TYPE(type, 0);
    // write value
    idx = ser->index;
    if (__cra_bin_serialize_varuint__(ser, val))
    {
        *buf |= (uint8_t)(ser->index - idx); // save size  [T:4,S:4]
        return true;
    }
    return false;
}

static inline bool
__cra_bin_deserialize_varuint(CraSerializer *ser, uint64_t *retval, CraType_e type)
{
    unsigned char *buf;

    assert(retval);

    // read type
    CRA_SERIALIZER_ENOUGH_AND_RETURN(ser, buf, 1);
    // check type
    if (CRA_SER_GET_TYPE(*buf) != type)
    {
        ser->error = CRA_SER_ERROR_TYPE_MISMATCH;
        return false;
    }
    // check size
    if (CRA_SER_GET_LOW(*buf) > ser->length - ser->index)
    {
        ser->error = CRA_SER_ERROR_INVALID_SIZE;
        return false;
    }
    // read value
    return __cra_bin_deserialize_varuint__(ser, retval);
}

static inline uint64_t
__cra_bin_serialize_zigzag(int64_t val)
{
    return (val << 1) ^ (val >> 63);
}

static inline int64_t
__cra_bin_deserialize_zigzag(uint64_t val)
{
    return (val >> 1) ^ -((int64_t)val & 1);
}

bool
cra_bin_serialize_varint(CraSerializer *ser, void *val, size_t size)
{
    uint64_t u = __cra_bin_serialize_zigzag(__cra_ptr_to_i64(val, size));
    return __cra_bin_serialize_varuint(ser, u, CRA_TYPE_VARINT);
}

bool
cra_bin_deserialize_varint(CraSerializer *ser, void *retval, size_t size)
{
    int64_t  i;
    uint64_t u;
    if (__cra_bin_deserialize_varuint(ser, &u, CRA_TYPE_VARINT))
    {
        i = __cra_bin_deserialize_zigzag(u);
        __cra_i64_to_ptr(i, retval, size);
        return true;
    }
    return false;
}

bool
cra_bin_serialize_varuint(CraSerializer *ser, void *val, size_t size)
{
    uint64_t u = __cra_ptr_to_u64(val, size);
    return __cra_bin_serialize_varuint(ser, u, CRA_TYPE_VARUINT);
}

bool
cra_bin_deserialize_varuint(CraSerializer *ser, void *retval, size_t size)
{
    uint64_t u;
    if (__cra_bin_deserialize_varuint(ser, &u, CRA_TYPE_VARUINT))
    {
        __cra_u64_to_ptr(u, retval, size);
        return true;
    }
    return false;
}

bool
cra_bin_serialize_float(CraSerializer *ser, void *val, size_t size)
{
    union
    {
        float    f;
        uint32_t u;
    } uf;
    union
    {
        double   d;
        uint64_t u;
    } ud;
    unsigned char *buf;

    assert(size == 4 || size == 8);

    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, 1 + size);
    // write type
    *buf++ = CRA_SER_MAKE_TYPE(CRA_TYPE_FLOAT, size);
    // write value
    switch (size)
    {
        case sizeof(float):
            uf.f = CRA_SER_SWAP32(*(float *)val);
            memcpy(buf, &uf.u, sizeof(uf.f));
            break;
        case sizeof(double):
            ud.d = CRA_SER_SWAP64(*(double *)val);
            memcpy(buf, &ud.u, sizeof(ud.d));
            break;
        default:
            ser->error = CRA_SER_ERROR_INVALID_SIZE;
            return false;
    }
    return true;
}

bool
cra_bin_deserialize_float(CraSerializer *ser, void *retval, size_t size)
{
    union
    {
        float    f;
        uint32_t u;
    } uf;
    union
    {
        double   d;
        uint64_t u;
    } ud;
    unsigned char *buf;

    assert(retval);
    assert(size == 4 || size == 8);

    CRA_SERIALIZER_ENOUGH_AND_RETURN(ser, buf, 1 + size);
    // check type
    if (CRA_SER_GET_TYPE(*buf) != CRA_TYPE_FLOAT)
    {
        ser->error = CRA_SER_ERROR_TYPE_MISMATCH;
        return false;
    }
    // check size
    if (CRA_SER_GET_LOW(*buf) != size)
    {
        ser->error = CRA_SER_ERROR_SIZE_MISMATCH;
        return false;
    }
    // read val
    ++buf;
    switch (size)
    {
        case sizeof(float):
            memcpy(&uf.u, buf, sizeof(uf.u));
            uf.u = CRA_SER_SWAP32(uf.u);
            *(float *)retval = uf.f;
            break;
        case sizeof(double):
            memcpy(&ud.u, buf, sizeof(ud.u));
            ud.u = CRA_SER_SWAP64(ud.u);
            *(double *)retval = ud.d;
            break;
        default:
            ser->error = CRA_SER_ERROR_INVALID_SIZE;
            return false;
    }
    return true;
}

static bool
__cra_bin_serialize_string(CraSerializer *ser, const char *val, const CraTypeMeta *meta)
{
    unsigned char *buf;
    size_t         length;

    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, 1);

    CRA_SER_PTR_NULL_AND_RETURN(val, meta->is_ptr, buf);

    // write type
    *buf = CRA_SER_MAKE_TYPE(CRA_TYPE_STRING, 0);
    // write length
    length = strnlen(val, SSIZE_MAX);
    if (!__cra_bin_serialize_varuint__(ser, (uint64_t)length))
    {
        ser->error = CRA_SER_ERROR_STRING_LENGTH;
        return false;
    }
    // write string
    if (length > 0)
    {
        CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, length);
        memcpy(buf, val, length);
    }
    return true;
}

static bool
__cra_bin_deserialize_string(CraSerializer *ser, char *retval, const CraTypeMeta *meta)
{
    unsigned char *buf;
    uint64_t       len;

    assert(retval);

    CRA_SER_CHECK_TYPE_AND_NULL(ser, buf, meta, retval);

    // read length
    if (!__cra_bin_deserialize_varuint__(ser, &len) || len > ser->length - ser->index)
    {
        ser->error = CRA_SER_ERROR_STRING_LENGTH;
        return false;
    }
    if (meta->is_ptr)
    {
        retval = *(char **)retval = (char *)cra_malloc(len + 1); // more 1 char
        cra_ser_release_add(&ser->release, retval, NULL, cra_free);
    }
    // char array enough?
    else if (len + 1 > meta->size)
    {
        ser->error = CRA_SER_ERROR_CHARARR_TOO_SMALL;
        return false;
    }
    // read & copy string
    if (len > 0)
    {
        buf = CRA_SERIALIZER_GET_BUF(ser);
        memcpy(retval, buf, len);
        ser->index += len;
    }
    retval[len] = '\0';

    return true;
}

bool
cra_bin_serialize_string(CraSerializer *ser, const char *val)
{
    static const CraTypeMeta meta = { false, CRA_TYPE_STRING, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    return __cra_bin_serialize_string(ser, val, &meta);
}

bool
cra_bin_deserialize_string(CraSerializer *ser, char *retval, size_t max_chars, bool is_ptr)
{
    assert((max_chars > 0 && !is_ptr) || (max_chars == 0 && is_ptr));
    const CraTypeMeta meta = { is_ptr, CRA_TYPE_STRUCT, 0, max_chars, 0, 0, 0, 0, 0, 0, 0 };
    return __cra_bin_deserialize_string(ser, retval, &meta);
}

static bool
__cra_bin_serialize_bytes(CraSerializer *ser, const char *val, uint64_t inlength, const CraTypeMeta *meta)
{
    unsigned char *buf;
    uint64_t       length;
    const char    *origin;

    origin = val;

    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, 1);

    CRA_SER_PTR_NULL_AND_RETURN(val, meta->is_ptr, buf);

    // write type
    *buf = CRA_SER_MAKE_TYPE(CRA_TYPE_BYTES, 0);
    // write length
    length = __cra_get_n((void *)origin, (void *)val, meta, inlength);
    if (!__cra_bin_serialize_varuint__(ser, length))
    {
        ser->error = CRA_SER_ERROR_BYTES_LENGTH;
        return false;
    }
    // write bytes
    if (length > 0)
    {
        CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, length);
        memcpy(buf, val, length);
    }
    return true;
}

static bool
__cra_bin_deserialize_bytes(CraSerializer *ser, char *retval, uint64_t *outlength, const CraTypeMeta *meta)
{
    unsigned char *buf;
    uint64_t       len;
    char          *origin;

    assert(retval);

    origin = retval;

    CRA_SER_CHECK_TYPE_AND_NULL(ser, buf, meta, retval);

    // read length
    if (!__cra_bin_deserialize_varuint__(ser, &len) || len > ser->length - ser->index)
    {
        ser->error = CRA_SER_ERROR_BYTES_LENGTH;
        return false;
    }
    if (meta->is_ptr)
    {
        retval = *(char **)retval = (char *)cra_malloc(len > 0 ? len : 1);
        cra_ser_release_add(&ser->release, retval, NULL, cra_free);
    }
    // char array enough?
    else if (len > meta->size)
    {
        ser->error = CRA_SER_ERROR_CHARARR_TOO_SMALL;
        return false;
    }
    // read & copy bytes
    if (len > 0)
    {
        buf = CRA_SERIALIZER_GET_BUF(ser);
        memcpy(retval, buf, len);
        ser->index += len;
    }
    __cra_set_n(origin, meta, len, outlength);

    return true;
}

bool
cra_bin_serialize_bytes(CraSerializer *ser, const char *val, uint64_t length)
{
    static const CraTypeMeta meta = { false, CRA_TYPE_BYTES, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    return __cra_bin_serialize_bytes(ser, val, length, &meta);
}

bool
cra_bin_deserialize_bytes(CraSerializer *ser, char *retval, size_t max_chars, bool is_ptr, uint64_t *retlength)
{
    assert((max_chars > 0 && !is_ptr) || (max_chars == 0 && is_ptr));
    const CraTypeMeta meta = { is_ptr, CRA_TYPE_STRUCT, 0, max_chars, 0, 0, 0, 0, 0, 0, 0 };
    return __cra_bin_deserialize_bytes(ser, retval, retlength, &meta);
}

static bool
__cra_bin_serialize_struct(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
__cra_bin_deserialize_struct(CraSerializer *ser, void *retval, const CraTypeMeta *meta);
static bool
__cra_bin_serialize_list(CraSerializer *ser, void *val, uint64_t incount, const CraTypeMeta *meta);
static bool
__cra_bin_deserialize_list(CraSerializer *ser, void *retval, uint64_t *outcount, const CraTypeMeta *meta);
static bool
__cra_bin_serialize_dict(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
__cra_bin_deserialize_dict(CraSerializer *ser, void *retval, const CraTypeMeta *meta);

static bool
__cra_bin_serialize_value(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    assert(val);
    assert(meta);

    switch (meta->type)
    {
        case CRA_TYPE_BOOL:
            assert(meta->size == sizeof(bool));
            return cra_bin_serialize_bool(ser, *(bool *)val);
        case CRA_TYPE_INT:
            return cra_bin_serialize_int(ser, val, meta->size);
        case CRA_TYPE_UINT:
            return cra_bin_serialize_uint(ser, val, meta->size);
        case CRA_TYPE_VARINT:
            return cra_bin_serialize_varint(ser, val, meta->size);
        case CRA_TYPE_VARUINT:
            return cra_bin_serialize_varuint(ser, val, meta->size);
        case CRA_TYPE_FLOAT:
            return cra_bin_serialize_float(ser, val, meta->size);
        case CRA_TYPE_STRING:
            return __cra_bin_serialize_string(ser, (char *)val, meta);
        case CRA_TYPE_BYTES:
            return __cra_bin_serialize_bytes(ser, (char *)val, 0, meta);
        case CRA_TYPE_STRUCT:
            return __cra_bin_serialize_struct(ser, val, meta);
        case CRA_TYPE_LIST:
            return __cra_bin_serialize_list(ser, val, 0, meta);
        case CRA_TYPE_DICT:
            return __cra_bin_serialize_dict(ser, val, meta);

        default:
            ser->error = CRA_SER_ERROR_INVALID_TYPE;
            return false;
    }
}

static bool
__cra_bin_deserialize_value(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    assert(retval);
    assert(meta);

    switch (meta->type)
    {
        case CRA_TYPE_BOOL:
            assert(meta->size == sizeof(bool));
            return cra_bin_deserialize_bool(ser, (bool *)retval);
        case CRA_TYPE_INT:
            return cra_bin_deserialize_int(ser, retval, meta->size);
        case CRA_TYPE_UINT:
            return cra_bin_deserialize_uint(ser, retval, meta->size);
        case CRA_TYPE_VARINT:
            return cra_bin_deserialize_varint(ser, retval, meta->size);
        case CRA_TYPE_VARUINT:
            return cra_bin_deserialize_varuint(ser, retval, meta->size);
        case CRA_TYPE_FLOAT:
            return cra_bin_deserialize_float(ser, retval, meta->size);
        case CRA_TYPE_STRING:
            return __cra_bin_deserialize_string(ser, retval, meta);
        case CRA_TYPE_BYTES:
            return __cra_bin_deserialize_bytes(ser, (char *)retval, NULL, meta);
        case CRA_TYPE_STRUCT:
            return __cra_bin_deserialize_struct(ser, retval, meta);
        case CRA_TYPE_LIST:
            return __cra_bin_deserialize_list(ser, retval, NULL, meta);
        case CRA_TYPE_DICT:
            return __cra_bin_deserialize_dict(ser, retval, meta);

        default:
            ser->error = CRA_SER_ERROR_INVALID_TYPE;
            return false;
    }
}

static bool
__cra_bin_serialize_struct(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    unsigned char *buf;

    assert(val);
    assert(meta->submeta);

    CRA_SER_NESTING_INC_AND_CHECK(ser);

    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, 1);

    CRA_SER_PTR_NULL_AND_RETURN(val, meta->is_ptr, buf);

    // write type
    *buf = CRA_SER_MAKE_TYPE(CRA_TYPE_STRUCT, 0);
    // write members
    for (CraTypeMeta *m = (CraTypeMeta *)meta->submeta; !!m->type; ++m)
    {
        if (!__cra_bin_serialize_value(ser, (char *)val + m->offset, m))
            return false;
    }
    // write struct end flag
    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, 1);
    *buf = CRA_SER_MAKE_TYPE(CRA_TYPE_STRUCT, CRA_SER_STRUCT_FLAG);

    CRA_SER_NESTING_DEC(ser);

    return true;
}

static bool
__cra_bin_deserialize_struct(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    unsigned char *buf;

    assert(retval);
    assert(meta->submeta);
    assert(meta->size >= meta->submeta->size);
    assert(!meta->dzer_i || (meta->dzer_i->init1 && !meta->dzer_i->append1));

    CRA_SER_NESTING_INC_AND_CHECK(ser);

    CRA_SER_CHECK_TYPE_AND_NULL(ser, buf, meta, retval);

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
    // read members
    for (CraTypeMeta *m = (CraTypeMeta *)meta->submeta; CRA_SERIALIZER_ENOUGH(ser, 1) && !!m->type; ++m)
    {
        buf = CRA_SERIALIZER_GET_BUF(ser);
        if (*buf == CRA_SER_MAKE_TYPE(CRA_TYPE_STRUCT, CRA_SER_STRUCT_FLAG))
            goto success;
        if (!__cra_bin_deserialize_value(ser, (char *)retval + m->offset, m))
            return false;
        assert(m->offset < meta->size);
    }
    // check struct end flag
    while (CRA_SERIALIZER_ENOUGH(ser, 1))
    {
        // struct end
        buf = CRA_SERIALIZER_GET_BUF(ser);
        if (*buf == CRA_SER_MAKE_TYPE(CRA_TYPE_STRUCT, CRA_SER_STRUCT_FLAG))
        {
        success:
            CRA_SER_NESTING_DEC(ser);
            ++ser->index; // skip 'end flag'
            return true;
        }
        // skip values
        ++ser->index;
    }
    ser->error = CRA_SER_ERROR_INVALID_VALUE;
    return false;
}

static bool
__cra_bin_serialize_list(CraSerializer *ser, void *val, uint64_t incount, const CraTypeMeta *meta)
{
    unsigned char *buf;
    uint64_t       count;
    void          *origin;

    assert(val);
    assert(meta->submeta);
    assert(meta->submeta[0].type != 0 && meta->submeta[1].type == 0); // element's meta必须只有一项

    origin = val;

    CRA_SER_NESTING_INC_AND_CHECK(ser);

    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, 1);

    CRA_SER_PTR_NULL_AND_RETURN(val, meta->is_ptr, buf);

    // write type
    *buf = CRA_SER_MAKE_TYPE(CRA_TYPE_LIST, 0);
    // get & write count
    count = __cra_get_n(origin, val, meta, incount);
    if (!__cra_bin_serialize_varuint__(ser, count))
    {
        ser->error = CRA_SER_ERROR_LIST_COUNT;
        return false;
    }
    // write elements
    if (meta->szer_i)
    {
        // list

        assert(meta->szer_i->iter_init && meta->szer_i->iter_next1);

        char  itbuf[64];
        void *elementptr;
        for (meta->szer_i->iter_init(val, itbuf, sizeof(itbuf)); meta->szer_i->iter_next1(itbuf, &elementptr);)
        {
            if (!__cra_bin_serialize_value(ser, elementptr, meta->submeta))
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
        for (size_t i = 0; i < count; ++i)
        {
            if (!__cra_bin_serialize_value(ser, (char *)val + slot_size * i, meta->submeta))
                return false;
        }
    }

    CRA_SER_NESTING_DEC(ser);

    return true;
}

static bool
__cra_bin_deserialize_list(CraSerializer *ser, void *retval, uint64_t *outcount, const CraTypeMeta *meta)
{
    unsigned char *buf;
    uint64_t       count;
    void          *origin;
    size_t         obj_size;
    size_t         slot_size;

    assert(retval);
    assert(meta->submeta);
    assert(meta->size > 0);
    assert(meta->submeta[0].type != 0 && meta->submeta[1].type == 0); // element's meta必须只有一项
    assert((meta->nsize > 0 && !meta->dzer_i) ||
           (meta->dzer_i && meta->dzer_i->init1 && meta->dzer_i->uninit && meta->dzer_i->append1));

    CRA_SER_NESTING_INC_AND_CHECK(ser);

    CRA_SER_CHECK_TYPE_AND_NULL(ser, buf, meta, retval);

    // read count
    if (!__cra_bin_deserialize_varuint__(ser, &count))
    {
        ser->error = CRA_SER_ERROR_LIST_COUNT;
        return false;
    }
    origin = retval;
    slot_size = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;
    if (meta->is_ptr)
    {
        obj_size = meta->dzer_i ? meta->size : slot_size * (count > 0 ? count : 1);
        retval = *(void **)retval = cra_malloc(obj_size);
    }
    else
    {
        obj_size = meta->size;
        // check array enough
        if (!meta->dzer_i && (count > obj_size / slot_size))
        {
            ser->error = CRA_SER_ERROR_ARRAY_TOO_SMALL;
            return false;
        }
    }
    // init
    if (meta->dzer_i)
        meta->dzer_i->init1(retval, (size_t)count, slot_size, meta->arg4dzer);
    else
        bzero(retval, obj_size);
    if (meta->is_ptr || meta->dzer_i)
    {
        cra_ser_release_add(
          &ser->release, retval, meta->dzer_i ? meta->dzer_i->uninit : NULL, meta->is_ptr ? cra_free : NULL);
    }

    // read elements
    char *element = (char *)cra_malloc(slot_size);
    for (uint64_t i = 0; i < count; ++i)
    {
        // read one element
        if (!__cra_bin_deserialize_value(ser, element, meta->submeta))
        {
        fail:
            cra_free(element);
            return false;
        }
        // list
        if (meta->dzer_i)
        {
            if (!meta->dzer_i->append1(retval, element))
            {
                ser->error = CRA_SER_ERROR_APPEND_FAILED;
                goto fail;
            }
        }
        // array
        else
        {
            memcpy((char *)retval + slot_size * i, element, slot_size);
        }
    }
    cra_free(element);

    // copy array count
    if (meta->nsize > 0)
        __cra_set_n(origin, meta, count, outcount);

    CRA_SER_NESTING_DEC(ser);

    return true;
}

static bool
__cra_bin_serialize_dict(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    unsigned char *buf;
    size_t         count;

    assert(val);
    assert(meta->submeta);
    // element's meta必须只有两项(key_meta, val_meta)
    assert(meta->submeta[0].type != 0 && meta->submeta[1].type != 0 && meta->submeta[2].type == 0);
    assert(meta->szer_i && meta->szer_i->get_count && meta->szer_i->iter_init && meta->szer_i->iter_next2);

    CRA_SER_NESTING_INC_AND_CHECK(ser);

    CRA_SERIALIZER_ENSURE_AND_RETURN(ser, buf, 1);

    CRA_SER_PTR_NULL_AND_RETURN(val, meta->is_ptr, buf);

    // write type
    *buf = CRA_SER_MAKE_TYPE(CRA_TYPE_DICT, 0);
    // get & write count
    count = meta->szer_i->get_count(val);
    if (!__cra_bin_serialize_varuint__(ser, (uint64_t)count))
    {
        ser->error = CRA_SER_ERROR_DICT_COUNT;
        return false;
    }
    // write k-v pair
    char  itbuf[64];
    void *keyptr, *valptr;
    for (meta->szer_i->iter_init(val, itbuf, sizeof(itbuf)); meta->szer_i->iter_next2(itbuf, &keyptr, &valptr);)
    {
        // write key
        if (!__cra_bin_serialize_value(ser, keyptr, meta->submeta))
            return false;
        // write val
        if (!__cra_bin_serialize_value(ser, valptr, meta->submeta + 1))
            return false;
        --count;
    }
    assert(count == 0);

    CRA_SER_NESTING_DEC(ser);

    return true;
}

static bool
__cra_bin_deserialize_dict(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    unsigned char *buf;
    uint64_t       count;
    size_t         key_size;
    size_t         val_size;
    void          *key, *val;

    assert(retval);
    assert(meta->submeta);
    assert(meta->size > 0);
    // element's meta必须只有两项(key_meta, val_meta)
    assert(meta->submeta[0].type != 0 && meta->submeta[1].type != 0 && meta->submeta[2].type == 0);
    assert(meta->dzer_i && meta->dzer_i->init2 && meta->dzer_i->uninit && meta->dzer_i->append2);

    CRA_SER_NESTING_INC_AND_CHECK(ser);

    CRA_SER_CHECK_TYPE_AND_NULL(ser, buf, meta, retval);

    // read count
    if (!__cra_bin_deserialize_varuint__(ser, &count))
    {
        ser->error = CRA_SER_ERROR_DICT_COUNT;
        return false;
    }
    if (meta->is_ptr)
        retval = *(void **)retval = cra_malloc(meta->size);
    if (meta->is_ptr || meta->dzer_i)
    {
        cra_ser_release_add(
          &ser->release, retval, meta->dzer_i ? meta->dzer_i->uninit : NULL, meta->is_ptr ? cra_free : NULL);
    }
    key_size = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;
    val_size = (meta->submeta + 1)->is_ptr ? sizeof(void *) : (meta->submeta + 1)->size;
    // init
    meta->dzer_i->init2(retval, (size_t)count, key_size, val_size, meta->arg4dzer);
    // read k-v pair
    key = cra_malloc(key_size + val_size);
    val = (char *)key + val_size;
    for (uint64_t i = 0; i < count; ++i)
    {
        // read key
        if (!__cra_bin_deserialize_value(ser, key, meta->submeta))
            goto fail;
        // read val
        if (!__cra_bin_deserialize_value(ser, val, meta->submeta + 1))
            goto fail;
        // add to dict
        if (!meta->dzer_i->append2(retval, key, val))
        {
            ser->error = CRA_SER_ERROR_APPEND_FAILED;
            return false;
        }
    }
    cra_free(key);

    CRA_SER_NESTING_DEC(ser);

    return true;

fail:
    cra_free(key);
    return false;
}

unsigned char *
cra_bin_serialize_struct(unsigned char    *buf,
                         size_t           *bufsize,
                         void             *val,
                         const CraTypeMeta members_meta[],
                         CraSerError_e    *reterror,
                         uint32_t          code)
{
    assert(bufsize);
    CraSerializer ser;
    if (cra_bin_serialize_begin(&ser, buf, *bufsize, code))
    {
        const CraTypeMeta meta = { false, CRA_TYPE_STRUCT, 0, 0, 0, 0, 0, members_meta, 0, 0, 0 };
        bool              b = __cra_bin_serialize_struct(&ser, val, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_bin_serialize_end(&ser, bufsize, reterror);
}

bool
cra_bin_deserialize_struct_with_dzer_i(unsigned char    *buf,
                                       size_t            bufsize,
                                       void             *retval,
                                       size_t            valsize,
                                       bool              is_ptr,
                                       const CraTypeMeta members_meta[],
                                       const CraDzer_i  *dzer_i,
                                       const void       *arg4dzer,
                                       CraSerError_e    *reterror,
                                       uint32_t          code)
{
    CraSerializer ser;
    if (cra_bin_deserialize_begin(&ser, buf, bufsize, code))
    {
        const CraTypeMeta meta = { is_ptr, CRA_TYPE_STRUCT, 0, valsize, 0, 0, 0, members_meta, 0, dzer_i, arg4dzer };
        bool              b = __cra_bin_deserialize_struct(&ser, retval, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_bin_deserialize_end(&ser, reterror);
}

static inline unsigned char *
__cra_bin_serialize_list_array(unsigned char    *buf,
                               size_t           *bufsize,
                               void             *val,
                               size_t            arraycount,
                               const CraTypeMeta element_meta[],
                               const CraSzer_i  *szer_i,
                               CraSerError_e    *reterror,
                               uint32_t          code)
{
    assert(bufsize);
    CraSerializer ser;
    if (cra_bin_serialize_begin(&ser, buf, *bufsize, code))
    {
        const CraTypeMeta meta = { false, CRA_TYPE_LIST, 0, 0, 0, 0, 0, element_meta, szer_i, 0, 0 };
        bool              b = __cra_bin_serialize_list(&ser, val, arraycount, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_bin_serialize_end(&ser, bufsize, reterror);
}

static inline bool
__cra_bin_deserialize_list_array(unsigned char    *buf,
                                 size_t            bufsize,
                                 void             *retval,
                                 size_t            valsize,
                                 bool              is_ptr,
                                 size_t           *retarraycount,
                                 const CraTypeMeta element_meta[],
                                 const CraDzer_i  *dzer_i,
                                 const void       *arg4dzer,
                                 CraSerError_e    *reterror,
                                 uint32_t          code)
{
    CraSerializer ser;
    if (cra_bin_deserialize_begin(&ser, buf, bufsize, code))
    {
        const CraTypeMeta meta = { is_ptr, CRA_TYPE_LIST, 0, valsize, 0, 0, 0, element_meta, 0, dzer_i, arg4dzer };
        bool              b = __cra_bin_deserialize_list(&ser, retval, retarraycount, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_bin_deserialize_end(&ser, reterror);
}

unsigned char *
cra_bin_serialize_array(unsigned char    *buf,
                        size_t           *bufsize,
                        void             *val,
                        size_t            arraycount,
                        const CraTypeMeta element_meta[],
                        CraSerError_e    *reterror,
                        uint32_t          code)
{
    return __cra_bin_serialize_list_array(buf, bufsize, val, arraycount, element_meta, NULL, reterror, code);
}

bool
cra_bin_deserialize_array(unsigned char    *buf,
                          size_t            bufsize,
                          void             *retval,
                          size_t            valsize,
                          bool              is_ptr,
                          size_t           *retarraycount,
                          const CraTypeMeta element_meta[],
                          CraSerError_e    *reterror,
                          uint32_t          code)
{
    return __cra_bin_deserialize_list_array(
      buf, bufsize, retval, valsize, is_ptr, retarraycount, element_meta, NULL, NULL, reterror, code);
}

unsigned char *
cra_bin_serialize_list(unsigned char    *buf,
                       size_t           *bufsize,
                       void             *val,
                       const CraTypeMeta element_meta[],
                       const CraSzer_i  *szer_i,
                       CraSerError_e    *reterror,
                       uint32_t          code)
{
    return __cra_bin_serialize_list_array(buf, bufsize, val, 0, element_meta, szer_i, reterror, code);
}

bool
cra_bin_deserialize_list(unsigned char    *buf,
                         size_t            bufsize,
                         void             *retval,
                         size_t            valsize,
                         bool              is_ptr,
                         const CraTypeMeta element_meta[],
                         const CraDzer_i  *dzer_i,
                         const void       *arg4dzer,
                         CraSerError_e    *reterror,
                         uint32_t          code)
{
    return __cra_bin_deserialize_list_array(
      buf, bufsize, retval, valsize, is_ptr, NULL, element_meta, dzer_i, arg4dzer, reterror, code);
}

unsigned char *
cra_bin_serialize_dict(unsigned char    *buf,
                       size_t           *bufsize,
                       void             *val,
                       const CraTypeMeta kv_meta[],
                       const CraSzer_i  *szer_i,
                       CraSerError_e    *reterror,
                       uint32_t          code)
{
    assert(bufsize);
    CraSerializer ser;
    if (cra_bin_serialize_begin(&ser, buf, *bufsize, code))
    {
        const CraTypeMeta meta = { false, CRA_TYPE_LIST, 0, 0, 0, 0, 0, kv_meta, szer_i, 0, 0 };
        bool              b = __cra_bin_serialize_dict(&ser, val, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_bin_serialize_end(&ser, bufsize, reterror);
}

bool
cra_bin_deserialize_dict(unsigned char    *buf,
                         size_t            bufsize,
                         void             *retval,
                         size_t            valsize,
                         bool              is_ptr,
                         const CraTypeMeta kv_meta[],
                         const CraDzer_i  *dzer_i,
                         const void       *arg4dzer,
                         CraSerError_e    *reterror,
                         uint32_t          code)
{
    CraSerializer ser;
    if (cra_bin_deserialize_begin(&ser, buf, bufsize, code))
    {
        const CraTypeMeta meta = { is_ptr, CRA_TYPE_LIST, 0, valsize, 0, 0, 0, kv_meta, 0, dzer_i, arg4dzer };
        bool              b = __cra_bin_deserialize_dict(&ser, retval, &meta);
        assert((ser.error && !b) || (!ser.error && b));
        CRA_UNUSED_VALUE(b);
    }
    return cra_bin_deserialize_end(&ser, reterror);
}
