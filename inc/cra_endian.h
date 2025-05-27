/**
 * @file cra_endian.h
 * @author Cracal
 * @brief 大小端字节序
 * @version 0.1
 * @date 2024-07-05
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_ENDIAN_H__
#define __CRA_ENDIAN_H__
#include "cra_defs.h"

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define CRA_IS_BIG_ENDIAN 1
#define CRA_IS_LITTLE_ENDIAN 0
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define CRA_IS_BIG_ENDIAN 0
#define CRA_IS_LITTLE_ENDIAN 1
#elif defined(CRA_OS_WIN)
#include <windows.h>
#if REG_DWORD == REG_DWORD_BIG_ENDIAN
#define CRA_IS_BIG_ENDIAN 1
#define CRA_IS_LITTLE_ENDIAN 0
#else
#define CRA_IS_BIG_ENDIAN 0
#define CRA_IS_LITTLE_ENDIAN 1
#endif
#elif defined(CRA_OS_LINUX)
#include <endian.h>
#if __BYTE_ORDER == __BIG_ENDIAN
#define CRA_IS_BIG_ENDIAN 1
#define CRA_IS_LITTLE_ENDIAN 0
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define CRA_IS_BIG_ENDIAN 0
#define CRA_IS_LITTLE_ENDIAN 1
#else
#error "Cannot determine endianness"
#endif
#else
#error "Cannot determine endianness"
#endif

#ifdef CRA_OS_LINUX
#include <byteswap.h>

#define CRA_BSWAP_UINT16 bswap_16
#define CRA_BSWAP_UINT32 bswap_32
#define CRA_BSWAP_UINT64 bswap_64

#elif defined(CRA_OS_WIN)
#include <stdlib.h>

#define CRA_BSWAP_UINT16 _byteswap_ushort
#define CRA_BSWAP_UINT32 _byteswap_ulong
#define CRA_BSWAP_UINT64 _byteswap_uint64

#endif

#endif