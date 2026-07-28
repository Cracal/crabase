/**
 * @file cra_serialize.h
 * @author Cracal
 * @brief
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_SERIALIZE_H__
#define __CRA_SERIALIZE_H__
#include "cra_defs.h"
#include "cra_ifs.h"

typedef struct CraSeriObject CraSeriObject;
typedef struct CraTypeMeta   CraTypeMeta;
typedef struct CraSerErr     CraSerErr;

typedef enum
{
    CRA_SER_ERR_OK = 0,
    CRA_SER_ERR_NOBUF,
    CRA_SER_ERR_ALLOC,
    CRA_SER_ERR_LENGTH,
    CRA_SER_ERR_NESTING,
    CRA_SER_ERR_TOO_SMALL,
    CRA_SER_ERR_ADD_FAILED,
    CRA_SER_ERR_INIT_FAILED,
    CRA_SER_ERR_INVALID_VAL,
    CRA_SER_ERR_SIZE_MISMATCH,
    CRA_SER_ERR_TYPE_MISMATCH,
    CRA_SER_ERR_CANNOT_BE_KEY,
    CRA_SER_ERR_CANNOT_BE_NULL,
} CraSerErr_e;

struct CraSerErr
{
    CraSerErr_e err;
    char        msg[124];
};

typedef enum
{
    CRA_TYPE_NULL,    // NULL
    CRA_TYPE_BOOL,    // bool
    CRA_TYPE_INT,     // int[8|16|32|64]
    CRA_TYPE_UINT,    // uint[8|16|32|64]
    CRA_TYPE_VARINT,  // variable length int
    CRA_TYPE_VARUINT, // variable length uint
    CRA_TYPE_FLOAT,   // float[32|64]
    CRA_TYPE_STRING,  // string
    CRA_TYPE_BYTES,   // bytes
    CRA_TYPE_STRUCT,  // struct
    CRA_TYPE_LIST,    // array|list
    CRA_TYPE_DICT,    // dict
} CraType_e;

struct CraTypeMeta
{
    bool               is_not_end;
    bool               is_len;
    bool               is_ptr;
    uint8_t            id;
    CraType_e          type;
    const char *const  name;
    size_t             size;
    size_t             offset;
    const CraTypeMeta *submeta;
    CRA_ITERABLE_DEF(*iter_i);
    CRA_APPENDABLE_DEF(*append_i);
    CRA_INITIALIZABLE_DEF(*init_i);
    void *arg;
};

struct CraSeriObject
{
    void       *objptr;
    CraTypeMeta meta[3];
};

#define __CRA_SFtrue(member)  sizeof(*(member))
#define __CRA_SF1             __CRA_SFtrue
#define __CRA_SFTRUE          __CRA_SFtrue
#define __CRA_SFTrue          __CRA_SFtrue
#define __CRA_SFfalse(member) sizeof(member)
#define __CRA_SF0             __CRA_SFfalse
#define __CRA_SFFALSE         __CRA_SFfalse
#define __CRA_SFFalse         __CRA_SFfalse

#define CRA_TYPE_META_DECL(meta_name)        CraTypeMeta meta_name[]
#define CRA_TYPE_META_DECL_CONST(meta_name)  const CRA_TYPE_META_DECL
#define CRA_TYPE_META_BEGIN(meta_name)       CRA_TYPE_META_DECL(meta_name) = {
#define CRA_TYPE_META_BEGIN_CONST(meta_name) const CRA_TYPE_META_BEGIN(meta_name)
#define CRA_TYPE_META_END() {0}}

// set interfaces
#define CRA_TYPE_META_SET_I(meta, _iter_i, _append_i, _init_i, _arg)                                             \
    (void)((meta)->iter_i = _iter_i, (meta)->append_i = _append_i, (meta)->init_i = _init_i, (meta)->arg = _arg)

// member meta

#define __CRA_TYPE_META_MEMBER(Type, member, _is_len, _id, _is_ptr, TYPE, _submeta, _iter_i, _append_i, _init_i, _arg) \
    {                                                                                                                  \
        .is_not_end = true,                                                                                            \
        .is_len = _is_len,                                                                                             \
        .is_ptr = _is_ptr,                                                                                             \
        .id = _id,                                                                                                     \
        .type = TYPE,                                                                                                  \
        .name = #member,                                                                                               \
        .size = __CRA_SF##_is_ptr(((Type *)0)->member),                                                                \
        .offset = offsetof(Type, member),                                                                              \
        .submeta = _submeta,                                                                                           \
        .iter_i = _iter_i,                                                                                             \
        .append_i = _append_i,                                                                                         \
        .init_i = _init_i,                                                                                             \
        .arg = _arg,                                                                                                   \
    },
#define __CRA_TYPE_META_MEMBER_BASE(Type, member, id, is_ptr, TYPE)                             \
    __CRA_TYPE_META_MEMBER(Type, member, false, id, is_ptr, TYPE, NULL, NULL, NULL, NULL, NULL)

// bool
#define CRA_TYPE_META_MEMBER_BOOL(Type, member, id) __CRA_TYPE_META_MEMBER_BASE(Type, member, id, false, CRA_TYPE_BOOL)
// int
#define CRA_TYPE_META_MEMBER_INT(Type, member, id)  __CRA_TYPE_META_MEMBER_BASE(Type, member, id, false, CRA_TYPE_INT)
// uint
#define CRA_TYPE_META_MEMBER_UINT(Type, member, id) __CRA_TYPE_META_MEMBER_BASE(Type, member, id, false, CRA_TYPE_UINT)
// varint
#define CRA_TYPE_META_MEMBER_VARINT(Type, member, id)                     \
    __CRA_TYPE_META_MEMBER_BASE(Type, member, id, false, CRA_TYPE_VARINT)
// varuint
#define CRA_TYPE_META_MEMBER_VARUINT(Type, member, id)                     \
    __CRA_TYPE_META_MEMBER_BASE(Type, member, id, false, CRA_TYPE_VARUINT)
// float
#define CRA_TYPE_META_MEMBER_FLOAT(Type, member, id)                     \
    __CRA_TYPE_META_MEMBER_BASE(Type, member, id, false, CRA_TYPE_FLOAT)
// string
#define CRA_TYPE_META_MEMBER_STRING(Type, member, id, is_ptr)              \
    __CRA_TYPE_META_MEMBER_BASE(Type, member, id, is_ptr, CRA_TYPE_STRING)
// bytes
#define CRA_TYPE_META_MEMBER_BYTES(Type, member, id, is_ptr)                                              \
    __CRA_TYPE_META_MEMBER_BASE(Type, member, id, is_ptr, CRA_TYPE_BYTES)                                 \
    __CRA_TYPE_META_MEMBER(Type, n##member, true, id, false, CRA_TYPE_UINT, NULL, NULL, NULL, NULL, NULL)
// struct
#define CRA_TYPE_META_MEMBER_STRUCT(Type, member, id, is_ptr, member_meta, init_i, arg)                            \
    __CRA_TYPE_META_MEMBER(Type, member, false, id, is_ptr, CRA_TYPE_STRUCT, member_meta, NULL, NULL, init_i, arg)
// c array
#define CRA_TYPE_META_MEMBER_ARRAY(Type, member, id, is_ptr, element_meta)                                       \
    __CRA_TYPE_META_MEMBER(Type, member, false, id, is_ptr, CRA_TYPE_LIST, element_meta, NULL, NULL, NULL, NULL) \
    __CRA_TYPE_META_MEMBER(Type, n##member, true, id, false, CRA_TYPE_UINT, NULL, NULL, NULL, NULL, NULL)
// list
#define CRA_TYPE_META_MEMBER_LIST(Type, member, id, is_ptr, element_meta, iter_i, append_i, init_i, arg) \
    __CRA_TYPE_META_MEMBER(Type, member, false, id, is_ptr, CRA_TYPE_LIST, element_meta, iter_i, append_i, init_i, arg)
// dict
#define CRA_TYPE_META_MEMBER_DICT(Type, member, id, is_ptr, kv_meta, iter_i, append_i, init_i, arg)                \
    __CRA_TYPE_META_MEMBER(Type, member, false, id, is_ptr, CRA_TYPE_DICT, kv_meta, iter_i, append_i, init_i, arg)

// element meta

#define __CRA_TYPE_META_ELEMENT(_is_len, _is_ptr, TYPE, _name, _size, _submeta, _iter_i, _append_i, _init_i, _arg) \
    {                                                                                                              \
        .is_not_end = true,                                                                                        \
        .is_len = _is_len,                                                                                         \
        .is_ptr = _is_ptr,                                                                                         \
        .id = 0,                                                                                                   \
        .type = TYPE,                                                                                              \
        .name = _name,                                                                                             \
        .size = _size,                                                                                             \
        .offset = 0,                                                                                               \
        .submeta = _submeta,                                                                                       \
        .iter_i = _iter_i,                                                                                         \
        .append_i = _append_i,                                                                                     \
        .init_i = _init_i,                                                                                         \
        .arg = _arg,                                                                                               \
    }
#define __CRA_TYPE_META_ELEMENT_BASE(is_ptr, TYPE, name, size)                             \
    __CRA_TYPE_META_ELEMENT(false, is_ptr, TYPE, name, size, NULL, NULL, NULL, NULL, NULL)
// bool
#define CRA_TYPE_META_ELEMENT_BOOL()     __CRA_TYPE_META_ELEMENT_BASE(false, CRA_TYPE_BOOL, "<<BOOL>>", sizeof(bool)),
// int
#define CRA_TYPE_META_ELEMENT_INT(type)  __CRA_TYPE_META_ELEMENT_BASE(false, CRA_TYPE_INT, "<<INT>>", sizeof(type)),
// uint
#define CRA_TYPE_META_ELEMENT_UINT(type) __CRA_TYPE_META_ELEMENT_BASE(false, CRA_TYPE_UINT, "<<UINT>>", sizeof(type)),
// varint
#define CRA_TYPE_META_ELEMENT_VARINT(type)                                            \
    __CRA_TYPE_META_ELEMENT_BASE(false, CRA_TYPE_VARINT, "<<VARINT>>", sizeof(type)),
// varuint
#define CRA_TYPE_META_ELEMENT_VARUINT(type)                                             \
    __CRA_TYPE_META_ELEMENT_BASE(false, CRA_TYPE_VARUINT, "<<VARUINT>>", sizeof(type)),
// float
#define CRA_TYPE_META_ELEMENT_FLOAT(type)                                           \
    __CRA_TYPE_META_ELEMENT_BASE(false, CRA_TYPE_FLOAT, "<<FLOAT>>", sizeof(type)),
// string
#define CRA_TYPE_META_ELEMENT_STRING(type, is_ptr)                                     \
    __CRA_TYPE_META_ELEMENT_BASE(is_ptr, CRA_TYPE_STRING, "<<STRING>>", sizeof(type)),
// bytes
#define CRA_TYPE_META_ELEMENT_BYTES(type, is_ptr, length_var)                                                       \
    __CRA_TYPE_META_ELEMENT_BASE(is_ptr, CRA_TYPE_BYTES, "<<BYTES>>", sizeof(type)),                                \
      __CRA_TYPE_META_ELEMENT(true, false, CRA_TYPE_UINT, "<<BYTES_LENGTH>>", sizeof(length_var), NULL, NULL, NULL, \
                              NULL, (void *)&(length_var)),
// struct
#define CRA_TYPE_META_ELEMENT_STRUCT(type, is_ptr, member_meta, init_i, arg)                                     \
    __CRA_TYPE_META_ELEMENT(false, is_ptr, CRA_TYPE_STRUCT, "<<STRUCT>>", sizeof(type), member_meta, NULL, NULL, \
                            init_i, arg),
// c array
#define CRA_TYPE_META_ELEMENT_ARRAY(type, is_ptr, narray_var, element_meta)                                          \
    __CRA_TYPE_META_ELEMENT(false, is_ptr, CRA_TYPE_LIST, "<<ARRAY>>", sizeof(type), element_meta, NULL, NULL, NULL, \
                            NULL),                                                                                   \
      __CRA_TYPE_META_ELEMENT(true, false, CRA_TYPE_UINT, "<<ARRAY_COUNT>>", sizeof(narray_var), NULL, NULL, NULL,   \
                              NULL, (void *)&(narray_var)),
// list
#define CRA_TYPE_META_ELEMENT_LIST(type, is_ptr, element_meta, iter_i, append_i, init_i, arg)                       \
    __CRA_TYPE_META_ELEMENT(false, is_ptr, CRA_TYPE_LIST, "<<LIST>>", sizeof(type), element_meta, iter_i, append_i, \
                            init_i, arg),
// dict
#define CRA_TYPE_META_ELEMENT_DICT(type, is_ptr, kv_meta, iter_i, append_i, init_i, arg)                               \
    __CRA_TYPE_META_ELEMENT(false, is_ptr, CRA_TYPE_DICT, "<<DICT>>", sizeof(type), kv_meta, iter_i, append_i, init_i, \
                            arg),

// make object with meta

#define __CRA_SERI_OBJ(obj, is_ptr, TYPE, name, submeta, second_meta, iter_i, append_i, init_i, arg)       \
    &(CraSeriObject)                                                                                    \
    {                                                                                                   \
        .objptr = (void *)&(obj),                                                                       \
        .meta = {                                                                                       \
            __CRA_TYPE_META_ELEMENT(false, is_ptr, TYPE, name,                                          \
                __CRA_SF##is_ptr(obj), submeta, iter_i, append_i, init_i, arg),                         \
            second_meta,                                                                                \
            { 0 },                                                                                      \
        },                                                                                              \
    }
// struct
#define CRA_SERI_STRUCT(stru, is_ptr, members_meta, init_i, arg)                                                \
    __CRA_SERI_OBJ(stru, is_ptr, CRA_TYPE_STRUCT, "<<*STRUCT*>>", members_meta, { 0 }, NULL, NULL, init_i, arg)
// array
#define CRA_SERI_ARRAY(array, is_ptr, narray_var, elements_meta)                                                      \
    __CRA_SERI_OBJ(array, is_ptr, CRA_TYPE_LIST, "<<*ARRAY*>>", elements_meta,                                        \
                   __CRA_TYPE_META_ELEMENT(true, false, CRA_TYPE_UINT, "<<*ARRAY_COUNT*>>", sizeof(narray_var), NULL, \
                                           NULL, NULL, NULL, (void *)&(narray_var)),                                  \
                   NULL, NULL, NULL, NULL)
// list
#define CRA_SERI_LIST(list, is_ptr, element_meta, iter_i, append_i, init_i, arg)                                  \
    __CRA_SERI_OBJ(list, is_ptr, CRA_TYPE_LIST, "<<*LIST*>>", element_meta, { 0 }, iter_i, append_i, init_i, arg)
// dict
#define CRA_SERI_DICT(dict, is_ptr, kv_meta, iter_i, append_i, init_i, arg)                                  \
    __CRA_SERI_OBJ(dict, is_ptr, CRA_TYPE_DICT, "<<*DICT*>>", kv_meta, { 0 }, iter_i, append_i, init_i, arg)

// if id is not unique, return id. else return -1
CRA_API int
cra_check_id_unique(const CraTypeMeta *meta);

// ===================== inner =======================

#ifdef __CRA_SER_INNER
#include "cra_assert.h"

typedef struct CraSerializer CraSerializer;
typedef struct CraRelaseMgr  CraReleaseMgr;
typedef struct CraRelaseBlk  CraReleaseBlk;

struct CraRelaseBlk
{
    void              *ptr;
    const CraTypeMeta *meta;
};

// 有uninit函数的对象（struct/array/list/dict）Mgr不记录其字段/元素
struct CraRelaseMgr
{
    size_t         size;
    size_t         count;
    CraReleaseBlk  nodes1[16];
    CraReleaseBlk *nodes2;
};

struct CraSerializer
{
    bool         format;
    unsigned int nesting;
    size_t       index;
    size_t       maxlen;
#ifdef __CRA_BUFFER_UNSIGNED
    unsigned char *buffer;
#else
    char *buffer;
#endif
    CraReleaseMgr release;
    CraSerErr     error;
};

static inline void
cra_release_mgr_init(CraReleaseMgr *mgr)
{
    mgr->size = CRA_NARRAY(mgr->nodes1);
    mgr->count = 0;
    mgr->nodes2 = NULL;
}

void
cra_release_mgr_uninit(CraReleaseMgr *mgr, bool free_ptr);

bool
cra_release_mgr_add(CraReleaseMgr *mgr, void *ptr, const CraTypeMeta *meta);

#define __CRA_SERIALIZER_ERROR(ser, name_fmt, ERR, fmt, ...)                                                      \
    do                                                                                                            \
    {                                                                                                             \
        (ser)->error.err = ERR;                                                                                   \
        snprintf((ser)->error.msg, sizeof((ser)->error.msg), "ERROR(index: %zu" name_fmt "): " fmt, (ser)->index, \
                 ##__VA_ARGS__);                                                                                  \
    } while (0)
#define CRA_SERIALIZER_ERROR(ser, meta, ERR, fmt, ...)                               \
    __CRA_SERIALIZER_ERROR(ser, ", name: %s", ERR, fmt, (meta)->name, ##__VA_ARGS__)
#define CRA_SERIALIZER_ERROR1(ser, ERR, fmt, ...) __CRA_SERIALIZER_ERROR(ser, , ERR, fmt, ##__VA_ARGS__)

#define CRA_SERIALIZER_CHECK_TYPE(ser, meta, _type)                                                      \
    do                                                                                                   \
    {                                                                                                    \
        if ((_type) != (meta)->type)                                                                     \
        {                                                                                                \
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_TYPE_MISMATCH,                                   \
                                 "type mismatch: expected a '%d', but got a '%d'", (meta)->type, _type); \
            return false;                                                                                \
        }                                                                                                \
    } while (0)
#define CRA_SERIALIZER_CHECK_SIZE(ser, meta, _size)                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((_size) != (meta)->size)                                                                                   \
        {                                                                                                              \
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_SIZE_MISMATCH, "size mismatch: expected '%zu', but got '%zu'", \
                                 (meta)->size, _size);                                                                 \
            return false;                                                                                              \
        }                                                                                                              \
    } while (0)
#define CRA_SERIALIZER_CHECK_NULL(ser, meta)                                                          \
    do                                                                                                \
    {                                                                                                 \
        if ((meta)->type <= CRA_TYPE_FLOAT || !(meta)->is_ptr)                                        \
        {                                                                                             \
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_CANNOT_BE_NULL, "only pointers can be null"); \
            return false;                                                                             \
        }                                                                                             \
    } while (0)
#define CRA_SERIALIZER_CHECK_KEY(ser, meta)                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((meta)->type > CRA_TYPE_STRING)                                                                            \
        {                                                                                                              \
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_CANNOT_BE_KEY, "this type(%d) cannot be a key", (meta)->type); \
            return false;                                                                                              \
        }                                                                                                              \
    } while (0)
#define CRA_SEIALIZER_CHECK_MEMORY(ser, meta, ptr)                                         \
    do                                                                                     \
    {                                                                                      \
        if (!(ptr))                                                                        \
        {                                                                                  \
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_ALLOC, "Allocate memory failed."); \
            return false;                                                                  \
        }                                                                                  \
    } while (0)
#define CRA_SERIALIZER_RELEASE_MGR_ADD_CHECK(ser, meta, ptr)                                      \
    do                                                                                            \
    {                                                                                             \
        if (!cra_release_mgr_add(&(ser)->release, ptr, meta))                                     \
        {                                                                                         \
            CRA_SERIALIZER_ERROR(ser, meta, CRA_SER_ERR_ALLOC, "Add to release manager failed."); \
            cra_free(ptr);                                                                        \
            return false;                                                                         \
        }                                                                                         \
    } while (0)

#define CRA_NAME(meta)                        ((meta)->name ? (meta)->name : "(null)")
#define CRA_SERIALIZER_GET_BUF(ser)           ((ser)->buffer + (ser)->index)
#define CRA_SERIALIZER_IS_ENOUGH(ser, needed) ((ser)->index + (needed) <= (ser)->maxlen)
#define CRA_SERIALIZER_GET_REAMAINING(ser)    ((ser)->maxlen - (ser)->index)
#define CRA_SERIALIZER_ENSURE_(ser, buf, needed, sub)                               \
    do                                                                              \
    {                                                                               \
        if (!CRA_SERIALIZER_IS_ENOUGH(ser, needed))                                 \
        {                                                                           \
            CRA_SERIALIZER_ERROR1(ser, CRA_SER_ERR_NOBUF, "buffer size too small"); \
            return false;                                                           \
        }                                                                           \
        buf = CRA_SERIALIZER_GET_BUF(ser);                                          \
        (ser)->index += ((needed) - (sub));                                         \
    } while (0)
#define CRA_SERIALIZER_ENSURE(ser, buf, needed) CRA_SERIALIZER_ENSURE_(ser, buf, needed, 0)

#define CRA_SERIALIZER_MAX_NESTING 1000
#define CRA_SERIALIZER_NESTING_INC_CHECK(ser)                                                                     \
    do                                                                                                            \
    {                                                                                                             \
        if (++(ser)->nesting > CRA_SERIALIZER_MAX_NESTING)                                                        \
        {                                                                                                         \
            CRA_SERIALIZER_ERROR1(ser, CRA_SER_ERR_NESTING, "excessive nesting(%d)", CRA_SERIALIZER_MAX_NESTING); \
            return false;                                                                                         \
        }                                                                                                         \
    } while (0)
#define CRA_SERIALIZER_NESTING_DEC(ser) --(ser)->nesting

#define cra_serializer_check_err(ser, err, status)                             \
    assert(((status) && (ser).error.err == CRA_SER_ERR_OK) ||                  \
           ((ser).error.err != CRA_SER_ERR_OK && (ser).error.msg[0] != '\0')); \
    if (err)                                                                   \
    memcpy(err, &(ser).error, sizeof(*err))

static inline void
#ifdef __CRA_BUFFER_UNSIGNED
cra_serializer_init(CraSerializer *ser, unsigned char *buffer, size_t maxlen, bool format)
#else
cra_serializer_init(CraSerializer *ser, char *buffer, size_t maxlen, bool format)
#endif
{
    assert(ser);
    assert(buffer);
    assert(maxlen > 0);

    ser->format = format;
    ser->nesting = 0;
    ser->index = 0;
    ser->maxlen = maxlen;
    ser->buffer = buffer;
    ser->error.err = CRA_SER_ERR_OK;
    ser->error.msg[0] = '\0';
    cra_release_mgr_init(&ser->release);
}

static inline void
cra_serializer_uninit(CraSerializer *ser, bool success)
{
    cra_release_mgr_uninit(&ser->release, !success);
}

static inline void
cra_serializer_p2i(void *ptr, int64_t *retval, const CraTypeMeta *meta)
{
    switch (meta->size)
    {
        case sizeof(int8_t):
            *retval = *(int8_t *)ptr;
            break;
        case sizeof(int16_t):
            *retval = *(int16_t *)ptr;
            break;
        case sizeof(int32_t):
            *retval = *(int32_t *)ptr;
            break;
        case sizeof(int64_t):
            *retval = *(int64_t *)ptr;
            break;
        default:
            assert_always(false);
    }
}

static inline void
cra_serializer_i2p(int64_t val, void *ptr, const CraTypeMeta *meta)
{
    switch (meta->size)
    {
        case sizeof(int8_t):
            *(int8_t *)ptr = (int8_t)val;
            break;
        case sizeof(int16_t):
            *(int16_t *)ptr = (int16_t)val;
            break;
        case sizeof(int32_t):
            *(int32_t *)ptr = (int32_t)val;
            break;
        case sizeof(int64_t):
            *(int64_t *)ptr = (int64_t)val;
            break;
        default:
            assert_always(false);
    }
}

static inline void
cra_serializer_p2u(void *ptr, uint64_t *retval, const CraTypeMeta *meta)
{
    switch (meta->size)
    {
        case sizeof(uint8_t):
            *retval = *(uint8_t *)ptr;
            break;
        case sizeof(uint16_t):
            *retval = *(uint16_t *)ptr;
            break;
        case sizeof(uint32_t):
            *retval = *(uint32_t *)ptr;
            break;
        case sizeof(uint64_t):
            *retval = *(uint64_t *)ptr;
            break;
        default:
            assert_always(false);
    }
}

static inline void
cra_serializer_u2p(uint64_t val, void *ptr, const CraTypeMeta *meta)
{
    switch (meta->size)
    {
        case sizeof(uint8_t):
            *(uint8_t *)ptr = (uint8_t)val;
            break;
        case sizeof(uint16_t):
            *(uint16_t *)ptr = (uint16_t)val;
            break;
        case sizeof(uint32_t):
            *(uint32_t *)ptr = (uint32_t)val;
            break;
        case sizeof(uint64_t):
            *(uint64_t *)ptr = (uint64_t)val;
            break;
        default:
            assert_always(false);
    }
}

#endif

#endif