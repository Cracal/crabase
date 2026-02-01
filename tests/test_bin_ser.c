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
#include "cra_assert.h"
#include "cra_malloc.h"
#include "serialize/cra_bin_ser.h"
#include "serialize/cra_ser_collections.h"
#include <float.h>

void
test_bool(void)
{
    struct B
    {
        bool b0;
        bool b1;
    };
    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_BOOL(struct B, b0, 1)
    CRA_TYPE_META_MEMBER_BOOL(struct B, b1, 2)
    CRA_TYPE_META_END();

    size_t        length;
    unsigned char buffer[10];
    struct B      in, *ib2, out, *rb2;

    in.b0 = false;
    in.b1 = true;
    ib2 = &in;

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
    assert_always(in.b0 == out.b0);
    assert_always(in.b1 == out.b1);

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(ib2, true, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(rb2, true, meta, NULL, NULL)));
    assert_always(ib2->b0 == rb2->b0);
    assert_always(ib2->b1 == rb2->b1);
    cra_free(rb2);

    // test error

    // small buffer
    length = 3;
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[5] = ((unsigned char)CRA_TYPE_INT) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
}

void
test_int(void)
{
    struct I
    {
        int8_t i8n1;
        int8_t i8n2;
        int8_t i8m;
        int8_t i8M;

        int16_t i16n1;
        int16_t i16n2;
        int16_t i16m;
        int16_t i16M;

        int32_t i32n1;
        int32_t i32n2;
        int32_t i32m;
        int32_t i32M;

        int64_t i64n1;
        int64_t i64n2;
        int64_t i64m;
        int64_t i64M;
    };
    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_INT(struct I, i8n1, 1)
    CRA_TYPE_META_MEMBER_INT(struct I, i8n2, 2)
    CRA_TYPE_META_MEMBER_INT(struct I, i8m, 3)
    CRA_TYPE_META_MEMBER_INT(struct I, i8M, 4)

    CRA_TYPE_META_MEMBER_INT(struct I, i16n1, 5)
    CRA_TYPE_META_MEMBER_INT(struct I, i16n2, 6)
    CRA_TYPE_META_MEMBER_INT(struct I, i16m, 7)
    CRA_TYPE_META_MEMBER_INT(struct I, i16M, 8)

    CRA_TYPE_META_MEMBER_INT(struct I, i32n1, 9)
    CRA_TYPE_META_MEMBER_INT(struct I, i32n2, 10)
    CRA_TYPE_META_MEMBER_INT(struct I, i32m, 11)
    CRA_TYPE_META_MEMBER_INT(struct I, i32M, 12)

    CRA_TYPE_META_MEMBER_INT(struct I, i64n1, 13)
    CRA_TYPE_META_MEMBER_INT(struct I, i64n2, 14)
    CRA_TYPE_META_MEMBER_INT(struct I, i64m, 15)
    CRA_TYPE_META_MEMBER_INT(struct I, i64M, 16)
    CRA_TYPE_META_END();

    unsigned char buffer[1024];
    size_t        length;

    struct I out;
    struct I in = {
        -8,  8,  INT8_MIN,  INT8_MAX,

        -16, 16, INT16_MIN, INT16_MAX,

        -32, 32, INT32_MIN, INT32_MAX,

        -64, 64, INT64_MIN, INT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
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
    length = 93;
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[6] = ((unsigned char)CRA_TYPE_FLOAT) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // size mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[9] = 34;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
}

void
test_uint(void)
{
    struct U
    {
        uint8_t u8n;
        uint8_t u8m;
        uint8_t u8M;

        uint16_t u16n;
        uint16_t u16m;
        uint16_t u16M;

        uint32_t u32n;
        uint32_t u32m;
        uint32_t u32M;

        uint64_t u64n;
        uint64_t u64m;
        uint64_t u64M;
    };
    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_UINT(struct U, u8n, 1)
    CRA_TYPE_META_MEMBER_UINT(struct U, u8m, 3)
    CRA_TYPE_META_MEMBER_UINT(struct U, u8M, 4)

    CRA_TYPE_META_MEMBER_UINT(struct U, u16n, 5)
    CRA_TYPE_META_MEMBER_UINT(struct U, u16m, 7)
    CRA_TYPE_META_MEMBER_UINT(struct U, u16M, 8)

    CRA_TYPE_META_MEMBER_UINT(struct U, u32n, 9)
    CRA_TYPE_META_MEMBER_UINT(struct U, u32m, 11)
    CRA_TYPE_META_MEMBER_UINT(struct U, u32M, 12)

    CRA_TYPE_META_MEMBER_UINT(struct U, u64n, 13)
    CRA_TYPE_META_MEMBER_UINT(struct U, u64m, 15)
    CRA_TYPE_META_MEMBER_UINT(struct U, u64M, 16)
    CRA_TYPE_META_END();

    unsigned char buffer[1024];
    size_t        length;

    struct U out;
    struct U in = {
        8,  0, UINT8_MAX,

        16, 0, UINT16_MAX,

        32, 0, UINT32_MAX,

        64, 0, UINT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
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
    length = 70;
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[6] = ((unsigned char)CRA_TYPE_INT) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // size mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[6] = 52;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
}

void
test_varint(void)
{
    struct I
    {
        int8_t i8n1;
        int8_t i8n2;
        int8_t i8m;
        int8_t i8M;

        int16_t i16n1;
        int16_t i16n2;
        int16_t i16m;
        int16_t i16M;

        int32_t i32n1;
        int32_t i32n2;
        int32_t i32m;
        int32_t i32M;

        int64_t i64n1;
        int64_t i64n2;
        int64_t i64m;
        int64_t i64M;
    };
    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i8n1, 1)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i8n2, 2)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i8m, 3)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i8M, 4)

    CRA_TYPE_META_MEMBER_VARINT(struct I, i16n1, 5)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i16n2, 6)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i16m, 7)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i16M, 8)

    CRA_TYPE_META_MEMBER_VARINT(struct I, i32n1, 9)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i32n2, 10)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i32m, 11)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i32M, 12)

    CRA_TYPE_META_MEMBER_VARINT(struct I, i64n1, 13)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i64n2, 14)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i64m, 15)
    CRA_TYPE_META_MEMBER_VARINT(struct I, i64M, 16)
    CRA_TYPE_META_END();

    unsigned char buffer[1024];
    size_t        length;

    struct I out;
    struct I in = {
        -8,  8,  INT8_MIN,  INT8_MAX,

        -16, 16, INT16_MIN, INT16_MAX,

        -32, 32, INT32_MIN, INT32_MAX,

        -64, 64, INT64_MIN, INT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
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
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[6] = ((unsigned char)CRA_TYPE_FLOAT) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
}

void
test_varuint(void)
{
    struct U
    {
        uint8_t u8n;
        uint8_t u8m;
        uint8_t u8M;

        uint16_t u16n;
        uint16_t u16m;
        uint16_t u16M;

        uint32_t u32n;
        uint32_t u32m;
        uint32_t u32M;

        uint64_t u64n;
        uint64_t u64m;
        uint64_t u64M;
    };
    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_VARUINT(struct U, u8n, 1)
    CRA_TYPE_META_MEMBER_VARUINT(struct U, u8m, 3)
    CRA_TYPE_META_MEMBER_VARUINT(struct U, u8M, 4)

    CRA_TYPE_META_MEMBER_VARUINT(struct U, u16n, 5)
    CRA_TYPE_META_MEMBER_VARUINT(struct U, u16m, 7)
    CRA_TYPE_META_MEMBER_VARUINT(struct U, u16M, 8)

    CRA_TYPE_META_MEMBER_VARUINT(struct U, u32n, 9)
    CRA_TYPE_META_MEMBER_VARUINT(struct U, u32m, 11)
    CRA_TYPE_META_MEMBER_VARUINT(struct U, u32M, 12)

    CRA_TYPE_META_MEMBER_VARUINT(struct U, u64n, 13)
    CRA_TYPE_META_MEMBER_VARUINT(struct U, u64m, 15)
    CRA_TYPE_META_MEMBER_VARUINT(struct U, u64M, 16)
    CRA_TYPE_META_END();

    unsigned char buffer[1024];
    size_t        length;

    struct U out;
    struct U in = {
        8,  0, UINT8_MAX,

        16, 0, UINT16_MAX,

        32, 0, UINT32_MAX,

        64, 0, UINT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
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
    length = 50;
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[6] = ((unsigned char)CRA_TYPE_VARINT) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
}

void
test_float(void)
{
    struct F
    {
        float fn;
        float fm;
        float fM;
        float fW;

        double dn;
        double dm;
        double dM;
        double dW;
    };
    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_FLOAT(struct F, fn, 1)
    CRA_TYPE_META_MEMBER_FLOAT(struct F, fm, 2)
    CRA_TYPE_META_MEMBER_FLOAT(struct F, fM, 3)
    CRA_TYPE_META_MEMBER_FLOAT(struct F, fW, 4)
    CRA_TYPE_META_MEMBER_FLOAT(struct F, dn, 5)
    CRA_TYPE_META_MEMBER_FLOAT(struct F, dm, 6)
    CRA_TYPE_META_MEMBER_FLOAT(struct F, dM, 7)
    CRA_TYPE_META_MEMBER_FLOAT(struct F, dW, 8)
    CRA_TYPE_META_END();

    unsigned char buffer[1024];
    size_t        length;

    struct F out, in = {
        -1.5f, FLT_MIN, FLT_MAX, -FLT_MAX,

        64.83, DBL_MIN, DBL_MAX, -DBL_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
    assert_always(cra_compare_float(in.fn, out.fn) == 0);
    assert_always(cra_compare_float(in.fm, out.fm) == 0);
    assert_always(cra_compare_float(in.fM, out.fM) == 0);
    assert_always(cra_compare_float(in.fW, out.fW) == 0);
    assert_always(cra_compare_double(in.dn, out.dn) == 0);
    assert_always(cra_compare_double(in.dm, out.dm) == 0);
    assert_always(cra_compare_double(in.dM, out.dM) == 0);
    assert_always(cra_compare_double(in.dW, out.dW) == 0);

    // test error

    // small buffer
    length = 50;
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[9] = ((unsigned char)CRA_TYPE_BOOL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // size mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[15] = 104;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
}

void
test_string(void)
{
    struct S
    {
        char *strp;
        char  stra[16];
        char *strnull;
        char *strpempty;
        char  straempty[32];
    };
    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_STRING(struct S, strp, 1, true)
    CRA_TYPE_META_MEMBER_STRING(struct S, stra, 2, false)
    CRA_TYPE_META_MEMBER_STRING(struct S, strnull, 3, true)
    CRA_TYPE_META_MEMBER_STRING(struct S, strpempty, 4, true)
    CRA_TYPE_META_MEMBER_STRING(struct S, straempty, 5, false)
    CRA_TYPE_META_END();

    unsigned char buffer[1024];
    size_t        length;

    struct S out, in = { "hello pointer", "hello array", NULL, "", "" };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
    assert_always(strcmp(in.strp, out.strp) == 0);
    assert_always(strcmp(in.stra, out.stra) == 0);
    assert_always(in.strnull == out.strnull);
    assert_always(strcmp(in.strpempty, out.strpempty) == 0);
    assert_always(strcmp(in.straempty, out.straempty) == 0);
    cra_free(out.strp);
    cra_free(out.strpempty);

    // test error

    // small buffer
    length = 13;
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // char[N] cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[19] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[19] = ((unsigned char)CRA_TYPE_BOOL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // char array too small
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    ((CraTypeMeta *)meta + 1)->size = 11;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
}

void
test_bytes(void)
{
    struct B
    {
        char *bytp;
        char  byta[16];
        char *bytnull;
        char *bytpempty;
        char  bytaempty[32];

        uint32_t nbytp;
        uint32_t nbyta;
        uint32_t nbytnull;
        uint32_t nbytpempty;
        uint32_t nbytaempty;
    };
    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_BYTES(struct B, bytp, 1, true)
    CRA_TYPE_META_MEMBER_BYTES(struct B, byta, 2, false)
    CRA_TYPE_META_MEMBER_BYTES(struct B, bytnull, 3, true)
    CRA_TYPE_META_MEMBER_BYTES(struct B, bytpempty, 4, true)
    CRA_TYPE_META_MEMBER_BYTES(struct B, bytaempty, 5, false)
    CRA_TYPE_META_END();

    unsigned char buffer[1024];
    size_t        length;

    struct B out, in = {
        "hello pointer",
        "hello array",
        NULL,
        "",
        "",

        sizeof("hello pointer") - 1,
        sizeof("hello array") - 1,
        0,
        0,
        0,
    };

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
    assert_always(in.nbytp == out.nbytp);
    assert_always(strncmp(in.bytp, out.bytp, out.nbytp) == 0);
    assert_always(in.nbyta == out.nbyta);
    assert_always(strncmp(in.byta, out.byta, out.nbyta) == 0);
    assert_always(in.nbytnull == out.nbytnull);
    assert_always(in.bytnull == out.bytnull);
    assert_always(in.nbytpempty == out.nbytpempty);
    assert_always(strncmp(in.bytpempty, out.bytpempty, out.nbytpempty) == 0);
    assert_always(in.nbytaempty == out.nbytaempty);
    assert_always(strncmp(in.bytaempty, out.bytaempty, out.nbytaempty) == 0);
    cra_free(out.bytp);
    cra_free(out.bytpempty);

    // test error

    // small buffer
    length = 13;
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // char[N] cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[19] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[19] = ((unsigned char)CRA_TYPE_STRING) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // char array too small
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    ((CraTypeMeta *)meta + 2)->size = 10;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
}

void
test_struct(void)
{
    struct S
    {
        int32_t i;
    };
    CRA_TYPE_META_BEGIN(smeta)
    CRA_TYPE_META_MEMBER_INT(struct S, i, 1)
    CRA_TYPE_META_END();
    struct SS
    {
        struct S *strup;
        struct S  strus;
        struct S *strunull;
    };
    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_STRUCT(struct SS, strup, 1, true, smeta, NULL, NULL)
    CRA_TYPE_META_MEMBER_STRUCT(struct SS, strus, 2, false, smeta, NULL, NULL)
    CRA_TYPE_META_MEMBER_STRUCT(struct SS, strunull, 3, true, smeta, NULL, NULL)
    CRA_TYPE_META_END();

    unsigned char buffer[1024];
    size_t        length;

    struct S   s = { .i = 1000 };
    struct SS  out, in = { .strup = &s, .strus = { .i = 2000 }, .strunull = NULL };
    struct SS *po = NULL;

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(po, true, meta, NULL, NULL)));
    assert_always(po);
    assert_always(!po->strunull);
    assert_always(po->strup);
    assert_always(in.strup->i == po->strup->i);
    assert_always(in.strus.i == po->strus.i);
    assert_always(in.strunull == po->strunull);
    cra_free(po->strup);
    cra_free(po);

    // test error

    // small buffer
    length = 13;
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[12] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_STRING) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
}

#if 1 // test struct id

struct O
{
    uint32_t u;
    float    f;
};
struct N
{
    uint32_t  u;
    int32_t   i;
    float     f;
    double    d;
    bool      b;
    char     *n; // null
    uint64_t  v; // varuint
    char     *s; // string
    char     *t; // btyes
    uint32_t  nt;
    struct O *o;    // struct
    struct O  a[5]; // array
    uint32_t  na;
    CraLList *l; // list<int32_t>
    CraDict   c; // dict<int32_t, int32_t>
};

CRA_TYPE_META_BEGIN(metao)
CRA_TYPE_META_MEMBER_UINT(struct O, u, 1)
CRA_TYPE_META_MEMBER_FLOAT(struct O, f, 2)
CRA_TYPE_META_END();

CRA_TYPE_META_BEGIN(metaa)
CRA_TYPE_META_ELEMENT_STRUCT(struct O, false, metao, NULL, NULL)
CRA_TYPE_META_END();

CRA_TYPE_META_BEGIN(metal)
CRA_TYPE_META_ELEMENT_INT(int32_t)
CRA_TYPE_META_END();

CRA_TYPE_META_BEGIN(metac)
CRA_TYPE_META_ELEMENT_INT(int32_t)
CRA_TYPE_META_ELEMENT_INT(int32_t)
CRA_TYPE_META_END();

CRA_TYPE_META_BEGIN(metan)
CRA_TYPE_META_MEMBER_INT(struct N, i, 3)
// old
CRA_TYPE_META_MEMBER_FLOAT(struct N, f, 2)
// new
CRA_TYPE_META_MEMBER_FLOAT(struct N, d, 4)
CRA_TYPE_META_MEMBER_BOOL(struct N, b, 5)
CRA_TYPE_META_MEMBER_STRING(struct N, n, 6, true) // null
CRA_TYPE_META_MEMBER_VARUINT(struct N, v, 7)
CRA_TYPE_META_MEMBER_STRING(struct N, s, 8, true)
CRA_TYPE_META_MEMBER_BYTES(struct N, t, 9, true)
CRA_TYPE_META_MEMBER_STRUCT(struct N, o, 10, true, metao, NULL, NULL)
CRA_TYPE_META_MEMBER_ARRAY(struct N, a, 11, false, metaa)
// CRA_TYPE_META_MEMBER_LIST(struct N, l, 12, true, metal, CRA_LLIST_SZER_I, NULL)
// CRA_TYPE_META_MEMBER_DICT(struct N, c, 13, false, metac, CRA_DICT_SZER_I, &cargs)
CRA_TYPE_META_MEMBER_LIST(struct N, l, 12, true, metal, NULL, NULL)
CRA_TYPE_META_MEMBER_DICT(struct N, c, 13, false, metac, NULL, NULL)
// old
CRA_TYPE_META_MEMBER_UINT(struct N, u, 1)
CRA_TYPE_META_END();

static void
init_on(void *obj, CraInitArgs *da)
{
    if (da->size == sizeof(struct O))
    {
        struct O *o = (struct O *)obj;
        o->u = 200;
        o->f = 1.0f;
    }
    else
    {
        struct N *n = (struct N *)obj;
        n->u = 300;
        n->i = -48;
        n->f = 2.5f;
        n->d = 88.9;
        n->b = false;
        n->n = NULL;
        n->v = 0;
        n->s = NULL;
        n->t = NULL;
        n->nt = 0;
        n->o = NULL;
        bzero(&n->a, sizeof(n->a));
        n->na = 0;
        n->l = NULL;
        bzero(&n->c, sizeof(n->c));
    }
}

// static void
// uninit_on(void *obj)
// {
//     CRA_UNUSED_VALUE(obj);
// }

const CraInitializable_i oninit_i = { .init = init_on, .uninit = NULL /*uninit_on*/ };

void
test_struct_id(void)
{
    unsigned char buffer[1024];
    size_t        length;

    CraDictSerArgs cargs = {
        .hash = (cra_hash_fn)cra_hash_int32_t_p,
        .compare = (cra_compare_fn)cra_compare_int32_t_p,
    };
    metan[12].szer_i = CRA_LLIST_SZER_I;
    metan[13].szer_i = CRA_DICT_SZER_I;
    metan[13].arg = &cargs;

    struct O outo, ino = { .u = 4000, .f = 6.7f };
    struct N outn, inn = { .u = 300,
                           .i = -30,
                           .f = 7.7f,
                           .d = 1002.3,
                           .b = true,
                           .n = NULL,
                           .v = 8000000,
                           .s = "string",
                           .t = "bytes",
                           .nt = 5,
                           .o = &ino,
                           .a[0] = { .f = 0.5f, .u = 100 },
                           .a[1] = { .f = 3.6f, .u = 300 },
                           .na = 2,
                           .l = cra_alloc(CraLList) };

    cra_llist_init0(int32_t, inn.l, false);
    cra_dict_init0(int32_t, int32_t, &inn.c, false, cargs.hash, cargs.compare);
    for (int32_t i = 0; i < 20; i++)
    {
        cra_llist_append(inn.l, &i);
        cra_dict_add(&inn.c, &i, &i);
    }

    // o => n
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(ino, false, metao, &oninit_i, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(outn, false, metan, &oninit_i, NULL)));
    assert_always(ino.u == outn.u);
    assert_always(-48 == outn.i);
    assert_always(cra_compare_float(ino.f, outn.f) == 0);
    assert_always(cra_compare_double(88.9, outn.d) == 0);
    assert_always(false == outn.b);
    assert_always(NULL == outn.n);
    assert_always(0 == outn.v);
    assert_always(NULL == outn.s);
    assert_always(NULL == outn.t);
    assert_always(0 == outn.nt);
    assert_always(NULL == outn.o);
    assert_always(0 == outn.na);
    assert_always(memcmp(&(struct O[CRA_NARRAY(outn.a)]){ 0 }, &outn.a, sizeof(struct O[CRA_NARRAY(outn.a)])) == 0);
    assert_always(NULL == outn.l);
    assert_always(memcmp(&(CraDict){ 0 }, &outn.c, sizeof(CraDict)) == 0);

    // n => o
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(inn, false, metan, &oninit_i, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(outo, false, metao, &oninit_i, NULL)));
    assert_always(inn.u == outo.u);
    assert_always(cra_compare_float(inn.f, outo.f) == 0);

    cra_dict_uninit(&inn.c);
    cra_llist_uninit(inn.l);
    cra_dealloc(inn.l);
}

#endif

void
test_array(void)
{
    struct A
    {
        int32_t   arraya[100];
        uint64_t *arrayp;
        char      (*arrsa)[20];
        char    **arrsp;
        float     arrayempty[10];
        float    *arrnull;

        uint16_t narraya;
        uint16_t narrayp;
        uint32_t narrsa;
        uint32_t narrsp;
        uint32_t narrayempty;
        uint32_t narrnull;
    };

    CRA_TYPE_META_BEGIN(metai32)
    CRA_TYPE_META_ELEMENT_INT(int32_t)
    CRA_TYPE_META_END();
    CRA_TYPE_META_BEGIN(metau64)
    CRA_TYPE_META_ELEMENT_UINT(uint64_t)
    CRA_TYPE_META_END();
    CRA_TYPE_META_BEGIN(metasa)
    CRA_TYPE_META_ELEMENT_STRING(char[20], false)
    CRA_TYPE_META_END();
    CRA_TYPE_META_BEGIN(metasp)
    CRA_TYPE_META_ELEMENT_STRING(char *, true)
    CRA_TYPE_META_END();
    CRA_TYPE_META_BEGIN(metaf32)
    CRA_TYPE_META_ELEMENT_FLOAT(float)
    CRA_TYPE_META_END();

    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_ARRAY(struct A, arraya, 1, false, metai32)
    CRA_TYPE_META_MEMBER_ARRAY(struct A, arrayp, 2, true, metau64)
    CRA_TYPE_META_MEMBER_ARRAY(struct A, arrsa, 3, true, metasa)
    CRA_TYPE_META_MEMBER_ARRAY(struct A, arrsp, 4, true, metasp)
    CRA_TYPE_META_MEMBER_ARRAY(struct A, arrayempty, 5, false, metaf32)
    CRA_TYPE_META_MEMBER_ARRAY(struct A, arrnull, 6, true, metaf32)
    CRA_TYPE_META_END();

    unsigned char buffer[8192];
    size_t        length;

    struct A out, in;

    in.arrayp = (uint64_t *)cra_calloc(50, sizeof(uint64_t));
    in.arrsa = (char (*)[20])cra_calloc(100, sizeof(in.arrsa[0]));
    in.arrsp = (char **)cra_calloc(50, sizeof(char *));
    in.arrnull = NULL;
    for (int i = 0; i < 100; i++)
    {
        in.arraya[i] = 24 + i;
        snprintf(in.arrsa[i], sizeof(in.arrsa[i]), "hello %d.", i);
    }
    for (int i = 0; i < 50; i++)
    {
        in.arrayp[i] = (uint64_t)(3.5 * (double)i);
        in.arrsp[i] = cra_malloc(30);
        snprintf(in.arrsp[i], 30, "good %d.", i);
    }
    in.narraya = 100;
    in.narrsa = 100;
    in.narrayp = 50;
    in.narrsp = 50;
    in.narrayempty = 0;
    in.narrnull = 0;

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
    assert_always(in.narraya == out.narraya);
    assert_always(in.narrayp == out.narrayp);
    assert_always(in.narrsa == out.narrsa);
    assert_always(in.narrsp == out.narrsp);
    assert_always(in.arrnull == out.arrnull);
    assert_always(in.narrnull == out.narrnull);
    for (int i = 0; i < 100; i++)
    {
        assert_always(in.arraya[i] == out.arraya[i]);
        assert_always(strcmp(in.arrsa[i], out.arrsa[i]) == 0);
    }
    for (int i = 0; i < 50; i++)
    {
        assert_always(in.arrayp[i] == out.arrayp[i]);
        assert_always(strcmp(in.arrsp[i], out.arrsp[i]) == 0);
        cra_free(out.arrsp[i]);
    }
    cra_free(out.arrayp);
    cra_free(out.arrsa);
    cra_free(out.arrsp);

    // test error

    // small buffer
    length = 1000;
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_STRING) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // array too small
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    ((CraTypeMeta *)meta)->size = 396;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    for (int i = 0; i < 50; i++)
    {
        cra_free(in.arrsp[i]);
    }
    cra_free(in.arrayp);
    cra_free(in.arrsa);
    cra_free(in.arrsp);
}

void
test_list(void)
{
    struct L
    {
        CraAList  alist;      // List<int32_t>
        CraLList *llist;      // List<char[20]>
        CraDeque *deque;      // List<char *>
        CraAList *alistempty; // List<int32_t>
        CraLList  llistempty; // List<char[20]>
        CraDeque  dequeempty; // List<char *>
        CraAList *alistnull;
        CraLList *llistnull;
        CraDeque *dequenull;
    };

    CRA_TYPE_META_BEGIN(metai32)
    CRA_TYPE_META_ELEMENT_INT(int32_t)
    CRA_TYPE_META_END();
    CRA_TYPE_META_BEGIN(metasa)
    CRA_TYPE_META_ELEMENT_STRING(char[20], false)
    CRA_TYPE_META_END();
    CRA_TYPE_META_BEGIN(metasp)
    CRA_TYPE_META_ELEMENT_STRING(char *, true)
    CRA_TYPE_META_END();

    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_LIST(struct L, alist, 1, false, metai32, CRA_ALIST_SZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct L, llist, 2, true, metasa, CRA_LLIST_SZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct L, deque, 3, true, metasp, CRA_DEQUE_SZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct L, alistempty, 4, true, metai32, CRA_ALIST_SZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct L, llistempty, 5, false, metasa, CRA_LLIST_SZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct L, dequeempty, 6, false, metasp, CRA_DEQUE_SZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct L, alistnull, 7, true, metai32, CRA_ALIST_SZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct L, llistnull, 8, true, metasa, CRA_LLIST_SZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct L, dequenull, 9, true, metasp, CRA_DEQUE_SZER_I, NULL)
    CRA_TYPE_META_END();

    unsigned char buffer[8192];
    size_t        length;

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
    cra_alist_init0(int32_t, &in.alist, false);
    cra_llist_init0(char[20], in.llist, false);
    cra_deque_init0(char *, in.deque, CRA_DEQUE_INFINITE, false);
    cra_alist_init0(int32_t, in.alistempty, false);
    cra_llist_init0(char[20], &in.llistempty, false);
    cra_deque_init0(char *, &in.dequeempty, CRA_DEQUE_INFINITE, false);
    for (int i = 0; i < 100; i++)
    {
        cra_alist_append(&in.alist, &i);
        cra_llist_append(in.llist, &s20);
        cra_deque_push(in.deque, &sp);
    }

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
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
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_STRING) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

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

#if 1 // test array list

static size_t
get_array_count(void *narray)
{
    return (size_t)*(uint32_t *)narray;
}

static bool
get_array_val(void *array, size_t index, void *retval)
{
    int32_t *arr = (int32_t *)array;
    *(int32_t *)retval = arr[index];
    return true;
}

void
test_array_list(void)
{
    CRA_TYPE_META_BEGIN(metai32)
    CRA_TYPE_META_ELEMENT_INT(int32_t)
    CRA_TYPE_META_END();

    int32_t  inarray[30], outarray[30];
    uint32_t ninarray, noutarray;
    CraAList inalist, outalist;
    CraLList inllist, outllist;
    CraDeque indeque, outdeque;

    ninarray = 30;
    cra_alist_init0(int32_t, &inalist, false);
    cra_llist_init0(int32_t, &inllist, false);
    cra_deque_init0(int32_t, &indeque, CRA_DEQUE_INFINITE, false);
    for (int32_t i = 0; i < 30; i++)
    {
        inarray[i] = i;
        if (i % 2 == 0)
            cra_alist_append(&inalist, &(int32_t){ i + 4 });
        if (i % 2 != 0)
            cra_llist_append(&inllist, &(int32_t){ i * 2 });
        if (i < 18)
            cra_deque_push(&indeque, &(int32_t){ i * 8 });
    }

    CraSeriObject *ins[] = {
        CRA_SERI_ARRAY(inarray, false, ninarray, metai32),
        CRA_SERI_LIST(inalist, false, metai32, CRA_ALIST_SZER_I, NULL),
        CRA_SERI_LIST(inllist, false, metai32, CRA_LLIST_SZER_I, NULL),
        CRA_SERI_LIST(indeque, false, metai32, CRA_DEQUE_SZER_I, NULL),
    };
    CraSeriObject *outs[] = {
        CRA_SERI_ARRAY(outarray, false, noutarray, metai32),
        CRA_SERI_LIST(outalist, false, metai32, CRA_ALIST_SZER_I, NULL),
        CRA_SERI_LIST(outllist, false, metai32, CRA_LLIST_SZER_I, NULL),
        CRA_SERI_LIST(outdeque, false, metai32, CRA_DEQUE_SZER_I, NULL),
    };
    size_t (*getcntfns[])(void *) = {
        get_array_count,
        (size_t (*)(void *))cra_alist_get_count,
        (size_t (*)(void *))cra_llist_get_count,
        (size_t (*)(void *))cra_deque_get_count,
    };
    bool (*getvalfns[])(void *, size_t, void *) = {
        get_array_val,
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
        assert_always(cra_bin_serialize(buffer, &length, ins[i]));
        for (size_t j = 0; j < CRA_NARRAY(outs); j++)
        {
            assert_always(cra_bin_deserialize(buffer, length, outs[j]));
            n1 = getcntfns[i](i == 0 ? ins[i]->meta[1].arg : ins[i]->objptr);
            n2 = getcntfns[j](j == 0 ? outs[j]->meta[1].arg : outs[j]->objptr);
            assert_always(n1 == n2);
            for (size_t k = 0; k < n1; k++)
            {
                getvalfns[i](ins[i]->objptr, k, &v1);
                getvalfns[j](outs[j]->objptr, k, &v2);
                assert_always(v1 == v2);
            }
            if (uninitfns[j])
                uninitfns[j](outs[j]->objptr);
        }
        if (uninitfns[i])
            uninitfns[i](ins[i]->objptr);
    }
}

#endif

void
test_dict(void)
{
    struct D
    {
        CraDict  ds;     // Dict<int32_t, float>
        CraDict *dp;     // Dict<double, uint8_t>
        CraDict *dempty; // Dict<int32_t, float>
        CraDict *dnull;
    };

    CRA_TYPE_META_BEGIN(metaif)
    CRA_TYPE_META_ELEMENT_INT(int32_t) // key
    CRA_TYPE_META_ELEMENT_FLOAT(float) // val
    CRA_TYPE_META_END();
    CRA_TYPE_META_BEGIN(metadu)
    CRA_TYPE_META_ELEMENT_FLOAT(double) // key
    CRA_TYPE_META_ELEMENT_INT(uint8_t)  // val
    CRA_TYPE_META_END();

    CraDictSerArgs daif = {
        .hash = (cra_hash_fn)cra_hash_int32_t_p,
        .compare = (cra_compare_fn)cra_compare_int32_t_p,
    };
    CraDictSerArgs dadu = {
        .hash = (cra_hash_fn)cra_hash_double_p,
        .compare = (cra_compare_fn)cra_compare_double_p,
    };

    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_DICT(struct D, ds, 1, false, metaif, CRA_DICT_SZER_I, &daif)
    CRA_TYPE_META_MEMBER_DICT(struct D, dp, 2, true, metadu, CRA_DICT_SZER_I, &dadu)
    CRA_TYPE_META_MEMBER_DICT(struct D, dempty, 3, true, metaif, CRA_DICT_SZER_I, &daif)
    CRA_TYPE_META_MEMBER_DICT(struct D, dnull, 3, true, metadu, CRA_DICT_SZER_I, &dadu)
    CRA_TYPE_META_END();

    unsigned char buffer[2048];
    size_t        length;

    int32_t     i1, *pi;
    float       f1, *pf;
    double      d1, *pd;
    uint8_t     u1, *pu;
    struct D    out, in;
    CraDictIter it;

    in.dnull = NULL;
    in.dp = cra_alloc(CraDict);
    in.dempty = cra_alloc(CraDict);
    cra_dict_init0(int32_t, float, &in.ds, false, daif.hash, daif.compare);
    cra_dict_init0(double, uint8_t, in.dp, false, dadu.hash, dadu.compare);
    cra_dict_init0(int32_t, float, in.dempty, false, daif.hash, daif.compare);
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
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
    assert_always(in.dnull == out.dnull);
    assert_always(in.ds.count == out.ds.count);
    assert_always(in.dp->count == out.dp->count);
    assert_always(in.dempty->count == out.dempty->count);
    for (cra_dict_iter_init(&in.ds, &it); cra_dict_iter_next(&it, (void **)&pi, (void **)&pf);)
    {
        cra_dict_get(&out.ds, pi, &f1);
        assert_always(cra_compare_float(f1, *pf) == 0);
    }
    for (cra_dict_iter_init(in.dp, &it); cra_dict_iter_next(&it, (void **)&pd, (void **)&pu);)
    {
        cra_dict_get(out.dp, pd, &u1);
        assert_always(u1 == *pu);
    }
    cra_dict_uninit(&out.ds);
    cra_dict_uninit(out.dp);
    cra_dict_uninit(out.dempty);
    cra_dealloc(out.dp);
    cra_dealloc(out.dempty);

    // test error

    // small buffer
    length = 1000;
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    buffer[3] = ((unsigned char)CRA_TYPE_STRING) << 4;
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    // invalid key type
    length = sizeof(buffer);
    ((CraTypeMeta *)metadu)->type = CRA_TYPE_BYTES;
    assert_always(!cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    cra_dict_uninit(&in.ds);
    cra_dict_uninit(in.dp);
    cra_dict_uninit(in.dempty);
    cra_dealloc(in.dp);
    cra_dealloc(in.dempty);
}

#if 1 // test deserialize failed
struct A
{
    char *str;
};
CRA_TYPE_META_BEGIN(metasa)
CRA_TYPE_META_MEMBER_STRING(struct A, str, 1, true)
CRA_TYPE_META_END();

void
a_uninit(void *obj, bool dont_free_ptr)
{
    // 只有在反序列化失败时才会调用
    // 并且不要在这里free
    struct A *a = (struct A *)obj;
    if (!dont_free_ptr && a->str)
        cra_free(a->str); // don't call it
}

CraInitializable_i a_init_i = { .init = NULL, .uninit = a_uninit };

void
test_deserialize_failed(void)
{
    CRA_TYPE_META_BEGIN(metastr_elem)
    CRA_TYPE_META_ELEMENT_STRING(char *, true)
    CRA_TYPE_META_END();

    CRA_TYPE_META_BEGIN(metasa_elem)
    CRA_TYPE_META_ELEMENT_STRUCT(struct A, true, metasa, &a_init_i, NULL)
    CRA_TYPE_META_END();

    CRA_TYPE_META_BEGIN(metasa_kv)
    CRA_TYPE_META_ELEMENT_STRING(char *, true)
    CRA_TYPE_META_ELEMENT_STRUCT(struct A, true, metasa, &a_init_i, NULL)
    CRA_TYPE_META_END();

    CraDictSerArgs dict_arg = {
        .hash = (cra_hash_fn)cra_hash_string1_p,
        .compare = (cra_compare_fn)cra_compare_string_p,
    };

    struct B
    {
        struct A *stru;
        char    **array;
        uint32_t  narray;
        CraAList *alist; // List<struct A *>
        CraLList *llist; // List<struct A *>
        CraDeque *deque; // List<struct A *>
        CraDict  *dict;  // Dict<char *, struct A *>
    };
    CRA_TYPE_META_BEGIN(meta)
    CRA_TYPE_META_MEMBER_STRUCT(struct B, stru, 1, true, metasa, &a_init_i, NULL)
    CRA_TYPE_META_MEMBER_ARRAY(struct B, array, 2, true, metastr_elem)
    CRA_TYPE_META_MEMBER_LIST(struct B, alist, 3, true, metasa_elem, CRA_ALIST_SZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct B, llist, 4, true, metasa_elem, CRA_LLIST_SZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct B, deque, 5, true, metasa_elem, CRA_DEQUE_SZER_I, NULL)
    CRA_TYPE_META_MEMBER_DICT(struct B, dict, 6, true, metasa_kv, CRA_DICT_SZER_I, &dict_arg)
    CRA_TYPE_META_END();

    struct B in, out;

    in.stru = cra_alloc(struct A);
    in.array = (char **)cra_calloc(3, sizeof(char *));
    in.alist = cra_alloc(CraAList);
    in.llist = cra_alloc(CraLList);
    in.deque = cra_alloc(CraDeque);
    in.dict = cra_alloc(CraDict);
    in.narray = 3;
    cra_alist_init0(struct A *, in.alist, false);
    cra_llist_init0(struct A *, in.llist, false);
    cra_deque_init0(struct A *, in.deque, CRA_DEQUE_INFINITE, false);
    cra_dict_init0(char *, struct A *, in.dict, false, dict_arg.hash, dict_arg.compare);

    char     *str;
    struct A *a;

    in.stru->str = (char *)cra_malloc(20);
    memcpy(in.stru->str, "struct.", sizeof("struct."));

    for (int i = 0; i < (int)in.narray; i++)
    {
        str = (char *)cra_malloc(20);
        snprintf(str, 20, "array %d.", i);
        in.array[i] = str;

        a = cra_alloc(struct A);
        a->str = (char *)cra_malloc(20);
        snprintf(a->str, 20, "alist  %d.", i);
        cra_alist_append(in.alist, &a);

        a = cra_alloc(struct A);
        a->str = (char *)cra_malloc(20);
        snprintf(a->str, 20, "llist   %d.", i);
        cra_llist_append(in.llist, &a);

        a = cra_alloc(struct A);
        a->str = (char *)cra_malloc(30);
        snprintf(a->str, 30, "deque    %d.", i);
        cra_deque_push(in.deque, &a);

        str = (char *)cra_malloc(30);
        a = cra_alloc(struct A);
        a->str = (char *)cra_malloc(30);
        snprintf(str, 30, "dict key     %d.", i);
        snprintf(a->str, 30, "dict val %d.", i);
        cra_dict_add(in.dict, &str, &a);
    }

    unsigned char buffer[8192];
    size_t        length;

    length = sizeof(buffer);
    assert_always(cra_bin_serialize(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL)));
    assert_always(!cra_bin_deserialize(buffer, length - 1, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));
    assert_always(cra_bin_deserialize(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL)));

    assert_always(out.stru && out.stru->str);
    assert_always(strcmp(in.stru->str, out.stru->str) == 0);
    assert_always(in.narray == out.narray);
    assert_always(in.alist->count == out.alist->count);
    assert_always(in.llist->count == out.llist->count);
    assert_always(in.deque->count == out.deque->count);
    assert_always(in.dict->count == out.dict->count);

    struct A *val1, *val2;
    for (int i = 0; i < (int)in.narray; i++)
    {
        assert_always(strcmp(in.array[i], out.array[i]) == 0);
        cra_free(in.array[i]);
        cra_free(out.array[i]);

        cra_alist_get(in.alist, i, &val1);
        cra_alist_get(out.alist, i, &val2);
        assert_always(strcmp(val1->str, val2->str) == 0);
        cra_free(val1->str);
        cra_free(val2->str);
        cra_free(val1);
        cra_free(val2);

        cra_llist_get(in.llist, i, &val1);
        cra_llist_get(out.llist, i, &val2);
        assert_always(strcmp(val1->str, val2->str) == 0);
        cra_free(val1->str);
        cra_free(val2->str);
        cra_free(val1);
        cra_free(val2);

        cra_deque_get(in.deque, i, &val1);
        cra_deque_get(out.deque, i, &val2);
        assert_always(strcmp(val1->str, val2->str) == 0);
        cra_free(val1->str);
        cra_free(val2->str);
        cra_free(val1);
        cra_free(val2);
    }

    char       *key;
    char      **pkey;
    struct A  **pval;
    CraDictIter it;
    for (cra_dict_iter_init(in.dict, &it); cra_dict_iter_next(&it, (void **)&pkey, (void **)&pval);)
    {
        cra_dict_pop(out.dict, pkey, &key, &val1);
        assert_always(strcmp(val1->str, (*pval)->str) == 0);
        cra_free(*pkey);
        cra_free(key);
        cra_free((*pval)->str);
        cra_free(val1->str);
        cra_free(*pval);
        cra_free(val1);
    }
    cra_free(in.stru->str);
    cra_free(out.stru->str);
    cra_free(in.stru);
    cra_free(out.stru);
    cra_free(in.array);
    cra_free(out.array);
    cra_alist_uninit(in.alist);
    cra_alist_uninit(out.alist);
    cra_llist_uninit(in.llist);
    cra_llist_uninit(out.llist);
    cra_deque_uninit(in.deque);
    cra_deque_uninit(out.deque);
    cra_dict_uninit(in.dict);
    cra_dict_uninit(out.dict);
    cra_dealloc(in.alist);
    cra_dealloc(out.alist);
    cra_dealloc(in.llist);
    cra_dealloc(out.llist);
    cra_dealloc(in.deque);
    cra_dealloc(out.deque);
    cra_dealloc(in.dict);
    cra_dealloc(out.dict);
}

#endif

int
main(void)
{
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

    cra_memory_leak_report();
    return 0;
}
