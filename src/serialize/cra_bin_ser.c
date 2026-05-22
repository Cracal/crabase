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
#define __CRA_SER_INNER
#define __CRA_BUFFER_UNSIGNED
#include "serialize/cra_serialize.h"
#include "serialize/cra_bin_ser.h"
#include "cra_endian.h"
#include "cra_malloc.h"

#if CRA_IS_BIG_ENDIAN
#define CRA_SER_SWAP16 CRA_BSWAP_UINT16
#define CRA_SER_SWAP32 CRA_BSWAP_UINT32
#define CRA_SER_SWAP64 CRA_BSWAP_UINT64
#else
#define CRA_SER_SWAP16(x) (x)
#define CRA_SER_SWAP32(x) (x)
#define CRA_SER_SWAP64(x) (x)
#endif

#define CRA_BIN_MAX_STRING_LENGTH       INT32_MAX
#define CRA_BIN_MAX_BYTES_LENGTH        INT32_MAX
#define CRA_BIN_MAX_STRUCT_MEMBER_COUNT UINT8_MAX
#define CRA_BIN_MAX_LIST_COUNT          INT32_MAX
#define CRA_BIN_MAX_DICT_COUNT          INT32_MAX

#define CRA_BIN_CHECK_LENGTH(_ser, _meta, _name, _len, _max)                   \
    do                                                                         \
    {                                                                          \
        if ((uint64_t)(_len) > (uint64_t)(_max))                               \
        {                                                                      \
            CRA_SERIALIZER_ERROR(_ser,                                         \
                                 _meta,                                        \
                                 CRA_SER_ERR_LENGTH,                           \
                                 "%s must be between 0 and %zu. but got %zu.", \
                                 _name,                                        \
                                 (uint64_t)_max,                               \
                                 (uint64_t)_len);                              \
            return false;                                                      \
        }                                                                      \
    } while (0)

#define CRA_BIN_MAKE_TYPE(_TYPE, _low) (((unsigned char)(_TYPE) << 4) | ((unsigned char)(_low) & 0x0f))
#define CRA_BIN_GET_TYPE(_buf)         ((CraType_e)((*(_buf)) >> 4))
#define CRA_BIN_GET_LOW(_buf)          ((size_t)(*(_buf) & 0x0f))

static bool
cra_bin_write_uint(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    unsigned char *buf;

    assert(meta->size == 1 || meta->size == 2 || meta->size == 4 || meta->size == 8);

    CRA_SERIALIZER_ENSURE(ser, buf, meta->size);
    switch (meta->size)
    {
        case sizeof(uint8_t):
            *buf = *(uint8_t *)val;
            return true;
        case sizeof(uint16_t):
            *(uint16_t *)buf = CRA_SER_SWAP16(*(uint16_t *)val);
            return true;
        case sizeof(uint32_t):
            *(uint32_t *)buf = CRA_SER_SWAP32(*(uint32_t *)val);
            return true;
        case sizeof(uint64_t):
            *(uint64_t *)buf = CRA_SER_SWAP64(*(uint64_t *)val);
            return true;

        default:
            assert_always(false);
    }
}

static bool
cra_bin_read_uint(CraSerializer *ser, void *retval, const CraTypeMeta *meta, size_t sizefrombuf)
{
    unsigned char *buf;

    assert(meta->size == 1 || meta->size == 2 || meta->size == 4 || meta->size == 8);

    CRA_SERIALIZER_CHECK_SIZE(ser, meta, sizefrombuf);

    CRA_SERIALIZER_ENSURE(ser, buf, meta->size);
    switch (meta->size)
    {
        case sizeof(uint8_t):
            *(uint8_t *)retval = *(uint8_t *)buf;
            return true;
        case sizeof(uint16_t):
            *(uint16_t *)retval = CRA_SER_SWAP16(*(uint16_t *)buf);
            return true;
        case sizeof(uint32_t):
            *(uint32_t *)retval = CRA_SER_SWAP32(*(uint32_t *)buf);
            return true;
        case sizeof(uint64_t):
            *(uint64_t *)retval = CRA_SER_SWAP64(*(uint64_t *)buf);
            return true;

        default:
            assert_always(false);
    }
}

static bool
__cra_bin_write_varuint(CraSerializer *ser, uint64_t val)
{
    unsigned char *buf;

    while (val > 0x7f)
    {
        CRA_SERIALIZER_ENSURE(ser, buf, 1);
        *buf = (unsigned char)val | 0x80;
        val >>= 7;
    }
    CRA_SERIALIZER_ENSURE(ser, buf, 1);
    *buf = (unsigned char)val;
    return true;
}

static bool
__cra_bin_read_varuint(CraSerializer *ser, uint64_t *retval)
{
    unsigned char *buf;
    int            shift;

    for (shift = 0, *retval = 0; shift < 64;)
    {
        CRA_SERIALIZER_ENSURE(ser, buf, 1);
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
    return false;
}

#define cra_bin_zigzag_i2u(_i64) (uint64_t)(((_i64) << 1) ^ ((_i64) >> 63))
#define cra_bin_zigzag_u2i(_u64) (int64_t)(((_u64) >> 1) ^ -(int64_t)((_u64) & 1))

static inline bool
cra_bin_write_varint(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    int64_t  i64;
    uint64_t u64;

    cra_serializer_p2i(val, &i64, meta);
    u64 = cra_bin_zigzag_i2u(i64);
    return __cra_bin_write_varuint(ser, u64);
}

static inline bool
cra_bin_read_varint(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    int64_t  i64;
    uint64_t u64;

    if (__cra_bin_read_varuint(ser, &u64))
    {
        i64 = cra_bin_zigzag_u2i(u64);
        cra_serializer_i2p(i64, retval, meta);
        return true;
    }
    if (!ser->error.err)
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid varint value");
    return false;
}

static inline bool
cra_bin_write_varuint(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    uint64_t u64;

    cra_serializer_p2u(val, &u64, meta);
    return __cra_bin_write_varuint(ser, u64);
}

static inline bool
cra_bin_read_varuint(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    uint64_t u64;

    if (__cra_bin_read_varuint(ser, &u64))
    {
        cra_serializer_u2p(u64, retval, meta);
        return true;
    }
    if (!ser->error.err)
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INVALID_VAL, "invalid varuint value");
    return false;
}

static bool
cra_bin_write_float(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    union
    {
        float    f32;
        uint32_t i32;
        double   f64;
        uint64_t i64;
    } u;
    unsigned char *buf;

    assert(meta->size == 4 || meta->size == 8);

    CRA_SERIALIZER_ENSURE(ser, buf, meta->size);
    switch (meta->size)
    {
        case sizeof(float):
            u.f32 = *(float *)val;
            *(uint32_t *)buf = CRA_SER_SWAP32(u.i32);
            return true;
        case sizeof(double):
            u.f64 = *(double *)val;
            *(uint64_t *)buf = CRA_SER_SWAP64(u.i64);
            return true;

        default:
            assert_always(false);
    }
}

static bool
cra_bin_read_float(CraSerializer *ser, void *retval, const CraTypeMeta *meta, size_t sizefrombuf)
{
    union
    {
        float    f32;
        uint32_t i32;
        double   f64;
        uint64_t i64;
    } u;
    unsigned char *buf;

    assert(meta->size == 4 || meta->size == 8);

    CRA_SERIALIZER_CHECK_SIZE(ser, meta, sizefrombuf);

    CRA_SERIALIZER_ENSURE(ser, buf, meta->size);
    switch (meta->size)
    {
        case sizeof(float):
            u.i32 = CRA_SER_SWAP32(*(uint32_t *)buf);
            *(float *)retval = u.f32;
            return true;
        case sizeof(double):
            u.i64 = CRA_SER_SWAP64(*(uint64_t *)buf);
            *(double *)retval = u.f64;
            return true;

        default:
            assert_always(false);
    }
}

static bool
__cra_bin_write_string(CraSerializer *ser, void *str, uint64_t len, const CraTypeMeta *meta)
{
    // check length
    if (meta->type == CRA_TYPE_STRING)
        CRA_BIN_CHECK_LENGTH(ser, meta, "String length", len, CRA_BIN_MAX_STRING_LENGTH);
    else
        CRA_BIN_CHECK_LENGTH(ser, meta, "Bytes length", len, CRA_BIN_MAX_BYTES_LENGTH);
    // write length
    if (!__cra_bin_write_varuint(ser, len))
    {
        CRA_SERIALIZER_ERROR(ser,
                             meta,
                             CRA_SER_ERR_LENGTH,
                             "failed to write %s length",
                             meta->type == CRA_TYPE_STRING ? "string" : "bytes");
        return false;
    }
    // write string
    if (len > 0)
    {
        unsigned char *buf;
        CRA_SERIALIZER_ENSURE(ser, buf, len);
        memcpy(buf, str, len);
    }
    return true;
}

static bool
__cra_bin_read_string(CraSerializer *ser, void *retval, uint64_t *retlen, const CraTypeMeta *meta)
{
    uint64_t len;
    // read length
    if (!__cra_bin_read_varuint(ser, &len))
    {
        CRA_SERIALIZER_ERROR(ser,
                             meta,
                             CRA_SER_ERR_LENGTH,
                             "failed to read %s length",
                             meta->type == CRA_TYPE_STRING ? "string" : "bytes");
        return false;
    }
    // check length
    if (meta->type == CRA_TYPE_STRING)
        CRA_BIN_CHECK_LENGTH(ser, meta, "String length", len, CRA_BIN_MAX_STRING_LENGTH);
    else
        CRA_BIN_CHECK_LENGTH(ser, meta, "Bytes length", len, CRA_BIN_MAX_BYTES_LENGTH);

    if (meta->is_ptr)
    {
        // alloc
        retval = *(void **)retval = cra_malloc(len + 1);
        CRA_SEIALIZER_CHECK_MEMORY(ser, meta, retval);
        CRA_SERIALIZER_RELEASE_MGR_ADD_CHECK(ser, meta, retval);
    }
    else
    {
        // is the char array enough?
        uint64_t l = len + (meta->type == CRA_TYPE_STRING ? 1 : 0);
        if (meta->size < l)
        {
            CRA_SERIALIZER_ERROR(ser,
                                 meta,
                                 CRA_SER_ERR_TOO_SMALL,
                                 "%s size too small(%zu < %zu)",
                                 meta->type == CRA_TYPE_STRING ? "string" : "bytes",
                                 meta->size,
                                 l);
            return false;
        }
    }

    // read value
    if (len > 0)
    {
        unsigned char *buf;
        CRA_SERIALIZER_ENSURE(ser, buf, len);
        memcpy(retval, buf, len);
    }
    if (meta->type == CRA_TYPE_STRING)
        ((char *)retval)[len] = '\0';
    else
        *retlen = len;
    return true;
}

static inline bool
cra_bin_write_string(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    char  *str;
    size_t len;

    str = meta->is_ptr ? *(char **)val : (char *)val;
    len = strlen(str);
    return __cra_bin_write_string(ser, str, len, meta);
}

static inline bool
cra_bin_read_string(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    return __cra_bin_read_string(ser, retval, NULL, meta);
}

static inline bool
cra_bin_write_bytes(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    void              *bytes;
    uint64_t           length;
    void              *plength;
    const CraTypeMeta *meta_len;

    // check length variant
    meta_len = meta + 1;
    assert(meta_len->is_len && meta_len->type == CRA_TYPE_UINT);

    // get length
    plength = !meta_len->arg ? (void *)((char *)val - meta->offset + meta_len->offset) : meta_len->arg;
    assert(plength);
    cra_serializer_p2u(plength, &length, meta_len);

    // write
    bytes = meta->is_ptr ? *(void **)val : val;
    return __cra_bin_write_string(ser, bytes, length, meta);
}

static inline bool
cra_bin_read_bytes(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    const CraTypeMeta *meta_len;
    void              *plength;
    uint64_t           length;

    // check length variant
    meta_len = meta + 1;
    assert(meta_len->is_len && meta_len->type == CRA_TYPE_UINT);

    // get length ptr
    plength = !meta_len->arg ? (void *)((char *)retval - meta->offset + meta_len->offset) : meta_len->arg;
    assert(plength);

    // read bytes
    if (__cra_bin_read_string(ser, retval, &length, meta))
    {
        cra_serializer_u2p(length, plength, meta_len);
        return true;
    }
    return false;
}

static bool
cra_bin_write_struct(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
cra_bin_read_struct(CraSerializer *ser, void *retval, const CraTypeMeta *meta);
static bool
cra_bin_write_array(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
cra_bin_read_array(CraSerializer *ser, void *retval, const CraTypeMeta *meta);
static bool
cra_bin_write_list(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
cra_bin_read_list(CraSerializer *ser, void *retval, const CraTypeMeta *meta);
static bool
cra_bin_write_dict(CraSerializer *ser, void *val, const CraTypeMeta *meta);
static bool
cra_bin_read_dict(CraSerializer *ser, void *retval, const CraTypeMeta *meta);

static bool
cra_bin_write_value(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    unsigned char *buf;

    assert(val);
    assert(meta);
    assert(meta->name);
    assert(meta->is_not_end);

    CRA_SERIALIZER_ENSURE(ser, buf, 1);

    // null?
    if (meta->is_ptr && !(*(void **)val))
    {
        CRA_SERIALIZER_CHECK_NULL(ser, meta);
        *buf = CRA_BIN_MAKE_TYPE(CRA_TYPE_NULL, 0);
        return true;
    }

    // write type
    *buf = CRA_BIN_MAKE_TYPE(meta->type, meta->size);

    // write value
    switch (meta->type)
    {
        case CRA_TYPE_BOOL:
            *buf = CRA_BIN_MAKE_TYPE(CRA_TYPE_BOOL, !!(*(bool *)val));
            return true;
        case CRA_TYPE_INT:
        case CRA_TYPE_UINT:
            return cra_bin_write_uint(ser, val, meta);
        case CRA_TYPE_VARINT:
            return cra_bin_write_varint(ser, val, meta);
        case CRA_TYPE_VARUINT:
            return cra_bin_write_varuint(ser, val, meta);
        case CRA_TYPE_FLOAT:
            return cra_bin_write_float(ser, val, meta);
        case CRA_TYPE_STRING:
            return cra_bin_write_string(ser, val, meta);
        case CRA_TYPE_BYTES:
            return cra_bin_write_bytes(ser, val, meta);
        case CRA_TYPE_STRUCT:
            return cra_bin_write_struct(ser, val, meta);
        case CRA_TYPE_LIST:
            if (meta->iter_i)
                return cra_bin_write_list(ser, val, meta);
            else
                return cra_bin_write_array(ser, val, meta);
        case CRA_TYPE_DICT:
            return cra_bin_write_dict(ser, val, meta);

        default:
            assert_always(false);
    }
}

static bool
cra_bin_read_value(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    unsigned char *buf;

    assert(retval);
    assert(meta);
    assert(meta->name);
    assert(meta->is_not_end);

    CRA_SERIALIZER_ENSURE(ser, buf, 1);

    // check null
    if (CRA_BIN_GET_TYPE(buf) == CRA_TYPE_NULL)
    {
        CRA_SERIALIZER_CHECK_NULL(ser, meta);
        *(void **)retval = NULL;
        return true;
    }

    // check type
    CRA_SERIALIZER_CHECK_TYPE(ser, meta, CRA_BIN_GET_TYPE(buf));

    switch (meta->type)
    {
        case CRA_TYPE_BOOL:
            *(bool *)retval = !!CRA_BIN_GET_LOW(buf);
            return true;
        case CRA_TYPE_INT:
        case CRA_TYPE_UINT:
            return cra_bin_read_uint(ser, retval, meta, CRA_BIN_GET_LOW(buf));
        case CRA_TYPE_VARINT:
            return cra_bin_read_varint(ser, retval, meta);
        case CRA_TYPE_VARUINT:
            return cra_bin_read_varuint(ser, retval, meta);
        case CRA_TYPE_FLOAT:
            return cra_bin_read_float(ser, retval, meta, CRA_BIN_GET_LOW(buf));
        case CRA_TYPE_STRING:
            return cra_bin_read_string(ser, retval, meta);
        case CRA_TYPE_BYTES:
            return cra_bin_read_bytes(ser, retval, meta);
        case CRA_TYPE_STRUCT:
            return cra_bin_read_struct(ser, retval, meta);
        case CRA_TYPE_LIST:
            if (meta->iter_i)
                return cra_bin_read_list(ser, retval, meta);
            else
                return cra_bin_read_array(ser, retval, meta);
        case CRA_TYPE_DICT:
            return cra_bin_read_dict(ser, retval, meta);

        default:
            assert_always(false);
    }
}

static bool
cra_bin_write_struct(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    unsigned char     *buf, *numbuf;
    int                nfields;
    char              *stru;
    const CraTypeMeta *m;

    assert(meta->submeta && meta->submeta->is_not_end); // at least 1 member

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // remaining num of members
    CRA_SERIALIZER_ENSURE(ser, numbuf, 1);
    // write members
    stru = meta->is_ptr ? *(char **)val : (char *)val;
    for (nfields = 0, m = meta->submeta; m->is_not_end; ++m)
    {
        if (m->is_len)
            continue;

        ++nfields;

        assert(nfields <= CRA_BIN_MAX_STRUCT_MEMBER_COUNT);
        assert(meta->size > m->offset);

        // write id
        CRA_SERIALIZER_ENSURE(ser, buf, 1);
        *buf = m->id;
        // write member
        if (!cra_bin_write_value(ser, stru + m->offset, m))
            return false;
    }
    // check num of members
    CRA_BIN_CHECK_LENGTH(ser, meta, "Struct member count", nfields, CRA_BIN_MAX_STRUCT_MEMBER_COUNT);
    // write num of members
    *numbuf = (unsigned char)nfields;

    CRA_SERIALIZER_NESTING_DEC(ser);

    return true;
}

static bool
cra_bin_skip_member(CraSerializer *ser)
{
    size_t         len;
    uint64_t       u64;
    unsigned char *buf;
    CRA_SERIALIZER_ENSURE(ser, buf, 1);
    switch (CRA_BIN_GET_TYPE(buf))
    {
        case CRA_TYPE_NULL:
        case CRA_TYPE_BOOL:
            break; // ENSURE() skip it
        case CRA_TYPE_INT:
        case CRA_TYPE_UINT:
        case CRA_TYPE_FLOAT:
            len = CRA_BIN_GET_LOW(buf);
            CRA_SERIALIZER_ENSURE(ser, buf, len);
            break;
        case CRA_TYPE_VARINT:
        case CRA_TYPE_VARUINT:
            while (true)
            {
                CRA_SERIALIZER_ENSURE(ser, buf, 1);
                if (*buf < 0x80)
                    break;
            }
            break;
        case CRA_TYPE_STRING:
        case CRA_TYPE_BYTES:
            // read length
            if (!__cra_bin_read_varuint(ser, &u64))
                return false;
            // skip
            CRA_SERIALIZER_ENSURE(ser, buf, u64);
            break;
        case CRA_TYPE_STRUCT:
            // read num
            CRA_SERIALIZER_ENSURE(ser, buf, 1);
            len = (size_t)(*buf);
            for (size_t i = 0; i < len; ++i)
            {
                // skip id
                CRA_SERIALIZER_ENSURE(ser, buf, 1);
                // skip member
                if (!cra_bin_skip_member(ser))
                    return false;
            }
            break;
        case CRA_TYPE_LIST:
            // read count
            if (!__cra_bin_read_varuint(ser, &u64))
                return false;
            for (uint64_t i = 0; i < u64; ++i)
            {
                // skip element
                if (!cra_bin_skip_member(ser))
                    return false;
            }
            break;
        case CRA_TYPE_DICT:
            // read count
            if (!__cra_bin_read_varuint(ser, &u64))
                return false;
            for (uint64_t i = 0; i < u64; ++i)
            {
                // skip key
                if (!cra_bin_skip_member(ser))
                    return false;
                // skip val
                if (!cra_bin_skip_member(ser))
                    return false;
            }
            break;

        default:
            return false;
    }
    return true;
}

static bool
cra_bin_read_struct(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    unsigned char     *buf;
    char              *stru;
    const CraTypeMeta *m, *last;
    uint8_t            nfields, id;

    assert(meta->submeta && meta->submeta->is_not_end); // at least 1 member

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    CRA_INITIALIZABLE_UNINIT_FN((*uninit)) = meta->init_i ? meta->init_i->uninit : NULL;

    // alloc
    if (meta->is_ptr)
    {
        stru = *(char **)retval = (char *)cra_malloc(meta->size);
        CRA_SEIALIZER_CHECK_MEMORY(ser, meta, stru);
    }
    else
    {
        stru = (char *)retval;
    }

    if (meta->is_ptr || uninit)
        CRA_SERIALIZER_RELEASE_MGR_ADD_CHECK(ser, meta, stru);

    // init
    if (meta->init_i && meta->init_i->init)
    {
        if (!meta->init_i->init(stru, meta->size, meta->arg))
        {
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INIT_FAILED, "failed to init struct");
            return false;
        }
    }
    else
    {
        bzero(stru, meta->size);
    }

    // read num of members
    CRA_SERIALIZER_ENSURE(ser, buf, 1);
    nfields = *buf;
    // check num of members
    // CRA_BIN_CHECK_LENGTH(ser, meta, "Struct member count", nfields, CRA_BIN_MAX_STRUCT_MEMBER_COUNT);

    // read members
    m = meta->submeta;
    for (uint8_t i = 0; i < nfields; ++i)
    {
        // read id
        CRA_SERIALIZER_ENSURE(ser, buf, 1);
        id = *buf;
        // match id
        last = m;
        while (true)
        {
            if (m->is_len)
            {
                if (!(++m)->is_not_end)
                    m = meta->submeta;
            }

            if (id == m->id)
            {
                // read member
                if (!cra_bin_read_value(ser, stru + m->offset, m))
                    return false;

                if (!(++m)->is_not_end)
                    m = meta->submeta;
                break;
            }

            if (!(++m)->is_not_end)
                m = meta->submeta;

            // skip this member
            if (m == last)
            {
                if (!cra_bin_skip_member(ser))
                {
                    CRA_SERIALIZER_ERROR1(ser,
                                          ser->error.err,
                                          "failed to skip the member. id: %d, type: %d",
                                          id,
                                          CRA_BIN_GET_TYPE(buf + 1));
                    return false;
                }
                break;
            }
        }
    }

    CRA_SERIALIZER_NESTING_DEC(ser);

    return true;
}

static bool
cra_bin_write_array(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    char              *arr;
    size_t             slot;
    uint64_t           count;
    void              *pcount;
    const CraTypeMeta *metacnt;

    assert(!meta->iter_i);
    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert((meta + 1)->is_len && (meta + 1)->type == CRA_TYPE_UINT);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    count = 0;
    metacnt = meta + 1;
    arr = meta->is_ptr ? *(char **)val : (char *)val;
    slot = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;
    pcount = !metacnt->arg ? (void *)((char *)val - meta->offset + metacnt->offset) : metacnt->arg;
    assert(pcount);

    // get count
    cra_serializer_p2u(pcount, &count, metacnt);
    // check count
    CRA_BIN_CHECK_LENGTH(ser, meta, "Array count", count, CRA_BIN_MAX_LIST_COUNT);

    // write count
    if (!__cra_bin_write_varuint(ser, count))
    {
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_LENGTH, "failed to write array count");
        return false;
    }

    // write elements
    for (uint64_t i = 0; i < count; ++i)
    {
        if (!cra_bin_write_value(ser, arr + (i * slot), meta->submeta))
            return false;
    }

    return true;
}

static bool
cra_bin_read_array(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    bool               ret;
    char              *arr;
    size_t             size;
    size_t             slot;
    uint64_t           count;
    void              *pcount;
    const CraTypeMeta *metacnt;

    assert(!meta->iter_i);
    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert((meta + 1)->is_len && (meta + 1)->type == CRA_TYPE_UINT);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    metacnt = meta + 1;
    slot = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;
    pcount = !metacnt->arg ? (void *)((char *)retval - meta->offset + metacnt->offset) : metacnt->arg;
    assert(pcount);

    // read count
    if (!__cra_bin_read_varuint(ser, &count))
    {
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_LENGTH, "failed to read array count");
        return false;
    }
    // check count
    CRA_BIN_CHECK_LENGTH(ser, meta, "Array count", count, CRA_BIN_MAX_LIST_COUNT);

    if (meta->is_ptr)
    {
        size = (count > 0 ? count : 1) * slot;
        arr = *(char **)retval = (char *)cra_malloc(size);
        CRA_SEIALIZER_CHECK_MEMORY(ser, meta, arr);
        CRA_SERIALIZER_RELEASE_MGR_ADD_CHECK(ser, meta, arr);
    }
    else
    {
        // enough?
        if (meta->size / slot < count)
        {
            CRA_SERIALIZER_ERROR(
              ser, meta, CRA_SER_ERR_TOO_SMALL, "array size too small(%zu < %zu)", meta->size / slot, count);
            return false;
        }
        size = meta->size;
        arr = (char *)retval;
    }
    bzero(arr, size);

    // read elements
    ret = true;
    for (uint64_t i = 0; i < count; ++i)
    {
        // read element
        if (!(ret = cra_bin_read_value(ser, arr + i * slot, meta->submeta)))
            break;
    }

    cra_serializer_u2p(count, pcount, metacnt);
    return ret;
}

static bool
cra_bin_write_list(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    size_t      count, i;
    char       *list;
    CraTwoVals  vals;
    CraIterator it;

    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert(meta->iter_i && meta->iter_i->init && meta->iter_i->next);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    list = meta->is_ptr ? *(char **)val : (char *)val;

    // init it & get count
    meta->iter_i->init(list, &it, &count, false);

    // check count
    CRA_BIN_CHECK_LENGTH(ser, meta, "List count", count, CRA_BIN_MAX_LIST_COUNT);

    // write count
    if (!__cra_bin_write_varuint(ser, (uint64_t)count))
    {
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_LENGTH, "failed to write list count");
        return false;
    }

    // write elements
    i = 0;
    for (i = 0; meta->iter_i->next(&it, &vals); ++i)
    {
        assert(vals.val1_ref);
        if (!cra_bin_write_value(ser, vals.val1_ref, meta->submeta))
            return false;
    }

    assert(i == count);

    CRA_SERIALIZER_NESTING_DEC(ser);

    return true;
}

static bool
cra_bin_read_list(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    bool       ret;
    CraTwoVals vals;
    char      *list;
    size_t     slot;
    uint64_t   count;

    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert(meta->init_i && meta->init_i->init);
    assert(meta->append_i && meta->append_i->append);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // read count
    if (!__cra_bin_read_varuint(ser, &count))
    {
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_LENGTH, "failed to read list count");
        return false;
    }
    // check count
    CRA_BIN_CHECK_LENGTH(ser, meta, "List count", count, CRA_BIN_MAX_LIST_COUNT);

    slot = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;

    // alloc
    if (meta->is_ptr)
    {
        list = *(char **)retval = (char *)cra_malloc(meta->size);
        CRA_SEIALIZER_CHECK_MEMORY(ser, meta, list);
    }
    else
    {
        list = (char *)retval;
    }

    if (meta->is_ptr || meta->init_i->uninit)
        CRA_SERIALIZER_RELEASE_MGR_ADD_CHECK(ser, meta, list);

    // init
    if (!meta->init_i->init(list, (size_t)count, meta->arg))
    {
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INIT_FAILED, "failed to init list");
        return false;
    }

#ifdef __STDC_NO_VLA__
    vals.val1_ref = cra_malloc(slot);
    CRA_SEIALIZER_CHECK_MEMORY(ser, meta, vals.val1_ref);
#else
    char element[slot];
    vals.val1_ref = element;
#endif

    // read elements
    ret = true;
    for (uint64_t i = 0; i < count; ++i)
    {
        // read element
        if (!(ret = cra_bin_read_value(ser, vals.val1_ref, meta->submeta)))
            break;

        // append
        if (!(ret = meta->append_i->append(list, &vals)))
        {
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_ADD_FAILED, "failed to add a list element");
            break;
        }
    }

#ifdef __STDC_NO_VLA__
    cra_free(vals.val1_ref);
#endif
    CRA_SERIALIZER_NESTING_DEC(ser);
    return ret;
}

static bool
cra_bin_write_dict(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    char       *dict;
    CraTwoVals  vals;
    CraIterator it;
    size_t      count, i;

    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert((meta->submeta + 1)->is_not_end);
    assert(meta->iter_i && meta->iter_i->init && meta->iter_i->next);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // check key
    CRA_SERIALIZER_CHECK_KEY(ser, meta->submeta);

    dict = meta->is_ptr ? *(char **)val : (char *)val;

    // init it & get count
    meta->iter_i->init(dict, &it, &count, false);

    // check count
    CRA_BIN_CHECK_LENGTH(ser, meta, "Dict count", count, CRA_BIN_MAX_DICT_COUNT);

    // write count
    if (!__cra_bin_write_varuint(ser, count))
    {
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_LENGTH, "failed to write dict count");
        return false;
    }

    // write elements
    for (i = 0; meta->iter_i->next(&it, &vals); ++i)
    {
        assert(vals.val1_ref);
        assert(vals.val2_ref);
        if (!cra_bin_write_value(ser, vals.val1_ref, meta->submeta))
            return false;
        if (!cra_bin_write_value(ser, vals.val2_ref, meta->submeta + 1))
            return false;
    }

    assert(i == count);

    CRA_SERIALIZER_NESTING_DEC(ser);

    return true;
}

static bool
cra_bin_read_dict(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    bool               ret;
    CraTwoVals         vals;
    char              *dict;
    uint64_t           count;
    size_t             key_size, val_size;
    const CraTypeMeta *keymeta, *valmeta;

    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert((meta->submeta + 1)->is_not_end);
    assert(meta->init_i && meta->init_i->init);
    assert(meta->append_i && meta->append_i->append);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // check key
    CRA_SERIALIZER_CHECK_KEY(ser, meta->submeta);

    // read count
    if (!__cra_bin_read_varuint(ser, &count))
    {
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_LENGTH, "failed to read dict count");
        return false;
    }
    // check count
    CRA_BIN_CHECK_LENGTH(ser, meta, "Dict count", count, CRA_BIN_MAX_DICT_COUNT);

    keymeta = meta->submeta;
    valmeta = meta->submeta + 1;
    key_size = keymeta->is_ptr ? sizeof(void *) : keymeta->size;
    val_size = valmeta->is_ptr ? sizeof(void *) : valmeta->size;

    // alloc
    if (meta->is_ptr)
    {
        dict = *(char **)retval = (char *)cra_malloc(meta->size);
        CRA_SEIALIZER_CHECK_MEMORY(ser, meta, dict);
    }
    else
    {
        dict = (char *)retval;
    }

    if (meta->is_ptr || meta->init_i->uninit)
    {
        CRA_SERIALIZER_RELEASE_MGR_ADD_CHECK(ser, meta, dict);
    }

    // init
    if (!meta->init_i->init(dict, (size_t)count, meta->arg))
    {
        CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_INIT_FAILED, "failed to init dict");
        return false;
    }

#ifdef __STDC_NO_VLA__
    vals.val1_ref = cra_malloc(key_size + val_size); // FIXME: alignment
    CRA_SEIALIZER_CHECK_MEMORY(ser, meta, vals.val1_ref);
#else
    char key[key_size + val_size];
    vals.val1_ref = key;
#endif
    vals.val2_ref = (char *)vals.val1_ref + key_size;

    // read elements
    ret = true;
    for (uint64_t i = 0; i < count; ++i)
    {
        // read key
        if (!(ret = cra_bin_read_value(ser, vals.val1_ref, keymeta)))
            break;
        // read value
        if (!(ret = cra_bin_read_value(ser, vals.val2_ref, valmeta)))
            break;

        if (!(ret = meta->append_i->append(dict, &vals)))
        {
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_ADD_FAILED, "failed to insert a key-value pair into dict");
            break;
        }
    }

#ifdef __STDC_NO_VLA__
    cra_free(vals.val1_ref);
#endif

    CRA_SERIALIZER_NESTING_DEC(ser);

    return ret;
}

bool
cra_bin_serialize_err(unsigned char *buf, size_t *len, CraSeriObject *obj, CraSerErr *err)
{
    bool          ret;
    CraSerializer ser;

    assert(buf);
    assert(len);
    assert(obj);
    assert(obj->objptr);

    cra_serializer_init(&ser, buf, *len, false);
    ret = cra_bin_write_value(&ser, obj->objptr, obj->meta);
    cra_serializer_check_err(ser, err, ret);
    cra_serializer_uninit(&ser, ret);
    *len = ser.index;
    return ret;
}

bool
cra_bin_deserialize_err(unsigned char *buf, size_t len, CraSeriObject *retobj, CraSerErr *err)
{
    assert(buf);
    assert(len > 0);
    assert(retobj);
    assert(retobj->objptr);

    bool          ret;
    CraSerializer ser;

    cra_serializer_init(&ser, buf, len, false);
    ret = cra_bin_read_value(&ser, retobj->objptr, retobj->meta);
    cra_serializer_check_err(ser, err, ret);
    cra_serializer_uninit(&ser, ret);
    return ret;
}

void
cra_bin_write_len(unsigned char *buf, uint64_t len, size_t len_size)
{
    switch (len_size)
    {
        case 1:
            *buf = (unsigned char)len;
            break;
        case 2:
            *(uint16_t *)buf = CRA_SER_SWAP16((uint16_t)len);
            break;
        case 4:
            *(uint32_t *)buf = CRA_SER_SWAP32((uint32_t)len);
            break;
        case 8:
            *(uint64_t *)buf = CRA_SER_SWAP64(len);
            break;

        default:
            assert_always(false);
    }
}

uint64_t
cra_bin_read_len(unsigned char *buf, size_t len_size)
{
    switch (len_size)
    {
        case 1:
            return *(uint8_t *)buf;
        case 2:
            return CRA_SER_SWAP16(*(uint16_t *)buf);
        case 4:
            return CRA_SER_SWAP32(*(uint32_t *)buf);
        case 8:
            return CRA_SER_SWAP64(*(uint64_t *)buf);

        default:
            assert_always(false);
    }
}
