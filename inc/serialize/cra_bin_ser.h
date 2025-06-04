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

CRA_API void cra_bin_serialize_begin(CraSerializer *ser, unsigned char *buffer, size_t buffer_length);
CRA_API unsigned char *cra_bin_serialize_end(CraSerializer *ser, size_t *buffer_length, CraSerError *error);

CRA_API bool cra_bin_serialize_bool(CraSerializer *ser, bool val);

CRA_API bool cra_bin_serialize_int8(CraSerializer *ser, int8_t val);
CRA_API bool cra_bin_serialize_int16(CraSerializer *ser, int16_t val);
CRA_API bool cra_bin_serialize_int32(CraSerializer *ser, int32_t val);
CRA_API bool cra_bin_serialize_int64(CraSerializer *ser, int64_t val);
CRA_API bool cra_bin_serialize_uint8(CraSerializer *ser, uint8_t val);
CRA_API bool cra_bin_serialize_uint16(CraSerializer *ser, uint16_t val);
CRA_API bool cra_bin_serialize_uint32(CraSerializer *ser, uint32_t val);
CRA_API bool cra_bin_serialize_uint64(CraSerializer *ser, uint64_t val);
CRA_API bool cra_bin_serialize_float(CraSerializer *ser, float val);
CRA_API bool cra_bin_serialize_double(CraSerializer *ser, double val);

CRA_API bool cra_bin_serialize_string_nz(CraSerializer *ser, const char *val, cra_ser_count_t length);
static inline bool cra_bin_serialize_string(CraSerializer *ser, const char *val)
{
    size_t len = val == NULL ? 0 : strlen(val);
    if (len >= CRA_SER_COUNT_MAX)
    {
        ser->error = CRA_SER_ERROR_STRING_TOO_LONG;
        return false;
    }
    return cra_bin_serialize_string_nz(ser, val, (cra_ser_count_t)len);
}

CRA_API bool cra_bin_serialize_struct(CraSerializer *ser, void *val, const CraTypeMeta *members_meta);

CRA_API bool cra_bin_serialize_list(CraSerializer *ser, void *val, const CraTypeMeta *element_meta, const CraTypeIter_i *iter_i);
CRA_API bool cra_bin_serialize_array(CraSerializer *ser, void *val, cra_ser_count_t count, const CraTypeMeta *element_meta);

CRA_API bool cra_bin_serialize_dict(CraSerializer *ser, void *val, const CraTypeMeta *kv_meta, const CraTypeIter_i *iter_i);

// ------

CRA_API void cra_bin_deserialize_begin(CraSerializer *ser, unsigned char *buffer, size_t buffer_length);
CRA_API void cra_bin_deserialize_end(CraSerializer *ser, CraSerError *error);

CRA_API bool cra_bin_deserialize_bool(CraSerializer *ser, bool *retval);

CRA_API bool cra_bin_deserialize_int8(CraSerializer *ser, int8_t *retval);
CRA_API bool cra_bin_deserialize_int16(CraSerializer *ser, int16_t *retval);
CRA_API bool cra_bin_deserialize_int32(CraSerializer *ser, int32_t *retval);
CRA_API bool cra_bin_deserialize_int64(CraSerializer *ser, int64_t *retval);
CRA_API bool cra_bin_deserialize_uint8(CraSerializer *ser, uint8_t *retval);
CRA_API bool cra_bin_deserialize_uint16(CraSerializer *ser, uint16_t *retval);
CRA_API bool cra_bin_deserialize_uint32(CraSerializer *ser, uint32_t *retval);
CRA_API bool cra_bin_deserialize_uint64(CraSerializer *ser, uint64_t *retval);
CRA_API bool cra_bin_deserialize_float(CraSerializer *ser, float *retval);
CRA_API bool cra_bin_deserialize_double(CraSerializer *ser, double *retval);

/**
 * @brief deserialize string with zero ("x...x\0")
 *
 * @param ser serializer
 * @param retval out string.
 *                          1. char[N]: char str[N] -> func(x, str, ...)
 *                          2. char * : char *str;  -> func(x, (char *)&str, ...)
 * @param max_length max string length:
 *                          1. `retval` is char[N] -> `is_char_ptr`=true, `max_length`=sizeof(char[N])
 *                          2. `retval` is char *  -> `is_char_ptr`=false, ignore `max_length`
 * @param is_char_ptr retval is char -> `true`; retval is * char[N] -> `false`
 * @param auto_free_if_fail
 * @return true success
 * @return false failure
 */
CRA_API bool cra_bin_deserialize_string(CraSerializer *ser, char *retval, cra_ser_count_t max_length, bool is_char_ptr, bool auto_free_if_fail);
/**
 * @brief deserialize string without zero ("x...x")
 *
 * @param ser serializer
 * @param retval out string.
 *                          1. char[N]: char str[N] -> func(x, str, ...)
 *                          2. char * : char *str;  -> func(x, (char *)&str, ...)
 * @param length in: max string length(only char[N]); out: real string length
 * @param is_char_ptr retval is char -> `true`; retval is * char[N] -> `false`
 * @param auto_free_if_fail
 * @return true success
 * @return false failure
 */
CRA_API bool cra_bin_deserialize_string_nz(CraSerializer *ser, char *retval, cra_ser_count_t *length, bool is_char_ptr, bool auto_free_if_fail);

/**
 * @brief
 *
 * @param ser serializer
 * @param retval out struct
 * @param valsize sizeof(struct X)
 * @param is_ptr retval is 'struct X *' -> true; retval is 'struct X' -> false
 * @param auto_free_if_fail
 * @param members_meta
 * @param init_i
 * @param args4init
 * @return
 */
CRA_API bool cra_bin_deserialize_struct(CraSerializer *ser, void *retval, size_t valsize, bool is_ptr, bool auto_free_if_fail,
                                        const CraTypeMeta *members_meta, const CraTypeInit_i *init_i, void *args4init);

CRA_API bool cra_bin_deserialize_list(CraSerializer *ser, void *retval, size_t valsize,
                                      bool is_ptr, bool auto_free_if_fail, const CraTypeMeta *element_meta,
                                      const CraTypeIter_i *iter_i, const CraTypeInit_i *init_i, void *args4init);
CRA_API bool cra_bin_deserialize_array(CraSerializer *ser, void *retval, size_t valsize,
                                       bool is_ptr, cra_ser_count_t *countptr, const CraTypeMeta *element_meta);

CRA_API bool cra_bin_deserialize_dict(CraSerializer *ser, void *retval, size_t valsize,
                                      bool is_ptr, bool auto_free_if_fail, const CraTypeMeta *kv_meta,
                                      const CraTypeIter_i *iter_i, const CraTypeInit_i *init_i, void *args4init);

//   --------------------------------

CRA_API unsigned char *cra_bin_serialize_struct0(unsigned char *buffer, size_t *buffer_length, void *val,
                                                 const CraTypeMeta *members_meta, CraSerError *error);
CRA_API void cra_bin_deserialize_struct0(unsigned char *buffer, size_t buffer_length, void *retval,
                                         size_t valsize, bool is_ptr, const CraTypeMeta *members_meta,
                                         const CraTypeInit_i *init_i, void *args4init, CraSerError *error);

CRA_API unsigned char *cra_bin_serialize_list0(unsigned char *buffer, size_t *buffer_length, void *val,
                                               const CraTypeMeta *element_meta, const CraTypeIter_i *iter_i,
                                               CraSerError *error);
CRA_API void cra_bin_deserialize_list0(unsigned char *buffer, size_t buffer_length, void *retval,
                                       size_t valsize, bool is_ptr, const CraTypeMeta *element_meta,
                                       const CraTypeIter_i *iter_i, const CraTypeInit_i *init_i,
                                       void *args4init, CraSerError *error);

CRA_API unsigned char *cra_bin_serialize_array0(unsigned char *buffer, size_t *buffer_length, void *val,
                                                cra_ser_count_t count, const CraTypeMeta *element_meta,
                                                CraSerError *error);
CRA_API void cra_bin_deserialize_array0(unsigned char *buffer, size_t buffer_length, void *retval, size_t valsize,
                                        bool is_ptr, cra_ser_count_t *countptr, const CraTypeMeta *element_meta,
                                        CraSerError *error);

CRA_API unsigned char *cra_bin_serialize_dict0(unsigned char *buffer, size_t *buffer_length, void *val,
                                               const CraTypeMeta *kv_meta, const CraTypeIter_i *iter_i,
                                               CraSerError *error);
CRA_API void cra_bin_deserialize_dict0(unsigned char *buffer, size_t buffer_length, void *retval, size_t valsize,
                                       bool is_ptr, const CraTypeMeta *kv_meta, const CraTypeIter_i *iter_i,
                                       const CraTypeInit_i *init_i, void *args4init, CraSerError *error);

CRA_API void cra_bin_serialize_print(const unsigned char *buffer, size_t buffer_length);

#endif