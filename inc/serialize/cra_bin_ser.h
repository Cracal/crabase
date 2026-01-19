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
 * struct:       [T:4,P:4][C:1]{IM}    -- 1 + 1 + N bytes
 * list/c array: [T:4,P:4][C:N]<E>     -- 1 + (1 ~ 10) + N bytes
 * dict:         [T:4,P:4][C:N]<KV>    -- 1 + (1 ~ 10) + N bytes
 *
 * T: type,   V: value,  S: size, L: length
 * C: count   M: member, K: key,  P: padding(ignore)
 * E: element F: flag, I: id
 *
 */
#ifndef __CRA_BIN_SER_H__
#define __CRA_BIN_SER_H__
#include "cra_serialize.h"

CRA_API bool
cra_bin_serialize(unsigned char *buf, size_t *len, CraSeriObject *obj);

CRA_API bool
cra_bin_deserialize(unsigned char *buf, size_t len, CraSeriObject *retobj);

CRA_API void
cra_bin_write_len(unsigned char *buf, uint64_t len, size_t size);

CRA_API uint64_t
cra_bin_read_len(unsigned char *buf, size_t size);

#endif