#include "cra_defs.h"

#define IS_SLASH(_c) CRA_IS_PATH_SEP(_c)
#define IS_ZERO(_c)  (_c == '\0')

int
cra_mkdirs(const char *path, cra_mode_t mode)
{
    char  *buf;
    size_t start;
    size_t len;
    size_t i;

    if (path == NULL || IS_ZERO(*path))
        return EINVAL;

    start = 0;
    len = strlen(path);
    buf = malloc(len + 1);
    if (!buf)
        return ENOMEM;

    memcpy(buf, path, len + 1);

#ifdef CRA_OS_WIN

    /* C:\\ */
    if (len >= 2 && buf[1] == ':')
        start = 3;
    /* \\server\share */
    else if (len >= 2 && IS_SLASH(buf[0]) && IS_SLASH(buf[1]))
    {
        int    count = 0;
        size_t slash = 2;

        while (buf[slash])
        {
            if (IS_SLASH(buf[slash]))
            {
                ++count;
                if (count == 2)
                {
                    ++slash;
                    break;
                }
            }

            ++slash;
        }

        start = slash;
    }

#else

    if (IS_SLASH(buf[0]))
        start = 1;

#endif

    int  err;
    char old;

    for (i = start; i < len; ++i)
    {
        if (!IS_SLASH(buf[i]))
            continue;

        old = buf[i];
        buf[i] = '\0';

        if (!IS_ZERO(buf[0]) && !cra_is_dir(buf))
        {
            if (cra_mkdir(buf, mode) != 0)
            {
                err = cra_get_last_error();
                if (err != EEXIST || !cra_is_dir(buf))
                {
                    free(buf);
                    return err;
                }
            }
        }

        buf[i] = old;
    }

    if (!cra_is_dir(buf))
    {
        if (cra_mkdir(buf, mode) != 0)
        {
            err = cra_get_last_error();
            if (err != EEXIST || !cra_is_dir(buf))
            {
                free(buf);
                return err;
            }
        }
    }

    free(buf);
    return 0;
}

char *
cra_basename(char *path)
{
    char *curr, *start, *slash;

    if (!path || IS_ZERO(*path))
        return ".";

    for (curr = start = slash = path; !IS_ZERO(*curr); ++curr)
    {
        if (!IS_SLASH(*curr))
            continue;

        if (curr != path && !IS_SLASH(*(curr - 1)))
            slash = curr;

        if (!IS_SLASH(*(curr + 1)) && !IS_ZERO(*(curr + 1)))
            start = ++curr;
    }
    if (slash > start)
        *slash = '\0';

    return start;
}

char *
cra_dirname(char *path)
{
    char *curr, *start, *slash, *last_slash;

    if (!path || IS_ZERO(*path))
        return ".";

    slash = NULL;
    last_slash = NULL;
    for (curr = start = path; !IS_ZERO(*curr); ++curr)
    {
        if (!IS_SLASH(*curr))
            continue;

        if (curr != path && !IS_SLASH(*(curr - 1)))
        {
            last_slash = slash;
            slash = curr;
        }

        if (!IS_SLASH(*(curr + 1)) && !IS_ZERO(*(curr + 1)))
            start = ++curr;
    }
    if (slash && slash <= start)
        *slash = '\0';
    else if (last_slash)
        *last_slash = '\0';
    else if (IS_SLASH(*path))
        *(path + 1) = '\0';
    else
        return ".";

    return path;
}