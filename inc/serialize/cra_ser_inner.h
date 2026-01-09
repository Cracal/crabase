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
#include "cra_serialize.h"
#include <stdbool.h>
#include <stdint.h>

#define CRA_SER_MAX_NESTING 1000
#define CRA_SER_NESTING_INC_AND_CHECK(_ser)                 \
    do                                                      \
    {                                                       \
        if (++(_ser)->nesting >= CRA_SER_MAX_NESTING)       \
        {                                                   \
            (_ser)->error = CRA_SER_ERROR_TOO_MUCH_NESTING; \
            return false;                                   \
        }                                                   \
    } while (0)
#define CRA_SER_NESTING_DEC(_ser) --(_ser)->nesting

#define CRA_SERIALIZER_ENOUGH(_ser, _needed) ((_ser)->index + (_needed) <= (_ser)->length)
#define CRA_SERIALIZER_GET_BUF(_ser)         ((_ser)->buffer + (_ser)->index)
#define CRA_SERIALIZER_ENSURE_AND_RETURN(_ser, _buf, _needed) \
    do                                                        \
    {                                                         \
        if (!CRA_SERIALIZER_ENOUGH(_ser, _needed))            \
        {                                                     \
            if ((_ser)->noalloc)                              \
            {                                                 \
                (_ser)->error = CRA_SER_ERROR_NOBUF;          \
                return false;                                 \
            }                                                 \
            cra_serializer_extend_buf(_ser, _needed);         \
        }                                                     \
        _buf = CRA_SERIALIZER_GET_BUF(_ser);                  \
        (_ser)->index += (_needed);                           \
    } while (0)
#define CRA_SERIALIZER_ENOUGH_AND_RETURN(_ser, _buf, _needed) \
    do                                                        \
    {                                                         \
        if (!CRA_SERIALIZER_ENOUGH(_ser, _needed))            \
        {                                                     \
            (_ser)->error = CRA_SER_ERROR_NOBUF;              \
            return false;                                     \
        }                                                     \
        _buf = CRA_SERIALIZER_GET_BUF(_ser);                  \
        (_ser)->index += (_needed);                           \
    } while (0)

void
cra_serializer_extend_buf(CraSerializer *ser, size_t needed);

void
cra_ser_release_init(CraSerRelease *release);

void
cra_ser_release_uninit(CraSerRelease *release, bool free_ptr);

void
cra_ser_release_add(CraSerRelease *release, void *ptr, void (*uninit_fn)(void *), void (*dealloc_fn)(void *));

static inline int64_t
__cra_ptr_to_i64(void *val, size_t size)
{
    int64_t ret = 0;

    assert(size == 1 || size == 2 || size == 4 || size == 8);

    switch (size)
    {
        case sizeof(int8_t):
            ret = *(int8_t *)val;
            break;
        case sizeof(int16_t):
            ret = *(int16_t *)val;
            break;
        case sizeof(int32_t):
            ret = *(int32_t *)val;
            break;
        case sizeof(int64_t):
            ret = *(int64_t *)val;
            break;
        default:
            assert_always(false && "error integer size.");
            break;
    }
    return ret;
}

static inline void
__cra_i64_to_ptr(int64_t i, void *retval, size_t size)
{
    assert(retval);
    assert(size == 1 || size == 2 || size == 4 || size == 8);

    switch (size)
    {
        case sizeof(int8_t):
            *(int8_t *)retval = (int8_t)i;
            break;
        case sizeof(int16_t):
            *(int16_t *)retval = (int16_t)i;
            break;
        case sizeof(int32_t):
            *(int32_t *)retval = (int32_t)i;
            break;
        case sizeof(int64_t):
            *(int64_t *)retval = i;
            break;
        default:
            assert_always(false && "error integer size.");
            break;
    }
}

static inline uint64_t
__cra_ptr_to_u64(void *val, size_t size)
{
    uint64_t ret = 0;

    assert(size == 1 || size == 2 || size == 4 || size == 8);

    switch (size)
    {
        case sizeof(uint8_t):
            ret = *(uint8_t *)val;
            break;
        case sizeof(uint16_t):
            ret = *(uint16_t *)val;
            break;
        case sizeof(uint32_t):
            ret = *(uint32_t *)val;
            break;
        case sizeof(uint64_t):
            ret = *(uint64_t *)val;
            break;
        default:
            assert_always(false && "error integer size.");
            break;
    }
    return ret;
}

static inline void
__cra_u64_to_ptr(uint64_t u, void *retval, size_t size)
{
    assert(retval);
    assert(size == 1 || size == 2 || size == 4 || size == 8);

    switch (size)
    {
        case sizeof(uint8_t):
            *(uint8_t *)retval = (uint8_t)u;
            break;
        case sizeof(uint16_t):
            *(uint16_t *)retval = (uint16_t)u;
            break;
        case sizeof(uint32_t):
            *(uint32_t *)retval = (uint32_t)u;
            break;
        case sizeof(uint64_t):
            *(uint64_t *)retval = u;
            break;
        default:
            assert_always(false && "error integer size.");
            break;
    }
}

static inline uint64_t
__cra_get_n(void *n_start, void *obj, const CraTypeMeta *meta, uint64_t in_n)
{
#define _N ((char *)n_start + meta->noffset - meta->offset)
    switch (meta->nsize)
    {
        case 0:
            if (meta->szer_i && meta->szer_i->get_count)
                return meta->szer_i->get_count(obj);
            else
                return in_n;
        case sizeof(uint8_t):
            return *(uint8_t *)_N;
        case sizeof(uint16_t):
            return *(uint16_t *)_N;
        case sizeof(uint32_t):
            return *(uint32_t *)_N;
        case sizeof(uint64_t):
            return *(uint64_t *)_N;

        default:
            assert_always(false && "error N size.");
            return 0;
    }
#undef _N
}

static inline void
__cra_set_n(void *n_start, const CraTypeMeta *meta, uint64_t n, uint64_t *out_n)
{
#define _N ((char *)n_start + meta->noffset - meta->offset)
    switch (meta->nsize)
    {
        case 0:
            assert(out_n);
            *out_n = n;
            break;
        case sizeof(uint8_t):
            *(uint8_t *)_N = (uint8_t)n;
            break;
        case sizeof(uint16_t):
            *(uint16_t *)_N = (uint16_t)n;
            break;
        case sizeof(uint32_t):
            *(uint32_t *)_N = (uint32_t)n;
            break;
        case sizeof(uint64_t):
            *(uint64_t *)_N = n;
            break;

        default:
            assert_always(false && "error N size.");
    }
#undef _N
}

#endif