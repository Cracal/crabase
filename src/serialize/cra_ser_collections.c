#include "serialize/cra_ser_collections.h"
#include "cra_assert.h"
#include "cra_malloc.h"

#if 1 // alist

static void
cra_alist_szer_iter_init(void *obj, uint64_t *retlen, void *it, size_t itsize)
{
    assert(itsize >= sizeof(CraAListIter));
    CRA_UNUSED_VALUE(itsize);

    CraAList *list = (CraAList *)obj;
    *retlen = cra_alist_get_count(list);
    cra_alist_iter_init(list, (CraAListIter *)it);
}

static bool
cra_alist_szer_iter_next(void *it, void **retval1, void **retval2)
{
    assert(retval1);
    CRA_UNUSED_VALUE(retval2);
    return cra_alist_iter_next((CraAListIter *)it, retval1);
}

static bool
cra_alist_szer_append(void *obj, void *val1, void *val2)
{
    CRA_UNUSED_VALUE(val2);
    return cra_alist_append((CraAList *)obj, val1);
}

static void
cra_alist_szer_init(void *obj, CraInitArgs *da)
{
    cra_alist_init_size((CraAList *)obj, da->val1size, da->length, true);
}

static void
cra_alist_szer_uninit(void *obj, bool b)
{
    CRA_UNUSED_VALUE(b);
    cra_alist_uninit((CraAList *)obj);
}

const CraSerializable_i cra_g_alist_szer_i = {
    .init_i.init = cra_alist_szer_init,
    .init_i.uninit = cra_alist_szer_uninit,
    .iter_init = cra_alist_szer_iter_init,
    .iter_next = cra_alist_szer_iter_next,
    .add = cra_alist_szer_append,
};

#endif // end alist

#if 1 // llist

static void
cra_llist_szer_iter_init(void *obj, uint64_t *retlen, void *it, size_t itsize)
{
    assert(itsize >= sizeof(CraLListIter));
    CRA_UNUSED_VALUE(itsize);

    CraLList *list = (CraLList *)obj;
    *retlen = cra_llist_get_count(list);
    cra_llist_iter_init(list, (CraLListIter *)it);
}

static bool
cra_llist_szer_iter_next(void *it, void **retval1, void **retval2)
{
    assert(retval1);
    CRA_UNUSED_VALUE(retval2);
    return cra_llist_iter_next((CraLListIter *)it, retval1);
}

static bool
cra_llist_szer_append(void *obj, void *val1, void *val2)
{
    CRA_UNUSED_VALUE(val2);
    return cra_llist_append((CraLList *)obj, val1);
}

static void
cra_llist_szer_init(void *obj, CraInitArgs *da)
{
    cra_llist_init_size((CraLList *)obj, da->val1size, da->length, true);
}

static void
cra_llist_szer_uninit(void *obj, bool b)
{
    CRA_UNUSED_VALUE(b);
    cra_llist_uninit((CraLList *)obj);
}

const CraSerializable_i cra_g_llist_szer_i = {
    .init_i.init = cra_llist_szer_init,
    .init_i.uninit = cra_llist_szer_uninit,
    .iter_init = cra_llist_szer_iter_init,
    .iter_next = cra_llist_szer_iter_next,
    .add = cra_llist_szer_append,
};

#endif // end llist

#if 1 // deque

static void
cra_deque_szer_iter_init(void *obj, uint64_t *retlen, void *it, size_t itsize)
{
    assert(itsize >= sizeof(CraDequeIter));
    CRA_UNUSED_VALUE(itsize);

    CraDeque *list = (CraDeque *)obj;
    *retlen = cra_deque_get_count(list);
    cra_deque_iter_init(list, (CraDequeIter *)it);
}

static bool
cra_deque_szer_iter_next(void *it, void **retval1, void **retval2)
{
    assert(retval1);
    CRA_UNUSED_VALUE(retval2);
    return cra_deque_iter_next((CraDequeIter *)it, retval1);
}

static bool
cra_deque_szer_append(void *obj, void *val1, void *val2)
{
    CRA_UNUSED_VALUE(val2);
    return cra_deque_push((CraDeque *)obj, val1);
}

static void
cra_deque_szer_init(void *obj, CraInitArgs *da)
{
    cra_deque_init((CraDeque *)obj, da->val1size, CRA_DEQUE_INFINITE, true);
}

static void
cra_deque_szer_uninit(void *obj, bool b)
{
    CRA_UNUSED_VALUE(b);
    cra_deque_uninit((CraDeque *)obj);
}

const CraSerializable_i cra_g_deque_szer_i = {
    .init_i.init = cra_deque_szer_init,
    .init_i.uninit = cra_deque_szer_uninit,
    .iter_init = cra_deque_szer_iter_init,
    .iter_next = cra_deque_szer_iter_next,
    .add = cra_deque_szer_append,
};

#endif // end deque

#if 1 // dict

static void
cra_dict_szer_iter_init(void *obj, uint64_t *retlen, void *it, size_t itsize)
{
    assert(itsize >= sizeof(CraDictIter));
    CRA_UNUSED_VALUE(itsize);

    CraDict *dict = (CraDict *)obj;
    *retlen = cra_dict_get_count(dict);
    cra_dict_iter_init(dict, (CraDictIter *)it);
}

static bool
cra_dict_szer_iter_next(void *it, void **retval1, void **retval2)
{
    return cra_dict_iter_next((CraDictIter *)it, retval1, retval2);
}

static bool
cra_dict_szer_append(void *obj, void *val1, void *val2)
{
    return cra_dict_add((CraDict *)obj, val1, val2);
}

static void
cra_dict_szer_init(void *obj, CraInitArgs *da)
{
    CraDictSerArgs *arg = (CraDictSerArgs *)da->arg;

    assert(arg);
    assert(arg->hash);
    assert(arg->compare);

    cra_dict_init((CraDict *)obj, da->val1size, da->val2size, true, arg->hash, arg->compare);
}

static void
cra_dict_szer_uninit(void *obj, bool b)
{
    CRA_UNUSED_VALUE(b);
    cra_dict_uninit((CraDict *)obj);
}

const CraSerializable_i cra_g_dict_szer_i = {
    .init_i.init = cra_dict_szer_init,
    .init_i.uninit = cra_dict_szer_uninit,
    .iter_init = cra_dict_szer_iter_init,
    .iter_next = cra_dict_szer_iter_next,
    .add = cra_dict_szer_append,
};

#endif // end dict
