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
#include "serialize/cra_bin_ser.h"
#include "cra_assert.h"
#include "cra_endian.h"
#include "serialize/cra_serialize.h"
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

#define CRA_SERIALIZER_NAME "BinSer"

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
            CRA_SERIALIZER_PRINT_ERROR(
              "Invalid [u]int size: %zu. correct: 1, 2, 4, 8. Name: %s.", meta->size, CRA_NAME(meta));
            return false;
    }
}

static bool
cra_bin_read_uint(CraSerializer *ser, void *retval, const CraTypeMeta *meta, size_t sizefrombuf)
{
    unsigned char *buf;

    assert(meta->size == 1 || meta->size == 2 || meta->size == 4 || meta->size == 8);

    if (sizefrombuf != meta->size)
    {
        CRA_SERIALIZER_PRINT_ERROR("Size mismatch. %zu != %zu. Name: %s.", sizefrombuf, meta->size, CRA_NAME(meta));
        return false;
    }

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
            CRA_SERIALIZER_PRINT_ERROR(
              "Invalid [u]int size: %zu. correct: 1, 2, 4, 8. Name: %s.", meta->size, CRA_NAME(meta));
            return false;
    }
}

static bool
__cra_bin_write_varuint(CraSerializer *ser, uint64_t val)
{
    unsigned char *buf;

    while (true)
    {
        CRA_SERIALIZER_ENSURE(ser, buf, 1);
        if (val > 0x7f)
        {
            *buf = (unsigned char)val | 0x80;
            val >>= 7;
        }
        else
        {
            *buf = (unsigned char)val;
            return true;
        }
    }
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

    if (!cra_serializer_p2i(val, &i64, meta))
        return false;
    u64 = cra_bin_zigzag_i2u(i64);
    return __cra_bin_write_varuint(ser, u64);
}

static inline bool
cra_bin_read_varint(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    int64_t  i64;
    uint64_t u64;

    if (!__cra_bin_read_varuint(ser, &u64))
    {
        CRA_SERIALIZER_PRINT_ERROR("Read varint failed: Invalid number. Name: %s.", CRA_NAME(meta));
        return false;
    }
    i64 = cra_bin_zigzag_u2i(u64);
    return cra_serializer_i2p(i64, retval, meta);
}

static inline bool
cra_bin_write_varuint(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    uint64_t u64;

    if (!cra_serializer_p2u(val, &u64, meta))
        return false;
    return __cra_bin_write_varuint(ser, u64);
}

static inline bool
cra_bin_read_varuint(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    uint64_t u64;

    if (!__cra_bin_read_varuint(ser, &u64))
    {
        CRA_SERIALIZER_PRINT_ERROR("Read varuint failed: Invalid number. Name: %s.", CRA_NAME(meta));
        return false;
    }
    return cra_serializer_u2p(u64, retval, meta);
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
            CRA_SERIALIZER_PRINT_ERROR("Invalid float size: %zu. correct: 4, 8. Name: %s.", meta->size, CRA_NAME(meta));
            return false;
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

    if (sizefrombuf != meta->size)
    {
        CRA_SERIALIZER_PRINT_ERROR("Size mismatch. %zu != %zu. Name: %s.", sizefrombuf, meta->size, CRA_NAME(meta));
        return false;
    }

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
            CRA_SERIALIZER_PRINT_ERROR("Invalid float size: %zu. correct: 4, 8. Name: %s.", meta->size, CRA_NAME(meta));
            return false;
    }
}

static bool
__cra_bin_write_string(CraSerializer *ser, void *str, uint64_t len, const CraTypeMeta *meta)
{
    // write length
    if (!__cra_bin_write_varuint(ser, len))
    {
        CRA_SERIALIZER_PRINT_ERROR(
          "Write %s length failed. Name: %s.", meta->type == CRA_TYPE_STRING ? "string" : "bytes", CRA_NAME(meta));
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
        CRA_SERIALIZER_PRINT_ERROR(
          "Read %s length failed. Name: %s.", meta->type == CRA_TYPE_STRING ? "string" : "bytes", CRA_NAME(meta));
        return false;
    }

    if (meta->is_ptr)
    {
        // alloc
        retval = *(void **)retval = cra_malloc(len + 1);
        cra_release_mgr_add(&ser->release, retval, meta);
    }
    else
    {
        // is the char array enough?
        uint64_t l = len + (meta->type == CRA_TYPE_STRING ? 1 : 0);
        if (meta->size < l)
        {
            CRA_SERIALIZER_PRINT_ERROR("The buffer of %s is too small. %zu < %zu. Name: %s.",
                                       meta->type == CRA_TYPE_STRING ? "string" : "bytes",
                                       meta->size,
                                       l,
                                       CRA_NAME(meta));
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
    unsigned char     *bytes;
    uint64_t           length;
    void              *plength;
    const CraTypeMeta *meta_len;

    // check length variant
    meta_len = meta + 1;
    if (!meta_len->is_len || meta_len->type != CRA_TYPE_UINT)
    {
        CRA_SERIALIZER_PRINT_ERROR("You must set a LENGTH VARIANT(type: UINT) for the BYTES. Name: %s.",
                                   CRA_NAME(meta));
        return false;
    }

    // get length
    plength = !meta_len->arg ? (void *)((char *)val - meta->offset + meta_len->offset) : meta_len->arg;
    if (!cra_serializer_p2u(plength, &length, meta_len))
        return false;

    // write
    bytes = meta->is_ptr ? *(unsigned char **)val : (unsigned char *)val;
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
    if (!meta_len->is_len || meta_len->type != CRA_TYPE_UINT)
    {
        CRA_SERIALIZER_PRINT_ERROR("You must set a LENGTH VARIANT(type: UINT) for the BYTES. Name: %s.",
                                   CRA_NAME(meta));
        return false;
    }

    // get length ptr
    plength = !meta_len->arg ? (void *)((char *)retval - meta->offset + meta_len->offset) : meta_len->arg;

    // read bytes
    if (__cra_bin_read_string(ser, retval, &length, meta))
        return cra_serializer_u2p(length, plength, meta_len);
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
    assert(meta->is_not_end);

    CRA_SERIALIZER_ENSURE(ser, buf, 1);

    // null?
    if (meta->is_ptr && !(*(void **)val))
    {
        if (meta->type <= CRA_TYPE_FLOAT)
        {
            CRA_SERIALIZER_PRINT_ERROR("Cannot be null. Name: %s.", CRA_NAME(meta));
            return false;
        }
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
            if (meta->szer_i)
                return cra_bin_write_list(ser, val, meta);
            else
                return cra_bin_write_array(ser, val, meta);
        case CRA_TYPE_DICT:
            return cra_bin_write_dict(ser, val, meta);

        default:
            CRA_SERIALIZER_PRINT_ERROR(
              "Invalid type: %d. Please check your meta data. Name: %s.", meta->type, CRA_NAME(meta));
            return false;
    }
}

static bool
cra_bin_read_value(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    unsigned char *buf;

    assert(retval);
    assert(meta);
    assert(meta->is_not_end);

    CRA_SERIALIZER_ENSURE(ser, buf, 1);

    // check null
    if (CRA_BIN_GET_TYPE(buf) == CRA_TYPE_NULL)
    {
        if (!meta->is_ptr || meta->type <= CRA_TYPE_FLOAT)
        {
            CRA_SERIALIZER_PRINT_ERROR("Cannot be null. Name: %s.", CRA_NAME(meta));
            return false;
        }
        *(void **)retval = NULL;
        return true;
    }

    // check type
    if (CRA_BIN_GET_TYPE(buf) != meta->type)
    {
        CRA_SERIALIZER_PRINT_ERROR(
          "Type mismatch. %d != %d. Name: %s.", CRA_BIN_GET_TYPE(buf), meta->type, CRA_NAME(meta));
        return false;
    }

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
            if (meta->szer_i)
                return cra_bin_read_list(ser, retval, meta);
            else
                return cra_bin_read_array(ser, retval, meta);
        case CRA_TYPE_DICT:
            return cra_bin_read_dict(ser, retval, meta);

        default:
            CRA_SERIALIZER_PRINT_ERROR(
              "Invalid type: %d. Please check your meta data. Name: %s.", meta->type, CRA_NAME(meta));
            return false;
    }
}

static bool
cra_bin_write_struct(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    unsigned char     *buf, *numbuf;
    char              *stru;
    int                num;
    const CraTypeMeta *m;

    assert(meta->submeta && meta->submeta->is_not_end); // at least 1 member

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // remaining num of members
    CRA_SERIALIZER_ENSURE(ser, numbuf, 1);
    // write members
    stru = meta->is_ptr ? *(char **)val : (char *)val;
    for (num = 0, m = meta->submeta; m->is_not_end; ++m)
    {
        if (m->is_len)
            continue;

        // write id
        CRA_SERIALIZER_ENSURE(ser, buf, 1);
        *buf = m->id;
        // write member
        if (!cra_bin_write_value(ser, stru + m->offset, m))
        {
            CRA_SERIALIZER_PRINT_ERROR("Write struct's member failed. Name: %s, id: %d.", CRA_NAME(m), m->id);
            return false;
        }
        // check members' count
        if (++num > UINT8_MAX)
        {
            CRA_SERIALIZER_PRINT_ERROR("A struct can have at most %d members.", UINT8_MAX);
            return false;
        }
        assert(meta->size > m->offset);
    }
    // write num of members
    *numbuf = (unsigned char)num;

    CRA_SERIALIZER_NESTING_DEC(ser);

    return true;
}

static bool
cra_bin_skip_one_member(CraSerializer *ser)
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
                if (!cra_bin_skip_one_member(ser))
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
                if (!cra_bin_skip_one_member(ser))
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
                if (!cra_bin_skip_one_member(ser))
                    return false;
                // skip val
                if (!cra_bin_skip_one_member(ser))
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
    uint8_t            num, id;
    const CraTypeMeta *m, *last;

    assert(meta->submeta && meta->submeta->is_not_end); // at least 1 member

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

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

    // read num of members
    CRA_SERIALIZER_ENSURE(ser, buf, 1);
    num = *buf;
    // read members
    m = meta->submeta;
    for (uint8_t i = 0; i < num; ++i)
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
                {
                    CRA_SERIALIZER_PRINT_ERROR(
                      "Read struct's member failed. Member: %s, id: {buf: %d, meta: %d}.", CRA_NAME(m), id, m->id);
                    return false;
                }

                if (!(++m)->is_not_end)
                    m = meta->submeta;
                break;
            }

            if (!(++m)->is_not_end)
                m = meta->submeta;

            // skip this member
            if (m == last)
            {
                if (!cra_bin_skip_one_member(ser))
                {
                    CRA_SERIALIZER_PRINT_ERROR("Skip value failed. id: %d, type: %d.", id, CRA_BIN_GET_TYPE(buf + 1));
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

    assert(meta->submeta);
    assert(!meta->szer_i);
    assert(meta->submeta->is_not_end);
    assert((meta + 1)->is_len && (meta + 1)->type == CRA_TYPE_UINT);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    count = 0;
    metacnt = meta + 1;
    arr = meta->is_ptr ? *(char **)val : (char *)val;
    slot = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;
    pcount = !metacnt->arg ? (void *)((char *)val - meta->offset + metacnt->offset) : metacnt->arg;

    // get count
    if (!cra_serializer_p2u(pcount, &count, metacnt))
    {
        CRA_SERIALIZER_PRINT_ERROR("Get array count failed. Name: %s.", CRA_NAME(meta));
        return false;
    }

    // write count
    if (!__cra_bin_write_varuint(ser, count))
    {
        CRA_SERIALIZER_PRINT_ERROR("Write array count failed. Name: %s.", CRA_NAME(meta));
        return false;
    }

    // write elements
    for (uint64_t i = 0; i < count; ++i)
    {
        if (!cra_bin_write_value(ser, arr + (i * slot), meta->submeta))
        {
            CRA_SERIALIZER_PRINT_ERROR("Write array elements failed. Name: %s.", CRA_NAME(meta));
            return false;
        }
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

    assert(meta->submeta);
    assert(!meta->szer_i);
    assert(meta->submeta->is_not_end);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    metacnt = meta + 1;
    slot = meta->submeta->is_ptr ? sizeof(void *) : meta->submeta->size;
    pcount = !metacnt->arg ? (void *)((char *)retval - meta->offset + metacnt->offset) : metacnt->arg;

    // read count
    if (!__cra_bin_read_varuint(ser, &count))
    {
        CRA_SERIALIZER_PRINT_ERROR("Read array count failed. Name: %s.", CRA_NAME(meta));
        return false;
    }

    if (meta->is_ptr)
    {
        size = (count > 0 ? count : 1) * slot;
        arr = *(char **)retval = (char *)cra_malloc(size);
        cra_release_mgr_add(&ser->release, arr, meta);
    }
    else
    {
        // enough?
        if (meta->size / slot < count)
        {
            CRA_SERIALIZER_PRINT_ERROR(
              "The array[%zu] is too small(< %zu). Name: %s.", meta->size / slot, count, CRA_NAME(meta));
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
        {
            CRA_SERIALIZER_PRINT_ERROR("Read array elements failed. Name: %s.", CRA_NAME(meta));
            break;
        }
    }

    if (ret)
        ret = cra_serializer_u2p(count, pcount, metacnt);
    return ret;
}

static bool
cra_bin_write_list(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    char    *list;
    uint64_t i, count = 0;
    void    *value = NULL;
    char     it[64] = { 0 };

    assert(meta->szer_i);
    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert(meta->szer_i->iter_init && meta->szer_i->iter_next);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    list = meta->is_ptr ? *(char **)val : (char *)val;

    // init it & get count
    meta->szer_i->iter_init(list, &count, it, sizeof(it));

    // write count
    if (!__cra_bin_write_varuint(ser, count))
    {
        CRA_SERIALIZER_PRINT_ERROR("Write list count failed. Name: %s.", CRA_NAME(meta));
        return false;
    }

    // write elements
    for (i = 0; meta->szer_i->iter_next(it, (void **)&value, NULL); ++i)
    {
        if (!cra_bin_write_value(ser, value, meta->submeta))
        {
            CRA_SERIALIZER_PRINT_ERROR("Write list elements failed. Name: %s.", CRA_NAME(meta));
            return false;
        }
    }

    assert(i == count);

    CRA_SERIALIZER_NESTING_DEC(ser);

    return true;
}

static bool
cra_bin_read_list(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    bool     ret;
    char    *list;
    size_t   slot;
    uint64_t count;

    assert(meta->szer_i);
    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert(meta->szer_i->append && meta->init_i->init);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    // read count
    if (!__cra_bin_read_varuint(ser, &count))
    {
        CRA_SERIALIZER_PRINT_ERROR("Read list count failed. Name: %s.", CRA_NAME(meta));
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
    CraInitArgs da = { .size = meta->size, .length = count, .val1size = slot, .val2size = 0, .arg = meta->arg };
    meta->init_i->init(list, &da);

#ifdef __STDC_NO_VLA__
    char *element = (char *)cra_malloc(slot);
#else
    char element[slot];
#endif

    // read elements
    ret = true;
    for (uint64_t i = 0; i < count; ++i)
    {
        // read element
        if (!(ret = cra_bin_read_value(ser, element, meta->submeta)))
        {
            CRA_SERIALIZER_PRINT_ERROR("Read list elements failed. Name: %s.", CRA_NAME(meta));
            break;
        }

        // append
        if (!(ret = meta->szer_i->append(list, element, NULL)))
        {
            CRA_SERIALIZER_PRINT_ERROR("Append list elements failed. Name: %s.", CRA_NAME(meta));
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
cra_bin_write_dict(CraSerializer *ser, void *val, const CraTypeMeta *meta)
{
    char    *dict;
    uint64_t i = 0;
    uint64_t count = 0;
    char     it[64] = { 0 };
    void    *key = NULL, *value = NULL;

    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert((meta->submeta + 1)->is_not_end);
    assert(meta->szer_i && meta->szer_i->iter_init && meta->szer_i->iter_next);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    if (meta->submeta->type >= CRA_TYPE_BYTES)
    {
        CRA_SERIALIZER_PRINT_ERROR("This type(%d) cannot be a key. Name: %s.", meta->submeta->type, CRA_NAME(meta));
        return false;
    }

    dict = meta->is_ptr ? *(char **)val : (char *)val;

    // init it & get count
    meta->szer_i->iter_init(dict, &count, it, sizeof(it));

    // write count
    if (!__cra_bin_write_varuint(ser, count))
    {
        CRA_SERIALIZER_PRINT_ERROR("Write dict count failed. Name: %s.", CRA_NAME(meta));
        return false;
    }

    // write elements
    for (i = 0; meta->szer_i->iter_next(it, (void **)&key, (void **)&value); ++i)
    {
        if (!cra_bin_write_value(ser, key, meta->submeta))
        {
            CRA_SERIALIZER_PRINT_ERROR("Write dict key failed. Name: %s.", CRA_NAME(meta));
            return false;
        }
        if (!cra_bin_write_value(ser, value, meta->submeta + 1))
        {
            CRA_SERIALIZER_PRINT_ERROR("Write dict value failed. Name: %s.", CRA_NAME(meta));
            return false;
        }
    }

    assert(i == count);

    CRA_SERIALIZER_NESTING_DEC(ser);

    return true;
}

static bool
cra_bin_read_dict(CraSerializer *ser, void *retval, const CraTypeMeta *meta)
{
    bool               ret;
    char              *dict;
    uint64_t           count;
    size_t             key_size, val_size;
    const CraTypeMeta *keymeta, *valmeta;

    assert(meta->submeta);
    assert(meta->submeta->is_not_end);
    assert((meta->submeta + 1)->is_not_end);
    assert(meta->szer_i && meta->szer_i->append && meta->init_i->init);

    CRA_SERIALIZER_NESTING_INC_CHECK(ser);

    if (meta->submeta->type >= CRA_TYPE_BYTES)
    {
        CRA_SERIALIZER_PRINT_ERROR("This type(%d) cannot be a key. Name: %s.", meta->submeta->type, CRA_NAME(meta));
        return false;
    }

    // read count
    if (!__cra_bin_read_varuint(ser, &count))
    {
        CRA_SERIALIZER_PRINT_ERROR("Read dict count failed. Name: %s.", CRA_NAME(meta));
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
        .length = count,
        .val1size = key_size,
        .val2size = val_size,
        .arg = meta->arg,
    };
    meta->init_i->init(dict, &da);

#ifdef __STDC_NO_VLA__
    char *key = (char *)cra_malloc(ser, key_size + val_size);
#else
    char key[key_size + val_size];
#endif
    char *value = key + key_size;

    // read elements
    ret = true;
    for (uint64_t i = 0; i < count; ++i)
    {
        // read key
        if (!(ret = cra_bin_read_value(ser, key, keymeta)))
        {
            CRA_SERIALIZER_PRINT_ERROR("Read dict key failed. Name: %s.", CRA_NAME(meta));
            break;
        }
        // read value
        if (!(ret = cra_bin_read_value(ser, value, valmeta)))
        {
            CRA_SERIALIZER_PRINT_ERROR("Read dict value failed. Name: %s.", CRA_NAME(meta));
            break;
        }

        if (!(ret = meta->szer_i->append(dict, key, value)))
        {
            CRA_SERIALIZER_PRINT_ERROR("Append dict K-V pair failed. Name: %s.", CRA_NAME(meta));
            break;
        }
    }

#ifdef __STDC_NO_VLA__
    cra_free(key);
#endif

    CRA_SERIALIZER_NESTING_DEC(ser);

    return ret;
}

bool
cra_bin_serialize(unsigned char *buf, size_t *len, CraSeriObject *obj)
{
    bool          ret;
    CraSerializer ser;

    assert(buf);
    assert(len);
    assert(obj);
    assert(obj->objptr);

    cra_serializer_init(&ser, buf, *len, false);
    ret = cra_bin_write_value(&ser, obj->objptr, obj->meta);
    *len = ser.index;
    cra_serializer_uninit(&ser, ret);
    return ret;
}

bool
cra_bin_deserialize(unsigned char *buf, size_t len, CraSeriObject *retobj)
{
    assert(buf);
    assert(len > 0);
    assert(retobj);
    assert(retobj->objptr);

    bool          ret;
    CraSerializer ser;

    cra_serializer_init(&ser, buf, len, false);
    ret = cra_bin_read_value(&ser, retobj->objptr, retobj->meta);
    cra_serializer_uninit(&ser, ret);
    return ret;
}

void
cra_bin_write_len(unsigned char *buf, uint64_t len, size_t size)
{
    switch (size)
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
            CRA_SERIALIZER_PRINT_ERROR("Invalid length size: %zu.", size);
            exit(EXIT_FAILURE);
            break;
    }
}

uint64_t
cra_bin_read_len(unsigned char *buf, size_t size)
{
    switch (size)
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
            CRA_SERIALIZER_PRINT_ERROR("Invalid length size: %zu.", size);
            exit(EXIT_FAILURE);
            break;
    }
}
