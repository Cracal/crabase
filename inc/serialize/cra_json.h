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

CRA_API unsigned char *cra_json_stringify_struct0(unsigned char *buffer, size_t *buffer_length, void *val,
                                                  const CraTypeMeta *members_meta, bool format,
                                                  CraSerError *error);
CRA_API void cra_json_parse_struct0(unsigned char *buffer, size_t buffer_length, void *retval,
                                    size_t valsize, bool is_ptr, const CraTypeMeta *members_meta,
                                    const ICraTypeInit *init_i, void *args4init, CraSerError *error);

CRA_API unsigned char *cra_json_stringify_list0(unsigned char *buffer, size_t *buffer_length, void *val,
                                                const CraTypeMeta *element_meta, const ICraTypeIter *iter_i,
                                                bool format, CraSerError *error);
CRA_API void cra_json_parse_list0(unsigned char *buffer, size_t buffer_length, void *retval,
                                  size_t valsize, bool is_ptr, const CraTypeMeta *element_meta,
                                  const ICraTypeIter *iter_i, const ICraTypeInit *init_i,
                                  void *args4init, CraSerError *error);

CRA_API unsigned char *cra_json_stringify_array0(unsigned char *buffer, size_t *buffer_length, void *val,
                                                 cra_ser_count_t count, const CraTypeMeta *element_meta,
                                                 bool format, CraSerError *error);
CRA_API void cra_json_parse_array0(unsigned char *buffer, size_t buffer_length, void *retval, size_t valsize,
                                   bool is_ptr, cra_ser_count_t *countptr, const CraTypeMeta *element_meta,
                                   CraSerError *error);

CRA_API unsigned char *cra_json_stringify_dict0(unsigned char *buffer, size_t *buffer_length, void *val,
                                                const CraTypeMeta *kv_meta, const ICraTypeIter *iter_i,
                                                bool format, CraSerError *error);
CRA_API void cra_json_parse_dict0(unsigned char *buffer, size_t buffer_length, void *retval, size_t valsize,
                                  bool is_ptr, const CraTypeMeta *kv_meta, const ICraTypeIter *iter_i,
                                  const ICraTypeInit *init_i, void *args4init, CraSerError *error);

#endif