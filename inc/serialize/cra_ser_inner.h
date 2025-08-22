/**
 * @file cra_ser_inner.h
 * @author Cracal
 * @brief inner header
 * @version 0.1
 * @date 2024-10-04
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_SER_INNER_H__
#define __CRA_SER_INNER_H__
#include "cra_serialize.h"
#include <stdbool.h>
#include <stdint.h>

#define CRA_SER_MAX_NESTING 1000

CRA_API unsigned char *
cra_serializer_buf(CraSerializer *ser, size_t needed);
#define CRA_SERIALIZER_BUF(_ser, _buf, _needed)      \
    if (!(_buf = cra_serializer_buf(_ser, _needed))) \
    return false

CRA_API void
cra_ser_release_init(CraSerRelease *release);

CRA_API void
cra_ser_release_uninit(CraSerRelease *release, bool free_ptr);

CRA_API void
cra_ser_release_add(CraSerRelease *release,
                    bool           _free,
                    void          *ptr,
                    void           (*uninit_fn)(void *),
                    void           (*dealloc_fn)(void *));

#endif