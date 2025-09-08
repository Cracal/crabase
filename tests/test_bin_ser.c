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
#include "collections/cra_alist.h"
#include "collections/cra_deque.h"
#include "collections/cra_dict.h"
#include "collections/cra_llist.h"
#include "cra_assert.h"
#include "cra_malloc.h"
#include "serialize/cra_bin_ser.h"
#include <float.h>

static void
test_boolean(void)
{
    unsigned char *buf;
    size_t         bufsize;
    CraSerError_e  error;
    bool           b;

    struct BS
    {
        bool btrue;
        bool bfalse;
        bool btrue_not_1;
    };
    CRA_TYPE_META_BEGIN(bs_meta)
    CRA_TYPE_META_MEMBER_BOOL(struct BS, btrue)
    CRA_TYPE_META_MEMBER_BOOL(struct BS, bfalse)
    CRA_TYPE_META_MEMBER_BOOL(struct BS, btrue_not_1)
    CRA_TYPE_META_END();

    struct BS bs = { .btrue = true, .bfalse = false, .btrue_not_1 = 2 };

    bufsize = 5;
    buf = cra_bin_serialize_struct(NULL, &bufsize, &bs, bs_meta, &error, 0x11223344);
    assert_always(buf && error == CRA_SER_ERROR_SUCCESS);

    struct BS rbs, *prbs;
    b = cra_bin_deserialize_struct(buf, bufsize, &rbs, sizeof(rbs), false, bs_meta, &error, 0x11223344);
    assert_always(b && !error);
    assert_always(bs.btrue == rbs.btrue);
    assert_always(bs.bfalse == rbs.bfalse);
    assert_always(rbs.btrue_not_1 == true);
    assert_always(b && !error);
    b = cra_bin_deserialize_struct(buf, bufsize, &prbs, sizeof(*prbs), true, bs_meta, &error, 0x11223344);
    assert_always(b && !error);
    assert_always(bs.btrue == prbs->btrue);
    assert_always(bs.bfalse == prbs->bfalse);
    assert_always(prbs->btrue_not_1 == true);
    assert_always(b && !error);

    cra_free(prbs);
    cra_free(buf);
}

static void
test_int(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    struct IS
    {
        int8_t  i8n;
        int8_t  i8m;
        int8_t  i8M;
        int16_t i16n;
        int16_t i16m;
        int16_t i16M;
        int32_t i32n;
        int32_t i32m;
        int32_t i32M;
        int64_t i64n;
        int64_t i64m;
        int64_t i64M;
    };
    CRA_TYPE_META_BEGIN(is_meta)
    CRA_TYPE_META_MEMBER_INT(struct IS, i8n)
    CRA_TYPE_META_MEMBER_INT(struct IS, i8m)
    CRA_TYPE_META_MEMBER_INT(struct IS, i8M)
    CRA_TYPE_META_MEMBER_INT(struct IS, i16n)
    CRA_TYPE_META_MEMBER_INT(struct IS, i16m)
    CRA_TYPE_META_MEMBER_INT(struct IS, i16M)
    CRA_TYPE_META_MEMBER_INT(struct IS, i32n)
    CRA_TYPE_META_MEMBER_INT(struct IS, i32m)
    CRA_TYPE_META_MEMBER_INT(struct IS, i32M)
    CRA_TYPE_META_MEMBER_INT(struct IS, i64n)
    CRA_TYPE_META_MEMBER_INT(struct IS, i64m)
    CRA_TYPE_META_MEMBER_INT(struct IS, i64M)
    CRA_TYPE_META_END();

    struct IS *is = cra_alloc(struct IS);
    is->i8n = -8;
    is->i8m = INT8_MIN;
    is->i8M = INT8_MAX;
    is->i16n = -16;
    is->i16m = INT16_MIN;
    is->i16M = INT16_MAX;
    is->i32n = -32;
    is->i32m = INT32_MIN;
    is->i32M = INT32_MAX;
    is->i64n = -64;
    is->i64m = INT64_MIN;
    is->i64M = INT64_MAX;

    bufsize = sizeof(buf);
    cra_bin_serialize_struct(buf, &bufsize, is, is_meta, &error, 0x55667788);
    assert_always(!error);

    struct IS ris, *pris;
    b = cra_bin_deserialize_struct(buf, bufsize, &ris, sizeof(ris), false, is_meta, &error, 0x55667788);
    assert_always(b && !error);
    assert_always(is->i8n == ris.i8n);
    assert_always(is->i8m == ris.i8m);
    assert_always(is->i8M == ris.i8M);
    assert_always(is->i16n == ris.i16n);
    assert_always(is->i16m == ris.i16m);
    assert_always(is->i16M == ris.i16M);
    assert_always(is->i32n == ris.i32n);
    assert_always(is->i32m == ris.i32m);
    assert_always(is->i32M == ris.i32M);
    assert_always(is->i64n == ris.i64n);
    assert_always(is->i64m == ris.i64m);
    assert_always(is->i64M == ris.i64M);
    b = cra_bin_deserialize_struct(buf, bufsize, &pris, sizeof(*pris), true, is_meta, &error, 0x55667788);
    assert_always(b && !error);
    assert_always(is->i8n == pris->i8n);
    assert_always(is->i8m == pris->i8m);
    assert_always(is->i8M == pris->i8M);
    assert_always(is->i16n == pris->i16n);
    assert_always(is->i16m == pris->i16m);
    assert_always(is->i16M == pris->i16M);
    assert_always(is->i32n == pris->i32n);
    assert_always(is->i32m == pris->i32m);
    assert_always(is->i32M == pris->i32M);
    assert_always(is->i64n == pris->i64n);
    assert_always(is->i64m == pris->i64m);
    assert_always(is->i64M == pris->i64M);

    cra_dealloc(is);
    cra_free(pris);
}

static void
test_uint(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    struct US
    {
        uint8_t  u8n;
        uint8_t  u8M;
        uint16_t u16n;
        uint16_t u16M;
        uint32_t u32n;
        uint32_t u32M;
        uint64_t u64n;
        uint64_t u64M;
    };
    CRA_TYPE_META_BEGIN(us_meta)
    CRA_TYPE_META_MEMBER_UINT(struct US, u8n)
    CRA_TYPE_META_MEMBER_UINT(struct US, u8M)
    CRA_TYPE_META_MEMBER_UINT(struct US, u16n)
    CRA_TYPE_META_MEMBER_UINT(struct US, u16M)
    CRA_TYPE_META_MEMBER_UINT(struct US, u32n)
    CRA_TYPE_META_MEMBER_UINT(struct US, u32M)
    CRA_TYPE_META_MEMBER_UINT(struct US, u64n)
    CRA_TYPE_META_MEMBER_UINT(struct US, u64M)
    CRA_TYPE_META_END();

    struct US *us = cra_alloc(struct US);
    us->u8n = 8;
    us->u8M = UINT8_MAX;
    us->u16n = 16;
    us->u16M = UINT16_MAX;
    us->u32n = 32;
    us->u32M = UINT32_MAX;
    us->u64n = 64;
    us->u64M = UINT64_MAX;

    bufsize = sizeof(buf);
    cra_bin_serialize_struct(buf, &bufsize, us, us_meta, &error, 0x55667788);
    assert_always(!error);

    struct US rus, *prus;
    b = cra_bin_deserialize_struct(buf, bufsize, &rus, sizeof(rus), false, us_meta, &error, 0x55667788);
    assert_always(b && !error);
    assert_always(us->u8n == rus.u8n);
    assert_always(us->u8M == rus.u8M);
    assert_always(us->u16n == rus.u16n);
    assert_always(us->u16M == rus.u16M);
    assert_always(us->u32n == rus.u32n);
    assert_always(us->u32M == rus.u32M);
    assert_always(us->u64n == rus.u64n);
    assert_always(us->u64M == rus.u64M);
    b = cra_bin_deserialize_struct(buf, bufsize, &prus, sizeof(*prus), true, us_meta, &error, 0x55667788);
    assert_always(b && !error);
    assert_always(us->u8n == prus->u8n);
    assert_always(us->u8M == prus->u8M);
    assert_always(us->u16n == prus->u16n);
    assert_always(us->u16M == prus->u16M);
    assert_always(us->u32n == prus->u32n);
    assert_always(us->u32M == prus->u32M);
    assert_always(us->u64n == prus->u64n);
    assert_always(us->u64M == prus->u64M);

    cra_dealloc(us);
    cra_free(prus);
}

static void
test_varint(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    struct VIS
    {
        int8_t  i8n;
        int8_t  i8m;
        int8_t  i8M;
        int16_t i16n;
        int16_t i16m;
        int16_t i16M;
        int32_t i32n;
        int32_t i32m;
        int32_t i32M;
        int64_t i64n;
        int64_t i64m;
        int64_t i64M;
    };
    CRA_TYPE_META_BEGIN(is_meta)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i8n)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i8m)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i8M)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i16n)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i16m)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i16M)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i32n)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i32m)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i32M)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i64n)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i64m)
    CRA_TYPE_META_MEMBER_VARINT(struct VIS, i64M)
    CRA_TYPE_META_END();

    struct VIS *is = cra_alloc(struct VIS);
    is->i8n = -8;
    is->i8m = INT8_MIN;
    is->i8M = INT8_MAX;
    is->i16n = -16;
    is->i16m = INT16_MIN;
    is->i16M = INT16_MAX;
    is->i32n = -32;
    is->i32m = INT32_MIN;
    is->i32M = INT32_MAX;
    is->i64n = -64;
    is->i64m = INT64_MIN;
    is->i64M = INT64_MAX;

    bufsize = sizeof(buf);
    cra_bin_serialize_struct(buf, &bufsize, is, is_meta, &error, 0x55667788);
    assert_always(!error);

    struct VIS ris, *pris;
    b = cra_bin_deserialize_struct(buf, bufsize, &ris, sizeof(ris), false, is_meta, &error, 0x55667788);
    assert_always(b && !error);
    assert_always(is->i8n == ris.i8n);
    assert_always(is->i8m == ris.i8m);
    assert_always(is->i8M == ris.i8M);
    assert_always(is->i16n == ris.i16n);
    assert_always(is->i16m == ris.i16m);
    assert_always(is->i16M == ris.i16M);
    assert_always(is->i32n == ris.i32n);
    assert_always(is->i32m == ris.i32m);
    assert_always(is->i32M == ris.i32M);
    assert_always(is->i64n == ris.i64n);
    assert_always(is->i64m == ris.i64m);
    assert_always(is->i64M == ris.i64M);
    b = cra_bin_deserialize_struct(buf, bufsize, &pris, sizeof(*pris), true, is_meta, &error, 0x55667788);
    assert_always(b && !error);
    assert_always(is->i8n == pris->i8n);
    assert_always(is->i8m == pris->i8m);
    assert_always(is->i8M == pris->i8M);
    assert_always(is->i16n == pris->i16n);
    assert_always(is->i16m == pris->i16m);
    assert_always(is->i16M == pris->i16M);
    assert_always(is->i32n == pris->i32n);
    assert_always(is->i32m == pris->i32m);
    assert_always(is->i32M == pris->i32M);
    assert_always(is->i64n == pris->i64n);
    assert_always(is->i64m == pris->i64m);
    assert_always(is->i64M == pris->i64M);

    cra_dealloc(is);
    cra_free(pris);
}

static void
test_varuint(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    struct VUS
    {
        uint8_t  u8n;
        uint8_t  u8M;
        uint16_t u16n;
        uint16_t u16M;
        uint32_t u32n;
        uint32_t u32M;
        uint64_t u64n;
        uint64_t u64M;
    };
    CRA_TYPE_META_BEGIN(us_meta)
    CRA_TYPE_META_MEMBER_VARUINT(struct VUS, u8n)
    CRA_TYPE_META_MEMBER_VARUINT(struct VUS, u8M)
    CRA_TYPE_META_MEMBER_VARUINT(struct VUS, u16n)
    CRA_TYPE_META_MEMBER_VARUINT(struct VUS, u16M)
    CRA_TYPE_META_MEMBER_VARUINT(struct VUS, u32n)
    CRA_TYPE_META_MEMBER_VARUINT(struct VUS, u32M)
    CRA_TYPE_META_MEMBER_VARUINT(struct VUS, u64n)
    CRA_TYPE_META_MEMBER_VARUINT(struct VUS, u64M)
    CRA_TYPE_META_END();

    struct VUS *us = cra_alloc(struct VUS);
    us->u8n = 8;
    us->u8M = UINT8_MAX;
    us->u16n = 16;
    us->u16M = UINT16_MAX;
    us->u32n = 32;
    us->u32M = UINT32_MAX;
    us->u64n = 64;
    us->u64M = UINT64_MAX;

    bufsize = sizeof(buf);
    cra_bin_serialize_struct(buf, &bufsize, us, us_meta, &error, 0x55667788);
    assert_always(!error);

    struct VUS rus, *prus;
    b = cra_bin_deserialize_struct(buf, bufsize, &rus, sizeof(rus), false, us_meta, &error, 0x55667788);
    assert_always(b && !error);
    assert_always(us->u8n == rus.u8n);
    assert_always(us->u8M == rus.u8M);
    assert_always(us->u16n == rus.u16n);
    assert_always(us->u16M == rus.u16M);
    assert_always(us->u32n == rus.u32n);
    assert_always(us->u32M == rus.u32M);
    assert_always(us->u64n == rus.u64n);
    assert_always(us->u64M == rus.u64M);
    b = cra_bin_deserialize_struct(buf, bufsize, &prus, sizeof(*prus), true, us_meta, &error, 0x55667788);
    assert_always(b && !error);
    assert_always(us->u8n == prus->u8n);
    assert_always(us->u8M == prus->u8M);
    assert_always(us->u16n == prus->u16n);
    assert_always(us->u16M == prus->u16M);
    assert_always(us->u32n == prus->u32n);
    assert_always(us->u32M == prus->u32M);
    assert_always(us->u64n == prus->u64n);
    assert_always(us->u64M == prus->u64M);

    cra_dealloc(us);
    cra_free(prus);
}

static void
test_float(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    struct FS
    {
        float  fn;
        float  fm;
        float  fM;
        float  fW;
        double dn;
        double dm;
        double dM;
        double dW;
    };
    CRA_TYPE_META_BEGIN(fs_meta)
    CRA_TYPE_META_MEMBER_FLOAT(struct FS, fn)
    CRA_TYPE_META_MEMBER_FLOAT(struct FS, fm)
    CRA_TYPE_META_MEMBER_FLOAT(struct FS, fM)
    CRA_TYPE_META_MEMBER_FLOAT(struct FS, fW)
    CRA_TYPE_META_MEMBER_FLOAT(struct FS, dn)
    CRA_TYPE_META_MEMBER_FLOAT(struct FS, dm)
    CRA_TYPE_META_MEMBER_FLOAT(struct FS, dM)
    CRA_TYPE_META_MEMBER_FLOAT(struct FS, dW)
    CRA_TYPE_META_END();

    struct FS fs = {
        .fn = -1.5f,
        .fm = FLT_MIN,
        .fM = FLT_MAX,
        .fW = -FLT_MAX,
        .dn = 60.83,
        .dm = DBL_MIN,
        .dM = DBL_MAX,
        .dW = -DBL_MAX,
    };

    bufsize = sizeof(buf);
    cra_bin_serialize_struct(buf, &bufsize, &fs, fs_meta, &error, 0x99887766);
    assert_always(!error);

    struct FS rfs, *prfs;
    b = cra_bin_deserialize_struct(buf, bufsize, &rfs, sizeof(struct FS), false, fs_meta, &error, 0x99887766);
    assert_always(b && !error);
    assert_always(cra_compare_float(fs.fn, rfs.fn) == 0);
    assert_always(cra_compare_float(fs.fm, rfs.fm) == 0);
    assert_always(cra_compare_float(fs.fM, rfs.fM) == 0);
    assert_always(cra_compare_float(fs.fW, rfs.fW) == 0);
    assert_always(cra_compare_double(fs.dn, rfs.dn) == 0);
    assert_always(cra_compare_double(fs.dm, rfs.dm) == 0);
    assert_always(cra_compare_double(fs.dM, rfs.dM) == 0);
    assert_always(cra_compare_double(fs.dW, rfs.dW) == 0);
    b = cra_bin_deserialize_struct(buf, bufsize, &prfs, sizeof(struct FS), true, fs_meta, &error, 0x99887766);
    assert_always(b && !error);
    assert_always(cra_compare_float(fs.fn, prfs->fn) == 0);
    assert_always(cra_compare_float(fs.fm, prfs->fm) == 0);
    assert_always(cra_compare_float(fs.fM, prfs->fM) == 0);
    assert_always(cra_compare_float(fs.fW, prfs->fW) == 0);
    assert_always(cra_compare_double(fs.dn, prfs->dn) == 0);
    assert_always(cra_compare_double(fs.dm, prfs->dm) == 0);
    assert_always(cra_compare_double(fs.dM, prfs->dM) == 0);
    assert_always(cra_compare_double(fs.dW, prfs->dW) == 0);

    cra_free(prfs);
}

static void
test_string(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    struct SS
    {
        char  anormal[10];
        char  aempty[100];
        char *pnormal;
        char *pempty;
        char *pnull;
    };
    CRA_TYPE_META_BEGIN(ss_meta)
    CRA_TYPE_META_MEMBER_STRING(struct SS, anormal, false)
    CRA_TYPE_META_MEMBER_STRING(struct SS, aempty, false)
    CRA_TYPE_META_MEMBER_STRING(struct SS, pnormal, true)
    CRA_TYPE_META_MEMBER_STRING(struct SS, pempty, true)
    CRA_TYPE_META_MEMBER_STRING(struct SS, pnull, true)
    CRA_TYPE_META_END();

    struct SS ss = {
        .anormal = "hello",
        .aempty = "",
        .pnormal = "world",
        .pempty = "",
        .pnull = NULL,
    };

    bufsize = sizeof(buf);
    cra_bin_serialize_struct(buf, &bufsize, &ss, ss_meta, &error, 0x12345678);
    assert_always(!error);

    struct SS rss, *prss;
    b = cra_bin_deserialize_struct(buf, bufsize, &rss, sizeof(struct SS), false, ss_meta, &error, 0x12345678);
    assert_always(b && !error);
    assert_always(strcmp(ss.anormal, rss.anormal) == 0);
    assert_always(strcmp(ss.aempty, rss.aempty) == 0);
    assert_always(strcmp(ss.pnormal, rss.pnormal) == 0);
    assert_always(strcmp(ss.pempty, rss.pempty) == 0);
    assert_always(ss.pnull == rss.pnull && ss.pnull == NULL);
    b = cra_bin_deserialize_struct(buf, bufsize, &prss, sizeof(struct SS), true, ss_meta, &error, 0x12345678);
    assert_always(b && !error);
    assert_always(strcmp(ss.anormal, prss->anormal) == 0);
    assert_always(strcmp(ss.aempty, prss->aempty) == 0);
    assert_always(strcmp(ss.pnormal, prss->pnormal) == 0);
    assert_always(strcmp(ss.pempty, prss->pempty) == 0);
    assert_always(ss.pnull == prss->pnull && ss.pnull == NULL);

    cra_free(rss.pnormal);
    cra_free(rss.pempty);
    cra_free(prss->pnormal);
    cra_free(prss->pempty);
    cra_free(prss);
}

static void
test_bytes(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    struct SNZS
    {
        unsigned int   nanormal;
        char           anormal[10];
        size_t         naempty;
        char           aempty[100];
        unsigned short npnormal;
        char          *pnormal;
        unsigned char  npempty;
        char          *pempty;
        size_t         npnull;
        char          *pnull;
    };
    CRA_TYPE_META_BEGIN(ss_meta)
    CRA_TYPE_META_MEMBER_BYTES(struct SNZS, anormal, false)
    CRA_TYPE_META_MEMBER_BYTES(struct SNZS, aempty, false)
    CRA_TYPE_META_MEMBER_BYTES(struct SNZS, pnormal, true)
    CRA_TYPE_META_MEMBER_BYTES(struct SNZS, pempty, true)
    CRA_TYPE_META_MEMBER_BYTES(struct SNZS, pnull, true)
    CRA_TYPE_META_END();

    struct SNZS ss = {
        .nanormal = 5,
        .anormal = "HELLO",
        .naempty = 0,
        .aempty = "",
        .npnormal = 3,
        .pnormal = "WORLD",
        .npempty = 0,
        .pempty = "",
        .npnull = 0,
        .pnull = NULL,
    };

    bufsize = sizeof(buf);
    cra_bin_serialize_struct(buf, &bufsize, &ss, ss_meta, &error, 0x12345678);
    assert_always(!error);

    struct SNZS rss, *prss;
    b = cra_bin_deserialize_struct(buf, bufsize, &rss, sizeof(struct SNZS), false, ss_meta, &error, 0x12345678);
    assert_always(b && !error);
    assert_always(ss.nanormal == rss.nanormal);
    assert_always(strncmp(ss.anormal, rss.anormal, rss.nanormal) == 0);
    assert_always(ss.naempty == rss.naempty);
    assert_always(strncmp(ss.aempty, rss.aempty, rss.naempty) == 0);
    assert_always(ss.npnormal == rss.npnormal);
    assert_always(strncmp(ss.pnormal, rss.pnormal, rss.npnormal) == 0);
    assert_always(ss.npempty == rss.npempty);
    assert_always(strncmp(ss.pempty, rss.pempty, rss.npempty) == 0);
    assert_always(ss.npnull == rss.npnull);
    assert_always(ss.pnull == rss.pnull && ss.pnull == NULL);
    b = cra_bin_deserialize_struct(buf, bufsize, &prss, sizeof(struct SNZS), true, ss_meta, &error, 0x12345678);
    assert_always(b && !error);
    assert_always(ss.nanormal == prss->nanormal);
    assert_always(strncmp(ss.anormal, prss->anormal, prss->nanormal) == 0);
    assert_always(ss.naempty == prss->naempty);
    assert_always(strncmp(ss.aempty, prss->aempty, prss->naempty) == 0);
    assert_always(ss.npnormal == prss->npnormal);
    assert_always(strncmp(ss.pnormal, prss->pnormal, prss->npnormal) == 0);
    assert_always(ss.npempty == prss->npempty);
    assert_always(strncmp(ss.pempty, prss->pempty, prss->npempty) == 0);
    assert_always(ss.npnull == prss->npnull);
    assert_always(ss.pnull == prss->pnull && ss.pnull == NULL);

    cra_free(rss.pnormal);
    cra_free(rss.pempty);
    cra_free(prss->pnormal);
    cra_free(prss->pempty);
    cra_free(prss);
}

static void
test_struct(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    struct AS
    {
        int    i;
        double d;
    };
    CRA_TYPE_META_BEGIN(as_meta)
    CRA_TYPE_META_MEMBER_INT(struct AS, i)
    CRA_TYPE_META_MEMBER_FLOAT(struct AS, d)
    CRA_TYPE_META_END();
    struct BS
    {
        struct AS  sas;
        struct AS *oas;
        struct AS *oasnull;
    };
    CRA_TYPE_META_BEGIN(bs_meta)
    CRA_TYPE_META_MEMBER_STRUCT(struct BS, sas, false, as_meta)
    CRA_TYPE_META_MEMBER_STRUCT(struct BS, oas, true, as_meta)
    CRA_TYPE_META_MEMBER_STRUCT(struct BS, oasnull, true, as_meta)
    CRA_TYPE_META_END();

    struct AS as = { 200, -6.8 };
    struct BS bs = { .sas = { 100, 3.4 }, .oas = &as, .oasnull = NULL };

    bufsize = sizeof(buf);
    cra_bin_serialize_struct(buf, &bufsize, &bs, bs_meta, &error, 12345);
    assert_always(!error);

    struct BS rbs, *prbs;
    b = cra_bin_deserialize_struct(buf, bufsize, &rbs, sizeof(struct BS), false, bs_meta, &error, 12345);
    assert_always(b && !error);
    assert_always(bs.sas.i == rbs.sas.i);
    assert_always(cra_compare_double(bs.sas.d, rbs.sas.d) == 0);
    assert_always(bs.oas->i == rbs.oas->i);
    assert_always(cra_compare_double(bs.oas->d, rbs.oas->d) == 0);
    assert_always(bs.oasnull == rbs.oasnull && rbs.oasnull == NULL);
    b = cra_bin_deserialize_struct(buf, bufsize, &prbs, sizeof(struct BS), true, bs_meta, &error, 12345);
    assert_always(b && !error);
    assert_always(bs.sas.i == prbs->sas.i);
    assert_always(cra_compare_double(bs.sas.d, prbs->sas.d) == 0);
    assert_always(bs.oas->i == prbs->oas->i);
    assert_always(cra_compare_double(bs.oas->d, prbs->oas->d) == 0);
    assert_always(bs.oasnull == prbs->oasnull && prbs->oasnull == NULL);

    cra_free(rbs.oas);
    cra_free(prbs->oas);
    cra_free(prbs);
}

static void
test_list(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    struct LS
    {
        uint32_t  ni32array;
        int32_t   i32array[10]; // array<int32_t>
        uint32_t  nbarray;
        bool     *barray; // array<bool>
        CraAList  alist;  // list<int32_t>
        CraLList *llist;  // list<bool>
        CraDeque *deque;  // deque<uint64_t>

        uint16_t  narrayempty1;
        uint16_t  narrayempty2;
        int32_t   arrayempty1[10];
        int32_t  *arrayempty2;
        CraAList  alistempty1;
        CraAList *alistempty2;
        CraLList  llistempty1;
        CraLList *llistempty2;
        CraDeque  dequeempty1;
        CraDeque *dequeempty2;

        uint32_t  narraynull;
        int32_t  *arraynull;
        CraAList *alistnull;
        CraLList *llistnull;
        CraDeque *dequenull;
    };

    CRA_TYPE_META_BEGIN(i32_meta)
    CRA_TYPE_META_ITEM_INT(int32_t)
    CRA_TYPE_META_END();
    CRA_TYPE_META_BEGIN(b_meta)
    CRA_TYPE_META_ITEM_BOOL(bool)
    CRA_TYPE_META_END();
    CRA_TYPE_META_BEGIN(u64_meta)
    CRA_TYPE_META_ITEM_UINT(uint64_t)
    CRA_TYPE_META_END();

    CRA_TYPE_META_BEGIN(ls_meta)
    CRA_TYPE_META_MEMBER_ARRAY(struct LS, i32array, false, i32_meta)
    CRA_TYPE_META_MEMBER_ARRAY(struct LS, barray, true, b_meta)
    CRA_TYPE_META_MEMBER_LIST(struct LS, alist, false, i32_meta, CRA_ALIST_SZER_I, CRA_ALIST_DZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct LS, llist, true, b_meta, CRA_LLIST_SZER_I, CRA_LLIST_DZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct LS, deque, true, u64_meta, CRA_DEQUE_SZER_I, CRA_DEQUE_DZER_I, NULL)

    CRA_TYPE_META_MEMBER_ARRAY(struct LS, arrayempty1, false, i32_meta)
    CRA_TYPE_META_MEMBER_ARRAY(struct LS, arrayempty2, true, i32_meta)
    CRA_TYPE_META_MEMBER_LIST(struct LS, alistempty1, false, i32_meta, CRA_ALIST_SZER_I, CRA_ALIST_DZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct LS, alistempty2, true, i32_meta, CRA_ALIST_SZER_I, CRA_ALIST_DZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct LS, llistempty1, false, i32_meta, CRA_LLIST_SZER_I, CRA_LLIST_DZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct LS, llistempty2, true, i32_meta, CRA_LLIST_SZER_I, CRA_LLIST_DZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct LS, dequeempty1, false, i32_meta, CRA_DEQUE_SZER_I, CRA_DEQUE_DZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct LS, dequeempty2, true, i32_meta, CRA_DEQUE_SZER_I, CRA_DEQUE_DZER_I, NULL)

    CRA_TYPE_META_MEMBER_ARRAY(struct LS, arraynull, true, i32_meta)
    CRA_TYPE_META_MEMBER_LIST(struct LS, alistnull, true, i32_meta, CRA_ALIST_SZER_I, CRA_ALIST_DZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct LS, llistnull, true, i32_meta, CRA_LLIST_SZER_I, CRA_LLIST_DZER_I, NULL)
    CRA_TYPE_META_MEMBER_LIST(struct LS, dequenull, true, i32_meta, CRA_DEQUE_SZER_I, CRA_DEQUE_DZER_I, NULL)
    CRA_TYPE_META_END();

    struct LS ls;
    ls.ni32array = 8;
    ls.nbarray = 20;
    ls.barray = cra_malloc(sizeof(ls.barray[0]) * ls.nbarray);
    ls.llist = cra_alloc(CraLList);
    ls.deque = cra_alloc(CraDeque);
    ls.narrayempty1 = 0;
    ls.narrayempty2 = 0;
    ls.arrayempty2 = cra_malloc(sizeof(ls.arrayempty2[0]) * 8);
    ls.alistempty2 = cra_alloc(CraAList);
    ls.llistempty2 = cra_alloc(CraLList);
    ls.dequeempty2 = cra_alloc(CraDeque);
    cra_alist_init0(int32_t, &ls.alist, false);
    cra_llist_init0(bool, ls.llist, false);
    cra_deque_init0(uint64_t, ls.deque, CRA_DEQUE_INFINITE, false);
    bzero(ls.arrayempty1, sizeof(ls.arrayempty1));
    bzero(ls.arrayempty2, sizeof(ls.arrayempty2[0]) * 8);
    cra_alist_init0(int32_t, &ls.alistempty1, false);
    cra_alist_init0(int32_t, ls.alistempty2, false);
    cra_llist_init0(int32_t, &ls.llistempty1, false);
    cra_llist_init0(int32_t, ls.llistempty2, false);
    cra_deque_init0(int32_t, &ls.dequeempty1, CRA_DEQUE_INFINITE, false);
    cra_deque_init0(int32_t, ls.dequeempty2, CRA_DEQUE_INFINITE, false);
    ls.narraynull = 0;
    ls.arraynull = NULL;
    ls.alistnull = NULL;
    ls.llistnull = NULL;
    ls.dequenull = NULL;
    for (int i = 0; i < 30; i++)
    {
        if ((uint32_t)i < ls.ni32array)
            ls.i32array[i] = i * 32;
        if ((uint32_t)i < ls.nbarray)
            ls.barray[i] = i % 2 == 0;
        if (i < 15)
            cra_alist_append(&ls.alist, &(int32_t){ i - 18 });
        cra_llist_append(ls.llist, &(bool){ i % 7 == 0 });
        cra_deque_push(ls.deque, &(uint64_t){ i * 80 });
    }

    bufsize = sizeof(buf);
    cra_bin_serialize_struct(buf, &bufsize, &ls, ls_meta, &error, 57864);
    assert_always(!error);

    struct LS rls;
    b = cra_bin_deserialize_struct(buf, bufsize, &rls, sizeof(struct LS), false, ls_meta, &error, 57864);
    assert_always(b && !error);
    assert_always(ls.ni32array == rls.ni32array);
    assert_always(ls.nbarray == rls.nbarray);
    assert_always(ls.alist.count == rls.alist.count);
    assert_always(ls.llist->count == rls.llist->count);
    assert_always(ls.deque->count == rls.deque->count);
    assert_always(ls.narrayempty1 == rls.narrayempty1);
    assert_always(ls.narrayempty2 == rls.narrayempty2);
    assert_always(ls.alistempty1.count == rls.alistempty1.count);
    assert_always(ls.alistempty2->count == rls.alistempty2->count);
    assert_always(ls.llistempty1.count == rls.llistempty1.count);
    assert_always(ls.llistempty2->count == rls.llistempty2->count);
    assert_always(ls.dequeempty1.count == rls.dequeempty1.count);
    assert_always(ls.dequeempty2->count == rls.dequeempty2->count);
    assert_always(ls.narraynull == rls.narraynull);
    assert_always(ls.arraynull == rls.arraynull);
    assert_always(ls.alistnull == rls.alistnull);
    assert_always(ls.llistnull == rls.llistnull);
    assert_always(ls.dequenull == rls.dequenull);
    for (uint32_t i = 0; i < ls.ni32array; i++)
    {
        assert_always(ls.i32array[i] == rls.i32array[i]);
    }
    for (uint32_t i = 0; i < ls.nbarray; i++)
    {
        assert_always(ls.barray[i] == rls.barray[i]);
    }
    int32_t i321, i322;
    for (size_t i = 0; i < ls.alist.count; i++)
    {
        cra_alist_get(&ls.alist, i, &i321);
        cra_alist_get(&rls.alist, i, &i322);
        assert_always(i321 == i322);
    }
    bool b1, b2;
    for (size_t i = 0; i < ls.llist->count; i++)
    {
        cra_llist_get(ls.llist, i, &b1);
        cra_llist_get(rls.llist, i, &b2);
        assert_always(b1 == b2);
    }
    uint64_t u641, u642;
    for (size_t i = 0; i < ls.deque->count; i++)
    {
        cra_deque_get(ls.deque, i, &u641);
        cra_deque_get(rls.deque, i, &u642);
        assert_always(u641 == u642);
    }

    cra_free(ls.barray);
    cra_free(rls.barray);
    cra_alist_uninit(&ls.alist);
    cra_alist_uninit(&rls.alist);
    cra_llist_uninit(ls.llist);
    cra_llist_uninit(rls.llist);
    cra_dealloc(ls.llist);
    cra_dealloc(rls.llist);
    cra_deque_uninit(ls.deque);
    cra_deque_uninit(rls.deque);
    cra_dealloc(ls.deque);
    cra_dealloc(rls.deque);
    cra_free(ls.arrayempty2);
    cra_free(rls.arrayempty2);
    cra_alist_uninit(&ls.alistempty1);
    cra_alist_uninit(&rls.alistempty1);
    cra_alist_uninit(ls.alistempty2);
    cra_alist_uninit(rls.alistempty2);
    cra_dealloc(ls.alistempty2);
    cra_dealloc(rls.alistempty2);
    cra_llist_uninit(&ls.llistempty1);
    cra_llist_uninit(&rls.llistempty1);
    cra_llist_uninit(ls.llistempty2);
    cra_llist_uninit(rls.llistempty2);
    cra_dealloc(ls.llistempty2);
    cra_dealloc(rls.llistempty2);
    cra_deque_uninit(&ls.dequeempty1);
    cra_deque_uninit(&rls.dequeempty1);
    cra_deque_uninit(ls.dequeempty2);
    cra_deque_uninit(rls.dequeempty2);
    cra_dealloc(ls.dequeempty2);
    cra_dealloc(rls.dequeempty2);
}

static void
test_dict(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    struct DS
    {
        CraDict  dict;       // dict<int32_t, float>
        CraDict  dictempty1; // dict<float, uint64_t>
        CraDict *dictempty2; // dict<int32_t, float>
        CraDict *dictnull;   // dict<int32_t, float>
    };

    CRA_TYPE_META_BEGIN(i32_f_meta)
    CRA_TYPE_META_ITEM_INT(int32_t)
    CRA_TYPE_META_ITEM_FLOAT(float)
    CRA_TYPE_META_END();

    CRA_TYPE_META_BEGIN(f_u64_meta)
    CRA_TYPE_META_ITEM_FLOAT(float)
    CRA_TYPE_META_ITEM_INT(uint64_t)
    CRA_TYPE_META_END();

    const CraDictDzerArg key_i32_fns = {
        .hash = (cra_hash_fn)cra_hash_int32_t_p,
        .compare = (cra_compare_fn)cra_compare_int32_t_p,
    };
    const CraDictDzerArg key_flt_fns = {
        .hash = (cra_hash_fn)cra_hash_float_p,
        .compare = (cra_compare_fn)cra_compare_float_p,
    };

    CRA_TYPE_META_BEGIN(ds_meta)
    CRA_TYPE_META_MEMBER_DICT(struct DS, dict, false, i32_f_meta, CRA_DICT_SZER_I, CRA_DICT_DZER_I, &key_i32_fns)
    CRA_TYPE_META_MEMBER_DICT(struct DS, dictempty1, false, f_u64_meta, CRA_DICT_SZER_I, CRA_DICT_DZER_I, &key_flt_fns)
    CRA_TYPE_META_MEMBER_DICT(struct DS, dictempty2, true, i32_f_meta, CRA_DICT_SZER_I, CRA_DICT_DZER_I, &key_i32_fns)
    CRA_TYPE_META_MEMBER_DICT(struct DS, dictnull, true, i32_f_meta, CRA_DICT_SZER_I, CRA_DICT_DZER_I, &key_i32_fns)
    CRA_TYPE_META_END();

    struct DS ds;
    cra_dict_init0(int32_t, float, &ds.dict, false, key_i32_fns.hash, key_i32_fns.compare);
    cra_dict_init0(float, uint64_t, &ds.dictempty1, false, key_flt_fns.hash, key_flt_fns.compare);
    ds.dictempty2 = cra_alloc(CraDict);
    cra_dict_init0(int32_t, float, ds.dictempty2, false, key_i32_fns.hash, key_i32_fns.compare);
    ds.dictnull = NULL;
    for (int i = 0; i < 40; i++)
    {
        cra_dict_put0(&ds.dict, &(int32_t){ i }, &(float){ i + 89.5f });
    }

    bufsize = sizeof(buf);
    cra_bin_serialize_struct(buf, &bufsize, &ds, ds_meta, &error, 12345);
    assert_always(!error);

    struct DS rds;
    b = cra_bin_deserialize_struct(buf, bufsize, &rds, sizeof(struct DS), false, ds_meta, &error, 12345);
    assert_always(b && !error);
    assert_always(ds.dict.count == rds.dict.count);
    assert_always(ds.dictempty1.count == rds.dictempty1.count);
    assert_always(ds.dictempty2->count == rds.dictempty2->count);
    assert_always(ds.dictnull == rds.dictnull);
    for (size_t i = 0; i < (size_t)ds.dict.count; i++)
    {
        float v1, v2;
        cra_dict_get(&ds.dict, &i, &v1);
        cra_dict_get(&rds.dict, &i, &v2);
        assert_always(cra_compare_float(v1, v2) == 0);
    }

    cra_dict_uninit(&ds.dict);
    cra_dict_uninit(&rds.dict);
    cra_dict_uninit(&ds.dictempty1);
    cra_dict_uninit(&rds.dictempty1);
    cra_dict_uninit(ds.dictempty2);
    cra_dict_uninit(rds.dictempty2);
    cra_dealloc(ds.dictempty2);
    cra_dealloc(rds.dictempty2);
}

#if 1

struct MyOld
{
    int   i;
    char *str;
};
CRA_TYPE_META_BEGIN(old_meta)
CRA_TYPE_META_MEMBER_INT(struct MyOld, i)
CRA_TYPE_META_MEMBER_STRING(struct MyOld, str, true)
CRA_TYPE_META_END();

struct MyNew
{
    int   i;
    float f;
    char *str;
    char  arr[10];
};
CRA_TYPE_META_BEGIN(new_meta)
CRA_TYPE_META_MEMBER_INT(struct MyNew, i)
CRA_TYPE_META_MEMBER_STRING(struct MyNew, str, true)
// 保证新旧版本的都有的字段的meta在定义中位置不变:
//      共有的在前保持位置一致
//      新增的在后
CRA_TYPE_META_MEMBER_FLOAT(struct MyNew, f)
CRA_TYPE_META_MEMBER_STRING(struct MyNew, arr, false)
CRA_TYPE_META_END();

static void
set_default_val_to_new(void *obj, size_t count, size_t element_size, const void *arg)
{
    CRA_UNUSED_VALUE(arg);
    CRA_UNUSED_VALUE(count);
    CRA_UNUSED_VALUE(element_size);
    assert(element_size == sizeof(struct MyNew));

    struct MyNew *n = (struct MyNew *)obj;
    n->i = 1000;
    n->f = 78.2f;
    n->str = NULL;
    memcpy(n->arr, "OK!", sizeof("OK!"));
}

static const CraDzer_i my_new_dzer_i = {
    .init1 = set_default_val_to_new,
};

static void
test_old_to_new(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    bufsize = sizeof(buf);
    struct MyOld o = { .i = 100, .str = "hello world!" };
    cra_bin_serialize_struct(buf, &bufsize, &o, old_meta, &error, 0x87654321);
    assert_always(!error);

    struct MyNew n;
    b = cra_bin_deserialize_struct_with_dzer_i(
      buf, bufsize, &n, sizeof(n), false, new_meta, &my_new_dzer_i, NULL, &error, 0x87654321);
    assert_always(b && !error);
    assert_always(o.i == n.i);
    assert_always(strcmp(o.str, n.str) == 0);
    assert_always(cra_compare_float(n.f, 78.2f) == 0); // default value, from `set_default_val_to_new()`
    assert_always(strcmp(n.arr, "OK!") == 0);          // default value, from `set_default_val_to_new()`

    cra_free(n.str);
}

static void
test_new_to_old(void)
{
    unsigned char buf[1024];
    size_t        bufsize;
    CraSerError_e error;
    bool          b;

    bufsize = sizeof(buf);
    struct MyNew n = { .i = 300, .str = "good~~~~", .arr = "ABCE", .f = 128.34f };
    cra_bin_serialize_struct(buf, &bufsize, &n, new_meta, &error, 0x87654321);
    assert_always(!error);

    struct MyOld o;
    b = cra_bin_deserialize_struct(buf, bufsize, &o, sizeof(o), false, old_meta, &error, 0x87654321);
    assert_always(b && !error);
    assert_always(o.i == n.i);
    assert_always(strcmp(o.str, n.str) == 0);

    cra_free(o.str);
}

#endif

int
main(void)
{
    test_boolean();
    test_int();
    test_uint();
    test_varint();
    test_varuint();
    test_float();
    test_string();
    test_bytes();
    test_struct();
    test_list();
    test_dict();
    test_old_to_new();
    test_new_to_old();

    cra_memory_leak_report();
    return 0;
}
