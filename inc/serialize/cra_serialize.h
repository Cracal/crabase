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

typedef struct _CraInitializable_i CraInitializable_i;
typedef struct _CraSerializable_i  CraSerializable_i;
typedef struct _CraSeriObject      CraSeriObject;
typedef struct _CraInitArgs        CraInitArgs;
typedef struct _CraTypeMeta        CraTypeMeta;

struct _CraInitializable_i
{
    void (*init)(void *obj, CraInitArgs *arg);
    // `dont_free_ptr_member`永远是`true`。仅作为一个醒目的提示，
    // 告知在实现uninit函数时不要对指针类型进行free操作。
    void (*uninit)(void *obj, bool dont_free_ptr_member);
};

struct _CraSerializable_i
{
    CraInitializable_i init_i;

    // `retlen`: return the elements' count
    void (*iter_init)(void *obj, uint64_t *retlen, void *it, size_t itsize);
    bool (*iter_next)(void *it, void **retval1, void **retval2);
    bool (*append)(void *obj, void *val1, void *val2);
};

struct _CraInitArgs
{
    size_t size;
    size_t length;
    size_t val1size;
    size_t val2size;
    void  *arg;
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

struct _CraTypeMeta
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
    union
    {
        const CraInitializable_i *init_i;
        const CraSerializable_i  *szer_i;
    };
    void *arg;
};

struct _CraSeriObject
{
    void       *objptr;
    CraTypeMeta meta[3];
};

#define __CRA_SFtrue(_member)  sizeof(*(_member))
#define __CRA_SF1              __CRA_SFtrue
#define __CRA_SFTRUE           __CRA_SFtrue
#define __CRA_SFTrue           __CRA_SFtrue
#define __CRA_SFfalse(_member) sizeof(_member)
#define __CRA_SF0              __CRA_SFfalse
#define __CRA_SFFALSE          __CRA_SFfalse
#define __CRA_SFFalse          __CRA_SFfalse

#define CRA_TYPE_META_DECL(_var)        CraTypeMeta _var[]
#define CRA_TYPE_META_DECL_CONST(_var)  const CRA_TYPE_META_DECL
#define CRA_TYPE_META_BEGIN(_var)       CRA_TYPE_META_DECL(_var) = {
#define CRA_TYPE_META_BEGIN_CONST(_var) const CRA_TYPE_META_BEGIN(_var)
#define CRA_TYPE_META_END() {0}}

// member meta

#define __CRA_TYPE_META_MEMBER_(_Type, _member, _is_len, _id, _is_ptr, _TYPE, _submeta, _i, _arg) \
    { true,                                                                                       \
      _is_len,                                                                                    \
      _is_ptr,                                                                                    \
      _id,                                                                                        \
      _TYPE,                                                                                      \
      #_member,                                                                                   \
      __CRA_SF##_is_ptr(((_Type *)0)->_member),                                                   \
      offsetof(_Type, _member),                                                                   \
      _submeta,                                                                                   \
      _i,                                                                                         \
      _arg },
#define __CRA_TYPE_META_MEMBER(_Type, _member, _id, _is_ptr, _TYPE, _submeta, _i, _arg)     \
    __CRA_TYPE_META_MEMBER_(_Type, _member, false, _id, _is_ptr, _TYPE, _submeta, _i, _arg)

// bool
#define CRA_TYPE_META_MEMBER_BOOL(_Type, _member, _id)                                      \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, false, CRA_TYPE_BOOL, NULL, { NULL }, NULL)
// int
#define CRA_TYPE_META_MEMBER_INT(_Type, _member, _id)                                      \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, false, CRA_TYPE_INT, NULL, { NULL }, NULL)
// uint
#define CRA_TYPE_META_MEMBER_UINT(_Type, _member, _id)                                      \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, false, CRA_TYPE_UINT, NULL, { NULL }, NULL)
// varint
#define CRA_TYPE_META_MEMBER_VARINT(_Type, _member, _id)                                      \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, false, CRA_TYPE_VARINT, NULL, { NULL }, NULL)
// varuint
#define CRA_TYPE_META_MEMBER_VARUINT(_Type, _member, _id)                                      \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, false, CRA_TYPE_VARUINT, NULL, { NULL }, NULL)
// float
#define CRA_TYPE_META_MEMBER_FLOAT(_Type, _member, _id)                                      \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, false, CRA_TYPE_FLOAT, NULL, { NULL }, NULL)
// string
#define CRA_TYPE_META_MEMBER_STRING(_Type, _member, _id, _is_ptr)                               \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, _is_ptr, CRA_TYPE_STRING, NULL, { NULL }, NULL)
// bytes
#define CRA_TYPE_META_MEMBER_BYTES(_Type, _member, _id, _is_ptr)                                      \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, _is_ptr, CRA_TYPE_BYTES, NULL, { NULL }, NULL)        \
    __CRA_TYPE_META_MEMBER_(_Type, n##_member, true, _id, false, CRA_TYPE_UINT, NULL, { NULL }, NULL)
// struct
#define CRA_TYPE_META_MEMBER_STRUCT(_Type, _member, _id, _is_ptr, _member_meta, _init_i, _arg)                       \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, _is_ptr, CRA_TYPE_STRUCT, _member_meta, { .init_i = _init_i }, _arg)
// c array
#define CRA_TYPE_META_MEMBER_ARRAY(_Type, _member, _id, _is_ptr, _element_meta)                        \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, _is_ptr, CRA_TYPE_LIST, _element_meta, { NULL }, NULL) \
    __CRA_TYPE_META_MEMBER_(_Type, n##_member, true, _id, false, CRA_TYPE_UINT, NULL, { NULL }, NULL)
// list
#define CRA_TYPE_META_MEMBER_LIST(_Type, _member, _id, _is_ptr, _element_meta, _szer_i, _arg)                       \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, _is_ptr, CRA_TYPE_LIST, _element_meta, { .szer_i = _szer_i }, _arg)
// dict
#define CRA_TYPE_META_MEMBER_DICT(_Type, _member, _id, _is_ptr, _kv_meta, _szer_i, _arg)                       \
    __CRA_TYPE_META_MEMBER(_Type, _member, _id, _is_ptr, CRA_TYPE_DICT, _kv_meta, { .szer_i = _szer_i }, _arg)

// element meta

#define __CRA_TYPE_META_ELEMENT_(_is_len, _is_ptr, _TYPE, _size, _submeta, _i, _arg) \
    { true, _is_len, _is_ptr, 0, _TYPE, NULL, _size, 0, _submeta, _i, _arg }
#define __CRA_TYPE_META_ELEMENT(_is_ptr, _TYPE, _size, _submeta, _i, _arg)     \
    __CRA_TYPE_META_ELEMENT_(false, _is_ptr, _TYPE, _size, _submeta, _i, _arg)
// bool
#define CRA_TYPE_META_ELEMENT_BOOL() __CRA_TYPE_META_ELEMENT(false, CRA_TYPE_BOOL, sizeof(bool), NULL, { NULL }, NULL),
// int
#define CRA_TYPE_META_ELEMENT_INT(_type)                                               \
    __CRA_TYPE_META_ELEMENT(false, CRA_TYPE_INT, sizeof(_type), NULL, { NULL }, NULL),
// uint
#define CRA_TYPE_META_ELEMENT_UINT(_type)                                               \
    __CRA_TYPE_META_ELEMENT(false, CRA_TYPE_UINT, sizeof(_type), NULL, { NULL }, NULL),
// varint
#define CRA_TYPE_META_ELEMENT_VARINT(_type)                                               \
    __CRA_TYPE_META_ELEMENT(false, CRA_TYPE_VARINT, sizeof(_type), NULL, { NULL }, NULL),
// varuint
#define CRA_TYPE_META_ELEMENT_VARUINT(_type)                                               \
    __CRA_TYPE_META_ELEMENT(false, CRA_TYPE_VARUINT, sizeof(_type), NULL, { NULL }, NULL),
// float
#define CRA_TYPE_META_ELEMENT_FLOAT(_type)                                               \
    __CRA_TYPE_META_ELEMENT(false, CRA_TYPE_FLOAT, sizeof(_type), NULL, { NULL }, NULL),
// string
#define CRA_TYPE_META_ELEMENT_STRING(_type, _is_ptr)                                        \
    __CRA_TYPE_META_ELEMENT(_is_ptr, CRA_TYPE_STRING, sizeof(_type), NULL, { NULL }, NULL),
// bytes
#define CRA_TYPE_META_ELEMENT_BYTES(_type, _is_ptr, _narray_var)                                  \
    __CRA_TYPE_META_ELEMENT(_is_ptr, CRA_TYPE_BYTES, sizeof(_type), NULL, { NULL }, NULL),        \
      __CRA_TYPE_META_ELEMENT_(                                                                   \
        true, false, CRA_TYPE_UINT, sizeof(_narray_var), NULL, { NULL }, (void *)&(_narray_var)),
// struct
#define CRA_TYPE_META_ELEMENT_STRUCT(_type, _is_ptr, _member_meta, _init_i, _arg)                                \
    __CRA_TYPE_META_ELEMENT(_is_ptr, CRA_TYPE_STRUCT, sizeof(_type), _member_meta, { .init_i = _init_i }, _arg),
// c array
#define CRA_TYPE_META_ELEMENT_ARRAY(_type, _is_ptr, _narray_var, _element_meta)                    \
    __CRA_TYPE_META_ELEMENT(_is_ptr, CRA_TYPE_LIST, sizeof(_type), _element_meta, { NULL }, NULL), \
      __CRA_TYPE_META_ELEMENT_(                                                                    \
        true, false, CRA_TYPE_UINT, sizeof(_narray_var), NULL, { NULL }, (void *)&(_narray_var)),
// list
#define CRA_TYPE_META_ELEMENT_LIST(_type, _is_ptr, _element_meta, _szer_i, _arg)                                \
    __CRA_TYPE_META_ELEMENT(_is_ptr, CRA_TYPE_LIST, sizeof(_type), _element_meta, { .szer_i = _szer_i }, _arg),
// dict
#define CRA_TYPE_META_ELEMENT_DICT(_type, _is_ptr, _kv_meta, _szer_i, _arg)                                \
    __CRA_TYPE_META_ELEMENT(_is_ptr, CRA_TYPE_DICT, sizeof(_type), _kv_meta, { .szer_i = _szer_i }, _arg),

// make object with meta

#define __CRA_SERI_OBJ(_obj, _is_ptr, _TYPE, _2meta, _submeta, _init_i, _arg)                          \
    &(CraSeriObject)                                                                                   \
    {                                                                                                  \
        .objptr = (void *)&(_obj), .meta = {                                                           \
            __CRA_TYPE_META_ELEMENT(_is_ptr, _TYPE, __CRA_SF##_is_ptr(_obj), _submeta, _init_i, _arg), \
            _2meta,                                                                                    \
            { 0 }                                                                                      \
        }                                                                                              \
    }
// struct
#define CRA_SERI_STRUCT(_stru, _is_ptr, _members_meta, _init_i, _arg)                                  \
    __CRA_SERI_OBJ(_stru, _is_ptr, CRA_TYPE_STRUCT, { 0 }, _members_meta, { .init_i = _init_i }, _arg)
// array
#define CRA_SERI_ARRAY(_array, _is_ptr, _narray_var, _elements_meta)                                           \
    __CRA_SERI_OBJ(_array,                                                                                     \
                   _is_ptr,                                                                                    \
                   CRA_TYPE_LIST,                                                                              \
                   __CRA_TYPE_META_ELEMENT_(                                                                   \
                     true, false, CRA_TYPE_UINT, sizeof(_narray_var), NULL, { NULL }, (void *)&(_narray_var)), \
                   _elements_meta,                                                                             \
                   { NULL },                                                                                   \
                   NULL)
// list
#define CRA_SERI_LIST(_list, _is_ptr, _element_meta, _szer_i, _arg)                                  \
    __CRA_SERI_OBJ(_list, _is_ptr, CRA_TYPE_LIST, { 0 }, _element_meta, { .szer_i = _szer_i }, _arg)
// dict
#define CRA_SERI_DICT(_dict, _is_ptr, _kv_meta, _szer_i, _arg)                                  \
    __CRA_SERI_OBJ(_dict, _is_ptr, CRA_TYPE_DICT, { 0 }, _kv_meta, { .szer_i = _szer_i }, _arg)

// ===================== inner =======================

#ifdef __CRA_SER_INNER
#include "cra_assert.h"
#include "cra_malloc.h"
#include "cra_serialize.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct _CraSerializer CraSerializer;
typedef struct _CraRelaseMgr  CraReleaseMgr;
typedef struct _CraRelaseBlk  CraReleaseBlk;

struct _CraRelaseBlk
{
    void              *ptr;
    const CraTypeMeta *meta;
};

// 有uninit函数的对象（struct/array/list/dict）Mgr不记录其字段/元素
struct _CraRelaseMgr
{
    size_t         size;
    size_t         count;
    CraReleaseBlk  nodes1[16];
    CraReleaseBlk *nodes2;
};

struct _CraSerializer
{
    bool           format;
    uint32_t       nesting;
    size_t         index;
    size_t         maxlen;
    unsigned char *buffer;
    CraReleaseMgr  release;
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

void
cra_release_mgr_add(CraReleaseMgr *mgr, void *ptr, const CraTypeMeta *meta);

#define CRA_SERIALIZER_PRINT_ERROR_(_fmt, ...) fprintf(stderr, "(ERROR): " _fmt "\n", ##__VA_ARGS__)
#define CRA_SERIALIZER_PRINT_ERROR(_fmt, ...)  fprintf(stderr, CRA_SERIALIZER_NAME "(ERROR): " _fmt "\n", ##__VA_ARGS__)

#define CRA_NAME(_meta)                         ((_meta)->name ? (_meta)->name : "(null)")
#define CRA_SERIALIZER_GET_BUF(_ser)            ((_ser)->buffer + (_ser)->index)
#define CRA_SERIALIZER_IS_ENOUGH(_ser, _needed) ((_ser)->index + (_needed) <= (_ser)->maxlen)
#define CRA_SERIALIZER_GET_REAMAINING(_ser)     ((_ser)->maxlen - (_ser)->index)
#define CRA_SERIALIZER_ENSURE_(_ser, _buf, _needed, _sub)  \
    do                                                     \
    {                                                      \
        if (!CRA_SERIALIZER_IS_ENOUGH(_ser, _needed))      \
        {                                                  \
            CRA_SERIALIZER_PRINT_ERROR("No more buffer."); \
            return false;                                  \
        }                                                  \
        _buf = CRA_SERIALIZER_GET_BUF(_ser);               \
        (_ser)->index += ((_needed) - (_sub));             \
    } while (0)
#define CRA_SERIALIZER_ENSURE(_ser, _buf, _needed) CRA_SERIALIZER_ENSURE_(_ser, _buf, _needed, 0)

#define CRA_SERIALIZER_MAX_NESTING 1000
#define CRA_SERIALIZER_NESTING_INC_CHECK(_ser)               \
    do                                                       \
    {                                                        \
        if (++(_ser)->nesting > CRA_SERIALIZER_MAX_NESTING)  \
        {                                                    \
            CRA_SERIALIZER_PRINT_ERROR("Too much nesting."); \
            return false;                                    \
        }                                                    \
    } while (0)
#define CRA_SERIALIZER_NESTING_DEC(_ser) --(_ser)->nesting

static inline void
cra_serializer_init(CraSerializer *ser, unsigned char *buffer, size_t maxlen, bool format)
{
    assert(ser);
    assert(buffer);
    assert(maxlen > 0);

    ser->format = format;
    ser->nesting = 0;
    ser->index = 0;
    ser->maxlen = maxlen;
    ser->buffer = buffer;
    cra_release_mgr_init(&ser->release);
}

static inline void
cra_serializer_uninit(CraSerializer *ser, bool success)
{
    cra_release_mgr_uninit(&ser->release, !success);
}

static inline bool
cra_serializer_p2i(void *ptr, int64_t *retval, const CraTypeMeta *meta)
{
    switch (meta->size)
    {
        case sizeof(int8_t):
            *retval = *(int8_t *)ptr;
            return true;
        case sizeof(int16_t):
            *retval = *(int16_t *)ptr;
            return true;
        case sizeof(int32_t):
            *retval = *(int32_t *)ptr;
            return true;
        case sizeof(int64_t):
            *retval = *(int64_t *)ptr;
            return true;
        default:
            CRA_SERIALIZER_PRINT_ERROR_("Invalid int size: %zu. Name: %s.", meta->size, CRA_NAME(meta));
            return false;
    }
}

static inline bool
cra_serializer_i2p(int64_t val, void *ptr, const CraTypeMeta *meta)
{
    switch (meta->size)
    {
        case sizeof(int8_t):
            *(int8_t *)ptr = (int8_t)val;
            return true;
        case sizeof(int16_t):
            *(int16_t *)ptr = (int16_t)val;
            return true;
        case sizeof(int32_t):
            *(int32_t *)ptr = (int32_t)val;
            return true;
        case sizeof(int64_t):
            *(int64_t *)ptr = (int64_t)val;
            return true;
        default:
            CRA_SERIALIZER_PRINT_ERROR_("Invalid int size: %zu. Name: %s.", meta->size, CRA_NAME(meta));
            return false;
    }
}

static inline bool
cra_serializer_p2u(void *ptr, uint64_t *retval, const CraTypeMeta *meta)
{
    switch (meta->size)
    {
        case sizeof(uint8_t):
            *retval = *(uint8_t *)ptr;
            return true;
        case sizeof(uint16_t):
            *retval = *(uint16_t *)ptr;
            return true;
        case sizeof(uint32_t):
            *retval = *(uint32_t *)ptr;
            return true;
        case sizeof(uint64_t):
            *retval = *(uint64_t *)ptr;
            return true;
        default:
            CRA_SERIALIZER_PRINT_ERROR_("Invalid uint size: %zu. Name: %s.", meta->size, CRA_NAME(meta));
            return false;
    }
}

static inline bool
cra_serializer_u2p(uint64_t val, void *ptr, const CraTypeMeta *meta)
{
    switch (meta->size)
    {
        case sizeof(uint8_t):
            *(uint8_t *)ptr = (uint8_t)val;
            return true;
        case sizeof(uint16_t):
            *(uint16_t *)ptr = (uint16_t)val;
            return true;
        case sizeof(uint32_t):
            *(uint32_t *)ptr = (uint32_t)val;
            return true;
        case sizeof(uint64_t):
            *(uint64_t *)ptr = (uint64_t)val;
            return true;
        default:
            CRA_SERIALIZER_PRINT_ERROR_("Invalid uint size: %zu. Name: %s.", meta->size, CRA_NAME(meta));
            return false;
    }
}

#endif

#endif