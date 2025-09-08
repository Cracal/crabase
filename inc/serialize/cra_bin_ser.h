/**
 * @file cra_bin_ser.h
 * @author Cracal
 * @brief binary srialization
 * @version 0.1
 * @date 2024-09-29
 *
 * @copyright Copyright (c) 2021
 *
 * null:         [T:4,P:4]             -- 1 byte
 * bool:         [T:4,V:4]             -- 1 byte
 * [u]int8:      [T:4,S:4][V:8]        -- 1 + 1 bytes
 * [u]int16:     [T:4,S:4][V:16]       -- 1 + 2 bytes
 * [u]int32:     [T:4,S:4][V:32]       -- 1 + 4 bytes
 * [u]int64:     [T:4,S:4][V:64]       -- 1 + 8 bytes
 * var[u]int:    [T:4,S:4][V:N]        -- 1 + (1 ~ 10) bytes
 * float:        [T:4,S:4][V:32]       -- 1 + 4 bytes
 * double:       [T:4,S:4][V:64]       -- 1 + 8 bytes
 * string:       [T:4,P:4][L:N][V]     -- 1 + (1 ~ 10) + N bytes
 * bytes:        [T:4,P:4][L:N][V]     -- 1 + (1 ~ 10) + N bytes
 * struct:       [T:4,P:4]{M}[T:4,F:4] -- 1 + N + 1 bytes
 * list/c array: [T:4,P:4][C:N]<E>     -- 1 + (1 ~ 10) + N bytes
 * dict:         [T:4,P:4][C:N]<KV>    -- 1 + (1 ~ 10) + N bytes
 *
 * T: type,   V: value,  S: size, L: length
 * C: count   M: member, K: key,  P: padding(ignore)
 * E: element F: flag
 *
 * seq: [code:32][ITEM-1]...[ITEM-N]
 *
 */
#ifndef __CRA_BIN_SER_H__
#define __CRA_BIN_SER_H__
#include "cra_serialize.h"

#if 1 // step

CRA_API bool
cra_bin_serialize_begin(CraSerializer *ser, unsigned char *buffer, size_t buffersize, uint32_t code);

CRA_API unsigned char *
cra_bin_serialize_end(CraSerializer *ser, size_t *retbuffersize, CraSerError_e *reterror);

CRA_API bool
cra_bin_deserialize_begin(CraSerializer *ser, unsigned char *buffer, size_t buffersize, uint32_t code);

CRA_API bool
cra_bin_deserialize_end(CraSerializer *ser, CraSerError_e *reterror);

CRA_API bool
cra_bin_serialize_bool(CraSerializer *ser, bool val);

CRA_API bool
cra_bin_deserialize_bool(CraSerializer *ser, bool *retval);

CRA_API bool
cra_bin_serialize_int(CraSerializer *ser, void *val, size_t size);

CRA_API bool
cra_bin_deserialize_int(CraSerializer *ser, void *retval, size_t size);

CRA_API bool
cra_bin_serialize_uint(CraSerializer *ser, void *val, size_t size);

CRA_API bool
cra_bin_deserialize_uint(CraSerializer *ser, void *retval, size_t size);

CRA_API bool
cra_bin_serialize_varint(CraSerializer *ser, void *val, size_t size);

CRA_API bool
cra_bin_deserialize_varint(CraSerializer *ser, void *retval, size_t size);

CRA_API bool
cra_bin_serialize_varuint(CraSerializer *ser, void *val, size_t size);

CRA_API bool
cra_bin_deserialize_varuint(CraSerializer *ser, void *retval, size_t size);

CRA_API bool
cra_bin_serialize_float(CraSerializer *ser, void *val, size_t size);

CRA_API bool
cra_bin_deserialize_float(CraSerializer *ser, void *retval, size_t size);

CRA_API bool
cra_bin_serialize_string(CraSerializer *ser, const char *val);

CRA_API bool
cra_bin_deserialize_string(CraSerializer *ser, char *retval, size_t max_chars, bool is_ptr);

CRA_API bool
cra_bin_serialize_bytes(CraSerializer *ser, const char *val, uint64_t length);

CRA_API bool
cra_bin_deserialize_bytes(CraSerializer *ser, char *retval, size_t max_chars, bool is_ptr, uint64_t *retlength);

#endif // end step

CRA_API unsigned char *
cra_bin_serialize_struct(unsigned char    *buf,
                         size_t           *bufsize,
                         void             *val,
                         const CraTypeMeta members_meta[],
                         CraSerError_e    *reterror,
                         uint32_t          code);

CRA_API bool
cra_bin_deserialize_struct_with_dzer_i(unsigned char    *buf,
                                       size_t            bufsize,
                                       void             *retval,
                                       size_t            valsize,
                                       bool              is_ptr,
                                       const CraTypeMeta members_meta[],
                                       const CraDzer_i  *dzer_i,
                                       const void       *arg4dzer,
                                       CraSerError_e    *reterror,
                                       uint32_t          code);
static inline bool
cra_bin_deserialize_struct(unsigned char    *buf,
                           size_t            bufsize,
                           void             *retval,
                           size_t            valsize,
                           bool              is_ptr,
                           const CraTypeMeta members_meta[],
                           CraSerError_e    *reterror,
                           uint32_t          code)
{
    return cra_bin_deserialize_struct_with_dzer_i(
      buf, bufsize, retval, valsize, is_ptr, members_meta, NULL, NULL, reterror, code);
}

CRA_API unsigned char *
cra_bin_serialize_array(unsigned char    *buf,
                        size_t           *bufsize,
                        void             *val,
                        size_t            arraycount,
                        const CraTypeMeta element_meta[],
                        CraSerError_e    *reterror,
                        uint32_t          code);

CRA_API bool
cra_bin_deserialize_array(unsigned char    *buf,
                          size_t            bufsize,
                          void             *retval,
                          size_t            valsize,
                          bool              is_ptr,
                          size_t           *retarraycount,
                          const CraTypeMeta element_meta[],
                          CraSerError_e    *reterror,
                          uint32_t          code);

CRA_API unsigned char *
cra_bin_serialize_list(unsigned char    *buf,
                       size_t           *bufsize,
                       void             *val,
                       const CraTypeMeta element_meta[],
                       const CraSzer_i  *szer_i,
                       CraSerError_e    *reterror,
                       uint32_t          code);

CRA_API bool
cra_bin_deserialize_list(unsigned char    *buf,
                         size_t            bufsize,
                         void             *retval,
                         size_t            valsize,
                         bool              is_ptr,
                         const CraTypeMeta element_meta[],
                         const CraDzer_i  *dzer_i,
                         const void       *arg4dzer,
                         CraSerError_e    *reterror,
                         uint32_t          code);

CRA_API unsigned char *
cra_bin_serialize_dict(unsigned char    *buf,
                       size_t           *bufsize,
                       void             *val,
                       const CraTypeMeta kv_meta[],
                       const CraSzer_i  *szer_i,
                       CraSerError_e    *reterror,
                       uint32_t          code);

CRA_API bool
cra_bin_deserialize_dict(unsigned char    *buf,
                         size_t            bufsize,
                         void             *retval,
                         size_t            valsize,
                         bool              is_ptr,
                         const CraTypeMeta kv_meta[],
                         const CraDzer_i  *dzer_i,
                         const void       *arg4dzer,
                         CraSerError_e    *reterror,
                         uint32_t          code);

#endif