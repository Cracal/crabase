/**
 * @file cra_json.h
 * @author Cracal
 * @brief JSON
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_JSON_H__
#define __CRA_JSON_H__
#include "cra_serialize.h"

#define CRA_MAX_SAFE_INT        9007199254740991    // 2^53 - 1
#define CRA_MIN_SAFE_INT        (-CRA_MAX_SAFE_INT) // -(2^53 - 1)
#define CRA_MAX_JSON_KEY_LENGTH 2048
static_assert(CRA_MAX_JSON_KEY_LENGTH <= CRA_SER_STRING_MAX_LENGTH,
              "CRA_MAX_JSON_KEY_LENGTH must be less than or equal to CRA_SER_STRING_MAX_LENGTH");

CRA_API bool
cra_json_stringify_err(char *buf, size_t *len, const void *obj, const CraSerMeta *meta, CraSerErr *error, bool format);

CRA_API bool
cra_json_parse_err(const char *buf, size_t len, void *obj, const CraSerMeta *meta, CraSerErr *error);

static inline bool
cra_json_stringify(char *buf, size_t *len, const void *obj, const CraSerMeta *meta, bool format)
{
    return cra_json_stringify_err(buf, len, obj, meta, NULL, format);
}

static inline bool
cra_json_parse(const char *buf, size_t len, void *obj, const CraSerMeta *meta)
{
    return cra_json_parse_err(buf, len, obj, meta, NULL);
}

#endif