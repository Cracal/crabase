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
#include "cra_macro.h"

typedef struct CraSerInfo CraSerInfo;
typedef struct CraSerMeta CraSerMeta;
typedef struct CraSerErr  CraSerErr;

typedef enum CraSerErr_e
{
    CRA_SER_ERR_OK = 0,
    CRA_SER_ERR_NO_BUF,
    CRA_SER_ERR_NO_MEM,
    CRA_SER_ERR_NESTING,
    CRA_SER_ERR_LENGTH_MAX,
    CRA_SER_ERR_INIT_FAILED,
    CRA_SER_ERR_OUT_OF_RANGE,
    CRA_SER_ERR_APPEND_FAILED,
    CRA_SER_ERR_INVALID_VALUE,
    CRA_SER_ERR_TYPE_MISMATCH,
    CRA_SER_ERR_CANNOT_BE_NULL,
    CRA_SER_ERR_DUPLICATE_MEMBER,
    CRA_SER_ERR_SKIP_MEMBER_FAILED,
} CraSerErr_e;

struct CraSerErr
{
    CraSerErr_e err;
    char        msg[128 - sizeof(CraSerErr_e)];
};

typedef enum CraSerType_e
{
    CRA_SER_TYPE_BOOL = 0, // bool
    CRA_SER_TYPE_VARINT,   // enum, i8, u8, i16, u16, i32, u32, i64, u64
    CRA_SER_TYPE_32BITS,   // i32, u32, f32
    CRA_SER_TYPE_64BITS,   // i64, u64, f64
    CRA_SER_TYPE_STRING,   // string
    CRA_SER_TYPE_STRUCT,   // struct
    CRA_SER_TYPE_LIST,     // list, c array
    CRA_SER_TYPE_DICT,     // dict

    CRA_SER_TYPE_COUNT,
} CraSerType_e;
static_assert(CRA_SER_TYPE_COUNT <= 0x08, "too many types, must be less than 8 types");

struct CraSerInfo
{
    uint16_t flags;
    uint16_t type;
    uint16_t size;
    uint16_t offset;
};

struct CraSerMeta
{
    const CraSerMeta **metas;
    const CraSerInfo  *infos;
    uint16_t           count;
    uint16_t           flags;
    uint16_t           type;
    uint16_t           size;
    union
    {
        struct
        {
            const char     *names;
            const uint16_t *namei;
            const uint16_t *ids;

            bool (*init)(void *stru, size_t size);
            void (*uninit)(void *stru);
        } for_struct;
        struct
        {
            uint16_t size_of_slot;
            uint16_t offsetof_len;

            void (*uninit)(void *array);
        } for_array;
        struct
        {
            bool (*init)(void *list, size_t itemsize, size_t count);
            void (*uninit)(void *list);
            bool (*append)(void *list, void *item);
            void (*iter_init)(void *iter, size_t itersize, void *list, size_t *retcount);
            bool (*iter_next)(void *iter, void *retitem);
        } for_list;
        struct
        {
            bool (*init)(void *dict, size_t keysize, size_t valsize, size_t count);
            void (*uninit)(void *dict);
            bool (*append)(void *dict, void *key, void *val);
            void (*iter_init)(void *iter, size_t itersize, void *dict, size_t *retcount);
            bool (*iter_next)(void *iter, void *retkey, void *retval);
        } for_dict;
    };
};

#define CRA_SER_STRING_MAX_LENGTH UINT16_MAX
#define CRA_SER_STRUCT_MAX_FIELDS UINT16_MAX
#define CRA_SER_LIST_MAX_COUNT    UINT16_MAX
#define CRA_SER_DICT_MAX_COUNT    UINT16_MAX

#define CRA_SER_FLAG_PTR      CRA_BITS(0)
#define CRA_SER_FLAG_BOOL     CRA_BITS(1)
#define CRA_SER_FLAG_ENUM     CRA_BITS(2)
#define CRA_SER_FLAG_FLOAT    CRA_BITS(3)
#define CRA_SER_FLAG_ARRAY    CRA_BITS(4)
#define CRA_SER_FLAG_UNSIGNED CRA_BITS(5)

#if 1 // check

static_assert(sizeof(bool) == 1, "bool must be 1 byte");
static_assert(sizeof(char) == 1, "char must be 1 byte");
static_assert(sizeof(unsigned char) == 1, "unsigned char must be 1 byte");
static_assert(sizeof(int8_t) == 1, "int8_t must be 1 byte");
static_assert(sizeof(int16_t) == 2, "int16_t must be 2 bytes");
static_assert(sizeof(int32_t) == 4, "int32_t must be 4 bytes");
static_assert(sizeof(int64_t) == 8, "int64_t must be 8 bytes");
static_assert(sizeof(uint8_t) == 1, "uint8_t must be 1 byte");
static_assert(sizeof(uint16_t) == 2, "uint16_t must be 2 bytes");
static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");
static_assert(sizeof(uint64_t) == 8, "uint64_t must be 8 bytes");
static_assert(sizeof(float) == 4, "float must be 4 bytes");
static_assert(sizeof(double) == 8, "double must be 8 bytes");
static_assert(sizeof(char[100]) == 100, "char[100] must be 100 bytes");

#if defined(BOOL)
static_assert(false, "Please undefine `BOOL` before including this header");
#endif
#if defined(ENUM)
static_assert(false, "Please undefine `ENUM` before including this header");
#endif
#if defined(INT8)
static_assert(false, "Please undefine `INT8` before including this header");
#endif
#if defined(INT16)
static_assert(false, "Please undefine `INT16` before including this header");
#endif
#if defined(INT32)
static_assert(false, "Please undefined `INT32` before include this header");
#endif
#if defined(INT64)
static_assert(false, "Please undefine `INT64` before including this header");
#endif
#if defined(UINT8)
static_assert(false, "Please undefine `UINT8` before including this header");
#endif
#if defined(UINT16)
static_assert(false, "Please undefine `UINT16` before including this header");
#endif
#if defined(UINT32)
static_assert(false, "Please undefine `UINT32` before including this header");
#endif
#if defined(UINT64)
static_assert(false, "Please undefine `UINT64` before including this header");
#endif
#if defined(FLOAT32)
static_assert(false, "Please undefine `FLOAT32` before including this header");
#endif
#if defined(FLOAT64)
static_assert(false, "Please undefine `FLOAT64` before including this header");
#endif
#if defined(STRING)
static_assert(false, "Please undefine `STRING` before including this header");
#endif
#if defined(STRUCT)
static_assert(false, "Please undefine `STRUCT` before including this header");
#endif
#if defined(LIST)
static_assert(false, "Please undefine `LIST` before including this header");
#endif
#if defined(DICT)
static_assert(false, "Please undefine `DICT` before including this header");
#endif

#endif // end check

#define CRA_SER_STAR0
#define CRA_SER_STAR1 *

#if 1 // id def

#define CRA_SER_SKIP(TYPE)

#define CRA_SER_ID_DEF_(TYPE, name, id, ...) id,
#define CRA_SER_ID_DEF(...)                  CRA_SER_ID_DEF_(__VA_ARGS__, 0)

#endif // end id def

#if 1 // names def

#define CRA_SER_NAMES_DEF(TYPE, name, ...) #name

#endif // end names def

#if 1 // namei def

#define CRA_SER_NAMEI_DEF_(TYPE, name, ...) +(sizeof(#name) - 1)
#define CRA_SER_NAMEI_DEF(index, field_def)                                                        \
    CRA_ITER(CRA_UNTUPLE_M, CRA_SER_NAMEI_DEF_, CRA_GET_N(CRA_INC(index), CRA_UNTUPLE field_def)),

#endif // end namei def

#if 1 // field def

#define CRA_SER_FIELD_DEF_BOOL(name, id)         bool name;
#define CRA_SER_FIELD_DEF_ENUM(name, id, type)   type name;
#define CRA_SER_FIELD_DEF_INT8(name, ...)        int8_t name;
#define CRA_SER_FIELD_DEF_INT16(name, ...)       int16_t name;
#define CRA_SER_FIELD_DEF_INT32(name, ...)       int32_t name;
#define CRA_SER_FIELD_DEF_INT64(name, ...)       int64_t name;
#define CRA_SER_FIELD_DEF_UINT8(name, ...)       uint8_t name;
#define CRA_SER_FIELD_DEF_UINT16(name, ...)      uint16_t name;
#define CRA_SER_FIELD_DEF_UINT32(name, ...)      uint32_t name;
#define CRA_SER_FIELD_DEF_UINT64(name, ...)      uint64_t name;
#define CRA_SER_FIELD_DEF_FLOAT32(name, ...)     float name;
#define CRA_SER_FIELD_DEF_FLOAT64(name, ...)     double name;
#define CRA_SER_FIELD_DEF_STRING2(name, id)      char *name;
#define CRA_SER_FIELD_DEF_STRING3(name, id, len) char name[len];
#define CRA_SER_FIELD_DEF_STRING(...)            CRA_CAT(CRA_SER_FIELD_DEF_STRING, CRA_CNT(__VA_ARGS__))(__VA_ARGS__)
#define CRA_SER_FIELD_DEF_STRUCT_(name, id, type, meta, is_ptr, ...) type CRA_SER_STAR##is_ptr name;
#define CRA_SER_FIELD_DEF_STRUCT(...)                                CRA_SER_FIELD_DEF_STRUCT_(__VA_ARGS__, 0, 0)
#define CRA_SER_FIELD_DEF_ARRAY(name, id, type, meta)                type name;
#define CRA_SER_FIELD_DEF_LIST                                       CRA_SER_FIELD_DEF_STRUCT
#define CRA_SER_FIELD_DEF_DICT                                       CRA_SER_FIELD_DEF_STRUCT

#define CRA_SER_FIELD_DEF(TYPE, ...) CRA_CAT(CRA_SER_FIELD_DEF_, TYPE)(__VA_ARGS__)

#endif // end field def

#if 1 // array item def

#define CRA_SER_ARRAY_ITEM_DEF1_BOOL1(_)                            bool *data;
#define CRA_SER_ARRAY_ITEM_DEF2_BOOL1(len, _)                       bool data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_ENUM2(type, _)                      type *data;
#define CRA_SER_ARRAY_ITEM_DEF2_ENUM2(len, type, _)                 type data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_INT81(_)                            int8_t *data;
#define CRA_SER_ARRAY_ITEM_DEF1_INT82(is_varint, _)                 int8_t *data;
#define CRA_SER_ARRAY_ITEM_DEF2_INT81(len, _)                       int8_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF2_INT82(len, is_varint, _)            int8_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_INT161(_)                           int16_t *data;
#define CRA_SER_ARRAY_ITEM_DEF1_INT162(is_varint, _)                int16_t *data;
#define CRA_SER_ARRAY_ITEM_DEF2_INT161(len, _)                      int16_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF2_INT162(len, is_varint, _)           int16_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_INT321(_)                           int32_t *data;
#define CRA_SER_ARRAY_ITEM_DEF1_INT322(is_varint, _)                int32_t *data;
#define CRA_SER_ARRAY_ITEM_DEF2_INT321(len, _)                      int32_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF2_INT322(len, is_varint, _)           int32_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_INT641(_)                           int64_t *data;
#define CRA_SER_ARRAY_ITEM_DEF1_INT642(is_varint, _)                int64_t *data;
#define CRA_SER_ARRAY_ITEM_DEF2_INT641(len, _)                      int64_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF2_INT642(len, is_varint, _)           int64_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_UINT81(_)                           uint8_t *data;
#define CRA_SER_ARRAY_ITEM_DEF1_UINT82(is_varint, _)                uint8_t *data;
#define CRA_SER_ARRAY_ITEM_DEF2_UINT81(len, _)                      uint8_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF2_UINT82(len, is_varint, _)           uint8_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_UINT161(_)                          uint16_t *data;
#define CRA_SER_ARRAY_ITEM_DEF1_UINT162(is_varint, _)               uint16_t *data;
#define CRA_SER_ARRAY_ITEM_DEF2_UINT161(len, _)                     uint16_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF2_UINT162(len, is_varint, _)          uint16_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_UINT321(_)                          uint32_t *data;
#define CRA_SER_ARRAY_ITEM_DEF1_UINT322(is_varint, _)               uint32_t *data;
#define CRA_SER_ARRAY_ITEM_DEF2_UINT321(len, _)                     uint32_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF2_UINT322(len, is_varint, _)          uint32_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_UINT641(_)                          uint64_t *data;
#define CRA_SER_ARRAY_ITEM_DEF1_UINT642(is_varint, _)               uint64_t *data;
#define CRA_SER_ARRAY_ITEM_DEF2_UINT641(len, _)                     uint64_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF2_UINT642(len, is_varint, _)          uint64_t data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_FLOAT321(_)                         float *data;
#define CRA_SER_ARRAY_ITEM_DEF2_FLOAT321(len, _)                    float data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_FLOAT641(_)                         double *data;
#define CRA_SER_ARRAY_ITEM_DEF2_FLOAT641(len, _)                    double data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_STRING1(_)                          char **data;
#define CRA_SER_ARRAY_ITEM_DEF1_STRING2(len, _)                     char (*data)[len];
#define CRA_SER_ARRAY_ITEM_DEF2_STRING1(alen, _)                    char *data[alen];
#define CRA_SER_ARRAY_ITEM_DEF2_STRING2(alen, ilen, _)              char data[alen][ilen];
#define CRA_SER_ARRAY_ITEM_DEF1_STRUCT3(type, meta, _)              type *data;
#define CRA_SER_ARRAY_ITEM_DEF1_STRUCT4(type, meta, is_ptr, _)      type CRA_SER_STAR##is_ptr *data;
#define CRA_SER_ARRAY_ITEM_DEF2_STRUCT3(len, type, meta, _)         type data[len];
#define CRA_SER_ARRAY_ITEM_DEF2_STRUCT4(len, type, meta, is_ptr, _) type CRA_SER_STAR##is_ptr data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_ARRAY3(type, meta, _)               type *data;
#define CRA_SER_ARRAY_ITEM_DEF2_ARRAY3(len, type, meta, _)          type data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_LIST3(type, meta, _)                type *data;
#define CRA_SER_ARRAY_ITEM_DEF1_LIST4(type, meta, is_ptr, _)        type CRA_SER_STAR##is_ptr *data;
#define CRA_SER_ARRAY_ITEM_DEF2_LIST3(len, type, meta, _)           type data[len];
#define CRA_SER_ARRAY_ITEM_DEF2_LIST4(len, type, meta, is_ptr, _)   type CRA_SER_STAR##is_ptr data[len];
#define CRA_SER_ARRAY_ITEM_DEF1_DICT3(type, meta, _)                type *data;
#define CRA_SER_ARRAY_ITEM_DEF1_DICT4(type, meta, is_ptr, _)        type CRA_SER_STAR##is_ptr *data;
#define CRA_SER_ARRAY_ITEM_DEF2_DICT3(len, type, meta, _)           type data[len];
#define CRA_SER_ARRAY_ITEM_DEF2_DICT4(len, type, meta, is_ptr, _)   type CRA_SER_STAR##is_ptr data[len];

#define CRA_SER_ARRAY_ITEM_DEF1(array_def, TYPE, ...)                                   \
    CRA_CAT(CRA_SER_ARRAY_ITEM_DEF1_, CRA_CAT(TYPE, CRA_CNT(__VA_ARGS__)))(__VA_ARGS__)
#define CRA_SER_ARRAY_ITEM_DEF2(array_def, TYPE, ...)                      \
    CRA_CAT(CRA_SER_ARRAY_ITEM_DEF2_, CRA_CAT(TYPE, CRA_CNT(__VA_ARGS__))) \
    (CRA_GET_I(1, CRA_UNTUPLE array_def), __VA_ARGS__)

#endif // end array item def

#if 1 // meta def

#define CRA_SER_META_DEF_(TYPE, name, id, _x1, meta, ...) meta,
#define CRA_SER_META_DEF(...)                             CRA_SER_META_DEF_(__VA_ARGS__, 0, 0, 0)

#define CRA_SER_ITEM_META_DEF_(TYPE, _x1, meta, ...) meta
#define CRA_SER_ITEM_META_DEF(...)                   CRA_SER_ITEM_META_DEF_(__VA_ARGS__, 0, 0, 0)

#endif // end meta def

#if 1 // info def

#define CRA_SER_INFO_VAR0(bits) bits##BITS
#define CRA_SER_INFO_VAR1(bits) VARINT
#define CRA_SER_INFO_PTR0       0
#define CRA_SER_INFO_PTR1       CRA_SER_FLAG_PTR

#define CRA_SER_INFO_DEF_SET(flags, TYPE, size, offset) { flags, CRA_CAT(CRA_SER_TYPE_, TYPE), size, offset },

#define CRA_SER_INFO_DEF_BOOL(offset, ...) CRA_SER_INFO_DEF_SET(CRA_SER_FLAG_BOOL, BOOL, sizeof(bool), offset)
#define CRA_SER_INFO_DEF_ENUM(offset, type, ...)                                                  \
    CRA_SER_INFO_DEF_SET(CRA_SER_FLAG_UNSIGNED | CRA_SER_FLAG_ENUM, VARINT, sizeof(type), offset)
#define CRA_SER_INFO_DEF_INT8(offset, ...)  CRA_SER_INFO_DEF_SET(0, VARINT, sizeof(int8_t), offset)
#define CRA_SER_INFO_DEF_INT16(offset, ...) CRA_SER_INFO_DEF_SET(0, VARINT, sizeof(int16_t), offset)
#define CRA_SER_INFO_DEF_INT32(offset, is_var, ...)                                         \
    CRA_SER_INFO_DEF_SET(0, CRA_CAT(CRA_SER_INFO_VAR, is_var)(32), sizeof(int32_t), offset)
#define CRA_SER_INFO_DEF_INT64(offset, is_var, ...)                                         \
    CRA_SER_INFO_DEF_SET(0, CRA_CAT(CRA_SER_INFO_VAR, is_var)(64), sizeof(int64_t), offset)
#define CRA_SER_INFO_DEF_UINT8(offset, ...) CRA_SER_INFO_DEF_SET(CRA_SER_FLAG_UNSIGNED, VARINT, sizeof(uint8_t), offset)
#define CRA_SER_INFO_DEF_UINT16(offset, ...)                                      \
    CRA_SER_INFO_DEF_SET(CRA_SER_FLAG_UNSIGNED, VARINT, sizeof(uint16_t), offset)
#define CRA_SER_INFO_DEF_UINT32(offset, is_var, ...)                                                             \
    CRA_SER_INFO_DEF_SET(CRA_SER_FLAG_UNSIGNED, CRA_CAT(CRA_SER_INFO_VAR, is_var)(32), sizeof(uint32_t), offset)
#define CRA_SER_INFO_DEF_UINT64(offset, is_var, ...)                                                             \
    CRA_SER_INFO_DEF_SET(CRA_SER_FLAG_UNSIGNED, CRA_CAT(CRA_SER_INFO_VAR, is_var)(64), sizeof(uint64_t), offset)
#define CRA_SER_INFO_DEF_FLOAT32(offset, ...) CRA_SER_INFO_DEF_SET(CRA_SER_FLAG_FLOAT, 32BITS, sizeof(float), offset)
#define CRA_SER_INFO_DEF_FLOAT64(offset, ...) CRA_SER_INFO_DEF_SET(CRA_SER_FLAG_FLOAT, 64BITS, sizeof(double), offset)
#define CRA_SER_INFO_DEF_STRING(offset, len, ...)                                              \
    CRA_SER_INFO_DEF_SET(len > 0 ? CRA_SER_FLAG_ARRAY : CRA_SER_FLAG_PTR, STRING, len, offset)
#define CRA_SER_INFO_DEF_STRUCT(offset, type, meta, is_ptr, ...)                          \
    CRA_SER_INFO_DEF_SET(CRA_CAT(CRA_SER_INFO_PTR, is_ptr), STRUCT, sizeof(type), offset)
#define CRA_SER_INFO_DEF_ARRAY(offset, type, meta, ...)                  \
    CRA_SER_INFO_DEF_SET(CRA_SER_FLAG_ARRAY, LIST, sizeof(type), offset)
#define CRA_SER_INFO_DEF_LIST(offset, type, meta, is_ptr, ...)                          \
    CRA_SER_INFO_DEF_SET(CRA_CAT(CRA_SER_INFO_PTR, is_ptr), LIST, sizeof(type), offset)
#define CRA_SER_INFO_DEF_DICT(offset, type, meta, is_ptr, ...)                          \
    CRA_SER_INFO_DEF_SET(CRA_CAT(CRA_SER_INFO_PTR, is_ptr), DICT, sizeof(type), offset)

#define CRA_SER_INFO_DEF__(stru_type, TYPE, name, id, ...)                   \
    CRA_CAT(CRA_SER_INFO_DEF_, TYPE)(offsetof(stru_type, name), __VA_ARGS__)
#define CRA_SER_INFO_DEF_(stru_type, ...)      CRA_SER_INFO_DEF__(stru_type, __VA_ARGS__, 0, 0)
#define CRA_SER_INFO_DEF(stru_type, field_def) CRA_SER_INFO_DEF_(stru_type, CRA_UNTUPLE field_def)

#define CRA_SER_ITEM_INFO_DEF_(TYPE, ...) CRA_CAT(CRA_SER_INFO_DEF_, TYPE)(0, __VA_ARGS__)
#define CRA_SER_ITEM_INFO_DEF(...)        CRA_SER_ITEM_INFO_DEF_(__VA_ARGS__, 0, 0, 0)

#endif // end info def

#if 1 // free struct field

#define CRA_SER_HAS_AND0 &
#define CRA_SER_HAS_AND1

#define CRA_SER_FREE_STRUCT_FIELD_BOOL(...)
#define CRA_SER_FREE_STRUCT_FIELD_ENUM(...)
#define CRA_SER_FREE_STRUCT_FIELD_INT8(...)
#define CRA_SER_FREE_STRUCT_FIELD_INT16(...)
#define CRA_SER_FREE_STRUCT_FIELD_INT32(...)
#define CRA_SER_FREE_STRUCT_FIELD_INT64(...)
#define CRA_SER_FREE_STRUCT_FIELD_UINT8(...)
#define CRA_SER_FREE_STRUCT_FIELD_UINT16(...)
#define CRA_SER_FREE_STRUCT_FIELD_UINT32(...)
#define CRA_SER_FREE_STRUCT_FIELD_UINT64(...)
#define CRA_SER_FREE_STRUCT_FIELD_FLOAT32(...)
#define CRA_SER_FREE_STRUCT_FIELD_FLOAT64(...)
#define CRA_SER_FREE_STRUCT_FIELD_STRING(name, id, _len, ...) \
    if (!(_len) && stru->name)                                \
        cra_free(stru->name);
#define CRA_SER_FREE_STRUCT_FIELD_STRUCT(name, id, type, meta, is_ptr, ...)         \
    if (CRA_CAT(CRA_SER_HAS_AND, is_ptr) stru->name)                                \
    {                                                                               \
        if ((meta)->for_struct.uninit)                                              \
            (meta)->for_struct.uninit(CRA_CAT(CRA_SER_HAS_AND, is_ptr) stru->name); \
        if (is_ptr)                                                                 \
            cra_free(CRA_CAT(CRA_SER_HAS_AND, is_ptr) stru->name);                  \
    }
#define CRA_SER_FREE_STRUCT_FIELD_ARRAY(name, id, type, meta, ...) \
    if ((meta)->for_array.uninit)                                  \
        (meta)->for_array.uninit(&stru->name);
#define CRA_SER_FREE_STRUCT_FIELD_LIST(name, id, type, meta, is_ptr, ...)         \
    if (CRA_CAT(CRA_SER_HAS_AND, is_ptr) stru->name)                              \
    {                                                                             \
        if ((meta)->for_list.uninit)                                              \
            (meta)->for_list.uninit(CRA_CAT(CRA_SER_HAS_AND, is_ptr) stru->name); \
        if (is_ptr)                                                               \
            cra_free(CRA_CAT(CRA_SER_HAS_AND, is_ptr) stru->name);                \
    }
#define CRA_SER_FREE_STRUCT_FIELD_DICT(name, id, type, meta, is_ptr, ...)         \
    if (CRA_CAT(CRA_SER_HAS_AND, is_ptr) stru->name)                              \
    {                                                                             \
        if ((meta)->for_dict.uninit)                                              \
            (meta)->for_dict.uninit(CRA_CAT(CRA_SER_HAS_AND, is_ptr) stru->name); \
        if (is_ptr)                                                               \
            cra_free(CRA_CAT(CRA_SER_HAS_AND, is_ptr) stru->name);                \
    }

#define CRA_SER_FREE_STRUCT_FIELD(TYPE, ...) CRA_CAT(CRA_SER_FREE_STRUCT_FIELD_, TYPE)(__VA_ARGS__, 0, 0)

#endif // end free struct field

#if 1 // free array item

#define CRA_SER_FREE_ARRAY_ITEM_BOOL(...)
#define CRA_SER_FREE_ARRAY_ITEM_ENUM(...)
#define CRA_SER_FREE_ARRAY_ITEM_INT8(...)
#define CRA_SER_FREE_ARRAY_ITEM_INT16(...)
#define CRA_SER_FREE_ARRAY_ITEM_INT32(...)
#define CRA_SER_FREE_ARRAY_ITEM_INT64(...)
#define CRA_SER_FREE_ARRAY_ITEM_UINT8(...)
#define CRA_SER_FREE_ARRAY_ITEM_UINT16(...)
#define CRA_SER_FREE_ARRAY_ITEM_UINT32(...)
#define CRA_SER_FREE_ARRAY_ITEM_UINT64(...)
#define CRA_SER_FREE_ARRAY_ITEM_FLOAT32(...)
#define CRA_SER_FREE_ARRAY_ITEM_FLOAT64(...)
#define CRA_SER_FREE_ARRAY_ITEM_STRING(_len, ...) \
    if (!(_len) && array->data)                   \
    {                                             \
        for (uint16_t i = 0; i < array->len; ++i) \
            cra_free(array->data[i]);             \
    }
#define CRA_SER_FREE_ARRAY_ITEM_STRUCT(type, meta, is_ptr, ...)           \
    if ((is_ptr || meta->for_struct.uninit) && array->data)               \
    {                                                                     \
        for (uint16_t i = 0; i < array->len; ++i)                         \
        {                                                                 \
            type *item = CRA_CAT(CRA_SER_HAS_AND, is_ptr) array->data[i]; \
            if (meta->for_struct.uninit)                                  \
                meta->for_struct.uninit(item);                            \
            if (is_ptr)                                                   \
                cra_free(item);                                           \
        }                                                                 \
    }
#define CRA_SER_FREE_ARRAY_ITEM_ARRAY(type, meta, ...) \
    if (meta->for_array.uninit && array->data)         \
    {                                                  \
        for (uint16_t i = 0; i < array->len; ++i)      \
            meta->for_array.uninit(&array->data[i]);   \
    }
#define CRA_SER_FREE_ARRAY_ITEM_LIST(type, meta, is_ptr, ...)             \
    if ((is_ptr || meta->for_list.uninit) && array->data)                 \
    {                                                                     \
        for (uint16_t i = 0; i < array->len; ++i)                         \
        {                                                                 \
            type *item = CRA_CAT(CRA_SER_HAS_AND, is_ptr) array->data[i]; \
            if (meta->for_list.uninit)                                    \
                meta->for_list.uninit(item);                              \
            if (is_ptr)                                                   \
                cra_free(item);                                           \
        }                                                                 \
    }
#define CRA_SER_FREE_ARRAY_ITEM_DICT(type, meta, is_ptr, ...)             \
    if ((is_ptr || meta->for_dict.uninit) && array->data)                 \
    {                                                                     \
        for (uint16_t i = 0; i < array->len; ++i)                         \
        {                                                                 \
            type *item = CRA_CAT(CRA_SER_HAS_AND, is_ptr) array->data[i]; \
            if (meta->for_dict.uninit)                                    \
                meta->for_dict.uninit(item);                              \
            if (is_ptr)                                                   \
                cra_free(item);                                           \
        }                                                                 \
    }

#define CRA_SER_FREE_ARRAY_ITEM_(TYPE, ...) CRA_CAT(CRA_SER_FREE_ARRAY_ITEM_, TYPE)(__VA_ARGS__)
#define CRA_SER_FREE_ARRAY_ITEM(...)        CRA_SER_FREE_ARRAY_ITEM_(__VA_ARGS__, 0, 0, 0)

#endif // end free array item

#if 1 // struct def

#define CRA_SER_DEF_STRUCT(name, ... /* fields */)                   \
    typedef struct name                                              \
    {                                                                \
        CRA_ITER(CRA_UNTUPLE_M, CRA_SER_FIELD_DEF, __VA_ARGS__)      \
    } name;                                                          \
    static_assert(sizeof(name) <= UINT16_MAX, "STRUCT is too large")
// @see CRA_SER_DEF_STRUCT_META_AND_STRUCT
#define CRA_SER_DEF_STRUCT_META(sc, name, init_fn, uninit_fn, ... /* fields */)                                      \
    sc const uint16_t    name##_ids[] = { CRA_ITER(CRA_UNTUPLE_M, CRA_SER_ID_DEF, __VA_ARGS__) };                    \
    sc const uint16_t    name##_namei[] = { 0, CRA_REPEAT(CRA_SER_NAMEI_DEF, (__VA_ARGS__), CRA_CNT(__VA_ARGS__)) }; \
    sc const CraSerMeta *name##_metas[] = { CRA_ITER(CRA_UNTUPLE_M, CRA_SER_META_DEF, __VA_ARGS__) };                \
    sc const CraSerInfo  name##_infos[] = { CRA_ITER(CRA_SER_INFO_DEF, name, __VA_ARGS__) };                         \
    sc const CraSerMeta  name##_meta = {                                                                             \
        .metas = name##_metas,                                                                                       \
        .infos = name##_infos,                                                                                       \
        .count = CRA_NARRAY(name##_infos),                                                                           \
        .type = CRA_SER_TYPE_STRUCT,                                                                                 \
        .size = sizeof(name),                                                                                        \
        .for_struct = { CRA_ITER(CRA_UNTUPLE_M, CRA_SER_NAMES_DEF, __VA_ARGS__), name##_namei, name##_ids, init_fn,  \
                        uninit_fn },                                                                                 \
    }

// @see CRA_SER_DEF_STRUCT_META_AND_STRUCT
#define CRA_SER_DEF_STRUCT_META_AND_STRUCT_AND_UNINIT(sc, name, init_fn, ... /* fields */)            \
    CRA_SER_DEF_STRUCT(name, __VA_ARGS__);                                                            \
    sc void CRA_CAT(name, _uninit)(name * stru)                                                       \
    {                                                                                                 \
        CRA_ITER(CRA_UNTUPLE_M, CRA_SER_FREE_STRUCT_FIELD, __VA_ARGS__)                               \
        bzero(stru, sizeof(name));                                                                    \
    }                                                                                                 \
    CRA_SER_DEF_STRUCT_META(sc, name, init_fn, (void (*)(void *))CRA_CAT(name, _uninit), __VA_ARGS__)

// sc = storage-class specifiers
// name = StructType => typedef struct { ... } name;
// init_fn = constructor(name *stru, size_t size);
// fields = (TYPE, name, id[, ...])[, ...]
// @see tests/test_bin_ser.c#test_meta_with_struct
//      bool:       (BOOL,       name, id)
//      enum:       (ENUM,       name, id,  enum_type)
//      int8-64:    (INT8-64,    name, id[, is_varint=false])
//      uint8-64:   (UINT8-64,   name, id[, is_varint=false])
//      float32-64: (FLOAT32-64, name, id)
//      string:     (STRING,     name, id[, len])
//      struct:     (STRUCT,     name, id, struct_type, struct_meta[, is_ptr=false])
//      array:      (ARRAY,      name, id, array_type,  array_meta)
//      list:       (LIST,       name, id, list_type,   list_meta[, is_ptr=false])
//      dict:       (DICT,       name, id, dict_type,   dict_meta[, is_ptr=false])
#define CRA_SER_DEF_STRUCT_META_AND_STRUCT(sc, name, init_fn, ... /* fields */) \
    CRA_SER_DEF_STRUCT(name, __VA_ARGS__);                                      \
    CRA_SER_DEF_STRUCT_META(sc, name, init_fn, NULL, __VA_ARGS__)

#endif // end struct def

#if 1 // array def

#define CRA_SER_DEF_ARRAY(array_def, item_def)                                                                \
    typedef struct CRA_GET_I(0, CRA_UNTUPLE array_def)                                                        \
    {                                                                                                         \
        CRA_APPLY(CRA_CAT(CRA_SER_ARRAY_ITEM_DEF, CRA_CNT array_def), (array_def, CRA_UNTUPLE item_def, NIL)) \
        uint16_t len;                                                                                         \
    } CRA_GET_I(0, CRA_UNTUPLE array_def);                                                                    \
    static_assert(sizeof(CRA_GET_I(0, CRA_UNTUPLE array_def)) <= UINT16_MAX, "ARRAY is too large")
// @see CRA_SER_DEF_ARRAY_META_AND_ARRAY
#define CRA_SER_DEF_ARRAY_META(sc, array_def, item_def, uninit_fn)                            \
    sc const CraSerMeta *CRA_CAT(CRA_GET_I(0, CRA_UNTUPLE array_def),                         \
                                 _metas)[] = { CRA_SER_ITEM_META_DEF(CRA_UNTUPLE item_def) }; \
    sc const CraSerInfo  CRA_CAT(CRA_GET_I(0, CRA_UNTUPLE array_def),                         \
                                 _infos)[] = { CRA_SER_ITEM_INFO_DEF(CRA_UNTUPLE item_def) }; \
    sc const CraSerMeta  CRA_CAT(CRA_GET_I(0, CRA_UNTUPLE array_def), _meta) = {              \
        .metas = CRA_CAT(CRA_GET_I(0, CRA_UNTUPLE array_def), _metas),                        \
        .infos = CRA_CAT(CRA_GET_I(0, CRA_UNTUPLE array_def), _infos),                        \
        .count = CRA_GET_I(1, CRA_UNTUPLE array_def, 0),                                      \
        .flags = CRA_SER_FLAG_ARRAY,                                                          \
        .type = CRA_SER_TYPE_LIST,                                                            \
        .size = sizeof(CRA_GET_I(0, CRA_UNTUPLE array_def)),                                  \
        .for_array = { sizeof(((CRA_GET_I(0, CRA_UNTUPLE array_def) *)0) -> data[0]),         \
                       offsetof(CRA_GET_I(0, CRA_UNTUPLE array_def), len), uninit_fn },       \
    }

#define CRA_SER_FREE_ARRAY_DATA1() \
    if (array->data)               \
    {                              \
        cra_free(array->data);     \
        array->data = NULL;        \
    }
#define CRA_SER_FREE_ARRAY_DATA2()
// @see tests/test_bin_ser.c#test_meta_with_array
// @see CRA_SER_DEF_ARRAY_META_AND_ARRAY
#define CRA_SER_DEF_ARRAY_META_AND_ARRAY_AND_UNINIT(sc, array_def, item_def)                                   \
    CRA_SER_DEF_ARRAY(array_def, item_def);                                                                    \
    sc void CRA_CAT(CRA_GET_I(0, CRA_UNTUPLE array_def), _uninit)(CRA_GET_I(0, CRA_UNTUPLE array_def) * array) \
    {                                                                                                          \
        CRA_SER_FREE_ARRAY_ITEM(CRA_UNTUPLE item_def)                                                          \
        CRA_CAT(CRA_SER_FREE_ARRAY_DATA, CRA_CNT(CRA_UNTUPLE array_def))() array->len = 0;                     \
    }                                                                                                          \
    CRA_SER_DEF_ARRAY_META(sc, array_def, item_def,                                                            \
                           (void (*)(void *))CRA_CAT(CRA_GET_I(0, CRA_UNTUPLE array_def), _uninit))

// sc = storage-class specifiers
// array_def = (name, max_items) => typedef struct { T data[max_items]; uint32_t len; } name;
// item_def = (ITEM_TYPE[, ...])
//      bool:       (BOOL)
//      enum:       (ENUM,     enum_type)
//      int8-64:    (INT8-64   [, is_varint=false])
//      uint8-64:   (UINT8-64  [, is_varint=false])
//      float32-64: (FLOAT32-64)
//      string:     (STRING    [, len])
//      struct:     (STRUCT,   struct_type, struct_meta[, is_ptr=false])
//      array:      (ARRAY,    array_type,  array_meta)
//      list:       (LIST,     list_type,   list_meta[, is_ptr=false])
//      dict:       (DICT,     dict_type,   dict_meta[, is_ptr=false])
// @see tests/test_bin_ser.c#test_meta_with_array
#define CRA_SER_DEF_ARRAY_META_AND_ARRAY(sc, array_def, item_def) \
    CRA_SER_DEF_ARRAY(array_def, item_def);                       \
    CRA_SER_DEF_ARRAY_META(sc, array_def, item_def, NULL)

#endif // end array def

#if 1 // list def

// sc = storage-class specifiers
// list_def = (ListType, name)
// item_def = (ITEM_TYPE[, ...])
// ... = init_fn, uninit_fn, append_fn, iter_init_fn, iter_next_fn
// @see CRA_SER_DEF_ARRAY_META_AND_ARRAY
// @see tests/test_bin_ser.c#test_meta_with_list
#define CRA_SER_DEF_LIST_META(sc, list_def, item_def, ...)                                        \
    static_assert(sizeof(CRA_GET_I(0, CRA_UNTUPLE list_def)) <= UINT16_MAX, "LIST is too large"); \
    sc const CraSerMeta *CRA_CAT(CRA_GET_I(1, CRA_UNTUPLE list_def),                              \
                                 _metas)[] = { CRA_SER_ITEM_META_DEF(CRA_UNTUPLE item_def) };     \
    sc const CraSerInfo  CRA_CAT(CRA_GET_I(1, CRA_UNTUPLE list_def),                              \
                                 _infos)[] = { CRA_SER_ITEM_INFO_DEF(CRA_UNTUPLE item_def) };     \
    sc const CraSerMeta  CRA_CAT(CRA_GET_I(1, CRA_UNTUPLE list_def), _meta) = {                   \
        .metas = CRA_CAT(CRA_GET_I(1, CRA_UNTUPLE list_def), _metas),                             \
        .infos = CRA_CAT(CRA_GET_I(1, CRA_UNTUPLE list_def), _infos),                             \
        .type = CRA_SER_TYPE_LIST,                                                                \
        .size = sizeof(CRA_GET_I(0, CRA_UNTUPLE list_def)),                                       \
        .for_list = { __VA_ARGS__ },                                                              \
    }

#endif // end list def

#if 1 // dict def

// valid key types

#define CRA_SER_BOOL_CANNOT_BE_KEY
#define CRA_SER_ENUM_CANNOT_BE_KEY
#define CRA_SER_INT8_CANNOT_BE_KEY
#define CRA_SER_INT16_CANNOT_BE_KEY
#define CRA_SER_INT32_CANNOT_BE_KEY
#define CRA_SER_INT64_CANNOT_BE_KEY
#define CRA_SER_UINT8_CANNOT_BE_KEY
#define CRA_SER_UINT16_CANNOT_BE_KEY
#define CRA_SER_UINT32_CANNOT_BE_KEY
#define CRA_SER_UINT64_CANNOT_BE_KEY
#define CRA_SER_FLOAT32_CANNOT_BE_KEY
#define CRA_SER_FLOAT64_CANNOT_BE_KEY
#define CRA_SER_STRING_CANNOT_BE_KEY

// sc = storage-class specifiers
// dict_def = (DictType, name)
// key_def = (KEY_TYPE[, ...])
// val_def = (VAL_TYPE[, ...])
// ... = init_fn, uninit_fn, append_fn, iter_init_fn, iter_next_fn
// @see CRA_SER_DEF_ARRAY_META_AND_ARRAY
// @see tests/test_bin_ser.c#test_meta_with_dict
#define CRA_SER_DEF_DICT_META(sc, dict_def, key_def, val_def, ...)                                        \
    static_assert(sizeof(CRA_GET_I(0, CRA_UNTUPLE dict_def)) <= UINT16_MAX, "DICT is too large");         \
    CRA_CAT(CRA_SER_, CRA_CAT(CRA_GET_I(0, CRA_UNTUPLE key_def), _CANNOT_BE_KEY))                         \
    sc const CraSerMeta *CRA_CAT(CRA_GET_I(1, CRA_UNTUPLE dict_def),                                      \
                                 _metas)[] = { CRA_SER_ITEM_META_DEF(CRA_UNTUPLE key_def),                \
                                               CRA_SER_ITEM_META_DEF(CRA_UNTUPLE val_def) };              \
    sc const CraSerInfo  CRA_CAT(CRA_GET_I(1, CRA_UNTUPLE dict_def), _infos)[] = { CRA_SER_ITEM_INFO_DEF( \
      CRA_UNTUPLE key_def) CRA_SER_ITEM_INFO_DEF(CRA_UNTUPLE val_def) };                                  \
    sc const CraSerMeta  CRA_CAT(CRA_GET_I(1, CRA_UNTUPLE dict_def), _meta) = {                           \
        .metas = CRA_CAT(CRA_GET_I(1, CRA_UNTUPLE dict_def), _metas),                                     \
        .infos = CRA_CAT(CRA_GET_I(1, CRA_UNTUPLE dict_def), _infos),                                     \
        .type = CRA_SER_TYPE_DICT,                                                                        \
        .size = sizeof(CRA_GET_I(0, CRA_UNTUPLE dict_def)),                                               \
        .for_dict = { __VA_ARGS__ },                                                                      \
    }

#endif // end dict def

CRA_API bool
cra_check_id_unique(const CraSerMeta *meta, uint16_t *duplicate_id);

#endif