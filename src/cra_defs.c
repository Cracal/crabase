#include "cra_defs.h"

#define IS_SLASH(_c) ((_c) == '/' || (_c) == '\\')
#define IS_ZERO(_c)  ((_c) == '\0')

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