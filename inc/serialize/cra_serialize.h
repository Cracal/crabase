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

typedef enum
{
    CRA_SER_ERROR_SUCCESS = 0,       // must be zero
    CRA_SER_ERROR_NOBUF,             // no more spare buffer
    CRA_SER_ERROR_CODE_MISMATCH,     // code mismatch
    CRA_SER_ERROR_TOO_MUCH_NESTING,  // too much nesting
    CRA_SER_ERROR_TYPE_MISMATCH,     // type mismatch
    CRA_SER_ERROR_SIZE_MISMATCH,     // value size mismatch
    CRA_SER_ERROR_INVALID_TYPE,      // invalid type, not supported
    CRA_SER_ERROR_INVALID_SIZE,      // invalid size, the type does not support the size
    CRA_SER_ERROR_INVALID_VALUE,     // invalid value
    CRA_SER_ERROR_CANNOT_BE_NULL,    // this type(not ptr) cannot be null
    CRA_SER_ERROR_STRING_LENGTH,     // error string length
    CRA_SER_ERROR_BYTES_LENGTH,      // error bytes length
    CRA_SER_ERROR_CHARARR_TOO_SMALL, // sizeof(char[N]) < string length from deserialization
    CRA_SER_ERROR_ARRAY_TOO_SMALL,   // array too small. sizeof(T[N]) < element count from deserialization
    CRA_SER_ERROR_LIST_COUNT,        // error list count
    CRA_SER_ERROR_DICT_COUNT,        // error dict count
    CRA_SER_ERROR_APPEND_FAILED,     // dzer_i.appand failed
    CRA_SER_ERROR_WRITE_KEY_FAILED,  // write json key failed
    CRA_SER_ERROR_CANNOT_BE_KEY,     // this type cannot be a json key
    CRA_SER_ERROR_INVALID_KEY,       // json key error
    CRA_SER_ERROR_FLOAT_NAN_OR_INF,  // float is NaN or Inf
} CraSerError_e;

typedef enum
{
    CRA_TYPE_NULL = 1, // null
    CRA_TYPE_BOOL,     // boolean
    CRA_TYPE_INT,      // int8, int16, int32, int64
    CRA_TYPE_UINT,     // uint8, uint16, uint32, uint64
    CRA_TYPE_VARINT,   // varint
    CRA_TYPE_VARUINT,  // varuint
    CRA_TYPE_FLOAT,    // float, double
    CRA_TYPE_STRING,   // string
    CRA_TYPE_BYTES,    // bytes
    CRA_TYPE_STRUCT,   // struct
    CRA_TYPE_LIST,     // c array, list
    CRA_TYPE_DICT,     // dict
} CraType_e;

typedef struct _CraTypeMeta CraTypeMeta;

typedef struct _CraSerReleaseNode
{
    void *ptr;
    void  (*uninit)(void *);
    void  (*dealloc)(void *);
} CraSerReleaseNode;

typedef struct _CraSerRelease
{
    size_t             current;
    size_t             n_nodes;
    CraSerReleaseNode  nodes1[32];
    CraSerReleaseNode *nodes2;
} CraSerRelease;

typedef struct
{
    bool           format;
    bool           noalloc;
    CraSerError_e  error;
    uint32_t       code;
    uint32_t       nesting;
    size_t         index;
    size_t         length;
    unsigned char *buffer;
    CraSerRelease  release;
} CraSerializer;

// serializer interface
typedef struct
{
    size_t (*get_count)(void *obj);
    void   (*iter_init)(void *obj, void *it, size_t itsize);
    union
    {
        bool (*iter_next1)(void *it, void **retvalptr);
        bool (*iter_next2)(void *it, void **retkeyptr, void **retvalptr);
    };
} CraSzer_i;
// deserializer interface
typedef struct
{
    union
    {
        void (*init1)(void *obj, size_t count, size_t element_size, const void *arg);
        void (*init2)(void *obj, size_t count, size_t key_size, size_t val_size, const void *arg);
    };
    void (*uninit)(void *obj);
    union
    {
        bool (*append1)(void *obj, void *val);
        bool (*append2)(void *obj, void *key, void *val);
    };
} CraDzer_i;

struct _CraTypeMeta
{
    // `true`: T *varname
    // `false`: T varname
    bool               is_ptr;
    CraType_e          type;
    const char *const  member; // member variable name
    size_t             size;
    size_t             offset;
    // `for c array`,                   size of the number of elements variable
    // `for string not ending in zero`, size of string length variable
    size_t             nsize;
    // `for c array`,                   offset of the number of elements variable in struct
    // `for string not ending in zero`, offset of string length variable
    size_t             noffset;
    // `for c array/list`, element's meta
    // `for dict`,         key-val's meta
    // `for struct`,       members' meta
    const CraTypeMeta *submeta;
    // for list/dict
    const CraSzer_i   *szer_i;
    // for list/dict
    const CraDzer_i   *dzer_i;
    // for list/dict
    const void        *arg4dzer;
};

#define __CRA_SF(_StruType, _member)  sizeof(((_StruType *)0)->_member)
#define __CRA_SFP(_StruType, _member) sizeof(*((_StruType *)0)->_member)
#define __CRA_SF_1                    __CRA_SFP
#define __CRA_SF_true                 __CRA_SF_1
#define __CRA_SF_TRUE                 __CRA_SF_1
#define __CRA_SF_True                 __CRA_SF_1
#define __CRA_SF_0                    __CRA_SF
#define __CRA_SF_false                __CRA_SF_0
#define __CRA_SF_FALSE                __CRA_SF_0
#define __CRA_SF_False                __CRA_SF_0

#define __CRA_CHECK(_cond, _setval) ((_cond) ? (_setval) : (1 / 0))

#define __CRA_CKS(_StruType, _member, _is_ptr)                                         \
    __CRA_CHECK(sizeof(((_StruType *)0)->_member[0]) == sizeof(char) &&                \
                  ((_is_ptr) ? __CRA_SF(_StruType, _member) == sizeof(void *) : true), \
                _is_ptr)
#define __CRA_CKS2(_maxlen, _is_ptr) __CRA_CHECK((_is_ptr) ? (_maxlen) == 0 : (_maxlen) > 0, _is_ptr)

#define __CRA_CKO(_size, _is_ptr) __CRA_CHECK(((_is_ptr) ? (_size) == sizeof(void *) : (_size) > 0), _is_ptr)

#define __CRA_CS1(_size, _cmp_size) __CRA_CHECK((_size) == (_cmp_size), (_cmp_size))
#define __CRA_CS2(_size, _cmp_size1, _cmp_size2)                             \
    __CRA_CHECK((_size) == (_cmp_size1) || (_size) == (_cmp_size2), (_size))
#define __CRA_CS4(_size, _cmp_size1, _cmp_size2, _cmp_size3, _cmp_size4)                         \
    __CRA_CHECK((_size) == (_cmp_size1) || (_size) == (_cmp_size2) || (_size) == (_cmp_size3) || \
                  (_size) == (_cmp_size4),                                                       \
                (_size))

// 当META定义失败(除零)时，请检查:
//    成员变量的大小是否匹配, 比如：bool的大小应该是1字节，但你在struct中错误的定义成了`int b`，b占用了4字节
#define CRA_TYPE_META_BEGIN(_name) CraTypeMeta _name[] = {
#define CRA_TYPE_META_END() { 0 } }

#if 1 // for members

#define __CRA_TYPE_META_BASE(_StruType, _member, _TYPE, _size)                         \
    { false, _TYPE, #_member, _size, offsetof(_StruType, _member), 0, 0, 0, 0, 0, 0 },
// def bool meta
#define CRA_TYPE_META_MEMBER_BOOL(_StruType, _member)                                                   \
    __CRA_TYPE_META_BASE(_StruType, _member, CRA_TYPE_BOOL, __CRA_CS1(__CRA_SF(_StruType, _member), 1))
// def int meta
#define CRA_TYPE_META_MEMBER_INT(_StruType, _member)                                                            \
    __CRA_TYPE_META_BASE(_StruType, _member, CRA_TYPE_INT, __CRA_CS4(__CRA_SF(_StruType, _member), 1, 2, 4, 8))
// def uint meta
#define CRA_TYPE_META_MEMBER_UINT(_StruType, _member)                                                            \
    __CRA_TYPE_META_BASE(_StruType, _member, CRA_TYPE_UINT, __CRA_CS4(__CRA_SF(_StruType, _member), 1, 2, 4, 8))
// def varint meta
#define CRA_TYPE_META_MEMBER_VARINT(_StruType, _member)                                                            \
    __CRA_TYPE_META_BASE(_StruType, _member, CRA_TYPE_VARINT, __CRA_CS4(__CRA_SF(_StruType, _member), 1, 2, 4, 8))
// def varuint meta
#define CRA_TYPE_META_MEMBER_VARUINT(_StruType, _member)                                                            \
    __CRA_TYPE_META_BASE(_StruType, _member, CRA_TYPE_VARUINT, __CRA_CS4(__CRA_SF(_StruType, _member), 1, 2, 4, 8))
// def float meta
#define CRA_TYPE_META_MEMBER_FLOAT(_StruType, _member)                                                      \
    __CRA_TYPE_META_BASE(_StruType, _member, CRA_TYPE_FLOAT, __CRA_CS2(__CRA_SF(_StruType, _member), 4, 8))
// def string meta
#define CRA_TYPE_META_MEMBER_STRING(_StruType, _member, _is_ptr) \
    { __CRA_CKS(_StruType, _member, _is_ptr),                    \
      CRA_TYPE_STRING,                                           \
      #_member,                                                  \
      _is_ptr ? 0 : __CRA_SF(_StruType, _member),                \
      offsetof(_StruType, _member),                              \
      0,                                                         \
      0,                                                         \
      0,                                                         \
      0,                                                         \
      0,                                                         \
      0 },
// def bytes meta
#define CRA_TYPE_META_MEMBER_BYTES(_StruType, _member, _is_ptr) \
    { __CRA_CKS(_StruType, _member, _is_ptr),                   \
      CRA_TYPE_BYTES,                                           \
      #_member,                                                 \
      _is_ptr ? 0 : __CRA_SF(_StruType, _member),               \
      offsetof(_StruType, _member),                             \
      __CRA_SF(_StruType, n##_member),                          \
      offsetof(_StruType, n##_member),                          \
      0,                                                        \
      0,                                                        \
      0,                                                        \
      0 },
// def struct meta
#define CRA_TYPE_META_MEMBER_STRUCT(_StruType, _member, _is_ptr, _members_meta) \
    { __CRA_CKO(__CRA_SF(_StruType, _member), _is_ptr),                         \
      CRA_TYPE_STRUCT,                                                          \
      #_member,                                                                 \
      __CRA_SF_##_is_ptr(_StruType, _member),                                   \
      offsetof(_StruType, _member),                                             \
      0,                                                                        \
      0,                                                                        \
      _members_meta,                                                            \
      0,                                                                        \
      0,                                                                        \
      0 },
// def array meta
#define CRA_TYPE_META_MEMBER_ARRAY(_StruType, _member, _is_ptr, _element_meta) \
    { __CRA_CKO(__CRA_SF(_StruType, _member), _is_ptr),                        \
      CRA_TYPE_LIST,                                                           \
      #_member,                                                                \
      __CRA_SF(_StruType, _member),                                            \
      offsetof(_StruType, _member),                                            \
      __CRA_SF(_StruType, n##_member),                                         \
      offsetof(_StruType, n##_member),                                         \
      _element_meta,                                                           \
      0,                                                                       \
      0,                                                                       \
      0 },
// def list meta
#define CRA_TYPE_META_MEMBER_LIST(_StruType, _member, _is_ptr, _element_meta, _szer_i, _dzer_i, _arg4dzer) \
    { __CRA_CKO(__CRA_SF(_StruType, _member), _is_ptr),                                                    \
      CRA_TYPE_LIST,                                                                                       \
      #_member,                                                                                            \
      __CRA_SF_##_is_ptr(_StruType, _member),                                                              \
      offsetof(_StruType, _member),                                                                        \
      0,                                                                                                   \
      0,                                                                                                   \
      _element_meta,                                                                                       \
      _szer_i,                                                                                             \
      _dzer_i,                                                                                             \
      _arg4dzer },
// def dict meta
#define CRA_TYPE_META_MEMBER_DICT(_StruType, _member, _is_ptr, _kv_meta, _szer_i, _dzer_i, _arg4dzer) \
    { __CRA_CKO(__CRA_SF(_StruType, _member), _is_ptr),                                               \
      CRA_TYPE_DICT,                                                                                  \
      #_member,                                                                                       \
      __CRA_SF_##_is_ptr(_StruType, _member),                                                         \
      offsetof(_StruType, _member),                                                                   \
      0,                                                                                              \
      0,                                                                                              \
      _kv_meta,                                                                                       \
      _szer_i,                                                                                        \
      _dzer_i,                                                                                        \
      _arg4dzer },

#endif // end for members

#if 1 // for items

#define __CRA_TYPE_META_ITEM_BASE(_TYPE, _size) { false, CRA_TYPE_##_TYPE, 0, _size, 0, 0, 0, 0, 0, 0, 0 },
// `type`: bool
#define CRA_TYPE_META_ITEM_BOOL(_type)          __CRA_TYPE_META_ITEM_BASE(BOOL, __CRA_CS1(sizeof(_type), 1))
// `type`: int8, int16, int32, int64
#define CRA_TYPE_META_ITEM_INT(_type)           __CRA_TYPE_META_ITEM_BASE(INT, __CRA_CS4(sizeof(_type), 1, 2, 4, 8))
// `type`: uint8, uint16, uint32, uint64
#define CRA_TYPE_META_ITEM_UINT(_type)          __CRA_TYPE_META_ITEM_BASE(UINT, __CRA_CS4(sizeof(_type), 1, 2, 4, 8))
// `type`: int8, int16, int32, int64
#define CRA_TYPE_META_ITEM_VARINT(_type)        __CRA_TYPE_META_ITEM_BASE(VARINT, __CRA_CS4(sizeof(_type), 1, 2, 4, 8))
// `type`: uint8, uint16, uint32, uint64
#define CRA_TYPE_META_ITEM_VARUINT(_type)       __CRA_TYPE_META_ITEM_BASE(VARUINT, __CRA_CS4(sizeof(_type), 1, 2, 4, 8))
// `type`: float, double
#define CRA_TYPE_META_ITEM_FLOAT(_type)         __CRA_TYPE_META_ITEM_BASE(FLOAT, __CRA_CS2(sizeof(_type), 4, 8))
// `maxlen`: is_ptr => 0; !is_ptr => sizeof(char[N])
#define CRA_TYPE_META_ITEM_STRING(_maxlen, _is_ptr)                                     \
    { __CRA_CKS2(_maxlen, _is_ptr), CRA_TYPE_STRING, 0, _maxlen, 0, 0, 0, 0, 0, 0, 0 },
// `type`: struct XX * (is_ptr), struct XX (!is_ptr)
#define CRA_TYPE_META_ITEM_STRUCT(_type, _is_ptr, _members_meta) \
    { __CRA_CKO(sizeof(_type), _is_ptr),                         \
      CRA_TYPE_STRUCT,                                           \
      0,                                                         \
      __CRA_SF_##_is_ptr(                                        \
        struct { _type o; }, o),                                 \
      0,                                                         \
      0,                                                         \
      0,                                                         \
      _members_meta,                                             \
      0,                                                         \
      0,                                                         \
      0 },
// `type`: List<T> * (is_ptr), List<T> (!is_ptr)
#define CRA_TYPE_META_ITEM_LIST(_type, _is_ptr, _element_meta, _szer_i, _dzer_i, _arg4dzer) \
    { __CRA_CKO(sizeof(_type), _is_ptr),                                                    \
      CRA_TYPE_LIST,                                                                        \
      0,                                                                                    \
      __CRA_SF_##_is_ptr(                                                                   \
        struct { _type o; }, o),                                                            \
      0,                                                                                    \
      0,                                                                                    \
      0,                                                                                    \
      _element_meta,                                                                        \
      _szer_i,                                                                              \
      _dzer_i,                                                                              \
      _arg4dzer },
// `type`: Dict<TK, TV> * (is_ptr), Dict<TK, TV> (!is_ptr)
#define CRA_TYPE_META_ITEM_DICT(_type, _is_ptr, _kv_meta, _szer_i, _dzer_i, _arg4dzer) \
    { __CRA_CKO(sizeof(_type), _is_ptr),                                               \
      CRA_TYPE_DICT,                                                                   \
      0,                                                                               \
      __CRA_SF_##_is_ptr(                                                              \
        struct { _type o; }, o),                                                       \
      0,                                                                               \
      0,                                                                               \
      0,                                                                               \
      _kv_meta,                                                                        \
      _szer_i,                                                                         \
      _dzer_i,                                                                         \
      _arg4dzer },

// static
// CRA_TYPE_META_BEGIN(g_test_meta)
// CRA_TYPE_META_ITEM_BOOL(bool)
// CRA_TYPE_META_ITEM_INT(int)
// CRA_TYPE_META_ITEM_UINT(uint64_t)
// CRA_TYPE_META_ITEM_VARINT(short)
// CRA_TYPE_META_ITEM_VARUINT(unsigned long)
// CRA_TYPE_META_ITEM_FLOAT(float)
// CRA_TYPE_META_ITEM_STRING(10, false)
// CRA_TYPE_META_ITEM_STRING(0, true)
// CRA_TYPE_META_ITEM_STRUCT(CraSerializer, false, 0)
// CRA_TYPE_META_ITEM_STRUCT(CraSerializer *, true, 0)
// CRA_TYPE_META_ITEM_LIST(CraSerializer, false, 0, 0, 0, 0)
// CRA_TYPE_META_ITEM_LIST(CraSerializer *, true, 0, 0, 0, 0)
// CRA_TYPE_META_ITEM_DICT(CraSerializer, false, 0, 0, 0, 0)
// CRA_TYPE_META_ITEM_DICT(CraSerializer *, true, 0, 0, 0, 0)
// CRA_TYPE_META_END();

#endif // end for items

#endif