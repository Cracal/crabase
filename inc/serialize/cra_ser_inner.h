/**
 * @file cra_ser_inner.h
 * @author Cracal
 * @brief inner header
 * @version 0.1
 * @date 2024-10-04
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_SER_INNER_H__
#define __CRA_SER_INNER_H__
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