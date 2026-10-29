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
#ifndef __CRABASE_SERIALIZE_CRA_BIN_SER_H__
#define __CRABASE_SERIALIZE_CRA_BIN_SER_H__
#include "cra_serialize.h"

CRA_API bool
cra_bin_serialize_err(unsigned char *buf, size_t *len, const void *obj, const CraSerMeta *meta, CraSerErr *error);

CRA_API bool
cra_bin_deserialize_err(const unsigned char *buf, size_t len, void *obj, const CraSerMeta *meta, CraSerErr *error);

static inline bool
cra_bin_serialize(unsigned char *buf, size_t *len, const void *obj, const CraSerMeta *meta)
{
    return cra_bin_serialize_err(buf, len, obj, meta, NULL);
}

static inline bool
cra_bin_deserialize(const unsigned char *buf, size_t len, void *obj, const CraSerMeta *meta)
{
    return cra_bin_deserialize_err(buf, len, obj, meta, NULL);
}

#endif