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
#include "cra_serialize_inner.h"
#include "cra_endian.h"
#include "cra_malloc.h"

// length/count encoding:
//      0: null
//      1: 0
//      2: 1
//      ...
//      N: N - 1

#define SWAP_BYTES CRA_IS_BIG_ENDIAN

#define WRITE_META(szer, id, type)                                    \
    do                                                                \
    {                                                                 \
        if (!cra_bin_write_varuint(szer, ((uint64_t)id << 3) | type)) \
            return false;                                             \
    } while (0)
#define READ_META(szer, id, type)                                  \
    do                                                             \
    {                                                              \
        uint64_t CRA_CAT(meta, __LINE__);                          \
        if (!cra_bin_read_varuint(szer, &CRA_CAT(meta, __LINE__))) \
            return false;                                          \
        type = (CraSerType_e)(CRA_CAT(meta, __LINE__) & 0x07);     \
        id = (uint16_t)(CRA_CAT(meta, __LINE__) >> 3);             \
    } while (0)

static bool
cra_bin_write_varuint(CraSerializer *szer, uint64_t val)
{
    unsigned char *buf;
    while (val > 0x7F)
    {
        CRA_SER_ENSURE(szer, 1, buf);
        *buf = (unsigned char)val | 0x80;
        val >>= 7;
    }
    CRA_SER_ENSURE(szer, 1, buf);
    *buf = (unsigned char)val;
    return true;
}

static bool
cra_bin_read_varuint(CraSerializer *szer, uint64_t *retval)
{
    unsigned char *buf;
    uint64_t       val;
    int            shift;

    assert(retval);

    val = 0, shift = 0;
    while (CRA_SER_ENOUGH(szer, 1) && shift < 70)
    {
        buf = szer->buf + szer->curr++;
        if (*buf & 0x80)
        {
            val |= (uint64_t)(*buf & 0x7F) << shift;
            shift += 7;
        }
        else
        {
            val |= (uint64_t)*buf << shift;
            *retval = val;
            return true;
        }
    }

    cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
    return false;
}

static inline bool
cra_bin_write_varint(CraSerializer *szer, int64_t val)
{
    uint64_t u64 = (uint64_t)((val << 1) ^ (val >> 63)); // ZigZag encode
    return cra_bin_write_varuint(szer, u64);
}

static inline bool
cra_bin_read_varint(CraSerializer *szer, int64_t *retval)
{
    uint64_t u64;
    CRA_SER_CHECK(cra_bin_read_varuint(szer, &u64));
    *retval = (int64_t)((u64 >> 1) ^ -(int64_t)(u64 & 1)); // ZigZag decode
    return true;
}

static inline bool
cra_bin_write_bool(CraSerializer *szer, const unsigned char *val, const CraSerInfo *info)
{
    CRA_UNUSED(info);
    unsigned char *buf;
    assert(info->size == sizeof(bool));
    CRA_SER_ENSURE(szer, sizeof(bool), buf);
    *buf = *val;
    return true;
}

static inline bool
cra_bin_read_bool(CraSerializer *szer, unsigned char *retval, const CraSerInfo *info)
{
    CRA_UNUSED(info);
    unsigned char *buf;
    assert(info->size == sizeof(bool));
    CRA_SER_ENSURE(szer, sizeof(bool), buf);
    *retval = !!(*buf);
    return true;
}

static inline bool
cra_bin_write_varint_uint(CraSerializer *szer, const unsigned char *val, const CraSerInfo *info)
{
    uint8_t  u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;

    switch (info->size)
    {
        case sizeof(uint8_t):
            memcpy(&u8, val, sizeof(u8));
            u64 = u8;
            break;
        case sizeof(uint16_t):
            memcpy(&u16, val, sizeof(u16));
            u64 = u16;
            break;
        case sizeof(uint32_t):
            memcpy(&u32, val, sizeof(u32));
            u64 = u32;
            break;
        case sizeof(uint64_t):
            memcpy(&u64, val, sizeof(u64));
            break;

        default:
            abort();
    }

    return cra_bin_write_varuint(szer, u64);
}

static inline bool
cra_bin_read_varint_uint(CraSerializer *szer, unsigned char *val, const CraSerInfo *info)
{
    uint64_t u64;
    uint32_t u32;
    uint16_t u16;
    uint8_t  u8;

    CRA_SER_CHECK(cra_bin_read_varuint(szer, &u64));

    switch (info->size)
    {
        case sizeof(uint8_t):
            u8 = (uint8_t)u64;
            memcpy(val, &u8, sizeof(u8));
            break;
        case sizeof(uint16_t):
            u16 = (uint16_t)u64;
            memcpy(val, &u16, sizeof(u16));
            break;
        case sizeof(uint32_t):
            u32 = (uint32_t)u64;
            memcpy(val, &u32, sizeof(u32));
            break;
        case sizeof(uint64_t):
            memcpy(val, &u64, sizeof(u64));
            break;

        default:
            abort();
    }

    return true;
}

static inline bool
cra_bin_write_varint_int(CraSerializer *szer, const unsigned char *val, const CraSerInfo *info)
{
    int8_t  i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;

    switch (info->size)
    {
        case sizeof(int8_t):
            memcpy(&i8, val, sizeof(i8));
            i64 = i8;
            break;
        case sizeof(int16_t):
            memcpy(&i16, val, sizeof(i16));
            i64 = i16;
            break;
        case sizeof(int32_t):
            memcpy(&i32, val, sizeof(i32));
            i64 = i32;
            break;
        case sizeof(int64_t):
            memcpy(&i64, val, sizeof(i64));
            break;

        default:
            abort();
    }

    return cra_bin_write_varint(szer, i64);
}

static inline bool
cra_bin_read_varint_int(CraSerializer *szer, unsigned char *val, const CraSerInfo *info)
{
    int64_t i64;
    int32_t i32;
    int16_t i16;
    int8_t  i8;

    CRA_SER_CHECK(cra_bin_read_varint(szer, &i64));

    switch (info->size)
    {
        case sizeof(int8_t):
            i8 = (int8_t)i64;
            memcpy(val, &i8, sizeof(i8));
            break;
        case sizeof(int16_t):
            i16 = (int16_t)i64;
            memcpy(val, &i16, sizeof(i16));
            break;
        case sizeof(int32_t):
            i32 = (int32_t)i64;
            memcpy(val, &i32, sizeof(i32));
            break;
        case sizeof(int64_t):
            memcpy(val, &i64, sizeof(i64));
            break;

        default:
            abort();
    }

    return true;
}

static inline bool
cra_bin_write_varint_number(CraSerializer *szer, const unsigned char *val, const CraSerInfo *info)
{
    if (info->flags & CRA_SER_FLAG_UNSIGNED)
        return cra_bin_write_varint_uint(szer, val, info);
    return cra_bin_write_varint_int(szer, val, info);
}

static inline bool
cra_bin_read_varint_number(CraSerializer *szer, unsigned char *val, const CraSerInfo *info)
{
    if (info->flags & CRA_SER_FLAG_UNSIGNED)
        return cra_bin_read_varint_uint(szer, val, info);
    return cra_bin_read_varint_int(szer, val, info);
}

static inline bool
cra_bin_write_number(CraSerializer *szer, const unsigned char *val, const CraSerInfo *info)
{
    unsigned char *buf;

    assert(info->size == sizeof(uint32_t) || info->size == sizeof(uint64_t));

    CRA_SER_ENSURE(szer, info->size, buf);
    memcpy(buf, val, info->size);
#if SWAP_BYTES
    for (uint16_t i = 0; i < info->size / 2; ++i)
    {
        uint8_t t = buf[i];
        buf[i] = buf[info->size - 1 - i];
        buf[info->size - 1 - i] = t;
    }
#endif
    return true;
}

static inline bool
cra_bin_read_number(CraSerializer *szer, unsigned char *val, const CraSerInfo *info)
{
    unsigned char *buf;

    assert(info->size == sizeof(uint32_t) || info->size == sizeof(uint64_t));

    CRA_SER_ENSURE(szer, info->size, buf);
    memcpy(val, buf, info->size);
#if SWAP_BYTES
    for (uint16_t i = 0; i < info->size / 2; ++i)
    {
        uint8_t t = val[i];
        val[i] = val[info->size - 1 - i];
        val[info->size - 1 - i] = t;
    }
#endif
    return true;
}

static bool
cra_bin_write_string(CraSerializer *szer, const unsigned char *val, const CraSerInfo *info)
{
    uint64_t       u64;
    uint64_t       len;
    const char    *str;
    unsigned char *buf;

    if (info->flags & CRA_SER_FLAG_PTR)
        memcpy(&str, val, sizeof(char *)); // str = *(char **)val;
    else
        str = (const char *)val;

    if (str)
    {
        len = strlen(str);
        CRA_SER_CHECK_LIMIT(szer, len, >=, CRA_SER_STRING_MAX_LENGTH);
        u64 = len + 1;
    }
    else
    {
        len = 0;
        u64 = 0;
    }

    // write length
    CRA_SER_CHECK(cra_bin_write_varuint(szer, u64));

    // write value
    if (len > 0)
    {
        assert(cra_string_view_is_valid_utf8_cstr(str, len));
        CRA_SER_ENSURE(szer, len, buf);
        memcpy(buf, str, len);
    }

    return true;
}

static bool
cra_bin_read_string(CraSerializer *szer, unsigned char *val, const CraSerInfo *info)
{
    uint64_t       u64;
    size_t         len;
    unsigned char *str;
    unsigned char *buf;

    // read length
    CRA_SER_CHECK(cra_bin_read_varuint(szer, &u64));
    CRA_SER_CHECK_LIMIT(szer, u64, >, CRA_SER_STRING_MAX_LENGTH);
    len = u64 > 0 ? (size_t)(u64 - 1) : 0;

    CRA_SER_ENSURE(szer, len, buf);

    // check UTF-8 string
    if (len > 0 && !cra_string_view_is_valid_utf8_cstr((char *)buf, len))
    {
        // invalid UTF-8 string
        cra_ser_error_set(szer, CRA_SER_ERR_INVALID_VALUE);
        return false;
    }

    // make string
    CRA_SER_NEW_STRING(szer, val, str, info, u64, len);

    // read value
    if (len > 0)
        memcpy(str, buf, len);

    return true;
}

static bool
cra_bin_write_struct(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta);
static bool
cra_bin_read_struct(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta);
static bool
cra_bin_write_array(CraSerializer *szer, const unsigned char *val, const CraSerMeta *meta);
static bool
cra_bin_read_array(CraSerializer *szer, unsigned char *val, const CraSerMeta *meta);
static bool
cra_bin_write_list(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta);
static bool
cra_bin_read_list(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta);
static bool
cra_bin_write_dict(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta);
static bool
cra_bin_read_dict(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta);

static bool
cra_bin_write_value(CraSerializer       *szer,
                    const unsigned char *val,
                    const CraSerMeta    *meta,
                    const CraSerInfo    *info,
                    uint16_t             index)
{
    assert(val);
    assert(info);
    assert(meta);
    assert(meta->metas);
    assert(!meta->metas[index] || info->size == meta->metas[index]->size);

    switch (info->type)
    {
        case CRA_SER_TYPE_BOOL:
            return cra_bin_write_bool(szer, val, info);
        case CRA_SER_TYPE_VARINT:
            return cra_bin_write_varint_number(szer, val, info);
        case CRA_SER_TYPE_32BITS:
        case CRA_SER_TYPE_64BITS:
            return cra_bin_write_number(szer, val, info);
        case CRA_SER_TYPE_STRING:
            return cra_bin_write_string(szer, val, info);
        case CRA_SER_TYPE_STRUCT:
            return cra_bin_write_struct(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[index]);
        case CRA_SER_TYPE_LIST:
            if (info->flags & CRA_SER_FLAG_ARRAY)
                return cra_bin_write_array(szer, val, meta->metas[index]);
            return cra_bin_write_list(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[index]);
        case CRA_SER_TYPE_DICT:
            return cra_bin_write_dict(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[index]);

        default:
            // unreachable
            abort();
    }
}

static bool
cra_bin_read_value(CraSerializer    *szer,
                   unsigned char    *val,
                   const CraSerMeta *meta,
                   const CraSerInfo *info,
                   uint16_t          index)
{
    assert(val);
    assert(info);
    assert(meta);
    assert(meta->metas);
    assert(!meta->metas[index] || info->size == meta->metas[index]->size);

    switch (info->type)
    {
        case CRA_SER_TYPE_BOOL:
            return cra_bin_read_bool(szer, val, info);
        case CRA_SER_TYPE_VARINT:
            return cra_bin_read_varint_number(szer, val, info);
        case CRA_SER_TYPE_32BITS:
        case CRA_SER_TYPE_64BITS:
            return cra_bin_read_number(szer, val, info);
        case CRA_SER_TYPE_STRING:
            return cra_bin_read_string(szer, val, info);
        case CRA_SER_TYPE_STRUCT:
            return cra_bin_read_struct(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[index]);
        case CRA_SER_TYPE_LIST:
            if (info->flags & CRA_SER_FLAG_ARRAY)
                return cra_bin_read_array(szer, val, meta->metas[index]);
            return cra_bin_read_list(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[index]);
        case CRA_SER_TYPE_DICT:
            return cra_bin_read_dict(szer, val, info->flags & CRA_SER_FLAG_PTR, meta->metas[index]);

        default:
            // never reach here
            abort();
    }
}

static bool
cra_bin_skip_member(CraSerializer *szer, CraSerType_e type)
{
    uint16_t     id;
    CraSerType_e typ;
    CraSerType_e typ2;
    size_t       u64;

    CRA_UNUSED(id);

    switch (type)
    {
#define XX(TYPE, size)                  \
    case CRA_CAT(CRA_SER_TYPE_, TYPE):  \
        if (CRA_SER_ENOUGH(szer, size)) \
        {                               \
            szer->curr += size;         \
            return true;                \
        }                               \
        return false
#define XXX()                              \
    if (!cra_bin_read_varuint(szer, &u64)) \
        return false;                      \
    u64 = u64 > 0 ? u64 - 1 : 0

        XX(BOOL, sizeof(bool));
        XX(32BITS, sizeof(uint32_t));
        XX(64BITS, sizeof(uint64_t));
        case CRA_SER_TYPE_VARINT:
            return cra_bin_read_varuint(szer, &u64);
        case CRA_SER_TYPE_STRING:
            XXX();
            if (!CRA_SER_ENOUGH(szer, u64)) // skip string data
                return false;
            szer->curr += u64;
            return true;
        case CRA_SER_TYPE_STRUCT:
            CRA_SER_NESTING_INC(szer);
            XXX();
            for (uint64_t i = 0; i < u64; ++i)
            {
                READ_META(szer, id, typ);            // read struct field meta
                if (!cra_bin_skip_member(szer, typ)) // skip struct field
                    return false;
            }
            CRA_SER_NESTING_DEC(szer);
            return true;
        case CRA_SER_TYPE_LIST:
            CRA_SER_NESTING_INC(szer);
            XXX();
            READ_META(szer, id, typ); // read list item meta
            for (uint64_t i = 0; i < u64; ++i)
            {
                if (!cra_bin_skip_member(szer, typ)) // skip list item
                    return false;
            }
            CRA_SER_NESTING_DEC(szer);
            return true;
        case CRA_SER_TYPE_DICT:
            CRA_SER_NESTING_INC(szer);
            XXX();
            READ_META(szer, id, typ);  // read dict key meta
            READ_META(szer, id, typ2); // read dict value meta
            for (uint64_t i = 0; i < u64; ++i)
            {
                if (!cra_bin_skip_member(szer, typ)) // skip dict key
                    return false;
                if (!cra_bin_skip_member(szer, typ2)) // skip dict value
                    return false;
            }
            CRA_SER_NESTING_DEC(szer);
            return true;

        default:
            // unknown type
            return false;

#undef XXX
#undef XX
    }
}

static bool
cra_bin_write_struct(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    const unsigned char *stru;
    const CraSerInfo    *info;
    uint16_t             index;
    uint16_t             count;
    uint16_t             u16;

    assert(val);
    assert(meta);

    CRA_SER_NESTING_INC(szer);

    if (is_ptr)
        memcpy(&stru, val, sizeof(void *)); // stru = *(T **)val
    else
        stru = val;

    if (stru)
    {
        CRA_SER_CHECK_LIMIT(szer, meta->count, >=, CRA_SER_STRUCT_MAX_FIELDS);
        count = meta->count;
        u16 = count + 1;
    }
    else
    {
        count = 0;
        u16 = 0;
    }

    // write count
    CRA_SER_CHECK(cra_bin_write_varuint(szer, u16));

    // write member values
    for (index = 0; index < count; ++index)
    {
        info = &meta->infos[index];
        WRITE_META(szer, meta->for_struct.ids[index], info->type);
        if (!cra_bin_write_value(szer, stru + info->offset, meta, info, index))
        {
            CraStringView sv = cra_ser_get_field_name(meta, index);
            cra_ser_error_append(szer, &sv);
            return false;
        }
    }

    CRA_SER_NESTING_DEC(szer);

    return true;
}

static bool
cra_bin_read_struct(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    uint16_t          id;
    CraSerType_e      type;
    uint16_t          count, i, j, k;
    const CraSerInfo *info;
    uint64_t          u64;
    unsigned char    *stru;
    unsigned char    *item;
    CraStringView     sv;

    CRA_SER_DEF_IDS_BITMAP(id_bitmap);

    assert(val);
    assert(meta);

    CRA_SER_NESTING_INC(szer);

    // read count
    CRA_SER_CHECK(cra_bin_read_varuint(szer, &u64));
    CRA_SER_CHECK_LIMIT(szer, meta->count, >, CRA_SER_STRUCT_MAX_FIELDS);
    count = u64 > 0 ? (uint16_t)(u64 - 1) : 0;

    // create struct
    CRA_SER_NEW_STRUCT(szer, val, stru, meta, is_ptr, u64 == 0);

    // read member values
    for (i = 0, j = 0; i < count; ++i)
    {
        READ_META(szer, id, type);

        // TODO: SIMD optimization
        // find struct member info by id
        for (k = j;;)
        {
            assert(j < meta->count);
            if (meta->for_struct.ids[j] == id)
            {
                k = j;
                info = &meta->infos[k];
                item = stru + info->offset;
                // j = (j + 1) % meta->count;
                if (++j >= meta->count)
                    j = 0;
                break;
            }

            // j = (j + 1) % meta->count;
            if (++j >= meta->count)
                j = 0;

            if (j == k)
            {
                // skip it
                if (!cra_bin_skip_member(szer, type))
                {
                    cra_ser_error_set(szer, CRA_SER_ERR_SKIP_MEMBER_FAILED);
                    goto fail2;
                }
                goto outer_loop;
            }
        }

        // check type
        if (type != info->type)
        {
            cra_ser_error_set(szer, CRA_SER_ERR_TYPE_MISMATCH);
            goto fail;
        }

        // check duplicate member
        if (cra_ser_check_id_duplicate(id_bitmap, id))
        {
            cra_ser_error_set(szer, CRA_SER_ERR_DUPLICATE_MEMBER);
            goto fail;
        }

        // read value
        if (!cra_bin_read_value(szer, item, meta, info, k))
        {
        fail:
            sv = cra_ser_get_field_name(meta, k);
            cra_ser_error_append(szer, &sv);
        fail2:
            CRA_SER_DEL_STRUCT(val, stru, meta, is_ptr);
            return false;
        }

    outer_loop:;
    }

    CRA_SER_NESTING_DEC(szer);

    return true;
}

static bool
cra_bin_write_array(CraSerializer *szer, const unsigned char *val, const CraSerMeta *meta)
{
    uint64_t             u64;
    uint16_t             index;
    uint16_t             count;
    const unsigned char *array;
    const CraSerInfo    *info;

    assert(val);
    assert(meta);

    CRA_SER_NESTING_INC(szer);

    // get array count
    memcpy(&count, val + meta->for_array.offsetof_len, sizeof(count));
    CRA_SER_CHECK_LIMIT(szer, count, >=, CRA_SER_LIST_MAX_COUNT);

    array = val;
    // meta->count == 0 -> Array { T *data; ... }
    if (meta->count == 0)
        memcpy(&array, array, sizeof(void *)); // array = *(T **)array;
    assert(array || (!array && count == 0));

    u64 = array ? count + 1 : 0;
    info = &meta->infos[0];

    // write count
    CRA_SER_CHECK(cra_bin_write_varuint(szer, u64));
    // write item meta data
    WRITE_META(szer, 0, info->type);

    // write array items
    for (index = 0; index < count; ++index)
    {
        if (!cra_bin_write_value(szer, array + index * meta->for_array.size_of_slot, meta, info, 0))
        {
            CraStringView sv = CRA_STRING_VIEW_INIT_CONST("<<ArrayItem>>");
            cra_ser_error_append(szer, &sv);
            return false;
        }
    }

    CRA_SER_NESTING_DEC(szer);

    return true;
}

static bool
cra_bin_read_array(CraSerializer *szer, unsigned char *val, const CraSerMeta *meta)
{
    bool              b;
    uint16_t          id;
    CraSerType_e      type;
    uint16_t          index;
    uint16_t          count;
    unsigned char    *array;
    const CraSerInfo *info;
    uint64_t          u64;

    assert(val);
    assert(meta);
    CRA_UNUSED(id);

    CRA_SER_NESTING_INC(szer);

    b = true;

    // read count
    CRA_SER_CHECK(cra_bin_read_varuint(szer, &u64));
    CRA_SER_CHECK_LIMIT(szer, u64, >, CRA_SER_LIST_MAX_COUNT);
    count = u64 > 0 ? (uint16_t)(u64 - 1) : 0;

    info = &meta->infos[0];

    // read item meta data
    READ_META(szer, id, type);
    // check item type
    if (type != info->type)
    {
        cra_ser_error_set(szer, CRA_SER_ERR_TYPE_MISMATCH);
        return false;
    }

    // create array items
    CRA_SER_NEW_ARRAY(szer, val, array, meta, u64, count);

    // read array items
    for (index = 0; index < count; ++index)
    {
        if (!cra_bin_read_value(szer, array + index * meta->for_array.size_of_slot, meta, info, 0))
        {
            b = false;
            CraStringView sv = CRA_STRING_VIEW_INIT_CONST("<<ArrayItem>>");
            cra_ser_error_append(szer, &sv);
            break;
        }
    }

    // update array count
    memcpy(val + meta->for_array.offsetof_len, &index, sizeof(index));

    CRA_SER_NESTING_DEC(szer);

    if (!b)
        CRA_SER_DEL_ARRAY(val, array, meta, index);
    return b;
}

static bool
cra_bin_write_list(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    size_t               st;
    uint16_t             index;
    uint16_t             count;
    uint16_t             slotsize;
    unsigned char        iter[64] = { 0 };
    const unsigned char *list;
    const CraSerInfo    *info;

    assert(val);
    assert(meta);
    assert(meta->for_list.iter_init);
    assert(meta->for_list.iter_next);

    CRA_SER_NESTING_INC(szer);

    if (is_ptr)
        memcpy(&list, val, sizeof(void *)); // list = *(T **)val;
    else
        list = val;

    if (list)
    {
        meta->for_list.iter_init(iter, sizeof(iter), (void *)list, &st);
        CRA_SER_CHECK_LIMIT(szer, st, >=, CRA_SER_LIST_MAX_COUNT);
        count = (uint16_t)st;
        ++st;
    }
    else
    {
        count = 0;
        st = 0;
    }

    info = &meta->infos[0];

    // write count
    CRA_SER_CHECK(cra_bin_write_varuint(szer, st));
    // write item meta data
    WRITE_META(szer, 0, info->type);

    if (count == 0)
        goto end;

    // write list items
    slotsize = (info->flags & CRA_SER_FLAG_PTR) ? sizeof(void *) : info->size;
    CRA_TEMP_NEW(unsigned char, value, slotsize);
    if (!value)
    {
        cra_ser_error_set(szer, CRA_SER_ERR_NO_MEM);
        return false;
    }
    for (index = 0; meta->for_list.iter_next(iter, value); ++index)
    {
        if (!cra_bin_write_value(szer, value, meta, info, 0))
        {
            CRA_TEMP_DEL(unsigned char, value, slotsize);
            CraStringView sv = CRA_STRING_VIEW_INIT_CONST("<<ListItem>>");
            cra_ser_error_append(szer, &sv);
            return false;
        }
    }
    CRA_TEMP_DEL(unsigned char, value, slotsize);
    assert(index == count);

end:
    CRA_SER_NESTING_DEC(szer);

    return true;
}

static bool
cra_bin_read_list(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    uint16_t          id;
    uint16_t          slotsize;
    uint16_t          index;
    uint16_t          count;
    CraSerType_e      type;
    uint64_t          u64;
    const CraSerInfo *info;
    unsigned char    *list;

    assert(val);
    assert(meta);
    assert(meta->for_list.init);
    assert(meta->for_list.append);
    CRA_UNUSED(id);

    CRA_SER_NESTING_INC(szer);

    // read count
    CRA_SER_CHECK(cra_bin_read_varuint(szer, &u64));
    CRA_SER_CHECK_LIMIT(szer, u64, >, CRA_SER_LIST_MAX_COUNT);
    count = u64 > 0 ? (uint16_t)(u64 - 1) : 0;

    info = &meta->infos[0];

    // read item meta data
    READ_META(szer, id, type);
    // check item type
    if (type != info->type)
    {
        cra_ser_error_set(szer, CRA_SER_ERR_TYPE_MISMATCH);
        return false;
    }

    // create list
    slotsize = (info->flags & CRA_SER_FLAG_PTR) ? sizeof(void *) : info->size;
    CRA_SER_NEW_LIST(szer, val, list, meta, is_ptr, u64, count, slotsize);

    CRA_TEMP_NEW(unsigned char, value, slotsize);
    if (!value)
    {
        cra_ser_error_set(szer, CRA_SER_ERR_NO_MEM);
        goto fail2;
    }
    for (index = 0; index < count; ++index)
    {
        if (!cra_bin_read_value(szer, value, meta, info, 0))
        {
        fail:
            CRA_TEMP_DEL(unsigned char, value, slotsize);
            CraStringView sv = CRA_STRING_VIEW_INIT_CONST("<<ListItem>>");
            cra_ser_error_append(szer, &sv);
        fail2:
            CRA_SER_DEL_LIST(val, list, meta, is_ptr);
            return false;
        }
        if (!meta->for_list.append(list, value))
        {
            cra_ser_error_set(szer, CRA_SER_ERR_APPEND_FAILED);
            goto fail;
        }
    }
    CRA_TEMP_DEL(unsigned char, value, slotsize);
    assert(index == count);

    CRA_SER_NESTING_DEC(szer);

    return true;
}

static bool
cra_bin_write_dict(CraSerializer *szer, const unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    size_t               st;
    uint16_t             index;
    uint16_t             count;
    uint16_t             keysize;
    uint16_t             valsize;
    unsigned char        iter[64] = { 0 };
    const unsigned char *dict;
    const CraSerInfo    *kinfo;
    const CraSerInfo    *vinfo;

    assert(val);
    assert(meta);
    assert(meta->for_dict.iter_init);
    assert(meta->for_dict.iter_next);
    assert(meta->infos[0].type <= CRA_SER_TYPE_STRING);

    CRA_SER_NESTING_INC(szer);

    if (is_ptr)
        memcpy(&dict, val, sizeof(void *)); // dict = *(T **)val;
    else
        dict = val;

    if (dict)
    {
        meta->for_dict.iter_init(iter, sizeof(iter), (void *)dict, &st);
        CRA_SER_CHECK_LIMIT(szer, st, >=, CRA_SER_DICT_MAX_COUNT);
        count = (uint16_t)st;
        ++st;
    }
    else
    {
        count = 0;
        st = 0;
    }

    kinfo = &meta->infos[0];
    vinfo = &meta->infos[1];

    // write count
    CRA_SER_CHECK(cra_bin_write_varuint(szer, st));
    // write key meta data
    WRITE_META(szer, 0, kinfo->type);
    // write value meta data
    WRITE_META(szer, 0, vinfo->type);

    if (count == 0)
        goto end;

    // write dict entries
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
        if (!cra_bin_write_value(szer, key, meta, kinfo, 0))
            goto fail;
        if (!cra_bin_write_value(szer, value, meta, vinfo, 1))
        {
        fail:
            CRA_TEMP_DEL(unsigned char, key, keysize + valsize);
            CraStringView sv = CRA_STRING_VIEW_INIT_CONST("<<DictItem>>");
            cra_ser_error_append(szer, &sv);
            return false;
        }
    }
    CRA_TEMP_DEL(unsigned char, key, keysize + valsize);
    assert(index == count);

end:
    CRA_SER_NESTING_DEC(szer);

    return true;
}

static bool
cra_bin_read_dict(CraSerializer *szer, unsigned char *val, bool is_ptr, const CraSerMeta *meta)
{
    uint16_t          id;
    uint16_t          index;
    uint16_t          count;
    CraSerType_e      ktype;
    CraSerType_e      vtype;
    uint16_t          keysize;
    uint16_t          valsize;
    uint64_t          u64;
    const CraSerInfo *kinfo;
    const CraSerInfo *vinfo;
    unsigned char    *dict;

    assert(val);
    assert(meta);
    assert(meta->for_dict.init);
    assert(meta->for_dict.append);
    assert(meta->infos[0].type <= CRA_SER_TYPE_STRING);
    CRA_UNUSED(id);

    CRA_SER_NESTING_INC(szer);

    // read count
    CRA_SER_CHECK(cra_bin_read_varuint(szer, &u64));
    CRA_SER_CHECK_LIMIT(szer, u64, >, CRA_SER_DICT_MAX_COUNT);
    count = u64 > 0 ? (uint16_t)(u64 - 1) : 0;

    kinfo = &meta->infos[0];
    vinfo = &meta->infos[1];

    // read key meta data
    READ_META(szer, id, ktype);
    // read value meta data
    READ_META(szer, id, vtype);
    // check type
    if (ktype != kinfo->type || vtype != vinfo->type)
    {
        cra_ser_error_set(szer, CRA_SER_ERR_TYPE_MISMATCH);
        return false;
    }

    // create dict
    keysize = (kinfo->flags & CRA_SER_FLAG_PTR) ? sizeof(void *) : kinfo->size;
    valsize = (vinfo->flags & CRA_SER_FLAG_PTR) ? sizeof(void *) : vinfo->size;
    CRA_SER_NEW_DICT(szer, val, dict, meta, is_ptr, u64, count, keysize, valsize);

    CRA_TEMP_NEW(unsigned char, key, keysize + valsize);
    if (!key)
    {
        cra_ser_error_set(szer, CRA_SER_ERR_NO_MEM);
        goto fail3;
    }
    unsigned char *value = key + keysize;
    for (index = 0; index < count; ++index)
    {
        if (!cra_bin_read_value(szer, key, meta, kinfo, 0))
            goto fail2;
        if (!cra_bin_read_value(szer, value, meta, vinfo, 1))
        {
        fail:
            if (kinfo->flags & CRA_SER_FLAG_PTR)
            {
                unsigned char *p;
                assert(kinfo->type == CRA_SER_TYPE_STRING);
                memcpy(&p, key, sizeof(void *));
                cra_free(p);
            }
        fail2:
            CRA_TEMP_DEL(unsigned char, key, keysize + valsize);
            CraStringView sv = CRA_STRING_VIEW_INIT_CONST("<<DictItem>>");
            cra_ser_error_append(szer, &sv);
        fail3:
            CRA_SER_DEL_DICT(val, dict, meta, is_ptr);
            return false;
        }

        if (!meta->for_dict.append(dict, key, value))
        {
            cra_ser_error_set(szer, CRA_SER_ERR_APPEND_FAILED);
            goto fail;
        }
    }
    CRA_TEMP_DEL(unsigned char, key, keysize + valsize);
    assert(index == count);

    CRA_SER_NESTING_DEC(szer);

    return true;
}

bool
cra_bin_serialize_err(unsigned char *buf, size_t *len, const void *obj, const CraSerMeta *meta, CraSerErr *error)
{
    CraSerializer szer;

    assert(buf);
    assert(len);
    assert(obj);
    assert(meta);
    assert(meta->infos);
    assert(meta->infos->offset == 0);

    cra_serializer_init(&szer, buf, *len, false);

    // write root meta data
    WRITE_META(&szer, 0, meta->type);

    switch (meta->type)
    {
        case CRA_SER_TYPE_STRUCT:
            cra_bin_write_struct(&szer, obj, false, meta);
            break;
        case CRA_SER_TYPE_LIST:
            if (meta->flags & CRA_SER_FLAG_ARRAY)
                cra_bin_write_array(&szer, obj, meta);
            else
                cra_bin_write_list(&szer, obj, false, meta);
            break;
        case CRA_SER_TYPE_DICT:
            cra_bin_write_dict(&szer, obj, false, meta);
            break;

        default:
            // unreachable
            abort();
    }

    *len = szer.curr;
    if (error)
        memcpy(error, &szer.error, sizeof(CraSerErr));

    assert(szer.error.err || szer.nesting == 0);

    return szer.error.err == CRA_SER_ERR_OK;
}

bool
cra_bin_deserialize_err(const unsigned char *buf, size_t len, void *obj, const CraSerMeta *meta, CraSerErr *error)
{
    uint16_t      id;
    CraSerType_e  type;
    CraSerializer szer;

    assert(buf);
    assert(len);
    assert(obj);
    assert(meta);
    assert(meta->infos);
    assert(meta->infos->offset == 0);
    CRA_UNUSED(id);

    cra_serializer_init(&szer, (void *)buf, len, false);

    // read root meta data
    READ_META(&szer, id, type);
    // check root meta data
    if (type != (CraSerType_e)meta->type)
    {
        cra_ser_error_set(&szer, CRA_SER_ERR_TYPE_MISMATCH);
        return false;
    }

    switch (type)
    {
        case CRA_SER_TYPE_STRUCT:
            cra_bin_read_struct(&szer, obj, false, meta);
            break;
        case CRA_SER_TYPE_LIST:
            if (meta->flags & CRA_SER_FLAG_ARRAY)
                cra_bin_read_array(&szer, obj, meta);
            else
                cra_bin_read_list(&szer, obj, false, meta);
            break;
        case CRA_SER_TYPE_DICT:
            cra_bin_read_dict(&szer, obj, false, meta);
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
