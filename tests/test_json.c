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
#include "cra_assert.h"
#include "serialize/cra_json.h"
#include "collections/cra_alist.h"
#include "collections/cra_llist.h"
#include "collections/cra_deque.h"
#include "collections/cra_dict.h"

#define FORMAT true

void test_base(void)
{
    struct A
    {
        bool bf;
        bool bt;

        int8_t i8M;
        int8_t i8m;
        int8_t i8n;
        int16_t i16M;
        int16_t i16m;
        int16_t i16n;
        int32_t i32M;
        int32_t i32m;
        int32_t i32n;
        int64_t i64M;
        int64_t i64m;
        int64_t i64n;

        uint8_t u8M;
        uint8_t u8n;
        uint16_t u16M;
        uint16_t u16n;
        uint32_t u32M;
        uint32_t u32n;
        uint64_t u64M;
        uint64_t u64n;

        float fM;
        float fMn;
        float fm;
        float fn;
        double dM;
        double dMn;
        double dm;
        double dn;
    };
    CRA_TYPE_META_BEGIN(meta_a)
    CRA_TYPE_META_BOOL_MEMBER(struct A, bf)
    CRA_TYPE_META_BOOL_MEMBER(struct A, bt)

    CRA_TYPE_META_INT8_MEMBER(struct A, i8M)
    CRA_TYPE_META_INT8_MEMBER(struct A, i8m)
    CRA_TYPE_META_INT8_MEMBER(struct A, i8n)
    CRA_TYPE_META_INT16_MEMBER(struct A, i16M)
    CRA_TYPE_META_INT16_MEMBER(struct A, i16m)
    CRA_TYPE_META_INT16_MEMBER(struct A, i16n)
    CRA_TYPE_META_INT32_MEMBER(struct A, i32M)
    CRA_TYPE_META_INT32_MEMBER(struct A, i32m)
    CRA_TYPE_META_INT32_MEMBER(struct A, i32n)
    CRA_TYPE_META_INT64_MEMBER(struct A, i64M)
    CRA_TYPE_META_INT64_MEMBER(struct A, i64m)
    CRA_TYPE_META_INT64_MEMBER(struct A, i64n)

    CRA_TYPE_META_UINT8_MEMBER(struct A, u8M)
    CRA_TYPE_META_UINT8_MEMBER(struct A, u8n)
    CRA_TYPE_META_UINT16_MEMBER(struct A, u16M)
    CRA_TYPE_META_UINT16_MEMBER(struct A, u16n)
    CRA_TYPE_META_UINT32_MEMBER(struct A, u32M)
    CRA_TYPE_META_UINT32_MEMBER(struct A, u32n)
    CRA_TYPE_META_UINT64_MEMBER(struct A, u64M)
    CRA_TYPE_META_UINT64_MEMBER(struct A, u64n)

    CRA_TYPE_META_FLOAT_MEMBER(struct A, fM)
    CRA_TYPE_META_FLOAT_MEMBER(struct A, fMn)
    CRA_TYPE_META_FLOAT_MEMBER(struct A, fm)
    CRA_TYPE_META_FLOAT_MEMBER(struct A, fn)
    CRA_TYPE_META_DOUBLE_MEMBER(struct A, dM)
    CRA_TYPE_META_DOUBLE_MEMBER(struct A, dMn)
    CRA_TYPE_META_DOUBLE_MEMBER(struct A, dm)
    CRA_TYPE_META_DOUBLE_MEMBER(struct A, dn)
    CRA_TYPE_META_END();

    unsigned char *buff;
    CraSerError_e error;
    size_t buffsize;

    struct A a = {
        .bf = false,
        .bt = true,

        .i8M = INT8_MAX,
        .i8m = INT8_MIN,
        .i8n = -100,
        .i16M = INT16_MAX,
        .i16m = INT16_MIN,
        .i16n = -1000,
        .i32M = INT32_MAX,
        .i32m = INT32_MIN,
        .i32n = -10000,
        .i64M = INT64_MAX,
        .i64m = INT64_MIN,
        .i64n = -100000,

        .u8M = UINT8_MAX,
        .u8n = 100,
        .u16M = UINT16_MAX,
        .u16n = 1000,
        .u32M = UINT32_MAX,
        .u32n = 10000,
        .u64M = UINT64_MAX,
        .u64n = 100000,

        .fM = FLT_MAX,
        .fMn = -FLT_MAX,
        .fm = FLT_MIN,
        .fn = -1.5f,
        .dM = DBL_MAX,
        .dMn = -DBL_MAX,
        .dm = DBL_MIN,
        .dn = 6.5,
    };
    buffsize = 10;
    buff = cra_json_stringify_struct0(NULL, &buffsize, &a, meta_a, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(buff != NULL);

    printf("%s\n\n", buff);

    struct A *aa;
    cra_json_parse_struct0(buff, buffsize, &aa, sizeof(struct A), true, meta_a, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(a.bf == aa->bf);
    assert_always(a.bt == aa->bt);
    assert_always(a.i8M == aa->i8M);
    assert_always(a.i8m == aa->i8m);
    assert_always(a.i8n == aa->i8n);
    assert_always(a.i16M == aa->i16M);
    assert_always(a.i16m == aa->i16m);
    assert_always(a.i16n == aa->i16n);
    assert_always(a.i32M == aa->i32M);
    assert_always(a.i32m == aa->i32m);
    assert_always(a.i32n == aa->i32n);
    assert_always(a.i64M == aa->i64M);
    assert_always(a.i64m == aa->i64m);
    assert_always(a.i64n == aa->i64n);
    assert_always(a.u8M == aa->u8M);
    assert_always(a.u8n == aa->u8n);
    assert_always(a.u16M == aa->u16M);
    assert_always(a.u16n == aa->u16n);
    assert_always(a.u32M == aa->u32M);
    assert_always(a.u32n == aa->u32n);
    assert_always(a.u64M == aa->u64M);
    assert_always(a.u64n == aa->u64n);
    assert_always(cra_compare_float(a.fM, aa->fM) == 0);
    assert_always(cra_compare_float(a.fMn, aa->fMn) == 0);
    assert_always(cra_compare_float(a.fm, aa->fm) == 0);
    assert_always(cra_compare_float(a.fn, aa->fn) == 0);
    assert_always(cra_compare_double(a.dM, aa->dM) == 0);
    assert_always(cra_compare_double(a.dMn, aa->dMn) == 0);
    assert_always(cra_compare_double(a.dm, aa->dm) == 0);
    assert_always(cra_compare_double(a.dn, aa->dn) == 0);

    cra_free(aa);

    // 模拟少KV对和KV对顺序不对

    CRA_TYPE_META_BEGIN(meta_aa)
    CRA_TYPE_META_BOOL_MEMBER(struct A, bt)
    CRA_TYPE_META_BOOL_MEMBER(struct A, bf)

    CRA_TYPE_META_UINT32_MEMBER(struct A, u32M)
    CRA_TYPE_META_UINT32_MEMBER(struct A, u32n)
    CRA_TYPE_META_UINT64_MEMBER(struct A, u64M)
    CRA_TYPE_META_UINT64_MEMBER(struct A, u64n)

    CRA_TYPE_META_INT8_MEMBER(struct A, i8M)
    CRA_TYPE_META_INT8_MEMBER(struct A, i8n)
    CRA_TYPE_META_INT16_MEMBER(struct A, i16M)
    CRA_TYPE_META_INT16_MEMBER(struct A, i16m)
    CRA_TYPE_META_INT16_MEMBER(struct A, i16n)
    CRA_TYPE_META_INT32_MEMBER(struct A, i32M)
    CRA_TYPE_META_INT32_MEMBER(struct A, i32m)
    CRA_TYPE_META_INT32_MEMBER(struct A, i32n)

    CRA_TYPE_META_FLOAT_MEMBER(struct A, fm)
    CRA_TYPE_META_FLOAT_MEMBER(struct A, fn)
    CRA_TYPE_META_DOUBLE_MEMBER(struct A, dn)
    CRA_TYPE_META_END();

    cra_json_parse_struct0(buff, buffsize, &aa, sizeof(struct A), true, meta_aa, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(a.bf == aa->bf);
    assert_always(a.bt == aa->bt);
    assert_always(a.i8M == aa->i8M);
    assert_always(0 == aa->i8m);
    assert_always(a.i8n == aa->i8n);
    assert_always(a.i16M == aa->i16M);
    assert_always(a.i16m == aa->i16m);
    assert_always(a.i16n == aa->i16n);
    assert_always(a.i32M == aa->i32M);
    assert_always(a.i32m == aa->i32m);
    assert_always(a.i32n == aa->i32n);
    assert_always(0 == aa->i64M);
    assert_always(0 == aa->i64m);
    assert_always(0 == aa->i64n);
    assert_always(0 == aa->u8M);
    assert_always(0 == aa->u8n);
    assert_always(0 == aa->u16M);
    assert_always(0 == aa->u16n);
    assert_always(a.u32M == aa->u32M);
    assert_always(a.u32n == aa->u32n);
    assert_always(a.u64M == aa->u64M);
    assert_always(a.u64n == aa->u64n);
    assert_always(0 == aa->fM);
    assert_always(cra_compare_float(a.fm, aa->fm) == 0);
    assert_always(cra_compare_float(a.fn, aa->fn) == 0);
    assert_always(0 == aa->dM);
    assert_always(0 == aa->dm);
    assert_always(cra_compare_double(a.dn, aa->dn) == 0);

    cra_free(aa);

    cra_free(buff);
}

#if 1 // number

struct D
{
    double M;
    double N;
    double m;
    double zp;
    double zn;
    double z;
    double p;
    double n;
};
CRA_TYPE_META_BEGIN(meta_D)
CRA_TYPE_META_DOUBLE_MEMBER(struct D, M)
CRA_TYPE_META_DOUBLE_MEMBER(struct D, N)
CRA_TYPE_META_DOUBLE_MEMBER(struct D, m)
CRA_TYPE_META_DOUBLE_MEMBER(struct D, zp)
CRA_TYPE_META_DOUBLE_MEMBER(struct D, zn)
CRA_TYPE_META_DOUBLE_MEMBER(struct D, z)
CRA_TYPE_META_DOUBLE_MEMBER(struct D, p)
CRA_TYPE_META_DOUBLE_MEMBER(struct D, n)
CRA_TYPE_META_END();

struct I
{
    int64_t M;
    int64_t N;
    int64_t m;
    int64_t zp;
    int64_t zn;
    int64_t z;
    int64_t p;
    int64_t n;
};
CRA_TYPE_META_BEGIN(meta_I)
CRA_TYPE_META_INT64_MEMBER(struct I, M)
CRA_TYPE_META_INT64_MEMBER(struct I, N)
CRA_TYPE_META_INT64_MEMBER(struct I, m)
CRA_TYPE_META_INT64_MEMBER(struct I, zp)
CRA_TYPE_META_INT64_MEMBER(struct I, zn)
CRA_TYPE_META_INT64_MEMBER(struct I, z)
CRA_TYPE_META_INT64_MEMBER(struct I, p)
CRA_TYPE_META_INT64_MEMBER(struct I, n)
CRA_TYPE_META_END();

void test_d2i(void)
{
    struct D d;
    struct I i;
    unsigned char buff[1024];
    size_t buffsize;
    CraSerError_e error;

    d.M = DBL_MAX;
    d.N = -DBL_MAX;
    d.m = DBL_MIN;
    d.zp = +0.0;
    d.zn = -0.0;
    d.z = 0.0;
    d.p = 100.5;
    d.n = -60.8;

    buffsize = sizeof(buff);
    cra_json_stringify_struct0(buff, &buffsize, &d, meta_D, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    printf("%s\n\n", buff);

    cra_json_parse_struct0(buff, buffsize, &i, sizeof(i), false, meta_I, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(i.M == CRA_MAX_SAFE_INT);
    assert_always(i.N == CRA_MIN_SAFE_INT);
    assert_always(i.m == 0);
    assert_always(i.zp == 0);
    assert_always(i.zn == 0);
    assert_always(i.z == 0);
    assert_always(i.p == (int64_t)d.p);
    assert_always(i.n == (int64_t)d.n);
}

void test_i2d(void)
{
    struct D d;
    struct I i;
    unsigned char buff[1024];
    size_t buffsize;
    CraSerError_e error;

    i.M = INT64_MAX;
    i.N = -INT64_MAX;
    i.m = INT64_MIN;
    i.zp = +0;
    i.zn = -0;
    i.z = 0;
    i.p = 100;
    i.n = -60;

    buffsize = sizeof(buff);
    cra_json_stringify_struct0(buff, &buffsize, &i, meta_I, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    printf("%s\n\n", buff);

    cra_json_parse_struct0(buff, buffsize, &d, sizeof(d), false, meta_D, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(cra_compare_double(d.M, (double)INT64_MAX) == 0);
    assert_always(cra_compare_double(d.N, (double)-INT64_MAX) == 0);
    assert_always(cra_compare_double(d.m, (double)INT64_MIN) == 0);
    assert_always(d.zp == 0.0);
    assert_always(d.zn == 0.0);
    assert_always(d.z == 0.0);
    assert_always(d.p == (double)i.p);
    assert_always(d.n == (double)i.n);
}

#endif // end number

void test_string(void)
{
    struct A
    {
        char *strpempty;
        char straempty[1];
    };
    CRA_TYPE_META_BEGIN(meta_a)
    CRA_TYPE_META_STRING_MEMBER(struct A, strpempty, true)
    CRA_TYPE_META_STRING_MEMBER(struct A, straempty, false)
    CRA_TYPE_META_END();

    unsigned char buff[1024];
    CraSerError_e error;
    size_t buffsize;

    // test empty string

    struct A a = {"", ""};

    buffsize = sizeof(buff);
    cra_json_stringify_struct0(buff, &buffsize, &a, meta_a, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    printf("%s\n\n", buff);

    struct A aa;
    cra_json_parse_struct0(buff, buffsize, &aa, sizeof(struct A), false, meta_a, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(strcmp(a.strpempty, aa.strpempty) == 0);
    assert_always(strcmp(a.straempty, aa.straempty) == 0);

    cra_free(aa.strpempty);

    // other

    struct B
    {
        char *strp;
        char stra[20];
        char *strnull;
        char *chinese;
        char *emoji;
        char escape[12];
    };
    CRA_TYPE_META_BEGIN(meta_b)
    CRA_TYPE_META_STRING_MEMBER(struct B, strp, true)
    CRA_TYPE_META_STRING_MEMBER(struct B, stra, false)
    CRA_TYPE_META_STRING_MEMBER(struct B, strnull, true)
    CRA_TYPE_META_STRING_MEMBER(struct B, chinese, true)
    CRA_TYPE_META_STRING_MEMBER(struct B, emoji, true)
    CRA_TYPE_META_STRING_MEMBER(struct B, escape, false)
    CRA_TYPE_META_END();

    struct B b = {
        "string (char pointer)",
        "string (char array)",
        NULL, // string null (pointer)
        "中文",
        "😀",
        {1, 2, 3, 4, '\b', '\t', '\n', '\f', '\r', '\"', '\\', '\0'},
    };

    buffsize = sizeof(buff);
    cra_json_stringify_struct0(buff, &buffsize, &b, meta_b, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    printf("%s\n\n", (char *)buff);

    // 1 normal
    struct B bb = {"hello", "a", NULL, NULL, NULL, ""};
    cra_json_parse_struct0(buff, buffsize, &bb, sizeof(struct B), false, meta_b, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(strcmp(b.strp, bb.strp) == 0);
    assert_always(strcmp(b.stra, bb.stra) == 0);
    assert_always(b.strnull == bb.strnull);
    assert_always(strcmp(b.chinese, bb.chinese) == 0);
    assert_always(strcmp(b.emoji, bb.emoji) == 0);
    assert_always(strcmp(b.escape, bb.escape) == 0);
    cra_free(bb.strp);
    cra_free(bb.chinese);
    cra_free(bb.emoji);

    // 2 char * <-> char[N]
    struct C
    {
        char strp[22];
        char *stra;
    };
    CRA_TYPE_META_BEGIN(meta_c)
    CRA_TYPE_META_STRING_MEMBER(struct C, strp, false)
    CRA_TYPE_META_STRING_MEMBER(struct C, stra, true)
    CRA_TYPE_META_END();

    struct C *c;
    cra_json_parse_struct0(buff, buffsize, &c, sizeof(struct C), true, meta_c, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(strcmp(b.strp, c->strp) == 0);
    assert_always(strcmp(b.stra, c->stra) == 0);
    cra_free(c->stra);
    cra_free(c);

    // 3 null -> char[N] !! ERROR
    struct D
    {
        char strnull[20];
    };
    CRA_TYPE_META_BEGIN(meta_d)
    CRA_TYPE_META_STRING_MEMBER(struct D, strnull, false)
    CRA_TYPE_META_END();

    struct D *d;
    cra_json_parse_struct0(buff, buffsize, &d, sizeof(struct D), true, meta_d, NULL, NULL, &error);
    assert_always(error != CRA_SER_ERROR_SUCCESS);

    // 4 string array is too small
    struct E
    {
        char strp[21];
    };
    CRA_TYPE_META_BEGIN(meta_e)
    CRA_TYPE_META_STRING_MEMBER(struct E, strp, false)
    CRA_TYPE_META_END();

    struct E *e;
    cra_json_parse_struct0(buff, buffsize, &e, sizeof(struct E), true, meta_e, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_STRING_BUF_TOO_SMALL);
}

void test_string_unicode(void)
{
    struct A
    {
        char *emoji;
        char *chinese;
    };
    CRA_TYPE_META_BEGIN(meta_a)
    CRA_TYPE_META_STRING_MEMBER(struct A, emoji, true)
    CRA_TYPE_META_STRING_MEMBER(struct A, chinese, true)
    CRA_TYPE_META_END();

    char json[] = "{"
                  "\"emoji\":\"\\uD83D\\uDE00\\/\","
                  "\"chinese\": \"\\u4f60\\u597d，\\u4e16\\u754c\""
                  "}";
    CraSerError_e error;
    struct A *a;

    cra_json_parse_struct0((unsigned char *)json, sizeof(json), &a, sizeof(struct A), true, meta_a, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("{\"emoji\": \"%s\", \"chinese\": \"%s\"}\n\n", a->emoji, a->chinese);
    assert_always(strcmp(a->emoji, "😀/") == 0);
    assert_always(strcmp(a->chinese, "你好，世界") == 0);
    cra_free(a->emoji);
    cra_free(a->chinese);
    cra_free(a);

    // ---------------

    struct B
    {
        char emoji[6];
        char chinese[16];
    };
    CRA_TYPE_META_BEGIN(meta_b)
    CRA_TYPE_META_STRING_MEMBER(struct B, emoji, false)
    CRA_TYPE_META_STRING_MEMBER(struct B, chinese, false)
    CRA_TYPE_META_END();

    struct B *b;

    cra_json_parse_struct0((unsigned char *)json, sizeof(json), &b, sizeof(struct B), true, meta_b, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("{\"emoji\": \"%s\", \"chinese\": \"%s\"}\n\n", b->emoji, b->chinese);
    assert_always(strcmp(b->emoji, "😀/") == 0);
    assert_always(strcmp(b->chinese, "你好，世界") == 0);
    cra_free(b);
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

    CraSerError_e error;
    unsigned char buff[1024];
    size_t buffsize;

    struct A a = {100, 3.4};

    buffsize = sizeof(buff);
    cra_json_stringify_struct0(buff, &buffsize, &a, meta_a, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    printf("%s\n\n", buff);

    // 1 normal
    struct A aa;
    cra_json_parse_struct0(buff, buffsize, &aa, sizeof(struct A), false, meta_a, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(a.i == aa.i);
    assert_always(a.d == aa.d);

    // 2 struct X -> struct X *
    struct A *ap;
    cra_json_parse_struct0(buff, buffsize, &ap, sizeof(struct A), true, meta_a, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(a.i == ap->i);
    assert_always(a.d == ap->d);
    cra_free(ap);

    // 2 null -> struct X !! ERROR
    struct B
    {
        struct A *null;
    };
    CRA_TYPE_META_BEGIN(meta_b)
    CRA_TYPE_META_STRUCT_MEMBER(struct B, null, true, meta_a, NULL, NULL)
    CRA_TYPE_META_END();
    struct BB
    {
        struct A null;
    };
    CRA_TYPE_META_BEGIN(meta_bb)
    CRA_TYPE_META_STRUCT_MEMBER(struct BB, null, false, meta_a, NULL, NULL)
    CRA_TYPE_META_END();

    struct B b, *bb;

    b.null = NULL;
    cra_json_stringify_struct0(buff, &buffsize, &b, meta_b, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("%s\n\n", buff);

    cra_json_parse_struct0(buff, buffsize, &bb, sizeof(struct B), true, meta_bb, NULL, NULL, &error);
    assert_always(error != CRA_SER_ERROR_SUCCESS);
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
    cra_json_stringify_struct0(buff, &buffsize, &b, meta_b, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    printf("%s\n\n", buff);

    struct B bs;
    cra_json_parse_struct0(buff, buffsize, &bs, sizeof(struct B), false, meta_b, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(bs.anull == b.anull && bs.anull == NULL);
    assert_always(bs.ap->i == b.ap->i);
    assert_always(bs.as.i == b.as.i);
    cra_free(bs.ap);

    struct B *bp;
    cra_json_parse_struct0(buff, buffsize, &bp, sizeof(struct B), true, meta_b, NULL, NULL, &error);
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
    cra_json_stringify_struct0(buff, &buffsize, x, meta_initx, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("%s\n\n", buff);

    struct InitX *xx;
    cra_json_parse_struct0(buff, buffsize, &xx, sizeof(struct InitX), true, meta_initx, &initx_i, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(x->i == xx->i);
    assert_always(xx->f == 1.5f); // default value (from init_initx)
    assert_always(x->d == xx->d);
    assert_always(strcmp(x->str, xx->str) == 0);
    cra_free(xx->str);
    cra_free(xx);

    // test failed to call uninit_initx
    calledcnt = 0;
    buff[buffsize - 2] = '\0'; // '}' -> '\0', parse failed.
    cra_json_parse_struct0(buff, buffsize, &xx, sizeof(struct InitX), true, meta_initx, &initx_i, NULL, &error);
    assert_always(error != CRA_SER_ERROR_SUCCESS);
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
    assert_always(alist->count == 0);
    cra_json_stringify_list0(buff, &buffsize, alist, meta_list_val, &g_alist_ser_iter_i, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("%s\n\n", buff);

    cra_json_parse_list0(buff, buffsize, &alist2, sizeof(CraAList), true, meta_list_val, &g_alist_ser_iter_i, &g_alist_ser_init_i, &args4alist, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(alist2->count == 0);
    cra_alist_uninit(alist2);
    cra_dealloc(alist2);

    // normal

    // add vals
    for (int i = 0; i < 10; i++)
        cra_alist_append(alist, &i);

    buffsize = sizeof(buff);
    cra_json_stringify_list0(buff, &buffsize, alist, meta_list_val, &g_alist_ser_iter_i, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    printf("%s\n\n", buff);

    // alist -> alist
    cra_json_parse_list0(buff, buffsize, &alist2, sizeof(CraAList), true, meta_list_val, &g_alist_ser_iter_i, &g_alist_ser_init_i, &args4alist, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    idx = 0;
    for (CraAListIter it = cra_alist_iter_init(alist2); cra_alist_iter_next(&it, (void **)&valptr); idx++)
    {
        cra_alist_get(alist, idx, &val);
        assert_always(val == *valptr);
    }

    // alist -> llist
    cra_json_parse_list0(buff, buffsize, &llist, sizeof(CraLList), false, meta_list_val, &g_llist_ser_iter_i, &g_llist_ser_init_i, &args4llist, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    idx = 0;
    for (CraLListIter it = cra_llist_iter_init(&llist); cra_llist_iter_next(&it, (void **)&valptr); idx++)
    {
        cra_alist_get(alist, idx, &val);
        assert_always(val == *valptr);
    }

    // alist -> deque
    cra_json_parse_list0(buff, buffsize, &deque, sizeof(CraDeque), false, meta_list_val, &g_deque_ser_iter_i, &g_deque_ser_init_i, &args4deque, &error);
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
    cra_json_parse_array0(buff, buffsize, &array, sizeof(array), true, &narray, meta_list_val, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(narray == alist->count);
    for (idx = 0; idx < (int32_t)narray; idx++)
    {
        cra_alist_get(alist, idx, &val);
        assert_always(val == array[idx]);
    }

    cra_alist_uninit(alist2);
    cra_dealloc(alist2);
    cra_llist_uninit(&llist);
    cra_deque_uninit(&deque);
    cra_free(array);
    cra_alist_uninit(alist);
    cra_dealloc(alist);
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
    cra_json_stringify_list0(buff, &buffsize, &list, meta_list_str, &g_alist_ser_iter_i, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("%s\n\n", buff);

    CraAList list2;
    cra_json_parse_list0(buff, buffsize, &list2, sizeof(CraAList), false, meta_list_str, &g_alist_ser_iter_i, &g_alist_ser_init_i, &args4list_str, &error);
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
    buff[buffsize - 2] = '\0';
    cra_json_parse_list0(buff, buffsize, &list2, sizeof(CraAList), false, meta_list_str, &g_alist_ser_iter_i, &g_alist_ser_init_i, &args4list_str, &error);
    assert_always(error != CRA_SER_ERROR_SUCCESS);

    cra_alist_uninit(&list);
}

void test_array(void)
{
    cra_ser_count_t narray;
    int32_t array[] = {1, 2, 3, 4};
    CRA_TYPE_META_BEGIN(meta_int32)
    CRA_TYPE_META_INT32_ELEMENT()
    CRA_TYPE_META_END();

    CraSerError_e error;
    unsigned char buff[1024];
    size_t buffsize;

    // test empty

    narray = 0;
    buffsize = sizeof(buff);
    assert_always(narray == 0);
    cra_json_stringify_array0(buff, &buffsize, array, narray, meta_int32, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("%s\n\n", buff);

    cra_ser_count_t narr;
    int32_t arr[10];
    cra_json_parse_array0(buff, buffsize, arr, sizeof(arr), false, &narr, meta_int32, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(narr == 0);

    // normal

    narray = sizeof(array) / sizeof(array[0]);
    buffsize = sizeof(buff);
    cra_json_stringify_array0(buff, &buffsize, array, narray, meta_int32, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("%s\n\n", buff);

    int32_t *array2;
    cra_ser_count_t narray2;
    cra_json_parse_array0(buff, buffsize, &array2, 0, true, &narray2, meta_int32, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(narray == narray2);
    for (cra_ser_count_t i = 0; i < narray; i++)
    {
        assert_always(array[i] == array2[i]);
    }
    cra_free(array2);

    int32_t array3[10];
    cra_ser_count_t narray3;
    cra_json_parse_array0(buff, buffsize, array3, sizeof(array3), false, &narray3, meta_int32, &error);
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
    cra_json_parse_list0(buff, buffsize, &list, sizeof(*list), true, meta_int32, &g_alist_ser_iter_i, &g_alist_ser_init_i, &args_list_i, &error);
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
        uint32_t narray;
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
    for (uint32_t i = 0; i < a.narray; i++)
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
    cra_json_stringify_struct0(buff, &buffsize, &a, meta_a, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("%s\n\n", buff);

    struct A *aa;
    cra_json_parse_struct0(buff, buffsize, &aa, sizeof(struct A), true, meta_a, NULL, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(a.narray == aa->narray);
    for (uint32_t i = 0; i < a.narray; i++)
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
    assert_always(dict.count == 0);
    cra_json_stringify_dict0(buff, &buffsize, &dict, meta_dict_kv, &g_dict_ser_iter_i, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("%s\n\n", buff);

    cra_json_parse_dict0(buff, buffsize, &dict2, sizeof(CraDict), true, meta_dict_kv, &g_dict_ser_iter_i, &g_dict_ser_init_i, &args4dict, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(dict2->count == 0);
    cra_dict_uninit(dict2);
    cra_dealloc(dict2);

    char cval[] = "hello x";
    for (int32_t i = 0; i < 10; i++)
    {
        cval[6] = '0' + (char)i;
        cra_dict_add(&dict, &i, &cval);
    }

    buffsize = sizeof(buff);
    cra_json_stringify_dict0(buff, &buffsize, &dict, meta_dict_kv, &g_dict_ser_iter_i, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("%s\n\n", buff);

    // dict -> dict
    cra_json_parse_dict0(buff, buffsize, &dict2, sizeof(CraDict), true, meta_dict_kv, &g_dict_ser_iter_i, &g_dict_ser_init_i, &args4dict, &error);
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
    cra_json_stringify_struct0(buff, &buffsize, &a, meta_a, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    printf("%s\n\n", buff);

    struct A aa;
    cra_json_parse_struct0(buff, buffsize, &aa, sizeof(struct A), false, meta_a, NULL, NULL, &error);
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

    cra_dict_uninit(&aa.sdict);
    cra_dict_uninit(aa.pdict);
    cra_dealloc(aa.pdict);
    cra_dict_uninit(&a.sdict);
    cra_dict_uninit(a.pdict);
    cra_dealloc(a.pdict);
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
CRA_TYPE_META_FLOAT_MEMBER(struct New, f) // 因为JSON有Key定位字段，所以meta的位置也可随意
CRA_TYPE_META_INT32_MEMBER(struct New, i)
CRA_TYPE_META_STRING_MEMBER(struct New, str, true)
CRA_TYPE_META_END();

void test_new2old(void)
{
    unsigned char buff[1024];
    size_t buffsize;
    CraSerError_e error;

    struct New n = {1.5f, 100, "hello world"};
    buffsize = sizeof(buff);
    cra_json_stringify_struct0(buff, &buffsize, &n, meta_new, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    printf("%s\n\n", buff);

    struct Old o;
    cra_json_parse_struct0(buff, buffsize, &o, sizeof(o), false, meta_old, NULL, NULL, &error);
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
    cra_json_stringify_struct0(buff, &buffsize, &o, meta_old, FORMAT, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);

    printf("%s\n\n", buff);

    struct New n;
    const CraTypeInit_i init_i = {0, 0, (void (*)(void *, void *))init_new, 0};
    cra_json_parse_struct0(buff, buffsize, &n, sizeof(n), false, meta_new, &init_i, NULL, &error);
    assert_always(error == CRA_SER_ERROR_SUCCESS);
    assert_always(o.i == n.i);
    assert_always(strcmp(o.str, n.str) == 0);
    assert_always(cra_compare_float(n.f, 2.8f) == 0);

    cra_free(n.str);
}

#endif

int main(void)
{
    test_base();
    test_d2i();
    test_i2d();
    test_string();
    test_string_unicode();
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

    cra_memory_leak_report(stdout);
    return 0;
}
