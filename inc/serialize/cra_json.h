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

#define CRA_MAX_SAFE_INT 9007199254740991  // 2^53 - 1
#define CRA_MIN_SAFE_INT -CRA_MAX_SAFE_INT // -(2^53 - 1)

CRA_API unsigned char *
cra_json_stringify_struct(unsigned char    *buf,
                          size_t           *bufsize,
                          void             *val,
                          const CraTypeMeta members_meta[],
                          CraSerError_e    *reterror,
                          bool              format);

CRA_API bool
cra_json_parse_struct_with_dzer_i(unsigned char    *buf,
                                  size_t            bufsize,
                                  void             *retval,
                                  size_t            valsize,
                                  bool              is_ptr,
                                  const CraTypeMeta members_meta[],
                                  const CraDzer_i  *dzer_i,
                                  const void       *arg4dzer,
                                  CraSerError_e    *reterror);

static inline bool
cra_json_parse_struct(unsigned char    *buf,
                      size_t            bufsize,
                      void             *retval,
                      size_t            valsize,
                      bool              is_ptr,
                      const CraTypeMeta members_meta[],
                      CraSerError_e    *reterror)
{
    return cra_json_parse_struct_with_dzer_i(buf, bufsize, retval, valsize, is_ptr, members_meta, NULL, NULL, reterror);
}

CRA_API unsigned char *
cra_json_stringify_array(unsigned char    *buf,
                         size_t           *bufsize,
                         void             *val,
                         size_t            arraycount,
                         const CraTypeMeta element_meta[],
                         CraSerError_e    *reterror,
                         bool              format);

CRA_API bool
cra_json_parse_array(unsigned char    *buf,
                     size_t            bufsize,
                     void             *retval,
                     size_t            valsize,
                     bool              is_ptr,
                     size_t           *retarraycount,
                     const CraTypeMeta element_meta[],
                     CraSerError_e    *reterror);

CRA_API unsigned char *
cra_json_stringify_list(unsigned char    *buf,
                        size_t           *bufsize,
                        void             *val,
                        const CraTypeMeta element_meta[],
                        const CraSzer_i  *szer_i,
                        CraSerError_e    *reterror,
                        bool              format);

CRA_API bool
cra_json_parse_list(unsigned char    *buf,
                    size_t            bufsize,
                    void             *retval,
                    size_t            valsize,
                    bool              is_ptr,
                    const CraTypeMeta element_meta[],
                    const CraDzer_i  *dzer_i,
                    const void       *arg4dzer,
                    CraSerError_e    *reterror);

CRA_API unsigned char *
cra_json_stringify_dict(unsigned char    *buf,
                        size_t           *bufsize,
                        void             *val,
                        const CraTypeMeta kv_meta[],
                        const CraSzer_i  *szer_i,
                        CraSerError_e    *reterror,
                        bool              format);

CRA_API bool
cra_json_parse_dict(unsigned char    *buf,
                    size_t            bufsize,
                    void             *retval,
                    size_t            valsize,
                    bool              is_ptr,
                    const CraTypeMeta kv_meta[],
                    const CraDzer_i  *dzer_i,
                    const void       *arg4dzer,
                    CraSerError_e    *reterror);

#endif