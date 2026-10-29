/**
 * @file test-json.c
 * @author Cracal
 * @brief test JSON
 * @version 0.1
 * @date 2024-10-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <float.h>
#include "cra_malloc.h"
#include "serialize/cra_json.h"
#include "collections/cra_serializable.h"

#define FORMAT false

void
test_bool(void)
{
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(static, B, NULL, (BOOL, b0, 1), (BOOL, b1, 2));

    CraSerErr err;
    size_t    length;
    char      buffer[32];
    struct B  out, in = { false, true };

    length = sizeof(buffer);
    assert_always(cra_json_stringify(buffer, &length, &in, &B_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &B_meta));
    assert_always(in.b0 == out.b0);
    assert_always(in.b1 == out.b1);

    // test error

    // small buffer
    length = 22;
    assert_always(!cra_json_stringify_err(buffer, &length, &in, &B_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_json_parse_err(buffer, length, &out, &B_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // error false
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &B_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 10 : 7] = 'x'; // false -> fxlse
    assert_always(!cra_json_parse_err(buffer, length, &out, &B_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    printf("%s\n", err.msg);
    // error true
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &B_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 24 : 18] = 'x'; // true -> txue
    assert_always(!cra_json_parse_err(buffer, length, &out, &B_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    printf("%s\n", err.msg);

    // // cannot be null
    // length = sizeof(buffer);
    // assert_always(cra_json_stringify_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(!err.err);
    // buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    // assert_always(!cra_json_parse_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    // printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &B_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 23 : 17] = '\"'; // bool -> string
    assert_always(!cra_json_parse_err(buffer, length, &out, &B_meta, &err));
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

    char      buffer[1024];
    size_t    length;
    CraSerErr err;

    struct I out;
    struct I in = {
        -8,  8,  INT8_MIN,  INT8_MAX,

        -16, 16, INT16_MIN, INT16_MAX,

        -32, 32, INT32_MIN, INT32_MAX,

        -64, 64, INT64_MIN, INT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_json_stringify(buffer, &length, &in, &I_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &I_meta));
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
#ifdef CRA_JSON_SAFE_INT
    assert_always(CRA_MIN_SAFE_INT == out.i64m);
    assert_always(CRA_MAX_SAFE_INT == out.i64M);
#else
    assert_always(in.i64m == out.i64m);
    assert_always(in.i64M == out.i64M);
#endif

    // test error

    // small buffer
    length = 220;
    assert_always(!cra_json_stringify_err(buffer, &length, &in, &I_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_json_parse_err(buffer, length, &out, &I_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // error value
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &I_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 35 : 26), "-.", sizeof("-.") - 1);
    assert_always(!cra_json_parse_err(buffer, length, &out, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    printf("%s\n", err.msg);

    // float -> int
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &I_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 50 : 38] = '.'; // int -> float: 127 -> 1.7
    assert_always(cra_json_parse_err(buffer, length, &out, &I_meta, &err));
    assert_always(out.i8M == 1);
    assert_always(!err.err);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &I_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 226 : 159] = '\"'; // int -> string
    assert_always(!cra_json_parse_err(buffer, length, &out, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // size mismatch
    // length = sizeof(buffer);
    // assert_always(cra_json_stringify(buffer, &length, &in, &I_meta) == CRA_SER_ERR_OK);
    // buffer[9] = 34;
    // assert_always(cra_json_parse(buffer, length, &out, &I_meta) == CRA_SER_ERR_SIZE_MISMATCH);
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

    char      buffer[1024];
    size_t    length;
    CraSerErr err;

    struct U out;
    struct U in = {
        8,  0, UINT8_MAX,

        16, 0, UINT16_MAX,

        32, 0, UINT32_MAX,

        64, 0, UINT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_json_stringify(buffer, &length, &in, &U_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &U_meta));
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
#ifdef CRA_JSON_SAFE_INT
    assert_always(CRA_MAX_SAFE_INT == out.u64M);
#else
    assert_always(in.u64M == out.u64M);
#endif

    // test error

    // small buffer
    length = 138;
    assert_always(!cra_json_stringify_err(buffer, &length, &in, &U_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_json_parse_err(buffer, length, &out, &U_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // error value
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &U_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 32 : 24), "a3", sizeof("a3") - 1);
    assert_always(!cra_json_parse_err(buffer, length, &out, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    printf("%s\n", err.msg);

    // float -> int
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &U_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    // uint -> float: 65536 -> -65.5
    memcpy(buffer + (FORMAT ? 71 : 53), "65.5 ", sizeof("65.5 ") - 1);
    assert_always(cra_json_parse_err(buffer, length, &out, &U_meta, &err));
    assert_always(out.u16M == 65);
    assert_always(!err.err);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &U_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 71 : 44] = '{'; // uint -> object
    assert_always(!cra_json_parse_err(buffer, length, &out, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // // size mismatch
    // length = sizeof(buffer);
    // assert_always(cra_json_stringify_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(!err.err);
    // buffer[6] = 52;
    // assert_always(!cra_json_parse_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
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

    char      buffer[1024];
    size_t    length;
    CraSerErr err;

    struct I out;
    struct I in = {
        -8,  8,  INT8_MIN,  INT8_MAX,

        -16, 16, INT16_MIN, INT16_MAX,

        -32, 32, INT32_MIN, INT32_MAX,

        -64, 64, INT64_MIN, INT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_json_stringify(buffer, &length, &in, &I_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &I_meta));
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
#ifdef CRA_JSON_SAFE_INT
    assert_always(CRA_MIN_SAFE_INT == out.i64m);
    assert_always(CRA_MAX_SAFE_INT == out.i64M);
#else
    assert_always(in.i64m == out.i64m);
    assert_always(in.i64M == out.i64M);
#endif

    // test error

    // small buffer
    length = 220;
    assert_always(!cra_json_stringify_err(buffer, &length, &in, &I_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_json_parse_err(buffer, length, &out, &I_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // error value
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &I_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 35 : 26), "-.", sizeof("-.") - 1);
    assert_always(!cra_json_parse_err(buffer, length, &out, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    printf("%s\n", err.msg);

    // float -> int
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &I_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 50 : 38] = '.'; // int -> float: 127 -> 1.7
    assert_always(cra_json_parse_err(buffer, length, &out, &I_meta, &err));
    assert_always(out.i8M == 1);
    assert_always(!err.err);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &I_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 226 : 159] = '\"'; // int -> string
    assert_always(!cra_json_parse_err(buffer, length, &out, &I_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // size mismatch
    // length = sizeof(buffer);
    // assert_always(cra_json_stringify(buffer, &length, &in, &I_meta) == CRA_SER_ERR_OK);
    // buffer[9] = 34;
    // assert_always(cra_json_parse(buffer, length, &out, &I_meta) == CRA_SER_ERR_SIZE_MISMATCH);
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

    char      buffer[1024];
    size_t    length;
    CraSerErr err;

    struct U out;
    struct U in = {
        8,  0, UINT8_MAX,

        16, 0, UINT16_MAX,

        32, 0, UINT32_MAX,

        64, 0, UINT64_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_json_stringify(buffer, &length, &in, &U_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &U_meta));
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
#ifdef CRA_JSON_SAFE_INT
    assert_always(CRA_MAX_SAFE_INT == out.u64M);
#else
    assert_always(in.u64M == out.u64M);
#endif

    // test error

    // small buffer
    length = 138;
    assert_always(!cra_json_stringify_err(buffer, &length, &in, &U_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_json_parse_err(buffer, length, &out, &U_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // error value
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &U_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 32 : 24), "a3", sizeof("a3") - 1);
    assert_always(!cra_json_parse_err(buffer, length, &out, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    printf("%s\n", err.msg);

    // float -> int
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &U_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    // uint -> float: 65536 -> -65.5
    memcpy(buffer + (FORMAT ? 71 : 53), "65.5 ", sizeof("65.5 ") - 1);
    assert_always(cra_json_parse_err(buffer, length, &out, &U_meta, &err));
    assert_always(out.u16M == 65);
    assert_always(!err.err);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &U_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 71 : 44] = '{'; // uint -> object
    assert_always(!cra_json_parse_err(buffer, length, &out, &U_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // // size mismatch
    // length = sizeof(buffer);
    // assert_always(cra_json_stringify_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(!err.err);
    // buffer[6] = 52;
    // assert_always(!cra_json_parse_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_SIZE_MISMATCH);
    // printf("%s\n", err.msg);
}

void
test_float(void)
{
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(

      static, F, NULL,

      (FLOAT32, fn, 1), (FLOAT32, fm, 2), (FLOAT32, fM, 3), (FLOAT32, fW, 4),

      (FLOAT64, dn, 5), (FLOAT64, dm, 6), (FLOAT64, dM, 7), (FLOAT64, dW, 8)

    );

    char      buffer[1024];
    size_t    length;
    CraSerErr err;

    struct F out, in = {
        -1.5f, FLT_MIN, FLT_MAX, -FLT_MAX,

        64.83, DBL_MIN, DBL_MAX, -DBL_MAX,
    };

    length = sizeof(buffer);
    assert_always(cra_json_stringify(buffer, &length, &in, &F_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &F_meta));
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
    length = 189;
    assert_always(!cra_json_stringify_err(buffer, &length, &in, &F_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_json_parse_err(buffer, length, &out, &F_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // // cannot be null
    // length = sizeof(buffer);
    // assert_always(cra_json_stringify_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(!err.err);
    // buffer[3] = ((unsigned char)CRA_TYPE_NULL) << 4;
    // assert_always(!cra_json_parse_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    // printf("%s\n", err.msg);

    // error value
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &F_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 32 : 16), "a3", sizeof("a3") - 1);
    assert_always(!cra_json_parse_err(buffer, length, &out, &F_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &F_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 22 : 16] = '\"'; // float -> string
    assert_always(!cra_json_parse_err(buffer, length, &out, &F_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);
}

void
test_int_float(void)
{
    CRA_SER_DEF_STRUCT_META_AND_STRUCT(

      static, I, NULL,

      (INT64, n, 1), (INT64, m, 2), (INT64, M, 3), (INT64, W, 4), (UINT64, UM, 5)

    );

    CRA_SER_DEF_STRUCT_META_AND_STRUCT(

      static, F, NULL,

      (FLOAT64, n, 1), (FLOAT64, m, 2), (FLOAT64, M, 3), (FLOAT64, W, 4), (FLOAT64, UM, 5)

    );

    struct I out_i, in_i = { .n = -987, .m = INT64_MIN, .M = INT64_MAX, .W = 20983478, .UM = UINT64_MAX };
    struct F out_f, in_f = { .n = -1.533, .m = DBL_MIN, .M = DBL_MAX, .W = -DBL_MAX, .UM = 0 };

    char   buffer[1024];
    size_t length;

    // i -> f
    length = sizeof(buffer);
    assert_always(cra_json_stringify(buffer, &length, &in_i, &I_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out_f, &F_meta));
    assert_always(cra_cmp_double((double)in_i.n, out_f.n) == 0);
    assert_always(cra_cmp_double((double)in_i.W, out_f.W) == 0);
#ifdef CRA_JSON_SAFE_INT
    assert_always(cra_cmp_double((double)CRA_MIN_SAFE_INT, out_f.m) == 0);
    assert_always(cra_cmp_double((double)CRA_MAX_SAFE_INT, out_f.M) == 0);
    assert_always(cra_cmp_double((double)CRA_MAX_SAFE_INT, out_f.UM) == 0);
#else
// 不做SAFE_INT检查，double d = (double)i;
// d甚至可能是NaN或Inf
#endif

    // f -> i
    length = sizeof(buffer);
    assert_always(cra_json_stringify(buffer, &length, &in_f, &F_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out_i, &I_meta));
    assert_always((int64_t)in_f.n == out_i.n);
    assert_always(0 == out_i.m);
    assert_always(0 == out_i.UM);
#ifdef CRA_JSON_SAFE_INT
    assert_always(CRA_MAX_SAFE_INT == out_i.M);
    assert_always(CRA_MIN_SAFE_INT == out_i.W);
#else
// 不做SAFE_INT检查，[u]int i = ([u]int)d;
#endif
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
    char      buffer[1024];
    size_t    length;
    CraSerErr err;

    struct Str out, in = { "hello pointer", "hello\tarray", NULL, "", "" };

    length = sizeof(buffer);
    bzero(buffer, sizeof(buffer));
    assert_always(cra_json_stringify(buffer, &length, &in, &Str_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &Str_meta));
    assert_always(strcmp(in.strp, out.strp) == 0);
    assert_always(strcmp(in.stra, out.stra) == 0);
    assert_always(in.strnull == out.strnull);
    assert_always(strcmp(in.strpempty, out.strpempty) == 0);
    assert_always(strcmp(in.straempty, out.straempty) == 0);
    Str_uninit(&out);

    // test error

    // small buffer
    length = 90;
    bzero(&out, sizeof(out));
    assert_always(!cra_json_stringify_err(buffer, &length, &in, &Str_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_json_parse_err(buffer, length, &out, &Str_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &Str_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 37 : 31), "null         ", 14);
    assert_always(!cra_json_parse_err(buffer, length, &out, &Str_meta, &err));
    assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    bzero(&out, sizeof(out));
    assert_always(cra_json_stringify_err(buffer, &length, &in, &Str_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 11 : 8] = '0'; // string -> number
    assert_always(!cra_json_parse_err(buffer, length, &out, &Str_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // char array too small
    length = sizeof(buffer);
    bzero(&out, sizeof(out));
    assert_always(cra_json_stringify_err(buffer, &length, &in, &Str_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 50 : 44), "xxxxx", 5);
    assert_always(!cra_json_parse_err(buffer, length, &out, &Str_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OUT_OF_RANGE);
    printf("%s\n", err.msg);
}

#endif // end test string

#if 1 // test uni

CRA_SER_DEF_STRUCT_META_AND_STRUCT_AND_UNINIT(static, UniTestA, NULL, (STRING, emoji, 1), (STRING, chinese, 2));
CRA_SER_DEF_STRUCT_META_AND_STRUCT_AND_UNINIT(static, UniTestB, NULL, (STRING, emoji, 1, 6), (STRING, chinese, 2, 16));
CRA_SER_DEF_STRUCT_META_AND_STRUCT(

  static,
  UniTestC,
  NULL,

  (STRING, t1, 1, 100),
  (STRING, t2, 2, 100),
  (STRING, t3, 3, 100),
  (STRING, t4, 4, 100),
  (STRING, t5, 5, 100),
  (STRING, t6, 6, 100),
  (STRING, t7, 7, 100)

);
void
test_string_unicode(void)
{
    char json[] = "\t   \n\t\n{ \t \n"
                  "\"emoji\"\t \n  :  \n\t \"\\uD83D\\uDE00\\/\" \t\r\n ,\r\n"
                  "\"chinese\"    : \"\\u4f60\\u597d，\\u4e16\\u754c\"   "
                  "}\t  \n";

    struct UniTestA a;

    assert_always(cra_json_parse(json, sizeof(json), &a, &UniTestA_meta));
    printf("{\"emoji\": \"%s\", \"chinese\": \"%s\"}\n", a.emoji, a.chinese);
    assert_always(strcmp(a.emoji, "😀/") == 0);
    assert_always(strcmp(a.chinese, "你好，世界") == 0);
    cra_free(a.emoji);
    cra_free(a.chinese);

    // ---------------

    struct UniTestB b;

    assert_always(cra_json_parse(json, sizeof(json), &b, &UniTestB_meta));
    printf("{\"emoji\": \"%s\", \"chinese\": \"%s\"}\n", b.emoji, b.chinese);
    assert_always(strcmp(b.emoji, "😀/") == 0);
    assert_always(strcmp(b.chinese, "你好，世界") == 0);

    // ---------------

    struct UniTestC c1, c2;

    size_t len;
    char   buffer[1000];
    char   t1[] = "{"
                  "\"t1\":\"Hello, World! 123\","
                  "\"t2\":\"你好，世界！\","
                  "\"t3\":\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\","
                  "\"t4\":\"\\u0041\\u4F60\\u597D\","
                  "\"t5\":\"\\uD83D\\uDE00\","
                  "\"t6\":\"哈哈，ABcd=1029\\n你好 😀\","
                  "\"t7\":\"\\u0001\\u0002\\u001F\""
                  "}";
    char   t2[] = "{"
                  "\"t1\":\"Hello, World! 123\","
                  "\"t2\":\"你好，世界！\","
                  "\"t3\":\"\\\" \\\\ / \\b \\f \\n \\r \\t\","
                  "\"t4\":\"A你好\","
                  "\"t5\":\"😀\","
                  "\"t6\":\"哈哈，ABcd=1029\\n你好 😀\","
                  "\"t7\":\"\\u0001\\u0002\\u001f\""
                  "}";

    memcpy(&c2.t1, "Hello, World! 123", 18);
    memcpy(&c2.t2, "你好，世界！", 19);
    memcpy(&c2.t3, "\" \\ / \b \f \n \r \t", 16);
    memcpy(&c2.t4, "A你好", 8);
    memcpy(&c2.t5, "😀", 5);
    memcpy(&c2.t6, "哈哈，ABcd=1029\n你好 😀", 31);
    c2.t7[0] = 0x01;
    c2.t7[1] = 0x02;
    c2.t7[2] = 0x1F;
    c2.t7[3] = 0x00; // null-terminator

    len = sizeof(buffer);
    assert_always(cra_json_stringify(buffer, &len, &c2, &UniTestC_meta, false));
    printf("%s\n", buffer);
    printf("%s\n", t2);
    printf("%s\n", t1);
    assert_always(strcmp(buffer, t2) == 0);

    assert_always(cra_json_parse(t1, sizeof(t1), &c1, &UniTestC_meta));
    assert_always(strcmp(c1.t1, c2.t1) == 0);
    assert_always(strcmp(c1.t2, c2.t2) == 0);
    assert_always(strcmp(c1.t3, c2.t3) == 0);
    assert_always(strcmp(c1.t4, c2.t4) == 0);
    assert_always(strcmp(c1.t5, c2.t5) == 0);
    assert_always(strcmp(c1.t6, c2.t6) == 0);
    assert_always(c1.t7[0] == c2.t7[0]);
    assert_always(c1.t7[1] == c2.t7[1]);
    assert_always(c1.t7[2] == c2.t7[2]);
    assert_always(c1.t7[3] == c2.t7[3]);

    // failed
    CraSerErr err;
    memcpy(t1 + 7, "\\x41", 4);
    assert_always(!cra_json_parse_err(t1, sizeof(t1), &c1, &UniTestC_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    memcpy(t1 + 7, "\\u123", 5);
    assert_always(!cra_json_parse_err(t1, sizeof(t1), &c1, &UniTestC_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    memcpy(t1 + 7, "\\u12X4", 6);
    assert_always(!cra_json_parse_err(t1, sizeof(t1), &c1, &UniTestC_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    memcpy(t1 + 7, "\\uD800", 6);
    assert_always(!cra_json_parse_err(t1, sizeof(t1), &c1, &UniTestC_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    memcpy(t1 + 7, "\\uDC00", 6);
    assert_always(!cra_json_parse_err(t1, sizeof(t1), &c1, &UniTestC_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
    memcpy(t1 + 7, "\\uD800\\u0041", 12);
    assert_always(!cra_json_parse_err(t1, sizeof(t1), &c1, &UniTestC_meta, &err));
    assert_always(err.err == CRA_SER_ERR_INVALID_VALUE);
}

#endif // end test uni

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
    char      buffer[1024];
    size_t    length;
    CraSerErr err;

    struct Struct1 s = { .i = 1000 };
    struct Struct2 out, in = { .strup = &s, .strus = { .i = 2000 }, .strunull = NULL };

    length = sizeof(buffer);
    assert_always(cra_json_stringify(buffer, &length, &in, &Struct2_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &Struct2_meta));
    assert_always(!out.strunull);
    assert_always(out.strup);
    assert_always(in.strup->i == out.strup->i);
    assert_always(in.strus.i == out.strus.i);
    assert_always(in.strunull == out.strunull);
    Struct2_uninit(&out);

    // test error

    // small buffer
    length = 55;
    assert_always(!cra_json_stringify_err(buffer, &length, &in, &Struct2_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_json_parse_err(buffer, length, &out, &Struct2_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &Struct2_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 40 : 28), "null", 4);
    assert_always(!cra_json_parse_err(buffer, length, &out, &Struct2_meta, &err));
    assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &Struct2_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_OK);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 40 : 28] = '8'; // struct -> number
    assert_always(!cra_json_parse_err(buffer, length, &out, &Struct2_meta, &err));
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
    char   buffer[1024];
    size_t length;

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
    assert_always(cra_json_stringify(buffer, &length, &ino, &O_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &outn, &N_meta));
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
    assert_always(cra_json_stringify(buffer, &length, &inn, &N_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &outo, &O_meta));
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
    char      buffer[8192];
    size_t    length;
    CraSerErr err;

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
    assert_always(cra_json_stringify(buffer, &length, &in, &StructArray_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &StructArray_meta));
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
    length = 2296;
    assert_always(!cra_json_stringify_err(buffer, &length, &in, &StructArray_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_json_parse_err(buffer, length, &out, &StructArray_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &StructArray_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 13 : 10), "null", 4);
    assert_always(!cra_json_parse_err(buffer, length, &out, &StructArray_meta, &err));
    assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &StructArray_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 13 : 10] = '{'; // array -> object
    assert_always(!cra_json_parse_err(buffer, length, &out, &StructArray_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // array too small
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &StructArray_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 436 : 334), ", 124],", 7); // 100 -> 101
    assert_always(!cra_json_parse_err(buffer, length, &out, &StructArray_meta, &err));
    assert_always(err.err == CRA_SER_ERR_OUT_OF_RANGE);
    printf("%s\n", err.msg);

    StructArray_uninit(&in);
}

#endif // end test array

#if 1 // test list

CRA_SER_DEF_LIST_META(static, (CraAList, ALI32), (INT32), CRA_ALIST_SRLZ);
CRA_SER_DEF_LIST_META(static, (CraLList, LLStr), (STRING, 20), CRA_LLIST_SRLZ);
CRA_DEQUE_DEF_SRLZ_UNINIT_FN(DeStr_uninit, char *, val, cra_free(val);)
CRA_SER_DEF_LIST_META(static, (CraDeque, DeStr), (STRING), CRA_DEQUE_SRLZ_WITH_UNINIT(DeStr_uninit));

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
    char      buffer[8192];
    size_t    length;
    CraSerErr err;

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
    assert_always(cra_json_stringify(buffer, &length, &in, &L_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &L_meta));
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
    length = 3300;
    assert_always(!cra_json_stringify_err(buffer, &length, &in, &L_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_json_parse_err(buffer, length, &out, &L_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &L_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 12 : 9), "null", 4);
    assert_always(!cra_json_parse_err(buffer, length, &out, &L_meta, &err));
    assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &L_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 12 : 9] = '{'; // list -> object
    assert_always(!cra_json_parse_err(buffer, length, &out, &L_meta, &err));
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

    char   buffer[4096];
    size_t length;

    size_t  n1, n2;
    int32_t v1, v2;
    for (size_t i = 0; i < CRA_NARRAY(ins); i++)
    {
        length = sizeof(buffer);
        assert_always(cra_json_stringify(buffer, &length, ins[i], meta[i], FORMAT));
        printf("%s\n", buffer);
        for (size_t j = 0; j < CRA_NARRAY(outs); j++)
        {
            assert_always(cra_json_parse(buffer, length, outs[j], meta[j]));
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
    char      buffer[4096];
    size_t    length;
    CraSerErr err;

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
    assert_always(cra_json_stringify(buffer, &length, &in, &D_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &D_meta));
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
    length = 1440;
    assert_always(!cra_json_stringify_err(buffer, &length, &in, &D_meta, &err, FORMAT));
    assert_always(err.err == CRA_SER_ERR_NO_BUF);
    printf("%s\n", err.msg);
    assert_always(!cra_json_parse_err(buffer, length, &out, &D_meta, &err));
    assert_always(err.err != CRA_SER_ERR_OK);
    printf("%s\n", err.msg);

    // cannot be null
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &D_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    memcpy(buffer + (FORMAT ? 9 : 6), "null", 4);
    assert_always(!cra_json_parse_err(buffer, length, &out, &D_meta, &err));
    assert_always(err.err == CRA_SER_ERR_CANNOT_BE_NULL);
    printf("%s\n", err.msg);

    // type mismatch
    length = sizeof(buffer);
    assert_always(cra_json_stringify_err(buffer, &length, &in, &D_meta, &err, FORMAT));
    assert_always(!err.err);
    printf("%s\n", err.msg);
    buffer[FORMAT ? 9 : 6] = '-'; // dict -> number
    assert_always(!cra_json_parse_err(buffer, length, &out, &D_meta, &err));
    assert_always(err.err == CRA_SER_ERR_TYPE_MISMATCH);
    printf("%s\n", err.msg);

    // // invalid key type
    // length = sizeof(buffer);
    // ((CraTypeMeta *)metadu)->type = CRA_TYPE_BYTES;
    // assert_always(!cra_json_stringify_err(buffer, &length, CRA_SERI_STRUCT(in, false, meta, NULL, NULL), &err));
    // assert_always(err.err == CRA_SER_ERR_CANNOT_BE_KEY);
    // printf("%s\n", err.msg);
    // assert_always(!cra_json_parse_err(buffer, length, CRA_SERI_STRUCT(out, false, meta, NULL, NULL), &err));
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

    struct B in, out;
    char     buffer[8192];
    size_t   length;

    B_init(&in);
    length = sizeof(buffer);
    assert_always(cra_json_stringify(buffer, &length, &in, &B_meta, FORMAT));
    printf("%s\n", buffer);
    assert_always(cra_json_parse(buffer, length, &out, &B_meta));

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

    assert_always(!cra_json_parse(buffer, length - 1, &out, &B_meta));
    assert_always(!cra_json_parse(buffer, 126, &out, &B_meta));
    assert_always(!cra_json_parse(buffer, 84, &out, &B_meta));
    assert_always(!cra_json_parse(buffer, 45, &out, &B_meta));
    assert_always(!cra_json_parse(buffer, 17, &out, &B_meta));

    B_uninit(&in);
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
    printf("========== test int float ==========\n");
    test_int_float();
    printf("========== test string ==========\n");
    test_string();
    printf("========== test string unicode ==========\n");
    test_string_unicode();
    printf("========== test struct ==========\n");
    test_struct();
    printf("========== test struct id ==========\n");
    test_struct_id();
    printf("========== test array ==========\n");
    test_array();
    printf("========== test list ==========\n");
    test_list();
    printf("========== test array list ==========\n");
    test_array_list();
    printf("========== test dict ==========\n");
    test_dict();
    printf("========== test deserialize failed ==========\n");
    test_deserialize_failed();

    printf("\n");
    cra_memory_leak_report();
    return 0;
}
