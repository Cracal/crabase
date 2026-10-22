/**
 * @file cra_serialize_inner.h
 * @author Cracal
 * @brief xxx
 * @version 0.1
 * @date 2026-09-01
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __CRA_SERIALIZE_INNER_H__
#define __CRA_SERIALIZE_INNER_H__
#include "serialize/cra_serialize.h"
#include "cra_string.h"
#include "cra_assert.h"

#define CRA_SER_NESTING_MAX 128

typedef struct CraSerializer CraSerializer;

struct CraSerializer
{
#ifdef __CRA_SER_UNSIGNED_BUF__
    unsigned char *buf;
#else
    char *buf;
#endif
    size_t size;
    size_t curr;

    uint32_t nesting;

    CraSerErr error;
};

#define CRA_SER_DEF_IDS_BITMAP(name) uint8_t name[(UINT16_MAX + 1) / 8] = { 0 }

#define CRA_SER_ENOUGH(szer, needed) ((szer)->curr + (needed) <= (szer)->size)
#define CRA_SER_ENSURE(szer, needed, buf)                \
    do                                                   \
    {                                                    \
        if (!CRA_SER_ENOUGH(szer, needed))               \
        {                                                \
            cra_ser_error_set(szer, CRA_SER_ERR_NO_BUF); \
            return false;                                \
        }                                                \
        buf = (szer)->buf + (szer)->curr;                \
        (szer)->curr += (needed);                        \
    } while (0)

#define CRA_SER_NESTING_INC(szer)                         \
    do                                                    \
    {                                                     \
        if (++(szer)->nesting > CRA_SER_NESTING_MAX)      \
        {                                                 \
            cra_ser_error_set(szer, CRA_SER_ERR_NESTING); \
            return false;                                 \
        }                                                 \
    } while (0)
#define CRA_SER_NESTING_DEC(szer) (void)(--(szer)->nesting)

#define CRA_SER_CHECK_LIMIT_(szer, num, op, max, errcode) \
    do                                                    \
    {                                                     \
        if (num op max)                                   \
        {                                                 \
            cra_ser_error_set(szer, errcode);             \
            return false;                                 \
        }                                                 \
    } while (0)

#define CRA_SER_CHECK_LIMIT(szer, num, op, max) CRA_SER_CHECK_LIMIT_(szer, num, op, max, CRA_SER_ERR_LENGTH_MAX)
#define CRA_SER_CHECK_RANGE(szer, num, op, max) CRA_SER_CHECK_LIMIT_(szer, num, op, max, CRA_SER_ERR_OUT_OF_RANGE)

#define CRA_SER_CHECK(fn) \
    do                    \
    {                     \
        if (!(fn))        \
            return false; \
    } while (0)

#define CRA_SER_CALL_INIT(pp, p, is_ptr, fn, ...)                  \
    do                                                             \
    {                                                              \
        if (!fn(p, __VA_ARGS__))                                   \
        {                                                          \
            if (is_ptr)                                            \
            {                                                      \
                cra_free(p);                                       \
                bzero(pp, sizeof(void *)); /* *(T **)pp = NULL; */ \
            }                                                      \
            cra_ser_error_set(szer, CRA_SER_ERR_INIT_FAILED);      \
            return false;                                          \
        }                                                          \
    } while (0)

#define CRA_SER_NEW_OBJ(obj, size)                       \
    do                                                   \
    {                                                    \
        obj = cra_malloc(size);                          \
        if (!(obj))                                      \
        {                                                \
            cra_ser_error_set(szer, CRA_SER_ERR_NO_MEM); \
            return false;                                \
        }                                                \
    } while (0)

#define CRA_SER_DEL_OBJ(pp, p, meta, is_ptr, fn) \
    do                                           \
    {                                            \
        if (fn)                                  \
            fn(p);                               \
        else                                     \
            bzero(p, (meta)->size);              \
        if (is_ptr)                              \
        {                                        \
            cra_free(p);                         \
            bzero(pp, sizeof(void *));           \
        }                                        \
    } while (0)

#define CRA_SER_NEW_STRING(szer, pstr, str, info, len1, len2)                \
    do                                                                       \
    {                                                                        \
        if ((info)->flags & CRA_SER_FLAG_PTR) /* char * */                   \
        {                                                                    \
            if (len1 > 0)                                                    \
            {                                                                \
                CRA_SER_NEW_OBJ(str, len1);                                  \
                str[len2] = '\0';                                            \
            }                                                                \
            else                                                             \
            {                                                                \
                str = NULL;                                                  \
            }                                                                \
            memcpy(pstr, &(str), sizeof(char *)); /* *(char**)pstr = str; */ \
        }                                                                    \
        else /* char[N] */                                                   \
        {                                                                    \
            CRA_SER_CHECK_RANGE(szer, len2, >=, (info)->size);               \
            str = (char *)pstr;                                              \
            str[len2] = '\0';                                                \
        }                                                                    \
    } while (0)

#define CRA_SER_NEW_STRUCT(szer, pstru, stru, meta, is_ptr, is_null)                           \
    do                                                                                         \
    {                                                                                          \
        if (is_ptr)                                                                            \
        {                                                                                      \
            if (!(is_null))                                                                    \
                CRA_SER_NEW_OBJ(stru, (meta)->size);                                           \
            else                                                                               \
                stru = NULL;                                                                   \
            memcpy(pstru, &(stru), sizeof(void *));                                            \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            stru = pstru;                                                                      \
        }                                                                                      \
                                                                                               \
        if (stru)                                                                              \
        {                                                                                      \
            if ((meta)->for_struct.init)                                                       \
                CRA_SER_CALL_INIT(pstru, stru, is_ptr, (meta)->for_struct.init, (meta)->size); \
            else                                                                               \
                bzero(stru, (meta)->size);                                                     \
        }                                                                                      \
    } while (0)
#define CRA_SER_DEL_STRUCT(pstru, stru, meta, is_ptr)                     \
    CRA_SER_DEL_OBJ(pstru, stru, meta, is_ptr, (meta)->for_struct.uninit)

#define CRA_SER_NEW_ARRAY(szer, parr, arr, meta, len1, len2)                                  \
    do                                                                                        \
    {                                                                                         \
        if ((meta)->count == 0)                                                               \
        {                                                                                     \
            if (len1 > 0)                                                                     \
                CRA_SER_NEW_OBJ(arr, (len2 > 0 ? len2 : 1) * (meta)->for_array.size_of_slot); \
            else                                                                              \
                arr = NULL;                                                                   \
            memcpy(parr, &(arr), sizeof(void *)); /* *(void **)parr = arr; */                 \
        }                                                                                     \
        else                                                                                  \
        {                                                                                     \
            CRA_SER_CHECK_RANGE(szer, len2, >, (meta)->count);                                \
            arr = parr;                                                                       \
        }                                                                                     \
    } while (0)
#define CRA_SER_DEL_ARRAY(parr, arr, meta, count_) \
    do                                             \
    {                                              \
        if ((meta)->for_array.uninit)              \
            (meta)->for_array.uninit(parr);        \
    } while (0)

#define CRA_SER_NEW_LIST(szer, plist, list, meta, is_ptr, len1, len2, itemsize)            \
    do                                                                                     \
    {                                                                                      \
        if (is_ptr)                                                                        \
        {                                                                                  \
            if (len1 > 0)                                                                  \
                CRA_SER_NEW_OBJ(list, (meta)->size);                                       \
            else                                                                           \
                list = NULL;                                                               \
            memcpy(plist, &(list), sizeof(void *)); /* *(void **)plist = list; */          \
        }                                                                                  \
        else                                                                               \
        {                                                                                  \
            list = plist;                                                                  \
        }                                                                                  \
                                                                                           \
        if (list)                                                                          \
            CRA_SER_CALL_INIT(plist, list, is_ptr, (meta)->for_list.init, itemsize, len2); \
    } while (0)
#define CRA_SER_DEL_LIST(plist, list, meta, is_ptr) CRA_SER_DEL_OBJ(plist, list, meta, is_ptr, (meta)->for_list.uninit)

#define CRA_SER_NEW_DICT(szer, pdict, dict, meta, is_ptr, len1, len2, keysize, valsize)            \
    do                                                                                             \
    {                                                                                              \
        if (is_ptr)                                                                                \
        {                                                                                          \
            if (len1 > 0)                                                                          \
                CRA_SER_NEW_OBJ(dict, (meta)->size);                                               \
            else                                                                                   \
                dict = NULL;                                                                       \
            memcpy(pdict, &(dict), sizeof(void *)); /* *(void **)pdict = dict; */                  \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            dict = pdict;                                                                          \
        }                                                                                          \
                                                                                                   \
        if (dict)                                                                                  \
            CRA_SER_CALL_INIT(pdict, dict, is_ptr, (meta)->for_dict.init, keysize, valsize, len2); \
    } while (0)
#define CRA_SER_DEL_DICT(pdict, dict, meta, is_ptr) CRA_SER_DEL_OBJ(pdict, dict, meta, is_ptr, (meta)->for_dict.uninit)

#define CRA_SER_ERR_OK_STR  "OK"
#define CRA_SER_ERR_SUFFIX  ". $ "
#define CRA_SER_ERR_PREFIX  "Error: "
#define CRA_SER_ERR_FMT     CRA_SER_ERR_PREFIX "%s" CRA_SER_ERR_SUFFIX
#define CRA_SER_ERR_OK_INIT CRA_SER_ERR_PREFIX CRA_SER_ERR_OK_STR CRA_SER_ERR_SUFFIX

static inline CraStringView
cra_ser_get_field_name(const CraSerMeta *meta, uint16_t index)
{
    assert(meta->type == CRA_SER_TYPE_STRUCT);
    char    *name = (char *)meta->for_struct.names + meta->for_struct.namei[index];
    uint16_t len = meta->for_struct.namei[index + 1] - meta->for_struct.namei[index];
    return (CraStringView)CRA_STRING_VIEW_INIT(name, len);
}

void
cra_ser_error_set(CraSerializer *szer, CraSerErr_e errcode);
void
cra_ser_error_append(CraSerializer *szer, const CraStringView *name);

static inline void
cra_serializer_init(CraSerializer *szer, void *buf, size_t size)
{
    szer->buf = buf;
    szer->size = size;
    szer->curr = 0;
    szer->nesting = 0;
    szer->error.err = CRA_SER_ERR_OK;
    memcpy(szer->error.msg, CRA_SER_ERR_OK_INIT, sizeof(CRA_SER_ERR_OK_INIT));
}

static inline bool
cra_ser_check_id_duplicate(uint8_t *bitmap, uint16_t id)
{
    uint16_t index = id >> 3;      // id / 8
    uint8_t  mask = 1 << (id & 7); // id % 8

    if (bitmap[index] & mask)
        return true;

    bitmap[index] |= mask; // mark as used
    return false;
}

#endif