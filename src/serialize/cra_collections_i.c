#include "serialize/cra_collections_i.h"

// ====================================== alist ======================================

// initializable

static CRA_INITIALIZABLE_INIT_FN(cra_alist_initializable_init)
{
    assert(obj);
    assert(params);
    CraAListInitializableParam *param = (CraAListInitializableParam *)params;
    return (cra_alist_init_with_size)((CraAList *)obj, param->itemsize, length);
}

CRA_INITIALIZABLE_DEF(cra_g_alist_initializable_i) = {
    .init = cra_alist_initializable_init,
    .uninit = (CRA_INITIALIZABLE_UNINIT_FN((*)))cra_alist_uninit,
};

// appendable

static CRA_APPENDABLE_APPEND_FN(cra_alist_appendable_append)
{
    assert(obj);
    assert(val);
    assert(val->val_ref);
    CraAList *list = (CraAList *)obj;
    return (cra_alist_insert)(list, list->count, val->val_ref);
}

CRA_APPENDABLE_DEF(cra_g_alist_appendable_i) = {
    .append = cra_alist_appendable_append,
};

// iterable

static CRA_ITERABLE_INIT_FN(cra_alist_iterable_init)
{
    assert(it);
    assert(obj);

    CraAList *list = (CraAList *)obj;

    if (retcnt)
        *retcnt = list->count;

    it->ic1.idx = reverse ? list->count : 0;
    it->obj = obj;

    return list->count > 0;
}

static CRA_ITERABLE_NEXT_FN(cra_alist_iterable_next)
{
    assert(it);
    assert(val);
    assert(it->obj);

    CraAList *list = (CraAList *)it->obj;
    if (it->ic1.idx < list->count)
    {
        val->val_ref = CRA_ALIST_PVAL(list, it->ic1.idx++);
        return true;
    }
    return false;
}

static CRA_ITERABLE_PREV_FN(cra_alist_iterable_prev)
{
    assert(it);
    assert(val);
    assert(it->obj);

    CraAList *list = (CraAList *)it->obj;
    if (it->ic1.idx > 0)
    {
        val->val_ref = CRA_ALIST_PVAL(list, --it->ic1.idx);
        return true;
    }
    return false;
}

CRA_ITERABLE_DEF(cra_g_alist_iterable_i) = {
    .init = cra_alist_iterable_init,
    .next = cra_alist_iterable_next,
    .prev = cra_alist_iterable_prev,
};

// ====================================== llist ======================================

// initializable

static CRA_INITIALIZABLE_INIT_FN(cra_llist_initializable_init)
{
    CraLList                   *list = (CraLList *)obj;
    CraLListInitializableParam *param = (CraLListInitializableParam *)params;

    assert(list);
    assert(param);
    assert(param->itemsize > 0);

    return (cra_llist_init_with_size)(list, param->itemsize, length);
}

CRA_INITIALIZABLE_DEF(cra_g_llist_initializable_i) = {
    .init = cra_llist_initializable_init,
    .uninit = (CRA_INITIALIZABLE_UNINIT_FN((*)))cra_llist_uninit,
};

// appendable

static CRA_APPENDABLE_APPEND_FN(cra_llist_appendable_append)
{
    CraLList *list;

    assert(obj);
    assert(val);
    assert(val->val_ref);

    list = (CraLList *)obj;
    return (cra_llist_insert)(list, list->count, val->val_ref);
}

CRA_APPENDABLE_DEF(cra_g_llist_appendable_i) = {
    .append = cra_llist_appendable_append,
};

// iterable

static CRA_ITERABLE_INIT_FN(cra_llist_iterable_init)
{
    CraLList *list = (CraLList *)obj;

    assert(it);
    assert(list);

    if (retcnt)
        *retcnt = list->count;

    it->obj = list;
    it->ic1.cur = list->head;
    if (reverse && list->head)
        it->ic1.cur = list->head->prev;

    return !!list->head;
}

static CRA_ITERABLE_NEXT_FN(cra_llist_iterable_next)
{
    CraLList     *list;
    CraLListNode *curr, *next;

    assert(it);
    assert(val);
    assert(it->obj);

    list = (CraLList *)it->obj;
    curr = (CraLListNode *)it->ic1.cur;

    if (!curr)
        return false;

    assert(curr->next);
    assert(curr->prev);

    next = curr->next;
    val->val_ref = curr->val;
    it->ic1.cur = next != list->head ? next : NULL;
    return true;
}

static CRA_ITERABLE_PREV_FN(cra_llist_iterable_prev)
{
    CraLList     *list;
    CraLListNode *curr, *prev;

    assert(it);
    assert(val);
    assert(it->obj);

    list = (CraLList *)it->obj;
    curr = (CraLListNode *)it->ic1.cur;

    if (!curr)
        return false;

    assert(curr->next);
    assert(curr->prev);

    prev = curr->prev;
    val->val_ref = curr->val;
    it->ic1.cur = prev != list->head->prev ? prev : NULL;
    return true;
}

CRA_ITERABLE_DEF(cra_g_llist_iterable_i) = {
    .init = cra_llist_iterable_init,
    .next = cra_llist_iterable_next,
    .prev = cra_llist_iterable_prev,
};

// ====================================== deque ======================================

// initializable

static CRA_INITIALIZABLE_INIT_FN(cra_deque_initializable_init)
{
    CraDeque                   *deque;
    CraDequeInitializableParam *param;

    assert(obj);
    assert(params);

    deque = (CraDeque *)obj;
    param = (CraDequeInitializableParam *)params;
    return (cra_deque_init_with_size)(deque, param->itemsize, length);
}

CRA_INITIALIZABLE_DEF(cra_g_deque_initializable_i) = {
    .init = cra_deque_initializable_init,
    .uninit = (CRA_INITIALIZABLE_UNINIT_FN((*)))cra_deque_uninit,
};

// appendable

static CRA_APPENDABLE_APPEND_FN(cra_deque_appendable_append)
{
    CraDeque *deque = (CraDeque *)obj;

    assert(val);
    assert(deque);
    assert(val->val_ref);
    assert(deque->itemsize > 0);

    return (cra_deque_push_back)(deque, val->val_ref);
}

CRA_APPENDABLE_DEF(cra_g_deque_appendable_i) = {
    .append = cra_deque_appendable_append,
};

// iterable

static CRA_ITERABLE_INIT_FN(cra_deque_iterable_init)
{
    CRA_UNUSED(it);
    CRA_UNUSED(obj);
    CRA_UNUSED(retcnt);
    CRA_UNUSED(reverse);
    return false;
}

static CRA_ITERABLE_NEXT_FN(cra_deque_iterable_next)
{
    CRA_UNUSED(it);
    CRA_UNUSED(val);
    return false;
}

static CRA_ITERABLE_PREV_FN(cra_deque_iterable_prev)
{
    CRA_UNUSED(it);
    CRA_UNUSED(val);
    return false;
}

CRA_ITERABLE_DEF(cra_g_deque_iterable_i) = {
    .init = cra_deque_iterable_init,
    .next = cra_deque_iterable_next,
    .prev = cra_deque_iterable_prev,
};

// ====================================== dict ======================================

// initializable

static CRA_INITIALIZABLE_INIT_FN(cra_dict_initializable_init)
{
    CraDict                   *dict = (CraDict *)obj;
    CraDictInitializableParam *param = (CraDictInitializableParam *)params;

    assert(dict);
    assert(param);

    return (cra_dict_init_with_size)(dict, param->key_size, param->val_size, param->key_align, param->val_align, length,
                                     param->hash_key, param->compare_key);
}

CRA_INITIALIZABLE_DEF(cra_g_dict_initializable_i) = {
    .init = cra_dict_initializable_init,
    .uninit = (CRA_INITIALIZABLE_UNINIT_FN((*)))cra_dict_uninit,
};

// appendable

static CRA_APPENDABLE_APPEND_FN(cra_dict_appendable_append)
{
    assert(obj);
    assert(val);
    assert(val->key_ref);
    assert(val->val_ref);

    CraDict *dict = (CraDict *)obj;
    return (cra_dict_put_and_return_kv)(dict, val->key_ref, val->val_ref, NULL, NULL, true);
}

CRA_APPENDABLE_DEF(cra_g_dict_appendable_i) = {
    .append = cra_dict_appendable_append,
};

// iterable

static CRA_ITERABLE_INIT_FN(cra_dict_iterable_init)
{
    CRA_UNUSED(it);
    CRA_UNUSED(obj);
    CRA_UNUSED(retcnt);
    CRA_UNUSED(reverse);
    return false;
}

static CRA_ITERABLE_NEXT_FN(cra_dict_iterable_next)
{
    CRA_UNUSED(it);
    CRA_UNUSED(val);
    return false;
}

static CRA_ITERABLE_PREV_FN(cra_dict_iterable_prev)
{
    CRA_UNUSED(it);
    CRA_UNUSED(val);
    return false;
}

CRA_ITERABLE_DEF(cra_g_dict_iterable_i) = {
    .init = cra_dict_iterable_init,
    .next = cra_dict_iterable_next,
    .prev = cra_dict_iterable_prev,
};
