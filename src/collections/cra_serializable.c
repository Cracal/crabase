#include "collections/cra_serializable.h"

#if 1 // for alist

void
cra_alist_srlz_iter_init(void *iter, size_t itersize, void *list, size_t *retcount)
{
    assert(iter);
    assert(list);
    assert(retcount);
    CRA_UNUSED(itersize);
    assert(itersize >= sizeof(CraAListIter) + sizeof(void *));

    CraAList *l = (CraAList *)list;
    *retcount = (size_t)l->count;
    memcpy((unsigned char *)iter + sizeof(CraAListIter), &l, sizeof(void *)); // iter->list = l
}

bool
cra_alist_srlz_iter_next(void *iter, void *retitem)
{
    assert(iter);
    CraAList     *list;
    CraAListIter *it = (CraAListIter *)iter;
    memcpy(&list, (unsigned char *)it + sizeof(CraAListIter), sizeof(void *)); // list = iter->list
    return CraAList_iter_next(it, list, retitem);
}

bool
cra_alist_srlz_append(void *list, void *item)
{
    assert(list);
    assert(item);
    CraAList *l = (CraAList *)list;
    return (cra_alist_insert)(l, l->count, item);
}

bool
cra_alist_srlz_init(void *list, size_t itemsize, size_t count)
{
    return (cra_alist_init_with_size)((CraAList *)list, itemsize, count);
}

#endif // end for alist

#if 1 // for llist

void
cra_llist_srlz_iter_init(void *iter, size_t itersize, void *list, size_t *retcount)
{
    assert(iter);
    assert(list);
    assert(retcount);
    CRA_UNUSED(itersize);
    assert(itersize >= sizeof(CraLListIter) + sizeof(void *));

    CraLList *l = (CraLList *)list;
    *retcount = (size_t)l->count;
    memcpy((unsigned char *)iter + sizeof(CraLListIter), &l, sizeof(void *)); // iter->list = l
}

bool
cra_llist_srlz_iter_next(void *iter, void *retitem)
{
    assert(iter);
    CraLList     *list;
    CraLListIter *it = (CraLListIter *)iter;
    memcpy(&list, (unsigned char *)it + sizeof(CraLListIter), sizeof(void *)); // list = iter->list
    return CraLList_iter_next(it, list, retitem);
}

bool
cra_llist_srlz_append(void *list, void *item)
{
    assert(list);
    assert(item);
    CraLList *l = (CraLList *)list;
    return (cra_llist_insert)(l, l->count, item);
}

bool
cra_llist_srlz_init(void *list, size_t itemsize, size_t count)
{
    return (cra_llist_init_with_size)((CraLList *)list, itemsize, count);
}

#endif // end for llist

#if 1 // for deque

void
cra_deque_srlz_iter_init(void *iter, size_t itersize, void *list, size_t *retcount)
{
    assert(iter);
    assert(list);
    assert(retcount);
    CRA_UNUSED(itersize);
    assert(itersize >= sizeof(CraDequeIter) + sizeof(void *));

    CraDeque *l = (CraDeque *)list;
    *retcount = (size_t)l->count;
    memcpy((unsigned char *)iter + sizeof(CraDequeIter), &l, sizeof(void *)); // iter->list = l
}

bool
cra_deque_srlz_iter_next(void *iter, void *retitem)
{
    assert(iter);
    CraDeque     *list;
    CraDequeIter *it = (CraDequeIter *)iter;
    memcpy(&list, (unsigned char *)it + sizeof(CraDequeIter), sizeof(void *)); // list = iter->list
    return CraDeque_iter_next(it, list, retitem);
}

bool
cra_deque_srlz_append(void *list, void *item)
{
    assert(list);
    assert(item);
    CraDeque *l = (CraDeque *)list;
    return (cra_deque_insert)(l, l->count, item);
}

bool
cra_deque_srlz_init(void *list, size_t itemsize, size_t count)
{
    return (cra_deque_init_with_size)((CraDeque *)list, itemsize, count);
}

#endif // end for deque

#if 1 // for dict

void
cra_dict_srlz_iter_init(void *iter, size_t itersize, void *dict, size_t *retcount)
{
    assert(iter);
    assert(dict);
    assert(retcount);
    CRA_UNUSED(itersize);
    assert(itersize >= sizeof(CraDictIter) + sizeof(void *));

    CraDict *d = (CraDict *)dict;
    *retcount = (size_t)d->count;
    memcpy((unsigned char *)iter + sizeof(CraDictIter), &d, sizeof(void *)); // iter->dict = d
}

bool
cra_dict_srlz_iter_next(void *iter, void *retkey, void *retval)
{
    assert(iter);
    CraDict     *dict;
    CraDictIter *it = (CraDictIter *)iter;
    memcpy(&dict, (unsigned char *)it + sizeof(CraDictIter), sizeof(void *)); // dict = iter->dict;
    return CraDict_iter_next(it, dict, retkey, retval);
}

bool
cra_dict_srlz_append(void *dict, void *key, void *val)
{
    assert(dict);
    assert(key);
    assert(val);
    CraDict *d = (CraDict *)dict;
    return (cra_dict_put_and_return_kv)(d, key, val, NULL, NULL, true);
}

#endif // end for dict
