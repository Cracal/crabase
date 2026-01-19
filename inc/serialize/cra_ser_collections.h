/**
 * @file cra_ser_collections.h
 * @author Cracal
 * @brief serialize collections
 * @version 0.1
 * @date 2026-01-15
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __CRA_SER_COLLECTIONS_H__
#define __CRA_SER_COLLECTIONS_H__
#include "collections/cra_alist.h"
#include "collections/cra_deque.h"
#include "collections/cra_dict.h"
#include "collections/cra_llist.h"
#include "serialize/cra_serialize.h"

#if 1 // alist

CRA_API const CraSerializable_i cra_g_alist_szer_i;
#define CRA_ALIST_SZER_I (&cra_g_alist_szer_i)

#endif // end alist

#if 1 // llist

CRA_API const CraSerializable_i cra_g_llist_szer_i;
#define CRA_LLIST_SZER_I (&cra_g_llist_szer_i)

#endif // end llist

#if 1 // deque

CRA_API const CraSerializable_i cra_g_deque_szer_i;
#define CRA_DEQUE_SZER_I (&cra_g_deque_szer_i)

#endif // end deque

#if 1 // dict

typedef struct _CraDictSerArgs
{
    cra_hash_fn    hash;
    cra_compare_fn compare;
} CraDictSerArgs;

CRA_API const CraSerializable_i cra_g_dict_szer_i;
#define CRA_DICT_SZER_I (&cra_g_dict_szer_i)

#endif // end dict

#endif