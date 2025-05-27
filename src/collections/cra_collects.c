/**
 * @file cra_collects.c
 * @author Cracal
 * @brief collections
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "collections/cra_collects.h"

cra_hash_t cra_hash_string1_p(const char **val)
{
    cra_hash_t hash = 0;
    const char *v = *val;
    for (size_t i = 0; *v; ++i)
    {
        if ((i & 1) == 0)
            hash ^= ((hash << 7) ^ (*v++) ^ (hash >> 3));
        else
            hash ^= (~((hash << 11) ^ (*v++) ^ (hash >> 5)));
    }
    return hash == -1 ? -2 : hash;
}

cra_hash_t cra_hash_string2_p(const char **val)
{
    cra_hash_t hash = 0;
    cra_hash_t seed = 131;
    const char *v = *val;
    while (*v)
        hash = hash * seed + (*v++);
    return hash == -1 ? -2 : hash;
}
