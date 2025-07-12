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
#include "cra_assert.h"
#include "serialize/cra_bin_ser.h"
#include "collections/cra_alist.h"
#include "collections/cra_llist.h"
#include "collections/cra_deque.h"
#include "collections/cra_dict.h"

void test_begin_end(void)
{
#if 1 // serialize
    CraSerializer ser;

    unsigned char buff[100];
    cra_bin_serialize_begin(&ser, buff, sizeof(buff));
    assert_always(ser.error == CRA_SER_ERROR_SUCCESS);
    assert_always(ser.index == 0);
    assert_always(ser.length == sizeof(buff));
    assert_always(ser.noalloc == true);
    assert_always(ser.buffer == buff);

    size_t buffsize;
    CraSerError_e error;
    unsigned char *resbuf = cra_bin_serialize_end(&ser, &buffsize, &error);
    assert_always(buffsize == 0);
    assert_always(resbuf == buff);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    cra_bin_serialize_begin(&ser, NULL, 1000);
    assert_always(ser.error == CRA_SER_ERROR_SUCCESS);
    assert_always(ser.index == 0);
    assert_always(ser.length == 1000);
    assert_always(ser.noalloc == false);
    assert_always(ser.buffer != NULL);

    resbuf = cra_bin_serialize_end(&ser, &buffsize, NULL);
    assert_always(buffsize == 0);
    assert_always(resbuf != NULL);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    cra_free(resbuf);

    // error input

    // cra_bin_serialize_begin(NULL, buff, 0);
    // cra_bin_serialize_begin(&ser, buff, 0);

    cra_bin_serialize_begin(&ser, buff, sizeof(buff));
    ser.error = CRA_SER_ERROR_NOBUF;
    resbuf = cra_bin_serialize_end(&ser, &buffsize, &error);
    assert_always(resbuf == NULL);
    assert_always(error != CRA_SER_ERROR_SUCCESS);

    // cra_bin_serialize_begin(&ser, buff, sizeof(buff));
    // cra_bin_serialize_end(&ser, NULL, &error);

    // cra_bin_serialize_begin(&ser, buff, sizeof(buff));
    // ser.index = ser.length;
    // cra_bin_serialize_end(&ser, &buffsize, &error);

    // cra_bin_serialize_begin(&ser, buff, sizeof(buff));
    // ser.index = ser.length + 1;
    // cra_bin_serialize_end(&ser, &buffsize, &error);

#endif // end serialize

#if 1 // deserialize

    cra_bin_deserialize_begin(&ser, buff, sizeof(buff));
    assert_always(ser.error == CRA_SER_ERROR_SUCCESS);
    assert_always(ser.noalloc == true);
    assert_always(ser.index == 0);
    assert_always(ser.length == sizeof(buff));
    assert_always(ser.buffer == buff);
    cra_bin_deserialize_end(&ser, NULL);
    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    // error input

    // cra_bin_deserialize_begin(NULL, buff, 100);
    // cra_bin_deserialize_begin(&ser, NULL, 100);
    // cra_bin_deserialize_begin(&ser, buff, 0);

    // cra_bin_deserialize_begin(&ser, buff, 100);
    // ser.buffer = NULL;
    // cra_bin_deserialize_end(&ser, &error);

    cra_bin_deserialize_begin(&ser, buff, 100);
    ser.index = ser.length;
    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    // cra_bin_deserialize_begin(&ser, buff, 100);
    // ser.index = ser.length + 1;
    // cra_bin_deserialize_end(&ser, &error);

#endif // end deserialize
}

void test_base(void)
{
    bool res;
    CraSerError_e error;
    CraSerializer ser;
    unsigned char buff[1024];
    size_t buffsize;

    buffsize = sizeof(buff);
    cra_bin_serialize_begin(&ser, buff, buffsize);

    res = cra_bin_serialize_bool(&ser, true);
    assert_always(res);
    res = cra_bin_serialize_bool(&ser, false);
    assert_always(res);

    res = cra_bin_serialize_int8(&ser, INT8_MIN);
    assert_always(res);
    res = cra_bin_serialize_int8(&ser, INT8_MAX);
    assert_always(res);
    res = cra_bin_serialize_int8(&ser, -100);
    assert_always(res);

    res = cra_bin_serialize_int16(&ser, INT16_MIN);
    assert_always(res);
    res = cra_bin_serialize_int16(&ser, INT16_MAX);
    assert_always(res);
    res = cra_bin_serialize_int16(&ser, -1000);
    assert_always(res);

    res = cra_bin_serialize_int32(&ser, INT32_MIN);
    assert_always(res);
    res = cra_bin_serialize_int32(&ser, INT32_MAX);
    assert_always(res);
    res = cra_bin_serialize_int32(&ser, -10000);
    assert_always(res);

    res = cra_bin_serialize_int64(&ser, INT64_MIN);
    assert_always(res);
    res = cra_bin_serialize_int64(&ser, INT64_MAX);
    assert_always(res);
    res = cra_bin_serialize_int64(&ser, -100000);
    assert_always(res);

    res = cra_bin_serialize_uint8(&ser, UINT8_MAX);
    assert_always(res);
    res = cra_bin_serialize_uint8(&ser, 100);
    assert_always(res);

    res = cra_bin_serialize_uint16(&ser, UINT16_MAX);
    assert_always(res);
    res = cra_bin_serialize_uint16(&ser, 1000);
    assert_always(res);

    res = cra_bin_serialize_uint32(&ser, UINT32_MAX);
    assert_always(res);
    res = cra_bin_serialize_uint32(&ser, 10000);
    assert_always(res);

    res = cra_bin_serialize_uint64(&ser, UINT64_MAX);
    assert_always(res);
    res = cra_bin_serialize_uint64(&ser, 100000);
    assert_always(res);

    res = cra_bin_serialize_float(&ser, FLT_MIN);
    assert_always(res);
    res = cra_bin_serialize_float(&ser, FLT_MAX);
    assert_always(res);
    res = cra_bin_serialize_float(&ser, -1000.8f);
    assert_always(res);

    res = cra_bin_serialize_double(&ser, DBL_MIN);
    assert_always(res);
    res = cra_bin_serialize_double(&ser, DBL_MAX);
    assert_always(res);
    res = cra_bin_serialize_double(&ser, 10000.5);
    assert_always(res);

    cra_bin_serialize_end(&ser, &buffsize, &error);
    assert_always(buffsize > 0);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    cra_bin_serialize_print(buff, buffsize);

    // =================================

    cra_bin_deserialize_begin(&ser, buff, buffsize);

    bool b;
    res = cra_bin_deserialize_bool(&ser, &b);
    assert_always(res);
    assert_always(b == true);
    res = cra_bin_deserialize_bool(&ser, &b);
    assert_always(res);
    assert_always(b == false);

    int8_t i8;
    res = cra_bin_deserialize_int8(&ser, &i8);
    assert_always(res);
    assert_always(i8 == INT8_MIN);
    res = cra_bin_deserialize_int8(&ser, &i8);
    assert_always(res);
    assert_always(i8 == INT8_MAX);
    res = cra_bin_deserialize_int8(&ser, &i8);
    assert_always(i8 == -100);
    assert_always(res);

    int16_t i16;
    res = cra_bin_deserialize_int16(&ser, &i16);
    assert_always(res);
    assert_always(i16 == INT16_MIN);
    res = cra_bin_deserialize_int16(&ser, &i16);
    assert_always(res);
    assert_always(i16 == INT16_MAX);
    res = cra_bin_deserialize_int16(&ser, &i16);
    assert_always(res);
    assert_always(i16 == -1000);

    int32_t i32;
    res = cra_bin_deserialize_int32(&ser, &i32);
    assert_always(res);
    assert_always(i32 == INT32_MIN);
    res = cra_bin_deserialize_int32(&ser, &i32);
    assert_always(res);
    assert_always(i32 == INT32_MAX);
    res = cra_bin_deserialize_int32(&ser, &i32);
    assert_always(res);
    assert_always(i32 == -10000);

    int64_t i64;
    res = cra_bin_deserialize_int64(&ser, &i64);
    assert_always(res);
    assert_always(i64 == INT64_MIN);
    res = cra_bin_deserialize_int64(&ser, &i64);
    assert_always(res);
    assert_always(i64 == INT64_MAX);
    res = cra_bin_deserialize_int64(&ser, &i64);
    assert_always(res);
    assert_always(i64 == -100000);

    uint8_t u8;
    res = cra_bin_deserialize_uint8(&ser, &u8);
    assert_always(res);
    assert_always(u8 == UINT8_MAX);
    res = cra_bin_deserialize_uint8(&ser, &u8);
    assert_always(res);
    assert_always(u8 == 100);

    uint16_t u16;
    res = cra_bin_deserialize_uint16(&ser, &u16);
    assert_always(res);
    assert_always(u16 == UINT16_MAX);
    res = cra_bin_deserialize_uint16(&ser, &u16);
    assert_always(res);
    assert_always(u16 == 1000);

    uint32_t u32;
    res = cra_bin_deserialize_uint32(&ser, &u32);
    assert_always(res);
    assert_always(u32 == UINT32_MAX);
    res = cra_bin_deserialize_uint32(&ser, &u32);
    assert_always(res);
    assert_always(u32 == 10000);

    uint64_t u64;
    res = cra_bin_deserialize_uint64(&ser, &u64);
    assert_always(res);
    assert_always(u64 == UINT64_MAX);
    res = cra_bin_deserialize_uint64(&ser, &u64);
    assert_always(res);
    assert_always(u64 == 100000);

    float flt;
    res = cra_bin_deserialize_float(&ser, &flt);
    assert_always(res);
    assert_always(flt == FLT_MIN);
    res = cra_bin_deserialize_float(&ser, &flt);
    assert_always(res);
    assert_always(flt == FLT_MAX);
    res = cra_bin_deserialize_float(&ser, &flt);
    assert_always(res);
    assert_always(flt == -1000.8f);

    double dbl;
    res = cra_bin_deserialize_double(&ser, &dbl);
    assert_always(res);
    assert_always(dbl == DBL_MIN);
    res = cra_bin_deserialize_double(&ser, &dbl);
    assert_always(res);
    assert_always(dbl == DBL_MAX);
    res = cra_bin_deserialize_double(&ser, &dbl);
    assert_always(res);
    assert_always(dbl == 10000.5);

    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
}

void test_string(void)
{
    bool res;
    CraSerError_e error;
    CraSerializer ser;
    unsigned char buff[1024];
    size_t buffsize;

#if 0 // out of string buf

    buffsize = sizeof(buff);
    cra_bin_serialize_begin(&ser, NULL, (size_t)(UINT32_MAX) + 20);

    char *stre = cra_malloc((size_t)(UINT32_MAX) + 2);
    for (size_t i = 0; i < UINT32_MAX; i += 4)
    {
        stre[i] = 'x';
        stre[i + 1] = 'y';
        stre[i + 2] = 'z';
        stre[i + 3] = 'w';
    }

#if 1 // string len > UINT32_MAX
    stre[(size_t)(UINT32_MAX) + 1] = '\0';
    res = cra_bin_serialize_string(&ser, stre);
    assert_always(!res);

    cra_bin_serialize_end(&ser, &buffsize, &error);
    assert_always(error == CRA_SER_ERROR_STRING_TOO_LONG);
#endif
#if 1 // string len == UINT32_MAX
    ser.index = 0;
    ser.error = CRA_SER_ERROR_SUCCESS;
    stre[UINT32_MAX] = '\0';
    res = cra_bin_serialize_string(&ser, stre);
    assert_always(!res);

    cra_bin_serialize_end(&ser, &buffsize, &error);
    assert_always(error == CRA_SER_ERROR_STRING_TOO_LONG);
#endif
    cra_free(ser.buffer);
    cra_free(stre);
#endif

    // test empty

    buffsize = sizeof(buff);
    cra_bin_serialize_begin(&ser, buff, buffsize);

    char *strpempty = "";
    char straempty[] = "";
    res = cra_bin_serialize_string(&ser, strpempty);
    assert_always(res);
    res = cra_bin_serialize_string(&ser, straempty);
    assert_always(res);

    cra_bin_serialize_end(&ser, &buffsize, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    cra_bin_serialize_print(buff, buffsize);

    cra_bin_deserialize_begin(&ser, buff, buffsize);

    char *resstrpempty;
    char resstraempty[1];
    res = cra_bin_deserialize_string(&ser, (char *)&resstrpempty, 0, true, true);
    assert_always(res);
    assert_always(strcmp(strpempty, resstrpempty) == 0);
    res = cra_bin_deserialize_string(&ser, resstraempty, sizeof(resstraempty), false, true);
    assert_always(res);
    assert_always(strcmp(straempty, resstraempty) == 0);

    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_free(resstrpempty);

    // other

    buffsize = sizeof(buff);
    cra_bin_serialize_begin(&ser, buff, buffsize);

    char *strp = "string (char pointer)";
    char stra[] = "string (char array)";
    char *strnull = NULL; // string null (pointer)
    res = cra_bin_serialize_string(&ser, strp);
    assert_always(res);
    res = cra_bin_serialize_string(&ser, stra);
    assert_always(res);
    res = cra_bin_serialize_string(&ser, strnull);
    assert_always(res);

    cra_bin_serialize_end(&ser, &buffsize, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    cra_bin_serialize_print(buff, buffsize);

    // 1 normal
    cra_bin_deserialize_begin(&ser, buff, buffsize);

    char *resstrp;
    char resstra[sizeof(stra) + 2];
    char *resstrnull;
    res = cra_bin_deserialize_string(&ser, (char *)&resstrp, 0, true, true);
    assert_always(res);
    assert_always(strcmp(strp, resstrp) == 0);
    res = cra_bin_deserialize_string(&ser, resstra, sizeof(resstra), false, true);
    assert_always(res);
    assert_always(strcmp(stra, resstra) == 0);
    res = cra_bin_deserialize_string(&ser, (char *)&resstrnull, 0, true, true);
    assert_always(res);
    assert_always(strnull == resstrnull && resstrnull == NULL);

    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_free(resstrp);

    // 2 char * <-> char[N]
    cra_bin_deserialize_begin(&ser, buff, buffsize);

    res = cra_bin_deserialize_string(&ser, resstra, sizeof(resstra), false, true);
    assert_always(res);
    assert_always(strcmp(strp, resstra) == 0);
    res = cra_bin_deserialize_string(&ser, (char *)&resstrp, 0, true, true);
    assert_always(res);
    assert_always(strcmp(stra, resstrp) == 0);
    res = cra_bin_deserialize_string(&ser, (char *)&resstrnull, 0, true, true);
    assert_always(res);
    assert_always(strnull == resstrnull && resstrnull == NULL);

    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_free(resstrp);

    // 3 null -> char[N] !! ERROR
    cra_bin_deserialize_begin(&ser, buff, buffsize);

    res = cra_bin_deserialize_string(&ser, (char *)&resstrp, 0, true, false);
    assert_always(res);
    assert_always(strcmp(strp, resstrp) == 0);
    res = cra_bin_deserialize_string(&ser, resstra, sizeof(resstra), false, true);
    assert_always(res);
    assert_always(strcmp(stra, resstra) == 0);
    char nullerr[100];
    res = cra_bin_deserialize_string(&ser, nullerr, sizeof(nullerr), false, true);
    assert_always(!res);

    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_CANNOT_BE_NULL);
    cra_free(resstrp);

    // 4 string array is too small
    cra_bin_deserialize_begin(&ser, buff, buffsize);

    char resarray[21];
    res = cra_bin_deserialize_string(&ser, resarray, sizeof(resarray), false, true);
    assert_always(!res);

    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_STRING_BUF_TOO_SMALL);
}

void test_string_nz(void)
{
    CraSerError_e error;
    CraSerializer ser;
    unsigned char buff[1024];
    size_t buffsize;

    // test empty

    buffsize = sizeof(buff);
    cra_bin_serialize_begin(&ser, buff, buffsize);

    char *strpempty = "";
    char straempty[] = "";
    cra_bin_serialize_string_nz(&ser, strpempty, 0);
    cra_bin_serialize_string_nz(&ser, straempty, 0);

    cra_bin_serialize_end(&ser, &buffsize, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    cra_bin_serialize_print(buff, buffsize);

    cra_bin_deserialize_begin(&ser, buff, buffsize);

    uint32_t len;
    char *resstrpempty;
    char resstraempty[1];
    cra_bin_deserialize_string_nz(&ser, (char *)&resstrpempty, &len, true, true);
    assert_always(len == 0);
    len = sizeof(resstraempty);
    cra_bin_deserialize_string_nz(&ser, resstraempty, &len, false, true);
    assert_always(len == 0);

    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_free(resstrpempty);

    // test normal

    buffsize = sizeof(buff);
    cra_bin_serialize_begin(&ser, buff, buffsize);
    char strnz[10];
    strnz[0] = 'a';
    strnz[1] = 'b';
    strnz[2] = 'c';
    strnz[3] = 'd';
    cra_bin_serialize_string_nz(&ser, strnz, 4);
    cra_bin_serialize_end(&ser, &buffsize, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_bin_serialize_print(buff, buffsize);

    char *strnz2;
    uint32_t nstrnz2;
    cra_bin_deserialize_begin(&ser, buff, buffsize);
    cra_bin_deserialize_string_nz(&ser, (char *)&strnz2, &nstrnz2, true, true);
    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(nstrnz2 == 4);
    assert_always(strncmp(strnz, strnz2, 4) == 0);
    cra_free(strnz2);

    struct A
    {
        uint32_t nstra;
        char stra[100];
        uint32_t nstrp;
        char *strp;
        uint32_t nstrnull;
        char *strnull;
    };
    CRA_TYPE_META_BEGIN(meta_a)
    CRA_TYPE_META_STRING_NZ_MEMBER(struct A, stra, false)
    CRA_TYPE_META_STRING_NZ_MEMBER(struct A, strp, true)
    CRA_TYPE_META_STRING_NZ_MEMBER(struct A, strnull, true)
    CRA_TYPE_META_END();

    struct A a = {
        4,
        "1234",
        3,
        "abc",
        0,
        NULL,
    };

    buffsize = sizeof(buff);
    cra_bin_serialize_struct0(buff, &buffsize, &a, meta_a, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_bin_serialize_print(buff, buffsize);

    struct A aa;
    cra_bin_deserialize_struct0(buff, buffsize, &aa, sizeof(struct A), false, meta_a, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(a.nstra == aa.nstra);
    assert_always(strncmp(a.stra, aa.stra, a.nstra) == 0);
    assert_always(a.nstrp == aa.nstrp);
    assert_always(strncmp(a.strp, aa.strp, a.nstrp) == 0);
    assert_always(a.nstrnull == aa.nstrnull);
    assert_always(a.strnull == aa.strnull && aa.strnull == NULL);
    cra_free(aa.strp);
}

void test_struct_base(void)
{
    struct A
    {
        int i;
        double d;
    };
    CRA_TYPE_META_BEGIN(meta_a)
    CRA_TYPE_META_INT32_MEMBER(struct A, i)
    CRA_TYPE_META_DOUBLE_MEMBER(struct A, d)
    CRA_TYPE_META_END();

    bool res;
    CraSerError_e error;
    CraSerializer ser;
    unsigned char buff[1024];
    size_t buffsize;

    buffsize = sizeof(buff);
    cra_bin_serialize_begin(&ser, buff, buffsize);

    struct A *anull = NULL;
    struct A a = {.i = 100, .d = 2.5};
    res = cra_bin_serialize_struct(&ser, anull, meta_a);
    assert_always(res);
    res = cra_bin_serialize_struct(&ser, &a, meta_a);
    assert_always(res);

    cra_bin_serialize_end(&ser, &buffsize, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    cra_bin_serialize_print(buff, buffsize);

    // 1 normal
    cra_bin_deserialize_begin(&ser, buff, buffsize);

    struct A *anull2;
    struct A as;
    res = cra_bin_deserialize_struct(&ser, &anull2, sizeof(struct A), true, true, meta_a, NULL, NULL);
    assert_always(res);
    assert_always(anull2 == anull && anull2 == NULL);
    res = cra_bin_deserialize_struct(&ser, &as, sizeof(struct A), false, true, meta_a, NULL, NULL);
    assert_always(res);
    assert_always(a.i == as.i);
    assert_always(a.d == as.d);

    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    // 2 struct X -> struct X *
    cra_bin_deserialize_begin(&ser, buff, buffsize);

    struct A *ap;
    res = cra_bin_deserialize_struct(&ser, &anull2, sizeof(struct A), true, true, meta_a, NULL, NULL);
    assert_always(res);
    assert_always(anull2 == anull && anull2 == NULL);
    res = cra_bin_deserialize_struct(&ser, &ap, sizeof(struct A), true, true, meta_a, NULL, NULL);
    assert_always(res);
    assert_always(a.i == ap->i);
    assert_always(a.d == ap->d);
    cra_free(ap);

    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    // 2 null -> struct X !! ERROR
    cra_bin_deserialize_begin(&ser, buff, buffsize);

    struct A asnull;
    res = cra_bin_deserialize_struct(&ser, &asnull, sizeof(struct A), false, true, meta_a, NULL, NULL);
    assert_always(!res);

    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_CANNOT_BE_NULL);
}

void test_struct_struct(void)
{
    struct A
    {
        int i;
    };
    CRA_TYPE_META_BEGIN(meta_a)
    CRA_TYPE_META_INT32_MEMBER(struct A, i)
    CRA_TYPE_META_END();
    struct B
    {
        struct A *anull;
        struct A *ap;
        struct A as;
    };
    CRA_TYPE_META_BEGIN(meta_b)
    CRA_TYPE_META_STRUCT_MEMBER(struct B, anull, true, meta_a, NULL, NULL)
    CRA_TYPE_META_STRUCT_MEMBER(struct B, ap, true, meta_a, NULL, NULL)
    CRA_TYPE_META_STRUCT_MEMBER(struct B, as, false, meta_a, NULL, NULL)
    CRA_TYPE_META_END();

    CraSerError_e error;
    unsigned char buff[1024];
    size_t buffsize;

    struct A a = {100};
    struct B b = {
        NULL,
        &a,
        {200},
    };

    buffsize = sizeof(buff);
    cra_bin_serialize_struct0(buff, &buffsize, &b, meta_b, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    cra_bin_serialize_print(buff, buffsize);

    struct B bs;
    cra_bin_deserialize_struct0(buff, buffsize, &bs, sizeof(struct B), false, meta_b, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(bs.anull == b.anull && bs.anull == NULL);
    assert_always(bs.ap->i == b.ap->i);
    assert_always(bs.as.i == b.as.i);
    cra_free(bs.ap);

    struct B *bp;
    cra_bin_deserialize_struct0(buff, buffsize, &bp, sizeof(struct B), true, meta_b, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(bp->anull == b.anull && bp->anull == NULL);
    assert_always(bp->ap->i == b.ap->i);
    assert_always(bp->as.i == b.as.i);
    cra_free(bp->ap);
    cra_free(bp);
}

#if 1 // test struct with init_i
struct InitX
{
    int i;
    float f;
    double d;
    char *str;
};
CRA_TYPE_META_BEGIN(meta_initx)
CRA_TYPE_META_INT32_MEMBER(struct InitX, i)
CRA_TYPE_META_DOUBLE_MEMBER(struct InitX, d)
CRA_TYPE_META_STRING_MEMBER(struct InitX, str, true)
CRA_TYPE_META_END();

int calledcnt = 0;

void *alloc_inix(void) { return ((void)++calledcnt, cra_alloc(struct InitX)); }
void dealloc_inix(void *obj) { ((void)++calledcnt, cra_dealloc(obj)); }
void init_initx(void *obj, void *args)
{
    CRA_UNUSED_VALUE(args);

    struct InitX *o = (struct InitX *)obj;
    o->i = 100;
    o->f = 1.5f;
    o->d = 2.5;
    o->str = NULL;
    (void)++calledcnt;
}
void uninit_initx(void *obj)
{
    struct InitX *o = (struct InitX *)obj;
    if (o->str)
        cra_free(o->str);
    bzero(o, sizeof(struct InitX));
    (void)++calledcnt;
}

const CraTypeInit_i initx_i = {
    .alloc = alloc_inix,
    .dealloc = dealloc_inix,
    .init = init_initx,
    .uinit = uninit_initx,
};

void test_struct_with_init_i(void)
{
    struct InitX *x = cra_alloc(struct InitX);
    x->i = 200;
    x->f = 200.5f;
    x->d = 2000.6;
    x->str = "hello world";

    unsigned char buff[1024];
    size_t buffsize;
    CraSerError_e error;

    buffsize = sizeof(buff);
    cra_bin_serialize_struct0(buff, &buffsize, x, meta_initx, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_bin_serialize_print(buff, buffsize);

    struct InitX *xx;
    cra_bin_deserialize_struct0(buff, buffsize, &xx, sizeof(struct InitX), true, meta_initx, &initx_i, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(x->i == xx->i);
    assert_always(xx->f == 1.5f); // default value (from init_initx)
    assert_always(x->d == xx->d);
    assert_always(strcmp(x->str, xx->str) == 0);
    cra_free(xx->str);
    cra_free(xx);

    // test failed to call uninit_initx
    CraSerializer ser;
    calledcnt = 0;
    cra_bin_deserialize_begin(&ser, buff, buffsize);
    cra_bin_deserialize_struct(&ser, &xx, sizeof(struct InitX), true, true, meta_initx, &initx_i, NULL);
    assert_always(strcmp(x->str, xx->str) == 0);
    ser.error = CRA_SER_ERROR_TYPE_MISMATCH;
    cra_bin_deserialize_end(&ser, &error);
    assert_always(error == CRA_SER_ERROR_TYPE_MISMATCH);
    assert_always(calledcnt == 4);

    cra_dealloc(x);
}
#endif

void test_list(void)
{
    int32_t idx, val;
    int32_t *valptr;
    CraAList *alist, *alist2;
    CraLList llist;
    CraDeque deque;

    CRA_LLIST_SER_ARGS(args4llist, false, sizeof(int32_t), NULL);
    CRA_DEQUE_SER_ARGS(args4deque, false, CRA_DEQUE_INFINITE, sizeof(int32_t), NULL);

    alist = cra_alloc(CraAList);
    cra_alist_init0(int32_t, alist, 10, true, NULL);
    CRA_TYPE_META_BEGIN(meta_list_val)
    CRA_TYPE_META_INT32_ELEMENT()
    CRA_TYPE_META_END();
    CRA_ALIST_SER_ARGS0(args4alist, alist);

    unsigned char buff[1024];
    size_t buffsize;
    CraSerError_e error;

    // test empty

    buffsize = sizeof(buff);
    cra_bin_serialize_list0(buff, &buffsize, alist, meta_list_val, &g_alist_ser_iter_i, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_bin_serialize_print(buff, buffsize);

    cra_bin_deserialize_list0(buff, buffsize, &alist2, sizeof(CraAList), true, meta_list_val, &g_alist_ser_iter_i, &g_alist_ser_init_i, &args4alist, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_alist_uninit(alist2);
    cra_dealloc(alist2);

    // add vals
    for (int i = 0; i < 10; i++)
        cra_alist_append(alist, &i);

    buffsize = sizeof(buff);
    cra_bin_serialize_list0(buff, &buffsize, alist, meta_list_val, &g_alist_ser_iter_i, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    cra_bin_serialize_print(buff, buffsize);

    // alist -> alist
    cra_bin_deserialize_list0(buff, buffsize, &alist2, sizeof(CraAList), true, meta_list_val, &g_alist_ser_iter_i, &g_alist_ser_init_i, &args4alist, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    idx = 0;
    for (CraAListIter it = cra_alist_iter_init(alist2); cra_alist_iter_next(&it, (void **)&valptr); idx++)
    {
        cra_alist_get(alist, idx, &val);
        assert_always(val == *valptr);
    }

    // alist -> llist
    cra_bin_deserialize_list0(buff, buffsize, &llist, sizeof(CraLList), false, meta_list_val, &g_llist_ser_iter_i, &g_llist_ser_init_i, &args4llist, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    idx = 0;
    for (CraLListIter it = cra_llist_iter_init(&llist); cra_llist_iter_next(&it, (void **)&valptr); idx++)
    {
        cra_alist_get(alist, idx, &val);
        assert_always(val == *valptr);
    }

    // alist -> deque
    cra_bin_deserialize_list0(buff, buffsize, &deque, sizeof(CraDeque), false, meta_list_val, &g_deque_ser_iter_i, &g_deque_ser_init_i, &args4deque, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    idx = 0;
    for (CraDequeIter it = cra_deque_iter_init(&deque); cra_deque_iter_next(&it, (void **)&valptr); idx++)
    {
        cra_alist_get(alist, idx, &val);
        assert_always(val == *valptr);
    }

    // alist -> c array
    int32_t *array;
    cra_ser_count_t narray;
    cra_bin_deserialize_array0(buff, buffsize, &array, sizeof(array), true, &narray, meta_list_val, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(narray == alist->count);
    for (idx = 0; idx < (int32_t)narray; idx++)
    {
        cra_alist_get(alist, idx, &val);
        assert_always(val == array[idx]);
    }

    cra_alist_uninit(alist);
    cra_dealloc(alist);
    cra_alist_uninit(alist2);
    cra_dealloc(alist2);
    cra_llist_uninit(&llist);
    cra_deque_uninit(&deque);
    cra_free(array);
}

void free_str_p(char **strp)
{
    cra_free(*strp);
}

void test_list_element_is_pointer(void)
{
    CraAList list;
    cra_alist_init0(char *, &list, 10, true, (cra_remove_val_fn)free_str_p);
    CRA_ALIST_SER_ARGS0(args4list_str, &list);
    CRA_TYPE_META_BEGIN(meta_list_str)
    CRA_TYPE_META_STRING_ELEMENT(0, true)
    CRA_TYPE_META_END();

    char *str;
    for (int i = 0; i < 10; i++)
    {
        str = cra_malloc(10);
        assert_always(str != NULL);
#ifdef CRA_COMPILER_MSVC
        sprintf_s(str, 10, "hello %d", i);
#else
        sprintf(str, "hello %d", i);
#endif
        cra_alist_append(&list, &str);
    }

    unsigned char buff[1024];
    size_t buffsize;
    CraSerError_e error;

    buffsize = sizeof(buff);
    cra_bin_serialize_list0(buff, &buffsize, &list, meta_list_str, &g_alist_ser_iter_i, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_bin_serialize_print(buff, buffsize);

    CraAList list2;
    cra_bin_deserialize_list0(buff, buffsize, &list2, sizeof(CraAList), false, meta_list_str, &g_alist_ser_iter_i, &g_alist_ser_init_i, &args4list_str, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    for (int i = 0; i < 10; i++)
    {
        char *retstr1, *retstr2;
        cra_alist_get(&list, i, &retstr1);
        cra_alist_get(&list2, i, &retstr2);
        assert_always(strcmp(retstr1, retstr2) == 0);
    }
    cra_alist_uninit(&list2);

    // test failed to call uninit func
    CraSerializer ser;
    cra_bin_deserialize_begin(&ser, buff, buffsize);
    cra_bin_deserialize_list(&ser, &list2, sizeof(CraAList), false, true, meta_list_str, &g_alist_ser_iter_i, &g_alist_ser_init_i, &args4list_str);
    assert_always(list2.count == list.count);
    ser.error = CRA_SER_ERROR_TYPE_MISMATCH;
    cra_bin_deserialize_end(&ser, &error); // list2 will be free
    assert_always(error == CRA_SER_ERROR_TYPE_MISMATCH);

    cra_alist_uninit(&list);
}

void test_array(void)
{
    int32_t array[] = {1, 2, 3, 4};
    cra_ser_count_t narray = sizeof(array) / sizeof(array[0]);
    CRA_TYPE_META_BEGIN(meta_int32)
    CRA_TYPE_META_INT32_ELEMENT()
    CRA_TYPE_META_END();

    CraSerError_e error;
    unsigned char buff[1024];
    size_t buffsize;

    buffsize = sizeof(buff);
    cra_bin_serialize_array0(buff, &buffsize, array, narray, meta_int32, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_bin_serialize_print(buff, buffsize);

    int32_t *array2;
    cra_ser_count_t narray2;
    cra_bin_deserialize_array0(buff, buffsize, &array2, 0, true, &narray2, meta_int32, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(narray == narray2);
    for (cra_ser_count_t i = 0; i < narray; i++)
    {
        assert_always(array[i] == array2[i]);
    }
    cra_free(array2);

    int32_t array3[10];
    cra_ser_count_t narray3;
    cra_bin_deserialize_array0(buff, buffsize, array3, sizeof(array3), false, &narray3, meta_int32, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(narray == narray3);
    for (cra_ser_count_t i = 0; i < narray; i++)
    {
        assert_always(array[i] == array3[i]);
    }

    // c array -> list
    int32_t val;
    CraAList *list;
    CRA_ALIST_SER_ARGS(args_list_i, true, sizeof(int32_t), NULL);
    cra_bin_deserialize_list0(buff, buffsize, &list, sizeof(*list), true, meta_int32, &g_alist_ser_iter_i, &g_alist_ser_init_i, &args_list_i, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(narray == list->count);
    for (cra_ser_count_t i = 0; i < narray; i++)
    {
        cra_alist_get(list, i, &val);
        assert_always(array[i] == val);
    }
    cra_alist_uninit(list);
    cra_dealloc(list);
}

void test_array_in_struct(void)
{
    struct A
    {
        cra_ser_count_t narray;
        char **array;
    };
    CRA_TYPE_META_BEGIN(meta_str)
    CRA_TYPE_META_STRING_ELEMENT(0, true)
    CRA_TYPE_META_END();

    CRA_TYPE_META_BEGIN(meta_a)
    CRA_TYPE_META_ARRAY_MEMBER(struct A, array, true, meta_str)
    CRA_TYPE_META_END();

    struct A a;
    a.narray = 8;
    a.array = cra_malloc(sizeof(char *) * a.narray);
    for (cra_ser_count_t i = 0; i < a.narray; i++)
    {
        char *str = cra_malloc(10);
        assert_always(str != NULL);
#ifdef CRA_COMPILER_MSVC
        sprintf_s(str, 10, "hello %d", i);
#else
        sprintf(str, "hello %d", i);
#endif
        a.array[i] = str;
    }

    CraSerError_e error;
    unsigned char buff[1024];
    size_t buffsize;

    buffsize = sizeof(buff);
    cra_bin_serialize_struct0(buff, &buffsize, &a, meta_a, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_bin_serialize_print(buff, buffsize);

    struct A *aa;
    cra_bin_deserialize_struct0(buff, buffsize, &aa, sizeof(struct A), true, meta_a, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(a.narray == aa->narray);
    for (cra_ser_count_t i = 0; i < a.narray; i++)
    {
        assert_always(strcmp(a.array[i], aa->array[i]) == 0);
        cra_free(a.array[i]);
        cra_free(aa->array[i]);
    }
    cra_free(a.array);
    cra_free(aa->array);
    cra_free(aa);
}

void test_dict(void)
{
    CraDict dict;
    CraDict *dict2;
    unsigned char buff[1024];
    size_t buffsize;
    CraSerError_e error;

    CRA_DICT_SER_ARGS(args4dict, false, sizeof(int32_t), sizeof(char[100]), (cra_hash_fn)cra_hash_int32_t_p, (cra_compare_fn)cra_compare_int_p, NULL, NULL);
    CRA_TYPE_META_BEGIN(meta_dict_kv)
    CRA_TYPE_META_INT32_ELEMENT()                          // key
    CRA_TYPE_META_STRING_ELEMENT(sizeof(char[100]), false) // val
    CRA_TYPE_META_END();

    // serialize dict
    cra_dict_init0(int32_t, char[100], &dict, true, (cra_hash_fn)cra_hash_int32_t_p, (cra_compare_fn)cra_compare_int32_t_p, NULL, NULL);

    // test empty

    buffsize = sizeof(buff);
    cra_bin_serialize_dict0(buff, &buffsize, &dict, meta_dict_kv, &g_dict_ser_iter_i, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_bin_serialize_print(buff, buffsize);

    cra_bin_deserialize_dict0(buff, buffsize, &dict2, sizeof(CraDict), true, meta_dict_kv, &g_dict_ser_iter_i, &g_dict_ser_init_i, &args4dict, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_dict_uninit(dict2);
    cra_dealloc(dict2);

    char cval[] = "hello x";
    for (int32_t i = 0; i < 10; i++)
    {
        cval[6] = '0' + (char)i;
        cra_dict_add(&dict, &i, &cval);
    }

    buffsize = sizeof(buff);
    cra_bin_serialize_dict0(buff, &buffsize, &dict, meta_dict_kv, &g_dict_ser_iter_i, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_bin_serialize_print(buff, buffsize);

    // dict -> dict
    cra_bin_deserialize_dict0(buff, buffsize, &dict2, sizeof(CraDict), true, meta_dict_kv, &g_dict_ser_iter_i, &g_dict_ser_init_i, &args4dict, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    char strv1[100];
    char strv2[100];
    for (int32_t i = 0; i < 10; i++)
    {
        cra_dict_get(&dict, &i, &strv1);
        cra_dict_get(dict2, &i, &strv2);
        assert_always(strcmp(strv1, strv2) == 0);
    }

    cra_dict_uninit(&dict);
    cra_dict_uninit(dict2);
    cra_dealloc(dict2);
}

void test_dict_in_struct(void)
{
    CRA_DICT_SER_ARGS(args4dict_i_d, false, sizeof(int32_t), sizeof(double), (cra_hash_fn)cra_hash_int32_t_p, (cra_compare_fn)cra_compare_int32_t_p, NULL, NULL);
    CRA_UNUSED_VALUE(args4dict_i_d);
    CRA_TYPE_META_BEGIN(meta_dict_i_d)
    CRA_TYPE_META_INT32_ELEMENT()  // key
    CRA_TYPE_META_DOUBLE_ELEMENT() // val
    CRA_TYPE_META_END();

    struct A
    {
        CraDict *dictnull;
        CraDict *pdict;
        CraDict sdict;
    };
    CRA_TYPE_META_BEGIN(meta_a)
    CRA_TYPE_META_DICT_MEMBER(struct A, dictnull, true, meta_dict_i_d, &g_dict_ser_iter_i, &g_dict_ser_init_i, &args4dict_i_d)
    CRA_TYPE_META_DICT_MEMBER(struct A, pdict, true, meta_dict_i_d, &g_dict_ser_iter_i, &g_dict_ser_init_i, &args4dict_i_d)
    CRA_TYPE_META_DICT_MEMBER(struct A, sdict, false, meta_dict_i_d, &g_dict_ser_iter_i, &g_dict_ser_init_i, &args4dict_i_d)
    CRA_TYPE_META_END();

    unsigned char buff[1024];
    size_t buffsize;
    CraSerError_e error;

    int32_t key;
    double val;
    struct A a;
    a.dictnull = NULL;
    a.pdict = cra_alloc(CraDict);
    cra_dict_init0(int32_t, double, a.pdict, args4dict_i_d.zero_memory, args4dict_i_d.hash_key_fn, args4dict_i_d.compare_key_fn, args4dict_i_d.remove_key_fn, args4dict_i_d.remove_val_fn);
    cra_dict_init0(int32_t, double, &a.sdict, args4dict_i_d.zero_memory, args4dict_i_d.hash_key_fn, args4dict_i_d.compare_key_fn, args4dict_i_d.remove_key_fn, args4dict_i_d.remove_val_fn);
    for (int i = 0; i < 10; i++)
    {
        key = rand();
        val = key * 2.5;
        cra_dict_add(a.pdict, &key, &val);
        val = key * 1.5;
        cra_dict_add(&a.sdict, &key, &val);
    }

    buffsize = sizeof(buff);
    cra_bin_serialize_struct0(buff, &buffsize, &a, meta_a, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    cra_bin_serialize_print(buff, buffsize);

    struct A aa;
    cra_bin_deserialize_struct0(buff, buffsize, &aa, sizeof(struct A), false, meta_a, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    CraDictIter it = cra_dict_iter_init(a.pdict);
    int32_t *rkey;
    double *rval, rval1, rval2;
    while (cra_dict_iter_next(&it, (void **)&rkey, (void **)&rval))
    {
        cra_dict_get(aa.pdict, rkey, &rval1);
        assert_always(*rval == rval1);

        cra_dict_get(&a.sdict, rkey, &rval1);
        cra_dict_get(&aa.sdict, rkey, &rval2);
        assert_always(rval1 == rval2);
    }

    cra_dict_uninit(&a.sdict);
    cra_dict_uninit(a.pdict);
    cra_dealloc(a.pdict);
    cra_dict_uninit(&aa.sdict);
    cra_dict_uninit(aa.pdict);
    cra_dealloc(aa.pdict);
}

#if 1 // 测试结构字段数不同

struct Old
{
    int32_t i;
    char *str;
};
CRA_TYPE_META_BEGIN(meta_old)
CRA_TYPE_META_INT32_MEMBER(struct Old, i)
CRA_TYPE_META_STRING_MEMBER(struct Old, str, true)
CRA_TYPE_META_END();
struct New
{
    float f; // 新字段在结构中位置随意
    int32_t i;
    char *str;
};
CRA_TYPE_META_BEGIN(meta_new)
CRA_TYPE_META_INT32_MEMBER(struct New, i)
CRA_TYPE_META_STRING_MEMBER(struct New, str, true)
CRA_TYPE_META_FLOAT_MEMBER(struct New, f) // 保证新的字段的meta在旧的meta后面
CRA_TYPE_META_END();

void test_new2old(void)
{
    unsigned char buff[1024];
    size_t buffsize;
    CraSerError_e error;

    struct New n = {1.5f, 100, "hello world"};
    buffsize = sizeof(buff);
    cra_bin_serialize_struct0(buff, &buffsize, &n, meta_new, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    cra_bin_serialize_print(buff, buffsize);

    struct Old o;
    cra_bin_deserialize_struct0(buff, buffsize, &o, sizeof(o), false, meta_old, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(o.i == n.i);
    assert_always(strcmp(o.str, n.str) == 0);

    cra_free(o.str);
}

// 设置缺省值
static void init_new(struct New *n, void *ignore)
{
    CRA_UNUSED_VALUE(ignore);
    n->f = 2.8f;
    n->i = 200;
    n->str = NULL;
}

void test_old2new(void)
{
    unsigned char buff[1024];
    size_t buffsize;
    CraSerError_e error;

    struct Old o = {100, "hello world"};
    buffsize = sizeof(buff);
    cra_bin_serialize_struct0(buff, &buffsize, &o, meta_old, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    cra_bin_serialize_print(buff, buffsize);

    struct New n;
    const CraTypeInit_i init_i = {0, 0, (void (*)(void *, void *))init_new, 0};
    cra_bin_deserialize_struct0(buff, buffsize, &n, sizeof(n), false, meta_new, &init_i, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(o.i == n.i);
    assert_always(strcmp(o.str, n.str) == 0);
    assert_always(cra_compare_float(n.f, 2.8f) == 0);

    cra_free(n.str);
}

#endif

int main(void)
{
    test_begin_end();
    test_base();
    test_string();
    test_string_nz();
    test_struct_base();
    test_struct_struct();
    test_struct_with_init_i();
    test_list();
    test_list_element_is_pointer();
    test_array();
    test_array_in_struct();
    test_dict();
    test_dict_in_struct();
    test_new2old();
    test_old2new();

    cra_memory_leak_report();
    return 0;
}
