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
#define CRA_MAX_JSON_KEY_LENGTH 1024

CRA_API bool
cra_json_stringify(char *buf, size_t *len, bool format, CraSeriObject *obj);

CRA_API bool
cra_json_parse(char *buf, size_t len, CraSeriObject *retobj);

#endif