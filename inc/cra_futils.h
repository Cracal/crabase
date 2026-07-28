/**
 * @file cra_futils.h
 * @author Cracal
 * @brief file utils
 * @version 0.1
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __CRA_FUTILS_H__
#define __CRA_FUTILS_H__
#include "cra_defs.h"

#ifdef CRA_OS_WIN

#include <direct.h>

#define CRA_PATH_SEP1 '/'
#define CRA_PATH_SEP2 '\\'

typedef int cra_mode_t;

static inline bool
cra_is_dir(const char *path)
{
    DWORD attr = GetFileAttributesA(path);
    if (attr == INVALID_FILE_ATTRIBUTES)
        return false;
    return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

static inline int
cra_mkdir(const char *path, cra_mode_t mode)
{
    CRA_UNUSED(mode);
    return _mkdir(path);
}

#else

#include <sys/stat.h>

#define CRA_PATH_SEP1 '/'
#define CRA_PATH_SEP2 '/'

typedef mode_t cra_mode_t;

static inline bool
cra_is_dir(const char *path)
{
    struct stat st;
    if (stat(path, &st) == -1)
        return false;
    return S_ISDIR(st.st_mode);
}

static inline int
cra_mkdir(const char *path, cra_mode_t mode)
{
    return mkdir(path, mode);
}

#endif

#define CRA_IS_PATH_SEP(c) (c == CRA_PATH_SEP1 || c == CRA_PATH_SEP2)

// 0:     success
// other: error code
CRA_API int
cra_mkdirs(const char *path, cra_mode_t mode);

CRA_API char *
cra_basename(char *path);

CRA_API char *
cra_dirname(char *path);

#endif