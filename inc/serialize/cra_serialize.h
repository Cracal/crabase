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

typedef uint32_t cra_ser_count_t;
#define CRA_SER_COUNT_MAX UINT32_MAX
#define CRA_SER_SWAP_COUNT CRA_SER_SWAP32

typedef enum _CraSerError_e
{
    CRA_SER_ERROR_SUCCESS = 0,           // success
    CRA_SER_ERROR_NOBUF,                 // (de)serializer no more buffer
    CRA_SER_ERROR_MAX_NESTING,           // nested > CRA_SER_MAX_NESTING
    CRA_SER_ERROR_NOT_SUPPORT,           // this type is not supported
    CRA_SER_ERROR_INVALID_VALUE,         // invalid value
    CRA_SER_ERROR_FLOAT_NAN_OR_INF,      // [double/float] == NaN/Inf
    CRA_SER_ERROR_CANNOT_BE_NULL,        // value cannot be null
    CRA_SER_ERROR_STRING_TOO_LONG,       // str.length >= CRA_SER_SWAP_COUNT
    CRA_SER_ERROR_STRING_BUF_TOO_SMALL,  // sizeof(str[N]) < deserializer.str.length
    CRA_SER_ERROR_ARRAY_TOO_SMALL,       // sizeof(array[N]) < deserializer.array.count
    CRA_SER_ERROR_MAX_ELEMENTS,          // [list/array/dict].count >= CRA_SER_SWAP_COUNT
    CRA_SER_ERROR_TYPE_MISMATCH,         // deserializer.val.type != val.meta.type
    CRA_SER_ERROR_APPEND_ELEMENT_FAILED, // [list/dict].append(...) failed
    CRA_SER_ERROR_JSON_KEY_IS_NULL,      // json.key is null
    CRA_SER_ERROR_JSON_CANNOT_BE_KEY,    // this value(type) cannot be a key
} CraSerError_e;

typedef enum _CraType_e
{
    CRA_TYPE_FALSE = 0,
    CRA_TYPE_TRUE = 1,
    CRA_TYPE_INT8,
    CRA_TYPE_INT16,
    CRA_TYPE_INT32,
    CRA_TYPE_INT64,
    CRA_TYPE_UINT8,
    CRA_TYPE_UINT16,
    CRA_TYPE_UINT32,
    CRA_TYPE_UINT64,
    CRA_TYPE_FLOAT,
    CRA_TYPE_DOUBLE,
    CRA_TYPE_STRING,
    CRA_TYPE_STRUCT,
    CRA_TYPE_LIST,
    CRA_TYPE_DICT,
    CRA_TYPE_NULL,
    __CRA_TYPE_END,
    __CRA_TYPE_END_OF_META,
} CraType_e;

typedef struct _CraSerReleaseNode
{
    bool free;
    void *ptr;
    void (*uninit)(void *);
    void (*dealloc)(void *);
} CraSerReleaseNode;

typedef struct _CraSerRelease
{
    size_t current;
    size_t n_nodes;
    CraSerReleaseNode nodes1[32];
    CraSerReleaseNode *nodes2;
} CraSerRelease;

typedef struct _CraSerializer
{
    CraSerError_e error;
    uint16_t nested;
    bool noalloc;
    bool format;
    size_t index;
    size_t length;
    unsigned char *buffer;
    CraSerRelease release;
} CraSerializer;

typedef struct _CraTypeMeta CraTypeMeta;

typedef union _CraTypeIter_i
{
    struct
    {
        void (*init)(void *obj, void *const it, size_t itbufsize);
        bool (*next)(void *it, void **retvalptr);
        bool (*append)(void *obj, void *val);
    } list;
    struct
    {
        void (*init)(void *obj, void *const it, size_t itbufsize);
        bool (*next)(void *it, void **retkeyptr, void **retvalptr);
        bool (*append)(void *obj, void *key, void *val);
    } dict;
} CraTypeIter_i;

#define CRA_SERI_ITER_SIZE 64

typedef struct _CraTypeInit_i
{
    // 可选
    void *(*alloc)(void);
    void (*dealloc)(void *obj);
    // 必须
    void (*init)(void *obj, void *args);
    void (*uinit)(void *obj);
} CraTypeInit_i;

struct _CraTypeMeta
{
    CraType_e type;
    bool is_ptr;
    size_t size;
    size_t offset;
    size_t nsize;
    size_t noffset;
    const char *member;
    const CraTypeMeta *meta;
    const CraTypeIter_i *iter_i;
    const CraTypeInit_i *init_i;
    void *args4init;
};

#define __CRA_SIZEOF_0(_Type, _member) sizeof(((_Type *)0)->_member)
#define __CRA_SIZEOF_FALSE __CRA_SIZEOF_0
#define __CRA_SIZEOF_false __CRA_SIZEOF_0
#define __CRA_SIZEOF_False __CRA_SIZEOF_0
#define __CRA_SIZEOF_1(_Type, _member) sizeof(*((_Type *)0)->_member)
#define __CRA_SIZEOF_TRUE __CRA_SIZEOF_1
#define __CRA_SIZEOF_true __CRA_SIZEOF_1
#define __CRA_SIZEOF_True __CRA_SIZEOF_1

#define CRA_TYPE_META_DECL(_name) const CraTypeMeta _name[]
#define CRA_TYPE_META_BEGIN(_name) \
    CRA_TYPE_META_DECL(_name) = {
#define CRA_TYPE_META_END()                                                    \
    {__CRA_TYPE_END_OF_META, false, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL}, \
    }

#define __CRA_TYPE_META_NUMBER_ELEMENT(_TYPE, _size) \
    {_TYPE, false, _size, 0, 0, 0, NULL, NULL, NULL, NULL, NULL},
#define CRA_TYPE_META_BOOL_ELEMENT() \
    __CRA_TYPE_META_NUMBER_ELEMENT(CRA_TYPE_FALSE, sizeof(bool))
#define CRA_TYPE_META_INT8_ELEMENT() \
    __CRA_TYPE_META_NUMBER_ELEMENT(CRA_TYPE_INT8, sizeof(int8_t))
#define CRA_TYPE_META_INT16_ELEMENT() \
    __CRA_TYPE_META_NUMBER_ELEMENT(CRA_TYPE_INT16, sizeof(int16_t))
#define CRA_TYPE_META_INT32_ELEMENT() \
    __CRA_TYPE_META_NUMBER_ELEMENT(CRA_TYPE_INT32, sizeof(int32_t))
#define CRA_TYPE_META_INT64_ELEMENT() \
    __CRA_TYPE_META_NUMBER_ELEMENT(CRA_TYPE_INT64, sizeof(int64_t))
#define CRA_TYPE_META_UINT8_ELEMENT() \
    __CRA_TYPE_META_NUMBER_ELEMENT(CRA_TYPE_UINT8, sizeof(uint8_t))
#define CRA_TYPE_META_UINT16_ELEMENT() \
    __CRA_TYPE_META_NUMBER_ELEMENT(CRA_TYPE_UINT16, sizeof(uint16_t))
#define CRA_TYPE_META_UINT32_ELEMENT() \
    __CRA_TYPE_META_NUMBER_ELEMENT(CRA_TYPE_UINT32, sizeof(uint32_t))
#define CRA_TYPE_META_UINT64_ELEMENT() \
    __CRA_TYPE_META_NUMBER_ELEMENT(CRA_TYPE_UINT64, sizeof(uint64_t))
#define CRA_TYPE_META_FLOAT_ELEMENT() \
    __CRA_TYPE_META_NUMBER_ELEMENT(CRA_TYPE_FLOAT, sizeof(float))
#define CRA_TYPE_META_DOUBLE_ELEMENT() \
    __CRA_TYPE_META_NUMBER_ELEMENT(CRA_TYPE_DOUBLE, sizeof(double))
// `_is_char_ptr`: is cahr pointer, not char array. `true`: char *; `false`: char[N];
#define CRA_TYPE_META_STRING_ELEMENT(_length, _is_char_ptr) \
    {CRA_TYPE_STRING, _is_char_ptr, _length, 0, 0, 0, NULL, NULL, NULL, NULL, NULL},
// `size`: `1. struct X name -> sizeof(struct X)`; `2. struct X *name -> sizeof(struct X)`
// `is_ptr`: true` -> struct X *; `false` -> struct X;
#define CRA_TYPE_META_STRUCT_ELEMENT(_size, _is_ptr, _members_meta, _init_i, _args4init) \
    {CRA_TYPE_STRUCT, _is_ptr, _size, 0, 0, 0, NULL, _members_meta, NULL, _init_i, _args4init},
// `size`: `1. ListType name -> sizeof(ListType)`; `2. ListType *name -> sizeof(ListType)`
// `is_ptr`: true` -> ListType *; `false` -> ListType;
#define CRA_TYPE_META_LIST_ELEMENT(_size, _is_ptr, _element_meta, _iter_i, _init_i, _args4init) \
    {CRA_TYPE_LIST, _is_ptr, _size, 0, 0, 0, NULL, _element_meta, _iter_i, _init_i, _args4init},
// `size`: `1. ArrayType name -> sizeof(ArrayType)`; `2. ArrayType *name -> sizeof(ArrayType)`
// `is_ptr`: true` -> ArrayType *; `false` -> ArrayType;
#define CRA_TYPE_META_ARRAY_ELEMENT(_size, _is_ptr, _element_meta) \
    {CRA_TYPE_LIST, _is_ptr, _size, 0, sizeof(uint32_t), 0, NULL, _element_meta, NULL, NULL, NULL},
// `size`: `1. DictType name -> sizeof(DictType)`; `2. DictType *name -> sizeof(DictType)`
// `is_ptr`: true` -> DictType *; `false` -> DictType;
#define CRA_TYPE_META_DICT_ELEMENT(_size, _is_ptr, _kv_meta, _iter_i, _init_i, _args4init) \
    {CRA_TYPE_DICT, _is_ptr, _size, 0, 0, 0, NULL, _kv_meta, _iter_i, _init_i, _args4init},

#define __CRA_TYPE_META_NUMBER_MEMBER(_Type, _TYPE, _member) \
    {_TYPE, false, __CRA_SIZEOF_0(_Type, _member), offsetof(_Type, _member), 0, 0, #_member, NULL, NULL, NULL, NULL},
#define CRA_TYPE_META_BOOL_MEMBER(_Type, _member) \
    __CRA_TYPE_META_NUMBER_MEMBER(_Type, CRA_TYPE_FALSE, _member)
#define CRA_TYPE_META_INT8_MEMBER(_Type, _member) \
    __CRA_TYPE_META_NUMBER_MEMBER(_Type, CRA_TYPE_INT8, _member)
#define CRA_TYPE_META_INT16_MEMBER(_Type, _member) \
    __CRA_TYPE_META_NUMBER_MEMBER(_Type, CRA_TYPE_INT16, _member)
#define CRA_TYPE_META_INT32_MEMBER(_Type, _member) \
    __CRA_TYPE_META_NUMBER_MEMBER(_Type, CRA_TYPE_INT32, _member)
#define CRA_TYPE_META_INT64_MEMBER(_Type, _member) \
    __CRA_TYPE_META_NUMBER_MEMBER(_Type, CRA_TYPE_INT64, _member)
#define CRA_TYPE_META_UINT8_MEMBER(_Type, _member) \
    __CRA_TYPE_META_NUMBER_MEMBER(_Type, CRA_TYPE_UINT8, _member)
#define CRA_TYPE_META_UINT16_MEMBER(_Type, _member) \
    __CRA_TYPE_META_NUMBER_MEMBER(_Type, CRA_TYPE_UINT16, _member)
#define CRA_TYPE_META_UINT32_MEMBER(_Type, _member) \
    __CRA_TYPE_META_NUMBER_MEMBER(_Type, CRA_TYPE_UINT32, _member)
#define CRA_TYPE_META_UINT64_MEMBER(_Type, _member) \
    __CRA_TYPE_META_NUMBER_MEMBER(_Type, CRA_TYPE_UINT64, _member)
#define CRA_TYPE_META_FLOAT_MEMBER(_Type, _member) \
    __CRA_TYPE_META_NUMBER_MEMBER(_Type, CRA_TYPE_FLOAT, _member)
#define CRA_TYPE_META_DOUBLE_MEMBER(_Type, _member) \
    __CRA_TYPE_META_NUMBER_MEMBER(_Type, CRA_TYPE_DOUBLE, _member)
// `_is_char_ptr`: is cahr pointer, not char array. `true`: char *; `false`: char[N];
#define CRA_TYPE_META_STRING_MEMBER(_Type, _member, _is_char_ptr)                                                \
    {CRA_TYPE_STRING, _is_char_ptr, __CRA_SIZEOF_##_is_char_ptr(_Type, _member), offsetof(_Type, _member), 0, 0, \
     #_member, NULL, NULL, NULL, NULL},
// `_is_char_ptr`: is not a char array. `true`: char *; `false`: char[N];
#define CRA_TYPE_META_STRING_NZ_MEMBER(_Type, _member, _is_char_ptr)                                       \
    {CRA_TYPE_STRING, _is_char_ptr, __CRA_SIZEOF_##_is_char_ptr(_Type, _member), offsetof(_Type, _member), \
     __CRA_SIZEOF_0(_Type, n##_member), offsetof(_Type, n##_member), #_member, NULL, NULL, NULL, NULL},
// `is_ptr`: `true` -> struct X *; `false` -> struct X;
#define CRA_TYPE_META_STRUCT_MEMBER(_Type, _member, _is_ptr, _members_meta, _init_i, _args4init)       \
    {CRA_TYPE_STRUCT, _is_ptr, __CRA_SIZEOF_##_is_ptr(_Type, _member), offsetof(_Type, _member), 0, 0, \
     #_member, _members_meta, NULL, _init_i, _args4init},
// `is_ptr`: `true` -> ListType *; `false` -> ListType;
#define CRA_TYPE_META_LIST_MEMBER(_Type, _member, _is_ptr, _element_meta, _iter_i, _init_i, _args4init) \
    {CRA_TYPE_LIST, _is_ptr, __CRA_SIZEOF_##_is_ptr(_Type, _member), offsetof(_Type, _member), 0, 0,    \
     #_member, _element_meta, _iter_i, _init_i, _args4init},
// `is_ptr`: `true` -> ArrayType *; `false` -> ArrayType[N];
#define CRA_TYPE_META_ARRAY_MEMBER(_Type, _member, _is_ptr, _element_meta)                     \
    {CRA_TYPE_LIST, _is_ptr, __CRA_SIZEOF_##_is_ptr(_Type, _member), offsetof(_Type, _member), \
     __CRA_SIZEOF_0(_Type, n##_member), offsetof(_Type, n##_member), #_member, _element_meta, NULL, NULL, NULL},
// `is_ptr`: `true` -> DictType *; `false` -> DictType;
#define CRA_TYPE_META_DICT_MEMBER(_Type, _member, _is_ptr, _kv_meta, _iter_i, _init_i, _args4init) \
    {CRA_TYPE_DICT, _is_ptr, __CRA_SIZEOF_##_is_ptr(_Type, _member), offsetof(_Type, _member),     \
     0, 0, #_member, _kv_meta, _iter_i, _init_i, _args4init},

#endif