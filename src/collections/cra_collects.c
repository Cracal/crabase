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

#ifdef CRA_RANDOM_STR_INIT_HASHCODE
#include <time.h>
#include "cra_atomic.h"

static cra_atomic_flag s_hash_locker = {0};
#undef LOCK
#undef UNLOCK
#define LOCK() while (cra_atomic_flag_test_and_set(&s_hash_locker))
#define UNLOCK() cra_atomic_flag_clear(&s_hash_locker)

static cra_hash_t cra_get_init_hash(void)
{
    static volatile cra_hash_t hash = -1;
    if (hash == -1)
    {
        LOCK();
        if (hash == -1)
        {
            srand((unsigned int)time(NULL));
            hash = rand();
        }
        UNLOCK();
    }
    return hash;
}
#else
#define cra_get_init_hash() 0
#endif

// BKDR hash function
cra_hash_t cra_hash_string1(const char *val)
{
    cra_hash_t hash = cra_get_init_hash();
    cra_hash_t seed = 131;
    while (*val)
        hash = hash * seed + (*val++);
    return hash == -1 ? -2 : hash;
}

// AP hash function
cra_hash_t cra_hash_string2(const char *val)
{
    cra_hash_t hash = cra_get_init_hash();
    for (size_t i = 0; *val; ++i)
    {
        if ((i & 1) == 0)
            hash ^= ((hash << 7) ^ (*val++) ^ (hash >> 3));
        else
            hash ^= (~((hash << 11) ^ (*val++) ^ (hash >> 5)));
    }
    return hash == -1 ? -2 : hash;
}
