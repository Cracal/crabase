/**
 * @file test-bin-ser.c
 * @author Cracal
 * @brief test binary serialization
 * @version 0.1
 * @date 2024-09-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <float.h>
#include "cra_malloc.h"
#include "serialize/cra_bin_ser.h"
#include "collections/cra_serializable.h"

// for cra_ser_check_id_duplicate & CRA_SER_DEF_IDS_BITMAP
#include "../src/serialize/cra_serialize_inner.h"

#if 1 // test object's info

static bool
init_mystruct(void *obj, size_t size)
{
    CRA_UNUSED(obj);
    CRA_UNUSED(size);
    return false;
}

void
test_meta_with_struct(void)
{
    typedef enum
    {
        MY_ENUM_A = 0,
        MY_ENUM_B = 1
    } MyEnum;
    typedef struct
    {
        unsigned char data[16];
    } MyList;
    typedef struct
    {
        unsigned char data[16];
    } MyDict;
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(static, MyStruct0, NULL, (BOOL, b, 1), (INT8, i8, 2));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(static, (MyArray, 4), (STRING, 20));
    CRA_SER_DEF_LIST_META(static, (MyList, MyListInt32), (INT32), NULL);
    CRA_SER_DEF_DICT_META(static, (MyDict, MyDict), (UINT16), (STRING), NULL);
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(

      , MyStruct, init_mystruct,

      // (BOOL, name, id)
      (BOOL, b, 1),

      // (ENUM, name, id, enum_type)
      (ENUM, e, 2, MyEnum),

      // (INT8, name, id[, is_varint=true]). always varint
      (INT8, i81, 3), (INT8, i82, 4, false), (INT8, i83, 5, true),
      // (INT16, name, id[, is_varint=true]). always varint
      (INT16, i161, 6), (INT16, i162, 7, false), (INT16, i163, 8, true),
      // (INT32, name, id[, is_varint=false])
      (INT32, i321, 9), (INT32, i322, 10, false), (INT32, i323, 11, true),
      // (INT64, name, id[, is_varint=false])
      (INT64, i641, 12), (INT64, i642, 13, false), (INT64, i643, 14, true),
      // (UINT8, name, id[, is_varint=true]). always varint
      (UINT8, u81, 15), (UINT8, u82, 16, false), (UINT8, u83, 17, true),
      // (UINT16, name, id[, is_varint=true]). always varint
      (UINT16, u161, 18), (UINT16, u162, 19, false), (UINT16, u163, 20, true),
      // (UINT32, name, id[, is_varint=false])
      (UINT32, u321, 21), (UINT32, u322, 22, false), (UINT32, u323, 23, true),
      // (UINT64, name, id[, is_varint=false])
      (UINT64, u641, 24), (UINT64, u642, 25, false), (UINT64, u643, 26, true),

      // (FLOAT[32|64], name, id)
      (FLOAT32, f32, 27), (FLOAT64, f64, 28),

      // (STRING, name, id[, len])
      (STRING, sp, 29), (STRING, sa, 30, 100),

      // (STRUCT, name, id, struct_type, struct_meta[, is_ptr=false])
      (STRUCT, stru1, 31, MyStruct0, &MyStruct0_meta), (STRUCT, stru2, 32, MyStruct0, &MyStruct0_meta, false),
      (STRUCT, stru3, 33, MyStruct0, &MyStruct0_meta, true),

      // (ARRAY, name, id, array_type, array_meta)
      (ARRAY, arr1, 34, MyArray, &MyArray_meta), (ARRAY, arr2, 35, MyArray, &MyArray_meta),
      (ARRAY, arr3, 36, MyArray, &MyArray_meta),

      // (LIST, name, id, list_type, list_meta[, is_ptr=false])
      (LIST, list1, 37, MyList, &MyListInt32_meta), (LIST, list2, 38, MyList, &MyListInt32_meta, false),
      (LIST, list3, 39, MyList, &MyListInt32_meta, true),

      // (DICT, name, id, dict_type, dict_meta[, is_ptr=false])
      (DICT, dict1, 40, MyDict, &MyDict_meta), (DICT, dict2, 41, MyDict, &MyDict_meta, false),
      (DICT, dict3, 42, MyDict, &MyDict_meta, true)

    );

#define CHECK_META(_type, _count, init_fn, uninit_fn)              \
    assert_always(_type##_meta.metas == _type##_metas);            \
    assert_always(_type##_meta.infos == _type##_infos);            \
    assert_always(_type##_meta.count == _count);                   \
    assert_always(_type##_meta.flags == 0);                        \
    assert_always(_type##_meta.type == CRA_SER_TYPE_STRUCT);       \
    assert_always(_type##_meta.size == sizeof(_type));             \
    assert_always(_type##_meta.for_struct.names != NULL);          \
    assert_always(_type##_meta.for_struct.namei == _type##_namei); \
    assert_always(_type##_meta.for_struct.ids == _type##_ids);     \
    assert_always(_type##_meta.for_struct.init == init_fn);        \
    assert_always(_type##_meta.for_struct.uninit == uninit_fn);    \
                                                                   \
    assert_always(CRA_NARRAY(_type##_metas) == _count);            \
    assert_always(CRA_NARRAY(_type##_infos) == _count)
#define CHECK_FIELD(index, _stru_type, meta, _name, TYPE, _flags, _size, _id)                                    \
    assert_always(_stru_type##_metas[index] == meta);                                                            \
    assert_always(_stru_type##_infos[index].flags == _flags);                                                    \
    assert_always(_stru_type##_infos[index].type == CRA_SER_TYPE_##TYPE);                                        \
    assert_always(_stru_type##_infos[index].size == _size);                                                      \
    assert_always(_stru_type##_infos[index].offset == offsetof(_stru_type, _name));                              \
    assert_always(_stru_type##_namei[index + 1] - _stru_type##_namei[index] == sizeof(#_name) - 1);              \
    assert_always(                                                                                               \
      strncmp(#_name, _stru_type##_meta.for_struct.names + _stru_type##_namei[index], sizeof(#_name) - 1) == 0); \
    assert_always(_stru_type##_ids[index] == _id)

    CHECK_META(MyStruct0, 2, NULL, NULL);
    CHECK_META(MyStruct, 42, init_mystruct, NULL);
    assert_always(cra_check_id_unique(&MyStruct_meta, NULL));
    assert_always(cra_check_id_unique(&MyStruct0_meta, NULL));

    CHECK_FIELD(0, MyStruct, NULL, b, BOOL, CRA_SER_FLAG_BOOL, sizeof(bool), 1);
    CHECK_FIELD(1, MyStruct, NULL, e, VARINT, (CRA_SER_FLAG_UNSIGNED | CRA_SER_FLAG_ENUM), sizeof(MyEnum), 2);
    CHECK_FIELD(2, MyStruct, NULL, i81, VARINT, 0, sizeof(int8_t), 3);
    CHECK_FIELD(3, MyStruct, NULL, i82, VARINT, 0, sizeof(int8_t), 4);
    CHECK_FIELD(4, MyStruct, NULL, i83, VARINT, 0, sizeof(int8_t), 5);
    CHECK_FIELD(5, MyStruct, NULL, i161, VARINT, 0, sizeof(int16_t), 6);
    CHECK_FIELD(6, MyStruct, NULL, i162, VARINT, 0, sizeof(int16_t), 7);
    CHECK_FIELD(7, MyStruct, NULL, i163, VARINT, 0, sizeof(int16_t), 8);
    CHECK_FIELD(8, MyStruct, NULL, i321, 32BITS, 0, sizeof(int32_t), 9);
    CHECK_FIELD(9, MyStruct, NULL, i322, 32BITS, 0, sizeof(int32_t), 10);
    CHECK_FIELD(10, MyStruct, NULL, i323, VARINT, 0, sizeof(int32_t), 11);
    CHECK_FIELD(11, MyStruct, NULL, i641, 64BITS, 0, sizeof(int64_t), 12);
    CHECK_FIELD(12, MyStruct, NULL, i642, 64BITS, 0, sizeof(int64_t), 13);
    CHECK_FIELD(13, MyStruct, NULL, i643, VARINT, 0, sizeof(int64_t), 14);
    CHECK_FIELD(14, MyStruct, NULL, u81, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t), 15);
    CHECK_FIELD(15, MyStruct, NULL, u82, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t), 16);
    CHECK_FIELD(16, MyStruct, NULL, u83, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t), 17);
    CHECK_FIELD(17, MyStruct, NULL, u161, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t), 18);
    CHECK_FIELD(18, MyStruct, NULL, u162, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t), 19);
    CHECK_FIELD(19, MyStruct, NULL, u163, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t), 20);
    CHECK_FIELD(20, MyStruct, NULL, u321, 32BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t), 21);
    CHECK_FIELD(21, MyStruct, NULL, u322, 32BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t), 22);
    CHECK_FIELD(22, MyStruct, NULL, u323, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t), 23);
    CHECK_FIELD(23, MyStruct, NULL, u641, 64BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t), 24);
    CHECK_FIELD(24, MyStruct, NULL, u642, 64BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t), 25);
    CHECK_FIELD(25, MyStruct, NULL, u643, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t), 26);
    CHECK_FIELD(26, MyStruct, NULL, f32, 32BITS, CRA_SER_FLAG_FLOAT, sizeof(float), 27);
    CHECK_FIELD(27, MyStruct, NULL, f64, 64BITS, CRA_SER_FLAG_FLOAT, sizeof(double), 28);
    CHECK_FIELD(28, MyStruct, NULL, sp, STRING, CRA_SER_FLAG_PTR, 0, 29);
    CHECK_FIELD(29, MyStruct, NULL, sa, STRING, CRA_SER_FLAG_ARRAY, sizeof(char[100]), 30);
    CHECK_FIELD(30, MyStruct, &MyStruct0_meta, stru1, STRUCT, 0, sizeof(MyStruct0), 31);
    CHECK_FIELD(31, MyStruct, &MyStruct0_meta, stru2, STRUCT, 0, sizeof(MyStruct0), 32);
    CHECK_FIELD(32, MyStruct, &MyStruct0_meta, stru3, STRUCT, CRA_SER_FLAG_PTR, sizeof(MyStruct0), 33);
    CHECK_FIELD(33, MyStruct, &MyArray_meta, arr1, LIST, CRA_SER_FLAG_ARRAY, sizeof(MyArray), 34);
    CHECK_FIELD(34, MyStruct, &MyArray_meta, arr2, LIST, CRA_SER_FLAG_ARRAY, sizeof(MyArray), 35);
    CHECK_FIELD(35, MyStruct, &MyArray_meta, arr3, LIST, CRA_SER_FLAG_ARRAY, sizeof(MyArray), 36);
    CHECK_FIELD(36, MyStruct, &MyListInt32_meta, list1, LIST, 0, sizeof(MyList), 37);
    CHECK_FIELD(37, MyStruct, &MyListInt32_meta, list2, LIST, 0, sizeof(MyList), 38);
    CHECK_FIELD(38, MyStruct, &MyListInt32_meta, list3, LIST, CRA_SER_FLAG_PTR, sizeof(MyList), 39);
    CHECK_FIELD(39, MyStruct, &MyDict_meta, dict1, DICT, 0, sizeof(MyDict), 40);
    CHECK_FIELD(40, MyStruct, &MyDict_meta, dict2, DICT, 0, sizeof(MyDict), 41);
    CHECK_FIELD(41, MyStruct, &MyDict_meta, dict3, DICT, CRA_SER_FLAG_PTR, sizeof(MyDict), 42);

#undef CHECK_FIELD
#undef CHECK_META
}

void
test_meta_with_array(void)
{
#define CHECK_ARRAY(_type, max_count, meta, TYPE, _flags, _size)                         \
    assert_always(_type##_meta.metas == _type##_metas);                                  \
    assert_always(_type##_meta.infos == _type##_infos);                                  \
    assert_always(_type##_meta.count == max_count);                                      \
    /*if (_type##_meta.count > 0)                                                        \
        assert_always(_type##_meta.count == CRA_NARRAY(((_type *)0)->data));*/           \
    assert_always(_type##_meta.flags == CRA_SER_FLAG_ARRAY);                             \
    assert_always(_type##_meta.type == CRA_SER_TYPE_LIST);                               \
    assert_always(_type##_meta.size == sizeof(_type));                                   \
    assert_always(_type##_meta.for_array.size_of_slot == sizeof(((_type *)0)->data[0])); \
    assert_always(_type##_meta.for_array.offsetof_len == offsetof(_type, len));          \
    assert_always(_type##_meta.for_array.uninit == NULL);                                \
                                                                                         \
    assert_always(CRA_NARRAY(_type##_metas) == 1);                                       \
    assert_always(CRA_NARRAY(_type##_infos) == 1);                                       \
                                                                                         \
    assert_always(_type##_metas[0] == meta);                                             \
    assert_always(_type##_infos[0].flags == _flags);                                     \
    assert_always(_type##_infos[0].type == CRA_SER_TYPE_##TYPE);                         \
    assert_always(_type##_infos[0].size == _size);                                       \
    assert_always(_type##_infos[0].offset == 0);

    typedef enum
    {
        MyEnum1 = 1,
        MyEnum2 = 2,
    } MyEnum;

    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AB1), (BOOL));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AB2, 10), (BOOL));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AE1), (ENUM, MyEnum));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AE2, 20), (ENUM, MyEnum));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI81), (INT8));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI82), (INT8, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI83), (INT8, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI84, 4), (INT8));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI85, 8), (INT8, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI86, 2), (INT8, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI161), (INT16));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI162), (INT16, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI163), (INT16, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI164, 4), (INT16));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI165, 8), (INT16, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI166, 2), (INT16, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI321), (INT32));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI322), (INT32, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI323), (INT32, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI324, 4), (INT32));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI325, 8), (INT32, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI326, 2), (INT32, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI641), (INT64));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI642), (INT64, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI643), (INT64, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI644, 4), (INT64));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI645, 8), (INT64, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AI646, 2), (INT64, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU81), (UINT8));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU82), (UINT8, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU83), (UINT8, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU84, 4), (UINT8));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU85, 8), (UINT8, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU86, 2), (UINT8, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU161), (UINT16));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU162), (UINT16, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU163), (UINT16, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU164, 4), (UINT16));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU165, 8), (UINT16, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU166, 2), (UINT16, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU321), (UINT32));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU322), (UINT32, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU323), (UINT32, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU324, 4), (UINT32));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU325, 8), (UINT32, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU326, 2), (UINT32, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU641), (UINT64));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU642), (UINT64, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU643), (UINT64, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU644, 4), (UINT64));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU645, 8), (UINT64, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AU646, 2), (UINT64, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AF321), (FLOAT32));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AF322, 10), (FLOAT32));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AF641), (FLOAT64));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AF642, 10), (FLOAT64));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AS1), (STRING));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AS2), (STRING, 100));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AS3, 10), (STRING));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AS4, 30), (STRING, 100));
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(static, MyStruct, NULL, (INT32, i32, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (ASU1), (STRUCT, MyStruct, &MyStruct_meta));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (ASU2), (STRUCT, MyStruct, &MyStruct_meta, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (ASU3), (STRUCT, MyStruct, &MyStruct_meta, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (ASU4, 4), (STRUCT, MyStruct, &MyStruct_meta));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (ASU5, 4), (STRUCT, MyStruct, &MyStruct_meta, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (ASU6, 4), (STRUCT, MyStruct, &MyStruct_meta, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AA1), (ARRAY, ASU1, &ASU1_meta));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AA2, 4), (ARRAY, ASU1, &ASU1_meta));
    typedef struct
    {
        unsigned char data[8];
    } MyList;
    CRA_SER_DEF_LIST_META(static, (MyList, MyList), (FLOAT32), NULL);
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AL1), (LIST, MyList, &MyList_meta));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AL2), (LIST, MyList, &MyList_meta, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AL3), (LIST, MyList, &MyList_meta, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AL4, 4), (LIST, MyList, &MyList_meta));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AL5, 4), (LIST, MyList, &MyList_meta, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AL6, 4), (LIST, MyList, &MyList_meta, true));
    typedef struct
    {
        unsigned char data[8];
    } MyDict;
    CRA_SER_DEF_DICT_META(static, (MyDict, MyDict), (UINT8), (STRING, 20), NULL);
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AD1), (DICT, MyDict, &MyDict_meta));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AD2), (DICT, MyDict, &MyDict_meta, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AD3), (DICT, MyDict, &MyDict_meta, true));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AD4, 4), (DICT, MyDict, &MyDict_meta));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AD5, 4), (DICT, MyDict, &MyDict_meta, false));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(, (AD6, 4), (DICT, MyDict, &MyDict_meta, true));

    CHECK_ARRAY(AB1, 0, NULL, BOOL, CRA_SER_FLAG_BOOL, sizeof(bool));
    CHECK_ARRAY(AB2, 10, NULL, BOOL, CRA_SER_FLAG_BOOL, sizeof(bool));
    CHECK_ARRAY(AE1, 0, NULL, VARINT, (CRA_SER_FLAG_UNSIGNED | CRA_SER_FLAG_ENUM), sizeof(MyEnum));
    CHECK_ARRAY(AE2, 20, NULL, VARINT, (CRA_SER_FLAG_UNSIGNED | CRA_SER_FLAG_ENUM), sizeof(MyEnum));
    CHECK_ARRAY(AI81, 0, NULL, VARINT, 0, sizeof(int8_t));
    CHECK_ARRAY(AI82, 0, NULL, VARINT, 0, sizeof(int8_t));
    CHECK_ARRAY(AI83, 0, NULL, VARINT, 0, sizeof(int8_t));
    CHECK_ARRAY(AI84, 4, NULL, VARINT, 0, sizeof(int8_t));
    CHECK_ARRAY(AI85, 8, NULL, VARINT, 0, sizeof(int8_t));
    CHECK_ARRAY(AI86, 2, NULL, VARINT, 0, sizeof(int8_t));
    CHECK_ARRAY(AI161, 0, NULL, VARINT, 0, sizeof(int16_t));
    CHECK_ARRAY(AI162, 0, NULL, VARINT, 0, sizeof(int16_t));
    CHECK_ARRAY(AI163, 0, NULL, VARINT, 0, sizeof(int16_t));
    CHECK_ARRAY(AI164, 4, NULL, VARINT, 0, sizeof(int16_t));
    CHECK_ARRAY(AI165, 8, NULL, VARINT, 0, sizeof(int16_t));
    CHECK_ARRAY(AI166, 2, NULL, VARINT, 0, sizeof(int16_t));
    CHECK_ARRAY(AI321, 0, NULL, 32BITS, 0, sizeof(int32_t));
    CHECK_ARRAY(AI322, 0, NULL, 32BITS, 0, sizeof(int32_t));
    CHECK_ARRAY(AI323, 0, NULL, VARINT, 0, sizeof(int32_t));
    CHECK_ARRAY(AI324, 4, NULL, 32BITS, 0, sizeof(int32_t));
    CHECK_ARRAY(AI325, 8, NULL, 32BITS, 0, sizeof(int32_t));
    CHECK_ARRAY(AI326, 2, NULL, VARINT, 0, sizeof(int32_t));
    CHECK_ARRAY(AI641, 0, NULL, 64BITS, 0, sizeof(int64_t));
    CHECK_ARRAY(AI642, 0, NULL, 64BITS, 0, sizeof(int64_t));
    CHECK_ARRAY(AI643, 0, NULL, VARINT, 0, sizeof(int64_t));
    CHECK_ARRAY(AI644, 4, NULL, 64BITS, 0, sizeof(int64_t));
    CHECK_ARRAY(AI645, 8, NULL, 64BITS, 0, sizeof(int64_t));
    CHECK_ARRAY(AI646, 2, NULL, VARINT, 0, sizeof(int64_t));
    CHECK_ARRAY(AU81, 0, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t));
    CHECK_ARRAY(AU82, 0, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t));
    CHECK_ARRAY(AU83, 0, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t));
    CHECK_ARRAY(AU84, 4, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t));
    CHECK_ARRAY(AU85, 8, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t));
    CHECK_ARRAY(AU86, 2, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t));
    CHECK_ARRAY(AU161, 0, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t));
    CHECK_ARRAY(AU162, 0, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t));
    CHECK_ARRAY(AU163, 0, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t));
    CHECK_ARRAY(AU164, 4, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t));
    CHECK_ARRAY(AU165, 8, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t));
    CHECK_ARRAY(AU166, 2, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t));
    CHECK_ARRAY(AU321, 0, NULL, 32BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t));
    CHECK_ARRAY(AU322, 0, NULL, 32BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t));
    CHECK_ARRAY(AU323, 0, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t));
    CHECK_ARRAY(AU324, 4, NULL, 32BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t));
    CHECK_ARRAY(AU325, 8, NULL, 32BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t));
    CHECK_ARRAY(AU326, 2, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t));
    CHECK_ARRAY(AU641, 0, NULL, 64BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t));
    CHECK_ARRAY(AU642, 0, NULL, 64BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t));
    CHECK_ARRAY(AU643, 0, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t));
    CHECK_ARRAY(AU644, 4, NULL, 64BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t));
    CHECK_ARRAY(AU645, 8, NULL, 64BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t));
    CHECK_ARRAY(AU646, 2, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t));
    CHECK_ARRAY(AF321, 0, NULL, 32BITS, CRA_SER_FLAG_FLOAT, sizeof(float));
    CHECK_ARRAY(AF322, 10, NULL, 32BITS, CRA_SER_FLAG_FLOAT, sizeof(float));
    CHECK_ARRAY(AF641, 0, NULL, 64BITS, CRA_SER_FLAG_FLOAT, sizeof(double));
    CHECK_ARRAY(AF642, 10, NULL, 64BITS, CRA_SER_FLAG_FLOAT, sizeof(double));
    CHECK_ARRAY(AS1, 0, NULL, STRING, CRA_SER_FLAG_PTR, 0);
    CHECK_ARRAY(AS2, 0, NULL, STRING, CRA_SER_FLAG_ARRAY, sizeof(char[100]));
    CHECK_ARRAY(AS3, 10, NULL, STRING, CRA_SER_FLAG_PTR, 0);
    CHECK_ARRAY(AS4, 30, NULL, STRING, CRA_SER_FLAG_ARRAY, sizeof(char[100]));
    CHECK_ARRAY(ASU1, 0, &MyStruct_meta, STRUCT, 0, sizeof(MyStruct));
    CHECK_ARRAY(ASU2, 0, &MyStruct_meta, STRUCT, 0, sizeof(MyStruct));
    CHECK_ARRAY(ASU3, 0, &MyStruct_meta, STRUCT, CRA_SER_FLAG_PTR, sizeof(MyStruct));
    CHECK_ARRAY(ASU4, 4, &MyStruct_meta, STRUCT, 0, sizeof(MyStruct));
    CHECK_ARRAY(ASU5, 4, &MyStruct_meta, STRUCT, 0, sizeof(MyStruct));
    CHECK_ARRAY(ASU6, 4, &MyStruct_meta, STRUCT, CRA_SER_FLAG_PTR, sizeof(MyStruct));
    CHECK_ARRAY(AA1, 0, &ASU1_meta, LIST, CRA_SER_FLAG_ARRAY, sizeof(ASU1));
    CHECK_ARRAY(AA2, 4, &ASU1_meta, LIST, CRA_SER_FLAG_ARRAY, sizeof(ASU1));
    CHECK_ARRAY(AL1, 0, &MyList_meta, LIST, 0, sizeof(MyList));
    CHECK_ARRAY(AL2, 0, &MyList_meta, LIST, 0, sizeof(MyList));
    CHECK_ARRAY(AL3, 0, &MyList_meta, LIST, CRA_SER_FLAG_PTR, sizeof(MyList));
    CHECK_ARRAY(AL4, 4, &MyList_meta, LIST, 0, sizeof(MyList));
    CHECK_ARRAY(AL5, 4, &MyList_meta, LIST, 0, sizeof(MyList));
    CHECK_ARRAY(AL6, 4, &MyList_meta, LIST, CRA_SER_FLAG_PTR, sizeof(MyList));
    CHECK_ARRAY(AD1, 0, &MyDict_meta, DICT, 0, sizeof(MyDict));
    CHECK_ARRAY(AD2, 0, &MyDict_meta, DICT, 0, sizeof(MyDict));
    CHECK_ARRAY(AD3, 0, &MyDict_meta, DICT, CRA_SER_FLAG_PTR, sizeof(MyDict));
    CHECK_ARRAY(AD4, 4, &MyDict_meta, DICT, 0, sizeof(MyDict));
    CHECK_ARRAY(AD5, 4, &MyDict_meta, DICT, 0, sizeof(MyDict));
    CHECK_ARRAY(AD6, 4, &MyDict_meta, DICT, CRA_SER_FLAG_PTR, sizeof(MyDict));

#undef CHECK_ARRAY
}

static bool
l_init(void *list, size_t itemsize, size_t count)
{
    CRA_UNUSED(list);
    CRA_UNUSED(itemsize);
    CRA_UNUSED(count);
    return false;
}

static void
l_uninit(void *list)
{
    CRA_UNUSED(list);
}

static bool
l_append(void *list, void *item)
{
    CRA_UNUSED(list);
    CRA_UNUSED(item);
    return false;
}

static void
l_iter_init(void *iter, size_t itersize, void *list, size_t *retcount)
{
    CRA_UNUSED(iter);
    CRA_UNUSED(itersize);
    CRA_UNUSED(list);
    CRA_UNUSED(retcount);
}

static bool
l_iter_next(void *iter, void *retitem)
{
    CRA_UNUSED(iter);
    CRA_UNUSED(retitem);
    return false;
}

#define FUNCS l_init, l_uninit, l_append, l_iter_init, l_iter_next

void
test_meta_with_list(void)
{
#define CHECK_LIST(_type, _name, meta, TYPE, _flags, _size)        \
    assert_always(_name##_meta.metas == _name##_metas);            \
    assert_always(_name##_meta.infos == _name##_infos);            \
    assert_always(_name##_meta.count == 0);                        \
    assert_always(_name##_meta.flags == 0);                        \
    assert_always(_name##_meta.type == CRA_SER_TYPE_LIST);         \
    assert_always(_name##_meta.size == sizeof(_type));             \
    assert_always(_name##_meta.for_list.init == l_init);           \
    assert_always(_name##_meta.for_list.uninit == l_uninit);       \
    assert_always(_name##_meta.for_list.append == l_append);       \
    assert_always(_name##_meta.for_list.iter_init == l_iter_init); \
    assert_always(_name##_meta.for_list.iter_next == l_iter_next); \
                                                                   \
    assert_always(CRA_NARRAY(_name##_metas) == 1);                 \
    assert_always(CRA_NARRAY(_name##_infos) == 1);                 \
                                                                   \
    assert_always(_name##_metas[0] == meta);                       \
    assert_always(_name##_infos[0].flags == _flags);               \
    assert_always(_name##_infos[0].type == CRA_SER_TYPE_##TYPE);   \
    assert_always(_name##_infos[0].size == _size);                 \
    assert_always(_name##_infos[0].offset == 0);

    typedef struct
    {
        unsigned char data[16];
    } MyList;
    typedef enum
    {
        MyEnum1,
        MyEnum2
    } MyEnum;
    typedef struct
    {
        unsigned char data[16];
    } MyDict;

    CRA_SER_DEF_LIST_META(, (MyList, b), (BOOL), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, e), (ENUM, MyEnum), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i81), (INT8), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i82), (INT8, false), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i83), (INT8, true), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i161), (INT16), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i162), (INT16, false), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i163), (INT16, true), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i321), (INT32), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i322), (INT32, false), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i323), (INT32, true), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i641), (INT64), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i642), (INT64, false), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, i643), (INT64, true), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u81), (UINT8), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u82), (UINT8, false), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u83), (UINT8, true), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u161), (UINT16), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u162), (UINT16, false), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u163), (UINT16, true), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u321), (UINT32), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u322), (UINT32, false), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u323), (UINT32, true), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u641), (UINT64), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u642), (UINT64, false), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, u643), (UINT64, true), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, f32), (FLOAT32), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, f64), (FLOAT64), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, str1), (STRING), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, str2), (STRING, 10), FUNCS);
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(static, MyStruct, NULL, (BOOL, b, 1));
    CRA_SER_DEF_LIST_META(, (MyList, stru1), (STRUCT, MyStruct, &MyStruct_meta), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, stru2), (STRUCT, MyStruct, &MyStruct_meta, false), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, stru3), (STRUCT, MyStruct, &MyStruct_meta, true), FUNCS);
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(static, (MyArray), (BOOL));
    CRA_SER_DEF_LIST_META(, (MyList, arr1), (ARRAY, MyArray, &MyArray_meta), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, list1), (LIST, MyList, &b_meta), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, list2), (LIST, MyList, &b_meta, false), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyList, list3), (LIST, MyList, &b_meta, true), FUNCS);
    CRA_SER_DEF_DICT_META(static, (MyDict, MyDict), (UINT16), (INT32, true), NULL);
    CRA_SER_DEF_LIST_META(, (MyDict, dict1), (DICT, MyDict, &MyDict_meta), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyDict, dict2), (DICT, MyDict, &MyDict_meta, false), FUNCS);
    CRA_SER_DEF_LIST_META(, (MyDict, dict3), (DICT, MyDict, &MyDict_meta, true), FUNCS);

    CHECK_LIST(MyList, b, NULL, BOOL, CRA_SER_FLAG_BOOL, sizeof(bool));
    CHECK_LIST(MyList, e, NULL, VARINT, (CRA_SER_FLAG_UNSIGNED | CRA_SER_FLAG_ENUM), sizeof(MyEnum));
    CHECK_LIST(MyList, i81, NULL, VARINT, 0, sizeof(int8_t));
    CHECK_LIST(MyList, i82, NULL, VARINT, 0, sizeof(int8_t));
    CHECK_LIST(MyList, i83, NULL, VARINT, 0, sizeof(int8_t));
    CHECK_LIST(MyList, i161, NULL, VARINT, 0, sizeof(int16_t));
    CHECK_LIST(MyList, i162, NULL, VARINT, 0, sizeof(int16_t));
    CHECK_LIST(MyList, i163, NULL, VARINT, 0, sizeof(int16_t));
    CHECK_LIST(MyList, i321, NULL, 32BITS, 0, sizeof(int32_t));
    CHECK_LIST(MyList, i322, NULL, 32BITS, 0, sizeof(int32_t));
    CHECK_LIST(MyList, i323, NULL, VARINT, 0, sizeof(int32_t));
    CHECK_LIST(MyList, i641, NULL, 64BITS, 0, sizeof(int64_t));
    CHECK_LIST(MyList, i642, NULL, 64BITS, 0, sizeof(int64_t));
    CHECK_LIST(MyList, i643, NULL, VARINT, 0, sizeof(int64_t));
    CHECK_LIST(MyList, u81, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t));
    CHECK_LIST(MyList, u82, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t));
    CHECK_LIST(MyList, u83, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t));
    CHECK_LIST(MyList, u161, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t));
    CHECK_LIST(MyList, u162, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t));
    CHECK_LIST(MyList, u163, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t));
    CHECK_LIST(MyList, u321, NULL, 32BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t));
    CHECK_LIST(MyList, u322, NULL, 32BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t));
    CHECK_LIST(MyList, u323, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t));
    CHECK_LIST(MyList, u641, NULL, 64BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t));
    CHECK_LIST(MyList, u642, NULL, 64BITS, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t));
    CHECK_LIST(MyList, u643, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t));
    CHECK_LIST(MyList, f32, NULL, 32BITS, CRA_SER_FLAG_FLOAT, sizeof(float));
    CHECK_LIST(MyList, f64, NULL, 64BITS, CRA_SER_FLAG_FLOAT, sizeof(double));
    CHECK_LIST(MyList, str1, NULL, STRING, CRA_SER_FLAG_PTR, 0);
    CHECK_LIST(MyList, str2, NULL, STRING, CRA_SER_FLAG_ARRAY, sizeof(char[10]));
    CHECK_LIST(MyList, stru1, &MyStruct_meta, STRUCT, 0, sizeof(MyStruct));
    CHECK_LIST(MyList, stru2, &MyStruct_meta, STRUCT, 0, sizeof(MyStruct));
    CHECK_LIST(MyList, stru3, &MyStruct_meta, STRUCT, CRA_SER_FLAG_PTR, sizeof(MyStruct));
    CHECK_LIST(MyList, arr1, &MyArray_meta, LIST, CRA_SER_FLAG_ARRAY, sizeof(MyArray));
    CHECK_LIST(MyList, list1, &b_meta, LIST, 0, sizeof(MyList));
    CHECK_LIST(MyList, list2, &b_meta, LIST, 0, sizeof(MyList));
    CHECK_LIST(MyList, list3, &b_meta, LIST, CRA_SER_FLAG_PTR, sizeof(MyList));
    CHECK_LIST(MyList, dict1, &MyDict_meta, DICT, 0, sizeof(MyDict));
    CHECK_LIST(MyList, dict2, &MyDict_meta, DICT, 0, sizeof(MyDict));
    CHECK_LIST(MyList, dict3, &MyDict_meta, DICT, CRA_SER_FLAG_PTR, sizeof(MyDict));

#undef CHECK_LIST
}

#undef FUNCS

static bool
d_init(void *dict, size_t keysize, size_t valsize, size_t count)
{
    CRA_UNUSED(dict);
    CRA_UNUSED(keysize);
    CRA_UNUSED(valsize);
    CRA_UNUSED(count);
    return false;
}

static void
d_uninit(void *dict)
{
    CRA_UNUSED(dict);
}

static bool
d_append(void *dict, void *key, void *val)
{
    CRA_UNUSED(dict);
    CRA_UNUSED(key);
    CRA_UNUSED(val);
    return false;
}

static void
d_iter_init(void *iter, size_t itersize, void *dict, size_t *retcount)
{
    CRA_UNUSED(iter);
    CRA_UNUSED(itersize);
    CRA_UNUSED(dict);
    CRA_UNUSED(retcount);
}

static bool
d_iter_next(void *iter, void *retkey, void *retval)
{
    CRA_UNUSED(iter);
    CRA_UNUSED(retkey);
    CRA_UNUSED(retval);
    return false;
}

#define FUNCS d_init, d_uninit, d_append, d_iter_init, d_iter_next

void
test_meta_with_dict(void)
{
#define CHECK_DICT(_type, _name, key_meta, KEY_TYPE, key_flags, key_size, val_meta, VAL_TYPE, val_flags, val_size) \
    assert_always(_name##_meta.metas == _name##_metas);                                                            \
    assert_always(_name##_meta.infos == _name##_infos);                                                            \
    assert_always(_name##_meta.count == 0);                                                                        \
    assert_always(_name##_meta.flags == 0);                                                                        \
    assert_always(_name##_meta.type == CRA_SER_TYPE_DICT);                                                         \
    assert_always(_name##_meta.size == sizeof(_type));                                                             \
    assert_always(_name##_meta.for_dict.init == d_init);                                                           \
    assert_always(_name##_meta.for_dict.uninit == d_uninit);                                                       \
    assert_always(_name##_meta.for_dict.append == d_append);                                                       \
    assert_always(_name##_meta.for_dict.iter_init == d_iter_init);                                                 \
    assert_always(_name##_meta.for_dict.iter_next == d_iter_next);                                                 \
                                                                                                                   \
    assert_always(CRA_NARRAY(_name##_metas) == 2);                                                                 \
    assert_always(CRA_NARRAY(_name##_infos) == 2);                                                                 \
                                                                                                                   \
    assert_always(_name##_metas[0] == key_meta);                                                                   \
    assert_always(_name##_infos[0].flags == key_flags);                                                            \
    assert_always(_name##_infos[0].type == CRA_SER_TYPE_##KEY_TYPE);                                               \
    assert_always(_name##_infos[0].size == key_size);                                                              \
    assert_always(_name##_infos[0].offset == 0);                                                                   \
    assert_always(_name##_metas[1] == val_meta);                                                                   \
    assert_always(_name##_infos[1].flags == val_flags);                                                            \
    assert_always(_name##_infos[1].type == CRA_SER_TYPE_##VAL_TYPE);                                               \
    assert_always(_name##_infos[1].size == val_size);                                                              \
    assert_always(_name##_infos[1].offset == 0);

    typedef struct
    {
        unsigned char data[16];
    } MyList;
    typedef enum
    {
        MyEnum1,
        MyEnum2
    } MyEnum;
    typedef struct
    {
        unsigned char data[16];
    } MyDict;

    CRA_SER_DEF_DICT_META(, (MyDict, b), (INT32), (BOOL), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, e), (BOOL), (ENUM, MyEnum), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, i8), (ENUM, MyEnum), (INT8), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, i16), (INT8, true), (INT16), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, i32), (INT16, false), (INT32), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, i64), (INT32, true), (INT64), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, u8), (INT64, true), (UINT8), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, u16), (UINT8, true), (UINT16), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, u32), (UINT16, false), (UINT32), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, u64), (UINT32, true), (UINT64), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, f32), (UINT64, true), (FLOAT32), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, f64), (FLOAT32), (FLOAT64), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, str), (FLOAT64), (STRING), FUNCS);
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(static, MyStruct, NULL, (STRING, str, 1, 100));
    CRA_SER_DEF_DICT_META(, (MyDict, stru), (STRING, 20), (STRUCT, MyStruct, &MyStruct_meta), FUNCS);
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(static, (MyArray), (STRING, 200));
    CRA_SER_DEF_DICT_META(, (MyDict, arr), (STRING), (ARRAY, MyArray, &MyArray_meta), FUNCS);
    CRA_SER_DEF_LIST_META(static, (MyList, MyList), (STRING), NULL);
    CRA_SER_DEF_DICT_META(, (MyDict, list), (STRING), (LIST, MyList, &MyList_meta), FUNCS);
    CRA_SER_DEF_DICT_META(, (MyDict, dict), (STRING), (DICT, MyDict, &b_meta), FUNCS);

    // CRA_SER_DEF_DICT_META(, (MyDict, invalid), (STRUCT, MyStruct, &MyStruct_meta), (BOOL), FUNCS);

    CHECK_DICT(MyDict, b, NULL, 32BITS, 0, sizeof(int32_t), NULL, BOOL, CRA_SER_FLAG_BOOL, sizeof(bool));
    CHECK_DICT(MyDict, e, NULL, BOOL, CRA_SER_FLAG_BOOL, sizeof(bool), NULL, VARINT,
               (CRA_SER_FLAG_UNSIGNED | CRA_SER_FLAG_ENUM), sizeof(MyEnum));
    CHECK_DICT(MyDict, i8, NULL, VARINT, (CRA_SER_FLAG_UNSIGNED | CRA_SER_FLAG_ENUM), sizeof(MyEnum), NULL, VARINT, 0,
               sizeof(int8_t));
    CHECK_DICT(MyDict, i16, NULL, VARINT, 0, sizeof(int8_t), NULL, VARINT, 0, sizeof(int16_t));
    CHECK_DICT(MyDict, i32, NULL, VARINT, 0, sizeof(int16_t), NULL, 32BITS, 0, sizeof(int32_t));
    CHECK_DICT(MyDict, i64, NULL, VARINT, 0, sizeof(int32_t), NULL, 64BITS, 0, sizeof(int64_t));
    CHECK_DICT(MyDict, u8, NULL, VARINT, 0, sizeof(int64_t), NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t));
    CHECK_DICT(MyDict, u16, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint8_t), NULL, VARINT, CRA_SER_FLAG_UNSIGNED,
               sizeof(uint16_t));
    CHECK_DICT(MyDict, u32, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint16_t), NULL, 32BITS, CRA_SER_FLAG_UNSIGNED,
               sizeof(uint32_t));
    CHECK_DICT(MyDict, u64, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint32_t), NULL, 64BITS, CRA_SER_FLAG_UNSIGNED,
               sizeof(uint64_t));
    CHECK_DICT(MyDict, f32, NULL, VARINT, CRA_SER_FLAG_UNSIGNED, sizeof(uint64_t), NULL, 32BITS, CRA_SER_FLAG_FLOAT,
               sizeof(float));
    CHECK_DICT(MyDict, f64, NULL, 32BITS, CRA_SER_FLAG_FLOAT, sizeof(float), NULL, 64BITS, CRA_SER_FLAG_FLOAT,
               sizeof(double));
    CHECK_DICT(MyDict, str, NULL, 64BITS, CRA_SER_FLAG_FLOAT, sizeof(double), NULL, STRING, CRA_SER_FLAG_PTR, 0);
    CHECK_DICT(MyDict, stru, NULL, STRING, CRA_SER_FLAG_ARRAY, sizeof(char[20]), &MyStruct_meta, STRUCT, 0,
               sizeof(MyStruct));
    CHECK_DICT(MyDict, arr, NULL, STRING, CRA_SER_FLAG_PTR, 0, &MyArray_meta, LIST, CRA_SER_FLAG_ARRAY,
               sizeof(MyArray));
    CHECK_DICT(MyDict, list, NULL, STRING, CRA_SER_FLAG_PTR, 0, &MyList_meta, LIST, 0, sizeof(MyList));
    CHECK_DICT(MyDict, dict, NULL, STRING, CRA_SER_FLAG_PTR, 0, &b_meta, DICT, 0, sizeof(MyDict));

#undef CHECK_DICT
}

#undef FUNCS

void
test_check_id_duplicate(void)
{
    CRA_SER_DEF_IDS_BITMAP(ids);

    for (int i = 0; i < CRA_SER_STRUCT_MAX_FIELDS; ++i)
        assert_always(!cra_ser_check_id_duplicate(ids, (uint16_t)i));

    for (int i = 0; i < CRA_SER_STRUCT_MAX_FIELDS; ++i)
        assert_always(cra_ser_check_id_duplicate(ids, (uint16_t)i));
}

#endif // test object's info

void
test_bool(void)
{
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(static, B, NULL, (BOOL, b0, 1), (BOOL, b1, 2));

    CraSerErr     err;
    size_t        length;
    unsigned char buffer[10];
    struct B      out, in = { false, true };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &B_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &B_meta));
    assert_always(in.b0 == out.b0);
    assert_always(in.b1 == out.b1);

    // test error

    // small buffer
    length = 3;
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &B_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &B_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // true != 1
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &B_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[3] = 0xFF; // 0 -> 255
    buffer[5] = 3;    // 1 -> 3
    assert_always(cra_bin_deserialize_err(buffer, length, &out, &B_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    assert_always(out.b0 == true);
    assert_always(out.b1 == true);
    printf("%s\n", err.msg);

    // // cannot be null
    // length = sizeof(buffer);
    // assert_always(cra_bin_serialize_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(!err.err);
    // buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    // assert_always(!cra_bin_deserialize_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    // printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &B_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[4] = 19; // bool -> 64bits
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &B_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);
}

void
test_int(void)
{
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(

      static, I, NULL,

      (INT8, i8n1, 1), (INT8, i8n2, 2), (INT8, i8m, 3), (INT8, i8M, 4),

      (INT16, i16n1, 5), (INT16, i16n2, 6), (INT16, i16m, 7), (INT16, i16M, 8),

      (INT32, i32n1, 9), (INT32, i32n2, 10), (INT32, i32m, 11), (INT32, i32M, 12),

      (INT64, i64n1, 13, false), (INT64, i64n2, 14), (INT64, i64m, 15), (INT64, i64M, 16)

    );

    unsigned char buffer[1024];
    size_t        length;
    CraSerErr     err;

    struct I out;
    struct I in = {
        -8,  8,  INT8_MIN,  INT8_MAX,

        -16, 16, INT16_MIN, INT16_MAX,

        -32, 32, INT32_MIN, INT32_MAX,

        -64, 64, INT64_MIN, INT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &I_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &I_meta));
    assert_always(in.i8n1 == out.i8n1);
    assert_always(in.i8n2 == out.i8n2);
    assert_always(in.i8m == out.i8m);
    assert_always(in.i8M == out.i8M);
    assert_always(in.i16n1 == out.i16n1);
    assert_always(in.i16n2 == out.i16n2);
    assert_always(in.i16m == out.i16m);
    assert_always(in.i16M == out.i16M);
    assert_always(in.i32n1 == out.i32n1);
    assert_always(in.i32n2 == out.i32n2);
    assert_always(in.i32m == out.i32m);
    assert_always(in.i32M == out.i32M);
    assert_always(in.i64n1 == out.i64n1);
    assert_always(in.i64n2 == out.i64n2);
    assert_always(in.i64m == out.i64m);
    assert_always(in.i64M == out.i64M);

    // test error

    // small buffer
    length = 80;
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &I_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // // cannot be null
    // length = sizeof(buffer);
    // assert_always(cra_bin_serialize_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(!err.err);
    // buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    // assert_always(!cra_bin_deserialize_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    // printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[6] = 31; // varint -> dict
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // size mismatch
    // length = sizeof(buffer);
    // assert_always(cra_bin_serialize(buffer, &length, &in, &I_meta) == CRA_SER_ERR_OK);
    // buffer[9] = 34;
    // assert_always(cra_bin_deserialize(buffer, length, &out, &I_meta) == CRA_SER_ERR_SIZE_MISMATCH);
}

void
test_uint(void)
{
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(

      static, U, NULL,

      (UINT8, u8n, 1), (UINT8, u8m, 3), (UINT8, u8M, 4),

      (UINT16, u16n, 5), (UINT16, u16m, 7), (UINT16, u16M, 8),

      (UINT32, u32n, 9, false), (UINT32, u32m, 11), (UINT32, u32M, 12),

      (UINT64, u64n, 13), (UINT64, u64m, 15), (UINT64, u64M, 16)

    );

    unsigned char buffer[1024];
    size_t        length;
    CraSerErr     err;

    struct U out;
    struct U in = {
        8,  0, UINT8_MAX,

        16, 0, UINT16_MAX,

        32, 0, UINT32_MAX,

        64, 0, UINT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &U_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &U_meta));
    assert_always(in.u8n == out.u8n);
    assert_always(in.u8m == out.u8m);
    assert_always(in.u8M == out.u8M);
    assert_always(in.u16n == out.u16n);
    assert_always(in.u16m == out.u16m);
    assert_always(in.u16M == out.u16M);
    assert_always(in.u32n == out.u32n);
    assert_always(in.u32m == out.u32m);
    assert_always(in.u32M == out.u32M);
    assert_always(in.u64n == out.u64n);
    assert_always(in.u64m == out.u64m);
    assert_always(in.u64M == out.u64M);

    // test error

    // small buffer
    length = 40;
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &U_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // // cannot be null
    // length = sizeof(buffer);
    // assert_always(cra_bin_serialize_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(!err.err);
    // buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    // assert_always(!cra_bin_deserialize_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    // printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[41] = 122; // 64bits -> 32bits
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // // size mismatch
    // length = sizeof(buffer);
    // assert_always(cra_bin_serialize_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(!err.err);
    // buffer[6] = 52;
    // assert_always(!cra_bin_deserialize_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_SIZE_MISMATCH);
    // printf("%s\n", err.msg);
}

void
test_varint(void)
{
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(
      static, I, NULL,

      (INT8, i8n1, 1, true), (INT8, i8n2, 2, true), (INT8, i8m, 3, true), (INT8, i8M, 4, true),

      (INT16, i16n1, 5, true), (INT16, i16n2, 6, true), (INT16, i16m, 7, true), (INT16, i16M, 8, true),

      (INT32, i32n1, 9, true), (INT32, i32n2, 10, true), (INT32, i32m, 11, true), (INT32, i32M, 12, true),

      (INT64, i64n1, 13, true), (INT64, i64n2, 14, true), (INT64, i64m, 15, true), (INT64, i64M, 16, true)

    );

    unsigned char buffer[1024];
    size_t        length;
    CraSerErr     err;

    struct I out;
    struct I in = {
        -8,  8,  INT8_MIN,  INT8_MAX,

        -16, 16, INT16_MIN, INT16_MAX,

        -32, 32, INT32_MIN, INT32_MAX,

        -64, 64, INT64_MIN, INT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &I_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &I_meta));
    assert_always(in.i8n1 == out.i8n1);
    assert_always(in.i8n2 == out.i8n2);
    assert_always(in.i8m == out.i8m);
    assert_always(in.i8M == out.i8M);
    assert_always(in.i16n1 == out.i16n1);
    assert_always(in.i16n2 == out.i16n2);
    assert_always(in.i16m == out.i16m);
    assert_always(in.i16M == out.i16M);
    assert_always(in.i32n1 == out.i32n1);
    assert_always(in.i32n2 == out.i32n2);
    assert_always(in.i32m == out.i32m);
    assert_always(in.i32M == out.i32M);
    assert_always(in.i64n1 == out.i64n1);
    assert_always(in.i64n2 == out.i64n2);
    assert_always(in.i64m == out.i64m);
    assert_always(in.i64M == out.i64M);

    // test error

    // small buffer
    length = 60;
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &I_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // // cannot be null
    // length = sizeof(buffer);
    // assert_always(cra_bin_serialize_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(!err.err);
    // buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    // assert_always(!cra_bin_deserialize_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    // printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[6] = 31; // varint -> dict
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // invalid value
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[55] = 255;
    buffer[56] = 1;
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    printf("%s\n", err.msg);
}

void
test_varuint(void)
{
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(

      static, U, NULL,

      (UINT8, u8n, 1, true), (UINT8, u8m, 3, true), (UINT8, u8M, 4, true),

      (UINT16, u16n, 5, true), (UINT16, u16m, 7, true), (UINT16, u16M, 8, true),

      (UINT32, u32n, 9, true), (UINT32, u32m, 11, true), (UINT32, u32M, 12, true),

      (UINT64, u64n, 13, true), (UINT64, u64m, 15, true), (UINT64, u64M, 16, true)

    );

    unsigned char buffer[1024];
    size_t        length;
    CraSerErr     err;

    struct U out;
    struct U in = {
        8,  0, UINT8_MAX,

        16, 0, UINT16_MAX,

        32, 0, UINT32_MAX,

        64, 0, UINT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &U_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &U_meta));
    assert_always(in.u8n == out.u8n);
    assert_always(in.u8m == out.u8m);
    assert_always(in.u8M == out.u8M);
    assert_always(in.u16n == out.u16n);
    assert_always(in.u16m == out.u16m);
    assert_always(in.u16M == out.u16M);
    assert_always(in.u32n == out.u32n);
    assert_always(in.u32m == out.u32m);
    assert_always(in.u32M == out.u32M);
    assert_always(in.u64n == out.u64n);
    assert_always(in.u64m == out.u64m);
    assert_always(in.u64M == out.u64M);

    // test error

    // small buffer
    length = 40;
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &U_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // // cannot be null
    // length = sizeof(buffer);
    // assert_always(cra_bin_serialize_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(!err.err);
    // buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    // assert_always(!cra_bin_deserialize_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    // printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[6] = 31; // varuint -> dict
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // invalid value
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[length - 1] = 255;
    buffer[length++] = 0;
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    printf("%s\n", err.msg);
}

void
test_float(void)
{
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(

      static, F, NULL,

      (FLOAT32, fn, 1), (FLOAT32, fm, 2), (FLOAT32, fM, 3), (FLOAT32, fW, 4),

      (FLOAT64, dn, 5), (FLOAT64, dm, 6), (FLOAT64, dM, 7), (FLOAT64, dW, 8)

    );

    unsigned char buffer[1024];
    size_t        length;
    CraSerErr     err;

    struct F out, in = {
        -1.5f, FLT_MIN, FLT_MAX, -FLT_MAX,

        64.83, DBL_MIN, DBL_MAX, -DBL_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &F_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &F_meta));
    assert_always(cra_cmp_float(in.fn, out.fn) == 0);
    assert_always(cra_cmp_float(in.fm, out.fm) == 0);
    assert_always(cra_cmp_float(in.fM, out.fM) == 0);
    assert_always(cra_cmp_float(in.fW, out.fW) == 0);
    assert_always(cra_cmp_double(in.dn, out.dn) == 0);
    assert_always(cra_cmp_double(in.dm, out.dm) == 0);
    assert_always(cra_cmp_double(in.dM, out.dM) == 0);
    assert_always(cra_cmp_double(in.dW, out.dW) == 0);

    // test error

    // small buffer
    length = 50;
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &F_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &F_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // // cannot be null
    // length = sizeof(buffer);
    // assert_always(cra_bin_serialize_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(!err.err);
    // buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    // assert_always(!cra_bin_deserialize_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    // printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &F_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[12] = 31; // 32bits -> dict
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &F_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // size mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &F_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[17] = 35; // 32bits -> 64bits
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &F_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);
}

#if 1 // test string

CRA_SER_DEF_STRUCT_META_AND_STRUCT_AND_UNINIT(

  static,
  Str,
  NULL,

  (STRING, strp, 1),
  (STRING, stra, 2, 16),
  (STRING, strnull, 3),
  (STRING, strpempty, 4),
  (STRING, straempty, 5, 32)

);

void
test_string(void)
{
    unsigned char buffer[1024];
    size_t        length;
    CraSerErr     err;

    struct Str out, in = { "hello pointer", "hello array", NULL, "", "" };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &Str_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &Str_meta));
    assert_always(strcmp(in.strp, out.strp) == 0);
    assert_always(strcmp(in.stra, out.stra) == 0);
    assert_always(in.strnull == out.strnull);
    assert_always(strcmp(in.strpempty, out.strpempty) == 0);
    assert_always(strcmp(in.straempty, out.straempty) == 0);
    Str_uninit(&out);

    // test error

    // small buffer
    length = 13;
    bzero(&out, sizeof(out));
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &Str_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &Str_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &Str_meta, &err));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[18] = 0;
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &Str_meta, &err));
    assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    bzero(&out, sizeof(out));
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &Str_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[2] = 13; // string -> struct
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &Str_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // char array too small
    length = sizeof(buffer);
    bzero(&out, sizeof(out));
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &Str_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[18] = 16 + 1;
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &Str_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OUT_OF_RANGE);
    printf("%s\n", err.msg);
}

#endif // end test string

#if 1 // test bytes

CRA_SER_DEF_ARRAY_META_AND_ARRAY_AND_UNINIT(static, (Bytesp), (UINT8));
CRA_SER_DEF_ARRAY_META_AND_ARRAY(static, (Bytesa, 16), (UINT8));
CRA_SER_DEF_STRUCT_META_AND_STRUCT_AND_UNINIT(

  static,
  MyBytes,
  NULL,

  //   (BYTES, bytp, 1), (BYTES, byta, 2, 16), (BYTES, bytnull, 3),
  //   (BYTES, bytpempty, 4), (BYTES, bytaempty, 5, 32)
  (ARRAY, bytp, 1, Bytesp, &Bytesp_meta),
  (ARRAY, byta, 2, Bytesa, &Bytesa_meta),
  (ARRAY, bytnull, 3, Bytesp, &Bytesp_meta),
  (ARRAY, bytpempty, 4, Bytesp, &Bytesp_meta),
  (ARRAY, bytaempty, 5, Bytesa, &Bytesa_meta)

);

void
test_bytes(void)
{
    unsigned char buffer[1024];
    size_t        length;
    CraSerErr     err;

    struct MyBytes out, in;
    unsigned char  bytep[] = "hello pointer";
    unsigned char  bytea[] = "hello array";
    in.bytp.data = bytep;
    in.bytp.len = sizeof(bytep) - 1;
    memcpy(in.byta.data, bytea, sizeof(bytea) - 1);
    in.byta.len = sizeof(bytea) - 1;
    in.bytnull.data = NULL;
    in.bytnull.len = 0;
    in.bytpempty.data = (unsigned char *)"";
    in.bytpempty.len = 0;
    in.bytaempty.len = 0;

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &MyBytes_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &MyBytes_meta));
    assert_always(in.bytp.len == out.bytp.len);
    assert_always(strncmp((char *)in.bytp.data, (char *)out.bytp.data, out.bytp.len) == 0);
    assert_always(in.byta.len == out.byta.len);
    assert_always(strncmp((char *)in.byta.data, (char *)out.byta.data, out.byta.len) == 0);
    assert_always(in.bytnull.len == out.bytnull.len);
    assert_always(in.bytnull.data == out.bytnull.data);
    assert_always(in.bytpempty.len == out.bytpempty.len);
    assert_always(strncmp((char *)in.bytpempty.data, (char *)out.bytpempty.data, out.bytpempty.len) == 0);
    assert_always(in.bytaempty.len == out.bytaempty.len);
    assert_always(strncmp((char *)in.bytaempty.data, (char *)out.bytaempty.data, out.bytaempty.len) == 0);
    MyBytes_uninit(&out);

    // test error

    // small buffer
    length = 13;
    bzero(&out, sizeof(out));
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &MyBytes_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &MyBytes_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &MyBytes_meta, &err));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[19] = 0;
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &MyBytes_meta, &err));
    assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    bzero(&out, sizeof(out));
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &MyBytes_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[2] = 10; // list(Array<byte>) -> 32bits
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &MyBytes_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // char array too small
    length = sizeof(buffer);
    bzero(&out, sizeof(out));
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &MyBytes_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[19] = 17 + 1;
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &MyBytes_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OUT_OF_RANGE);
    printf("%s\n", err.msg);
}

#endif // end test bytes

#if 1 // test struct

CRA_SER_DEF_STRUCT_META_AND_STRUCT(static, Struct1, NULL, (INT32, i, 1));
CRA_SER_DEF_STRUCT_META_AND_STRUCT_AND_UNINIT(

  static,
  Struct2,
  NULL,

  (STRUCT, strup, 1, Struct1, &Struct1_meta, true),
  (STRUCT, strus, 2, Struct1, &Struct1_meta, false),
  (STRUCT, strunull, 3, Struct1, &Struct1_meta, true)

);

void
test_struct(void)
{
    unsigned char buffer[1024];
    size_t        length;
    CraSerErr     err;

    struct Struct1 s = { .i = 1000 };
    struct Struct2 out, in = { .strup = &s, .strus = { .i = 2000 }, .strunull = NULL };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &Struct2_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &Struct2_meta));
    assert_always(!out.strunull);
    assert_always(out.strup);
    assert_always(in.strup->i == out.strup->i);
    assert_always(in.strus.i == out.strus.i);
    assert_always(in.strunull == out.strunull);
    Struct2_uninit(&out);

    // test error

    // small buffer
    length = 13;
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &Struct2_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &Struct2_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &Struct2_meta, &err));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[10] = 0;
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &Struct2_meta, &err));
    assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &Struct2_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[9] = 22; // struct -> list
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &Struct2_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);
}

#endif // end test struct

#if 1 // test struct id

static bool
init_o(void *stru, size_t size);
static bool
init_n(void *stru, size_t size);

CRA_SER_DEF_STRUCT_META_AND_STRUCT(static, O, init_o, (UINT32, u, 1), (FLOAT32, f, 3));

CRA_SER_DEF_ARRAY_META_AND_ARRAY(static, (Bytes, 8), (UINT8));
CRA_SER_DEF_ARRAY_META_AND_ARRAY(static, (Array_o, 5), (STRUCT, O, &O_meta));
CRA_SER_DEF_LIST_META(static, (CraLList, CraLList_i32), (INT32), CRA_LLIST_SRLZ);
CRA_DICT_DEF_SRLZ_INIT_FN(d_init_i32, int32_t, int32_t, cra_hash_int32_p, cra_cmp_int32_p)
CRA_SER_DEF_DICT_META(static, (CraDict, CraDict_i32), (INT32), (INT32), CRA_DICT_SRLZ(d_init_i32));

CRA_SER_DEF_STRUCT_META_AND_STRUCT(

  static,
  N,
  init_n,

  (UINT32, u, 1),
  (INT32, i, 2),
  (FLOAT32, f, 3),
  (FLOAT64, d, 4),
  (BOOL, b, 5),
  (STRING, n, 6),                                    // null
  (UINT64, v, 7, true),                              // varint
  (STRING, s, 8),                                    // string
  (ARRAY, t, 9, Bytes, &Bytes_meta),                 // bytes
  (STRUCT, o, 10, O, &O_meta, true),                 // struct
  (ARRAY, a, 11, Array_o, &Array_o_meta),            // array
  (LIST, l, 12, CraLList, &CraLList_i32_meta, true), // list
  (DICT, c, 13, CraDict, &CraDict_i32_meta, false)   // dict

);

static bool
init_o(void *stru, size_t size)
{
    assert_always(size == sizeof(struct O));
    struct O *o = (struct O *)stru;
    o->u = 200;
    o->f = 1.0f;
    return true;
}

static bool
init_n(void *stru, size_t size)
{
    assert_always(size == sizeof(struct N));
    struct N *n = (struct N *)stru;
    n->u = 300;
    n->i = -48;
    n->f = 2.5f;
    n->d = 88.9;
    n->b = false;
    n->n = NULL;
    n->v = 0;
    n->s = NULL;
    bzero(&n->t, sizeof(n->t));
    n->o = NULL;
    bzero(&n->a, sizeof(n->a));
    n->l = NULL;
    bzero(&n->c, sizeof(n->c));
    return true;
}

void
test_struct_id(void)
{
    unsigned char buffer[1024];
    size_t        length;

    struct O outo, ino = { .u = 4000, .f = 6.7f };
    struct N outn, inn = { .u = 300,
                           .i = -30,
                           .f = 7.7f,
                           .d = 1002.3,
                           .b = true,
                           .n = NULL,
                           .v = 8000000,
                           .s = "string",
                           .t.data = { 'b', 'y', 't', 'e', 's' },
                           .t.len = 5,
                           .o = &ino,
                           .a.data[0] = { .f = 0.5f, .u = 100 },
                           .a.data[1] = { .f = 3.6f, .u = 300 },
                           .a.len = 2,
                           .l = cra_alloc(CraLList) };

    cra_llist_init(int32_t, inn.l);
    cra_dict_init(int32_t, int32_t, &inn.c, cra_hash_int32_p, cra_cmp_int32_p);
    for (int32_t i = 0; i < 20; i++)
    {
        cra_llist_append(inn.l, &i);
        cra_dict_add(&inn.c, &i, &i);
    }

    // o => n
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &ino, &O_meta));
    assert_always(cra_bin_deserialize(buffer, length, &outn, &N_meta));
    assert_always(ino.u == outn.u);
    assert_always(-48 == outn.i);
    assert_always(cra_cmp_float(ino.f, outn.f) == 0);
    assert_always(cra_cmp_double(88.9, outn.d) == 0);
    assert_always(false == outn.b);
    assert_always(NULL == outn.n);
    assert_always(0 == outn.v);
    assert_always(NULL == outn.s);
    assert_always(memcmp(&(Bytes){ 0 }, &outn.t, sizeof(outn.t)) == 0);
    assert_always(NULL == outn.o);
    assert_always(memcmp(&(Array_o){ 0 }, &outn.a, sizeof(Array_o)) == 0);
    assert_always(NULL == outn.l);
    assert_always(memcmp(&(CraDict){ 0 }, &outn.c, sizeof(CraDict)) == 0);

    // n => o
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &inn, &N_meta));
    assert_always(cra_bin_deserialize(buffer, length, &outo, &O_meta));
    assert_always(inn.u == outo.u);
    assert_always(cra_cmp_float(inn.f, outo.f) == 0);

    cra_dict_uninit(&inn.c);
    cra_llist_uninit(inn.l);
    cra_dealloc(inn.l);
}

#endif // end test struct id

#if 1 // test array

CRA_SER_DEF_ARRAY_META_AND_ARRAY(static, (I32, 100), (INT32));
CRA_SER_DEF_ARRAY_META_AND_ARRAY_AND_UNINIT(static, (U64), (UINT64));
CRA_SER_DEF_ARRAY_META_AND_ARRAY_AND_UNINIT(static, (CA), (STRING, 20));
CRA_SER_DEF_ARRAY_META_AND_ARRAY_AND_UNINIT(static, (CP), (STRING));
CRA_SER_DEF_ARRAY_META_AND_ARRAY_AND_UNINIT(static, (F32), (FLOAT32));

CRA_SER_DEF_STRUCT_META_AND_STRUCT_AND_UNINIT(

  static,
  StructArray,
  NULL,

  (ARRAY, arraya, 1, I32, &I32_meta),
  (ARRAY, arrayp, 2, U64, &U64_meta),
  (ARRAY, arrsa, 3, CA, &CA_meta),

  (ARRAY, arrsp, 4, CP, &CP_meta),
  (ARRAY, arrayempty, 5, F32, &F32_meta),
  (ARRAY, arrnull, 6, F32, &F32_meta)

);

void
test_array(void)
{
    unsigned char buffer[8192];
    size_t        length;
    CraSerErr     err;

    struct StructArray out, in;

    in.arrayp.data = (uint64_t *)cra_calloc(50, sizeof(uint64_t));
    in.arrsa.data = (char (*)[20])cra_calloc(100, sizeof(in.arrsa.data[0]));
    in.arrsp.data = (char **)cra_calloc(50, sizeof(char *));
    in.arrayempty.data = (float *)cra_calloc(2, sizeof(float));
    in.arrnull.data = NULL;
    for (int i = 0; i < 100; i++)
    {
        in.arraya.data[i] = 24 + i;
        snprintf(in.arrsa.data[i], sizeof(in.arrsa.data[i]), "hello %d.", i);
    }
    for (int i = 0; i < 50; i++)
    {
        in.arrayp.data[i] = (uint64_t)(3.5 * (double)i);
        in.arrsp.data[i] = cra_malloc(30);
        snprintf(in.arrsp.data[i], 30, "good %d.", i);
    }
    in.arraya.len = 100;
    in.arrsa.len = 100;
    in.arrayp.len = 50;
    in.arrsp.len = 50;
    in.arrayempty.len = 0;
    in.arrnull.len = 0;

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &StructArray_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &StructArray_meta));
    assert_always(in.arraya.len == out.arraya.len);
    assert_always(in.arrayp.len == out.arrayp.len);
    assert_always(in.arrsa.len == out.arrsa.len);
    assert_always(in.arrsp.len == out.arrsp.len);
    assert_always(in.arrayempty.len == out.arrayempty.len);
    assert_always(NULL != out.arrayempty.data);
    assert_always(in.arrnull.len == out.arrnull.len);
    assert_always(in.arrnull.data == out.arrnull.data);
    for (int i = 0; i < 100; i++)
    {
        assert_always(in.arraya.data[i] == out.arraya.data[i]);
        assert_always(strcmp(in.arrsa.data[i], out.arrsa.data[i]) == 0);
    }
    for (int i = 0; i < 50; i++)
    {
        assert_always(in.arrayp.data[i] == out.arrayp.data[i]);
        assert_always(strcmp(in.arrsp.data[i], out.arrsp.data[i]) == 0);
    }
    StructArray_uninit(&out);

    // test error

    // small buffer
    length = 1000;
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &StructArray_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &StructArray_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &StructArray_meta, &err));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[3] = 0;
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &StructArray_meta, &err));
    assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &StructArray_meta, &err));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[808] = 31; // list -> dict
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &StructArray_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // array too small
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &StructArray_meta, &err));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[3] = 101 + 1;
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &StructArray_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OUT_OF_RANGE);
    printf("%s\n", err.msg);

    StructArray_uninit(&in);
}

#endif // end test array

#if 1 // test list

CRA_SER_DEF_LIST_META(static, (CraAList, ALI32), (INT32), CRA_ALIST_SRLZ);
CRA_SER_DEF_LIST_META(static, (CraLList, LLStr), (STRING, 20), CRA_LLIST_SRLZ);
CRA_SER_DEF_LIST_META(static, (CraDeque, DeStr), (STRING), CRA_DEQUE_SRLZ);

CRA_SER_DEF_STRUCT_META_AND_STRUCT_AND_UNINIT(

  static,
  L,
  NULL,

  (LIST, alist, 1, CraAList, &ALI32_meta),
  (LIST, llist, 2, CraLList, &LLStr_meta, true),
  (LIST, deque, 3, CraDeque, &DeStr_meta, true),
  (LIST, alistempty, 4, CraAList, &ALI32_meta, true),
  (LIST, llistempty, 5, CraLList, &LLStr_meta, false),
  (LIST, dequeempty, 6, CraDeque, &DeStr_meta, false),
  (LIST, alistnull, 7, CraAList, &ALI32_meta, true),
  (LIST, llistnull, 8, CraLList, &LLStr_meta, true),
  (LIST, dequenull, 9, CraDeque, &DeStr_meta, true)

);

void
test_list(void)
{
    unsigned char buffer[8192];
    size_t        length;
    CraSerErr     err;

    int32_t  vali1, vali2;
    char     valsa1[20], valsa2[20];
    char    *valsp1, *valsp2;
    struct L out, in;
    char     s20[20] = "good char array";
    char    *sp = "hello sp";

    in.alistnull = NULL;
    in.llistnull = NULL;
    in.dequenull = NULL;
    in.llist = cra_alloc(CraLList);
    in.deque = cra_alloc(CraDeque);
    in.alistempty = cra_alloc(CraAList);
    cra_alist_init(int32_t, &in.alist);
    cra_llist_init(char[20], in.llist);
    cra_deque_init(char *, in.deque);
    cra_alist_init(int32_t, in.alistempty);
    cra_llist_init(char[20], &in.llistempty);
    cra_deque_init(char *, &in.dequeempty);
    for (int i = 0; i < 100; i++)
    {
        cra_alist_append(&in.alist, &i);
        cra_llist_append(in.llist, &s20);
        cra_deque_push_back(in.deque, &sp);
    }

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &L_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &L_meta));
    assert_always(in.alist.count == out.alist.count);
    assert_always(in.llist->count == out.llist->count);
    assert_always(in.deque->count == out.deque->count);
    assert_always(in.alistempty->count == out.alistempty->count);
    assert_always(in.llistempty.count == out.llistempty.count);
    assert_always(in.dequeempty.count == out.dequeempty.count);
    assert_always(in.alistnull == out.alistnull);
    assert_always(in.llistnull == out.llistnull);
    assert_always(in.dequenull == out.dequenull);
    for (int i = 0; i < 100; i++)
    {
        cra_alist_get(&in.alist, i, &vali1);
        cra_alist_get(&out.alist, i, &vali2);
        assert_always(vali1 == vali2);

        cra_llist_get(in.llist, i, &valsa1);
        cra_llist_get(out.llist, i, &valsa2);
        assert_always(strcmp(valsa1, valsa2) == 0);

        cra_deque_get(in.deque, i, &valsp1);
        cra_deque_get(out.deque, i, &valsp2);
        assert_always(strcmp(valsp1, valsp2) == 0);
        cra_free(valsp2);
    }
    cra_alist_uninit(&out.alist);
    cra_llist_uninit(out.llist);
    cra_deque_uninit(out.deque);
    cra_alist_uninit(out.alistempty);
    cra_llist_uninit(&out.llistempty);
    cra_deque_uninit(&out.dequeempty);
    cra_dealloc(out.llist);
    cra_dealloc(out.deque);
    cra_dealloc(out.alistempty);

    // test error

    // small buffer
    length = 1000;
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &L_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &L_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &L_meta, &err));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[3] = 0;
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &L_meta, &err));
    assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &L_meta, &err));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[2] = 15; // list -> dict
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &L_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    cra_alist_uninit(&in.alist);
    cra_llist_uninit(in.llist);
    cra_deque_uninit(in.deque);
    cra_alist_uninit(in.alistempty);
    cra_llist_uninit(&in.llistempty);
    cra_deque_uninit(&in.dequeempty);
    cra_dealloc(in.llist);
    cra_dealloc(in.deque);
    cra_dealloc(in.alistempty);
}

#endif // end test list

#if 1 // test array list

CRA_SER_DEF_ARRAY_META_AND_ARRAY(static, (Array, 30), (INT32));
CRA_SER_DEF_LIST_META(static, (CraAList, CraAList), (INT32), CRA_ALIST_SRLZ);
CRA_SER_DEF_LIST_META(static, (CraLList, CraLList), (INT32), CRA_LLIST_SRLZ);
CRA_SER_DEF_LIST_META(static, (CraDeque, CraDeque), (INT32), CRA_DEQUE_SRLZ);

static size_t
get_array_count(Array *array)
{
    return array->len;
}

static size_t
get_alist_count(CraAList *alist)
{
    return alist->count;
}

static size_t
get_llist_count(CraLList *llist)
{
    return llist->count;
}

static size_t
get_deque_count(CraDeque *deque)
{
    return deque->count;
}

static bool
get_array_val(Array *array, size_t index, int32_t *retval)
{
    *retval = array->data[index];
    return true;
}

void
test_array_list(void)
{
    Array    inarray, outarray;
    CraAList inalist, outalist;
    CraLList inllist, outllist;
    CraDeque indeque, outdeque;

    inarray.len = 30;
    cra_alist_init(int32_t, &inalist);
    cra_llist_init(int32_t, &inllist);
    cra_deque_init(int32_t, &indeque);
    for (int32_t i = 0; i < 30; i++)
    {
        inarray.data[i] = i;
        if (i % 2 == 0)
            cra_alist_append(&inalist, &(int32_t){ i + 4 });
        if (i % 2 != 0)
            cra_llist_append(&inllist, &(int32_t){ i * 2 });
        if (i < 18)
            cra_deque_push_back(&indeque, &(int32_t){ i * 8 });
    }

    const CraSerMeta *meta[] = { &Array_meta, &CraAList_meta, &CraLList_meta, &CraDeque_meta };
    void             *outs[] = { &outarray, &outalist, &outllist, &outdeque };
    void             *ins[] = { &inarray, &inalist, &inllist, &indeque };
    size_t            (*getcntfns[])(void *) = {
        (size_t (*)(void *))get_array_count,
        (size_t (*)(void *))get_alist_count,
        (size_t (*)(void *))get_llist_count,
        (size_t (*)(void *))get_deque_count,
    };
    bool (*getvalfns[])(void *, size_t, void *) = {
        (bool (*)(void *, size_t, void *))get_array_val,
        (bool (*)(void *, size_t, void *))cra_alist_get,
        (bool (*)(void *, size_t, void *))cra_llist_get,
        (bool (*)(void *, size_t, void *))cra_deque_get,
    };
    void (*uninitfns[])(void *) = {
        NULL,
        (void (*)(void *))cra_alist_uninit,
        (void (*)(void *))cra_llist_uninit,
        (void (*)(void *))cra_deque_uninit,
    };

    unsigned char buffer[4096];
    size_t        length;

    size_t  n1, n2;
    int32_t v1, v2;
    for (size_t i = 0; i < CRA_NARRAY(ins); i++)
    {
        length = sizeof(buffer);
        assert_always(cra_bin_serialize(buffer, &length, ins[i], meta[i]));
        for (size_t j = 0; j < CRA_NARRAY(outs); j++)
        {
            assert_always(cra_bin_deserialize(buffer, length, outs[j], meta[j]));
            n2 = getcntfns[j](outs[j]);
            n1 = getcntfns[i](ins[i]);
            assert_always(n1 == n2);
            for (size_t k = 0; k < n1; k++)
            {
                getvalfns[i](ins[i], k, &v1);
                getvalfns[j](outs[j], k, &v2);
                assert_always(v1 == v2);
            }
            if (uninitfns[j])
                uninitfns[j](outs[j]);
        }
        if (uninitfns[i])
            uninitfns[i](ins[i]);
    }
}

#endif

#if 1 // test dict

CRA_DICT_DEF_SRLZ_INIT_FN(d_init_if, int32_t, float, cra_hash_int32_p, cra_cmp_int32_p)
CRA_DICT_DEF_SRLZ_INIT_FN(d_init_du, double, uint8_t, cra_hash_double_p, cra_cmp_double_p)
CRA_SER_DEF_DICT_META(static, (CraDict, DIF), (INT32), (FLOAT32), CRA_DICT_SRLZ(d_init_if));
CRA_SER_DEF_DICT_META(static, (CraDict, DDU), (FLOAT64), (UINT8), CRA_DICT_SRLZ(d_init_du));
CRA_SER_DEF_STRUCT_META_AND_STRUCT_AND_UNINIT(

  static,
  D,
  NULL,

  (DICT, ds, 1, CraDict, &DIF_meta),

  (DICT, dp, 2, CraDict, &DDU_meta, true),

  (DICT, dempty, 3, CraDict, &DIF_meta, true),

  (DICT, dnull, 4, CraDict, &DDU_meta, true)

);

void
test_dict(void)
{
    unsigned char buffer[2048];
    size_t        length;
    CraSerErr     err;

    int32_t  i1;
    float    f1, f2;
    double   d1;
    uint8_t  u1, u2;
    struct D out, in;

    in.dnull = NULL;
    in.dp = cra_alloc(CraDict);
    in.dempty = cra_alloc(CraDict);
    cra_dict_init(int32_t, float, &in.ds, cra_hash_int32_p, cra_cmp_int32_p);
    cra_dict_init(double, uint8_t, in.dp, cra_hash_double_p, cra_cmp_double_p);
    cra_dict_init(int32_t, float, in.dempty, cra_hash_int32_p, cra_cmp_int32_p);
    for (int i = 0; i < 100; i++)
    {
        i1 = i * 7;
        f1 = (float)i * 1.5f;
        cra_dict_add(&in.ds, &i1, &f1);
        if (i % 2 == 0)
        {
            d1 = (double)i + 3;
            u1 = (uint8_t)i;
            cra_dict_add(in.dp, &d1, &u1);
        }
    }

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &D_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &D_meta));
    assert_always(in.dnull == out.dnull);
    assert_always(in.ds.count == out.ds.count);
    assert_always(in.dp->count == out.dp->count);
    assert_always(in.dempty->count == out.dempty->count);
    CRA_FOREACH(CraDict, &in.ds, &i1, &f1)
    {
        cra_dict_get(&out.ds, &i1, &f2);
        assert_always(cra_cmp_float(f1, f2) == 0);
    }
    CRA_FOREACH(CraDict, in.dp, &d1, &u1)
    {
        cra_dict_get(out.dp, &d1, &u2);
        assert_always(u1 == u2);
    }
    D_uninit(&out);

    // test error

    // small buffer
    length = 1000;
    assert_always(!cra_bin_serialize_err(buffer, &length, &in, &D_meta, &err));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &D_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &D_meta, &err));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[3] = 0;
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &D_meta, &err));
    assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize_err(buffer, &length, &in, &D_meta, &err));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[2] = 10; // dict -> 32bits
    assert_always(!cra_bin_deserialize_err(buffer, length, &out, &D_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // // invalid key type
    // length = sizeof(buffer);
    // ((CraTypeMeta *)metadu)->type = CRA_TYPE_BYTES;
    // assert_always(!cra_bin_serialize_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_CANNOT_BE_KEY);
    // printf("%s\n", err.msg);
    // assert_always(!cra_bin_deserialize_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_CANNOT_BE_KEY);
    // printf("%s\n", err.msg);

    cra_dict_uninit(&in.ds);
    cra_dict_uninit(in.dp);
    cra_dict_uninit(in.dempty);
    cra_dealloc(in.dp);
    cra_dealloc(in.dempty);
}

#endif // end test dict

#if 1 // test deserialize failed

CRA_SER_DEF_STRUCT_META_AND_STRUCT_AND_UNINIT(static, A, NULL, (STRING, str, 1));
CRA_SER_DEF_ARRAY_META_AND_ARRAY_AND_UNINIT(static, (ArrStr), (STRING));
CRA_ALIST_DEF_SRLZ_UNINIT_FN(AListA_uninit, A *, a, A_uninit(a); cra_free(a);)
CRA_SER_DEF_LIST_META(static,
                      (CraAList, AListA),
                      (STRUCT, A, &A_meta, true),
                      CRA_ALIST_SRLZ_WITH_UNINIT(AListA_uninit));
CRA_LLIST_DEF_SRLZ_UNINIT_FN(LListA_uninit, A *, a, A_uninit(a); cra_free(a);)
CRA_SER_DEF_LIST_META(static,
                      (CraLList, LListA),
                      (STRUCT, A, &A_meta, true),
                      CRA_LLIST_SRLZ_WITH_UNINIT(LListA_uninit));
CRA_DEQUE_DEF_SRLZ_UNINIT_FN(DequeA_uninit, A *, a, A_uninit(a); cra_free(a);)
CRA_SER_DEF_LIST_META(static,
                      (CraDeque, DequeA),
                      (STRUCT, A, &A_meta, true),
                      CRA_DEQUE_SRLZ_WITH_UNINIT(DequeA_uninit));
CRA_DICT_DEF_SRLZ_INIT_FN(DictA_init, char *, A *, cra_hash_string1_p, cra_cmp_string_p)
CRA_DICT_DEF_SRLZ_UNINIT_FN(DictA_uninit, char *, key, A *, val, {
    cra_free(key);
    A_uninit(val);
    cra_free(val);
})
CRA_SER_DEF_DICT_META(static,
                      (CraDict, DictA),
                      (STRING),
                      (STRUCT, A, &A_meta, true),
                      CRA_DICT_SRLZ_WITH_UNINIT(DictA_init, DictA_uninit));
CRA_SER_DEF_STRUCT_META_AND_STRUCT_AND_UNINIT(

  static,
  B,
  NULL,

  (STRUCT, stru, 1, A, &A_meta, true),            // struct A *
  (ARRAY, array, 2, ArrStr, &ArrStr_meta),        // ArrStr
  (LIST, alist, 3, CraAList, &AListA_meta, true), // AList<struct A *>
  (LIST, llist, 4, CraLList, &LListA_meta, true), // LList<struct A *>
  (LIST, deque, 5, CraDeque, &DequeA_meta, true), // Deque<struct A *>
  (DICT, dict, 6, CraDict, &DictA_meta, true)     // Dict<char *, struct A *>

);

static void
B_init(struct B *b)
{
    b->stru = cra_malloc(sizeof(struct A));
    b->array.data = (char **)cra_calloc(3, sizeof(char *));
    b->array.len = 3;
    b->alist = cra_malloc(sizeof(CraAList));
    b->llist = cra_malloc(sizeof(CraLList));
    b->deque = cra_malloc(sizeof(CraDeque));
    b->dict = cra_malloc(sizeof(CraDict));
    cra_alist_init(struct A *, b->alist);
    cra_llist_init(struct A *, b->llist);
    cra_deque_init(struct A *, b->deque);
    cra_dict_init(char *, struct A *, b->dict, cra_hash_string1_p, cra_cmp_string_p);

    char     *str;
    struct A *a;

    b->stru->str = (char *)cra_malloc(20);
    memcpy(b->stru->str, "struct.", sizeof("struct."));

    for (int i = 0; i < (int)b->array.len; i++)
    {
        str = (char *)cra_malloc(20);
        snprintf(str, 20, "array %d.", i);
        b->array.data[i] = str;

        a = cra_malloc(sizeof(struct A));
        a->str = (char *)cra_malloc(20);
        snprintf(a->str, 20, "alist  %d.", i);
        cra_alist_append(b->alist, &a);

        a = cra_malloc(sizeof(struct A));
        a->str = (char *)cra_malloc(20);
        snprintf(a->str, 20, "llist   %d.", i);
        cra_llist_append(b->llist, &a);

        a = cra_malloc(sizeof(struct A));
        a->str = (char *)cra_malloc(30);
        snprintf(a->str, 30, "deque    %d.", i);
        cra_deque_push_back(b->deque, &a);

        str = (char *)cra_malloc(30);
        a = cra_malloc(sizeof(struct A));
        a->str = (char *)cra_malloc(30);
        snprintf(str, 30, "dict key     %d.", i);
        snprintf(a->str, 30, "dict val %d.", i);
        cra_dict_add(b->dict, &str, &a);
    }
}

void
test_deserialize_failed(void)
{

    struct B      in, out;
    unsigned char buffer[8192];
    size_t        length;

    B_init(&in);
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, &in, &B_meta));
    assert_always(cra_bin_deserialize(buffer, length, &out, &B_meta));

    assert_always(out.stru && out.stru->str);
    assert_always(strcmp(in.stru->str, out.stru->str) == 0);
    assert_always(in.array.len == out.array.len);
    assert_always(in.alist->count == out.alist->count);
    assert_always(in.llist->count == out.llist->count);
    assert_always(in.deque->count == out.deque->count);
    assert_always(in.dict->count == out.dict->count);

    struct A *val1, *val2;
    for (int i = 0; i < (int)in.array.len; i++)
    {
        assert_always(strcmp(in.array.data[i], out.array.data[i]) == 0);

        cra_alist_get(in.alist, i, &val1);
        cra_alist_get(out.alist, i, &val2);
        assert_always(strcmp(val1->str, val2->str) == 0);

        cra_llist_get(in.llist, i, &val1);
        cra_llist_get(out.llist, i, &val2);
        assert_always(strcmp(val1->str, val2->str) == 0);

        cra_deque_get(in.deque, i, &val1);
        cra_deque_get(out.deque, i, &val2);
        assert_always(strcmp(val1->str, val2->str) == 0);
    }

    char *key;
    CRA_FOREACH(CraDict, in.dict, &key, &val1)
    {
        cra_dict_get(out.dict, &key, &val2);
        assert_always(strcmp(val1->str, val2->str) == 0);
    }

    B_uninit(&out);

    assert_always(!cra_bin_deserialize(buffer, length - 1, &out, &B_meta));
    assert_always(!cra_bin_deserialize(buffer, 126, &out, &B_meta));
    assert_always(!cra_bin_deserialize(buffer, 84, &out, &B_meta));
    assert_always(!cra_bin_deserialize(buffer, 45, &out, &B_meta));
    assert_always(!cra_bin_deserialize(buffer, 17, &out, &B_meta));

    B_uninit(&in);
}

#endif

CRA_DICT_DEF_SRLZ_INIT_FN(DSI64_init, char *, int64_t, cra_hash_string2_p, cra_cmp_string_p)
void
test_to_file(void)
{
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(static, Fi, NULL, (INT32, i, 1), (FLOAT32, f, 2));
    CRA_SER_DEF_ARRAY_META_AND_ARRAY(static, (Bytestf), (UINT8));
    CRA_SER_DEF_LIST_META(static, (CraAList, ALI32tf), (INT32), CRA_ALIST_SRLZ);
    CRA_SER_DEF_DICT_META(static, (CraDict, DSI64tf), (STRING), (INT64), CRA_DICT_SRLZ(DSI64_init));
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(

      static, Fo, NULL,

      (INT16, si, 1), (INT32, i, 2), (FLOAT64, d, 3),

      (STRING, s, 4), (BOOL, b, 5),

      (ARRAY, bt, 6, Bytestf, &Bytestf_meta),

      (STRUCT, fi, 7, Fi, &Fi_meta),

      (LIST, l, 8, CraAList, &ALI32tf_meta),

      (DICT, dict, 9, CraDict, &DSI64tf_meta)

    );

    // ============

    struct Fo in = {
        .si = 100,
        .i = 1000,
        .d = 1000000.0,
        .s = "Hello 世界！",
        .b = true,
        .bt = { (uint8_t *)"中文", sizeof("中文") - 1 },
        .fi = { .i = 1000000, .f = 1000000.0 },
    };
    cra_alist_init(int32_t, &in.l);
    cra_alist_append(&in.l, &(int32_t){ 1 });
    cra_alist_append(&in.l, &(int32_t){ 2 });
    cra_alist_append(&in.l, &(int32_t){ 3 });
    cra_dict_init(char *, int64_t, &in.dict, cra_hash_string2_p, cra_cmp_string_p);
    cra_dict_add(&in.dict, &(char *){ "a" }, &(int64_t){ 1000000000000000000 });
    cra_dict_add(&in.dict, &(char *){ "b" }, &(int64_t){ 2000000000000000000 });

    // serialize
    unsigned char buffer[1024];
    size_t        length = sizeof(buffer);
    char         *filename = "test.bin";
    assert_always(cra_bin_serialize(buffer, &length, &in, &Fo_meta));
    // write to file
    FILE *fp = fopen(filename, "wb");
    assert_always(fp != NULL);
    fwrite(buffer, 1, length, fp);
    fclose(fp);

    // read from file
    bzero(buffer, sizeof(buffer));
    FILE *fp2 = fopen(filename, "rb");
    assert_always(fp2 != NULL);
    length = fread(buffer, 1, sizeof(buffer), fp2);
    assert_always(length > 0);
    fclose(fp2);

    // deserialize
    struct Fo out;
    assert_always(cra_bin_deserialize(buffer, length, &out, &Fo_meta));
    // compare
    assert_always(out.si == in.si);
    assert_always(out.i == in.i);
    assert_always(cra_cmp_double(out.d, in.d) == 0);
    assert_always(strcmp(out.s, in.s) == 0);
    assert_always(out.b == in.b);
    assert_always(out.bt.len == in.bt.len);
    assert_always(memcmp(out.bt.data, in.bt.data, in.bt.len) == 0);
    assert_always(out.fi.i == in.fi.i);
    assert_always(cra_cmp_float(out.fi.f, in.fi.f) == 0);
    assert_always(out.l.count == in.l.count);
    assert_always(out.dict.count == in.dict.count);

    char   *key;
    int64_t val;
    cra_free(out.s);
    cra_free(out.bt.data);
    CRA_FOREACH(CraDict, &out.dict, &key, &val)
    {
        cra_free(key);
    }
    cra_alist_uninit(&in.l);
    cra_alist_uninit(&out.l);
    cra_dict_uninit(&in.dict);
    cra_dict_uninit(&out.dict);
}

int
main(void)
{
    printf("========== test meta with struct ==========\n");
    test_meta_with_struct();
    printf("========== test meta with array ==========\n");
    test_meta_with_array();
    printf("========== test meta with list ==========\n");
    test_meta_with_list();
    printf("========== test meta with dict ==========\n");
    test_meta_with_dict();
    printf("========== test check id duplicate ==========\n");
    test_check_id_duplicate();
    printf("\n");

    printf("========== test bool ==========\n");
    test_bool();
    printf("========== test int ==========\n");
    test_int();
    printf("========== test uint ==========\n");
    test_uint();
    printf("========== test varint ==========\n");
    test_varint();
    printf("========== test varuint ==========\n");
    test_varuint();
    printf("========== test float ==========\n");
    test_float();
    printf("========== test string ==========\n");
    test_string();
    printf("========== test bytes ==========\n");
    test_bytes();
    printf("========== test struct ==========\n");
    test_struct();
    printf("========== test struct id ==========\n");
    test_struct_id();
    printf("========== test c array ==========\n");
    test_array();
    printf("========== test list ==========\n");
    test_list();
    printf("========== test array list ==========\n");
    test_array_list();
    printf("========== test dict ==========\n");
    test_dict();
    printf("========== test deserialize failed ==========\n");
    test_deserialize_failed();
    printf("========== test to file ==========\n");
    test_to_file();

    printf("\n");
    cra_memory_leak_report();
    return 0;
}
