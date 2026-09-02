/**
 * @file cra_bin_ser.h
 * @author Cracal
 * @brief binary srialization
 * @version 0.1
 * @date 2024-09-29
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_BIN_SER_H__
#define __CRA_BIN_SER_H__
#include "cra_serialize.h"

#if 0
CRA_API bool
cra_bin_serialize_err(unsigned char *buf, size_t *len, CraSeriObject *obj, CraSerErr *err);

CRA_API bool
cra_bin_deserialize_err(unsigned char *buf, size_t len, CraSeriObject *retobj, CraSerErr *err);

static inline bool
cra_bin_serialize(unsigned char *buf, size_t *len, CraSeriObject *obj)
{
    return cra_bin_serialize_err(buf, len, obj, NULL);
}

static inline bool
cra_bin_deserialize(unsigned char *buf, size_t len, CraSeriObject *retobj)
{
    return cra_bin_deserialize_err(buf, len, retobj, NULL);
}

CRA_API void
cra_bin_write_len(unsigned char *buf, uint64_t len, size_t len_size);

CRA_API uint64_t
cra_bin_read_len(unsigned char *buf, size_t len_size);
#endif

#endif