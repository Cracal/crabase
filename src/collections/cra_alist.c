/**
 * @file cra_list.c
 * @author Cracal
 * @brief 动态数组
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "cra_malloc.h"
#include "collections/cra_alist.h"

#define _CRA_ALIST_DEFAULT_CAPACITY 32

#define _CRA_ALIST_VALUE_PTR(_list, _index) ((_list)->array + (_index) * (_list)->ele_size)

static inline bool __cra_alist_pop__(CraAList *list, size_t index, void *retval)
{
    if (index >= list->count)
        return false;

    if (retval)
        memcpy(retval, _CRA_ALIST_VALUE_PTR(list, index), list->ele_size);
    else if (list->remove_val)
        list->remove_val(_CRA_ALIST_VALUE_PTR(list, index));

    size_t move_count = list->count - index;
    if (move_count > 1)
        memmove(_CRA_ALIST_VALUE_PTR(list, index),
                _CRA_ALIST_VALUE_PTR(list, index + 1),
                --move_count * list->ele_size);
    --list->count;
    if (list->zero_memory)
        bzero(list->array + list->count * list->ele_size, list->ele_size);

    // 缩容检查
    if (list->capacity > _CRA_ALIST_DEFAULT_CAPACITY && list->count <= (list->capacity >> 1))
    {
        if (!cra_alist_resize(list, CRA_MAX(list->capacity >> 1, _CRA_ALIST_DEFAULT_CAPACITY)))
            return false;
    }
    return true;
}

static inline bool __cra_alist_set_and_pop_old__(CraAList *list, size_t index, void *newval, void *retoldval)
{
    if (index >= list->count)
        return false;
    if (retoldval)
        memcpy(retoldval, _CRA_ALIST_VALUE_PTR(list, index), list->ele_size);
    else if (list->remove_val)
        list->remove_val(_CRA_ALIST_VALUE_PTR(list, index));
    memcpy(_CRA_ALIST_VALUE_PTR(list, index), newval, list->ele_size);
    return true;
}

CraAListIter cra_alist_iter_init(CraAList *list)
{
    CraAListIter it;
    it.index = 0;
    it.list = list;
    return it;
}

bool cra_alist_iter_next(CraAListIter *it, void **retvalptr)
{
    if (it->index < it->list->count)
    {
        *retvalptr = _CRA_ALIST_VALUE_PTR(it->list, it->index++);
        return true;
    }
    return false;
}

void cra_alist_init(CraAList *list, size_t element_size, size_t init_capacity,
                    bool zero_memory, cra_remove_val_fn remove_val)
{
    assert(!!list && element_size > 0);
    list->remove_val = remove_val;
    list->zero_memory = zero_memory;
    list->count = 0;
    list->ele_size = element_size;
    list->capacity = init_capacity == 0 ? 8 : init_capacity;
    list->array = cra_malloc(element_size * list->capacity);
    if (zero_memory)
        bzero(list->array, element_size * list->capacity);
}

void cra_alist_uninit(CraAList *list)
{
    cra_alist_clear(list);
    cra_free(list->array);
    list->array = NULL;
}

void cra_alist_clear(CraAList *list)
{
    if (!!list->remove_val)
    {
        for (size_t i = 0; i < list->count; i++)
            list->remove_val(_CRA_ALIST_VALUE_PTR(list, i));
    }
    if (list->zero_memory)
        bzero(list->array, list->ele_size * list->count);
    list->count = 0;
}

bool cra_alist_resize(CraAList *list, size_t new_capacity)
{
    unsigned char *newarr;
    // 新容量必须大于alist存有的元素个数
    if (new_capacity < list->count)
        return false;
    newarr = (unsigned char *)cra_realloc(list->array, new_capacity * list->ele_size);
    if (list->zero_memory && new_capacity > list->capacity)
        bzero(newarr + list->capacity * list->ele_size, (new_capacity - list->capacity) * list->ele_size);
    list->array = newarr;
    list->capacity = new_capacity;
    return true;
}

bool cra_alist_insert(CraAList *list, size_t index, void *val)
{
    if (index > list->count)
        return false;
    if (list->count >= list->capacity)
    {
        if (!cra_alist_resize(list, list->capacity + (list->capacity >> 1)))
            return false;
    }
    size_t move_count = list->count - index;
    if (move_count > 0)
        memmove(_CRA_ALIST_VALUE_PTR(list, index + 1),
                _CRA_ALIST_VALUE_PTR(list, index),
                move_count * list->ele_size);
    memcpy(_CRA_ALIST_VALUE_PTR(list, index), val, list->ele_size);
    list->count++;
    return true;
}

bool cra_alist_remove_at(CraAList *list, size_t index)
{
    return __cra_alist_pop__(list, index, NULL);
}

bool cra_alist_pop(CraAList *list, size_t index, void *retval)
{
    return __cra_alist_pop__(list, index, retval);
}

size_t cra_alist_remove_match(CraAList *list, cra_match_fn match, void *arg)
{
    size_t remove_count = 0;

    assert(match != NULL);

    for (size_t i = 0; i < list->count;)
    {
        if (match(_CRA_ALIST_VALUE_PTR(list, i), arg))
        {
            __cra_alist_pop__(list, i, NULL);
            remove_count++;
            continue;
        }
        i++;
    }
    return remove_count;
}

bool cra_alist_set(CraAList *list, size_t index, void *newval)
{
    return __cra_alist_set_and_pop_old__(list, index, newval, NULL);
}

bool cra_alist_set_and_pop_old(CraAList *list, size_t index, void *newval, void *retoldval)
{
    return __cra_alist_set_and_pop_old__(list, index, newval, retoldval);
}

bool cra_alist_get(CraAList *list, size_t index, void *retval)
{
    if (index >= list->count)
        return false;
    memcpy(retval, _CRA_ALIST_VALUE_PTR(list, index), list->ele_size);
    return true;
}

bool cra_alist_get_ptr(CraAList *list, size_t index, void **retvalptr)
{
    if (index >= list->count)
        return false;
    *retvalptr = _CRA_ALIST_VALUE_PTR(list, index);
    return true;
}

void cra_alist_reverse(CraAList *list)
{
    size_t middle;
    unsigned char *val1, *val2;

#ifdef CRA_COMPILER_MSVC
    unsigned char *temp = cra_malloc(list->ele_size);
#else
    unsigned char temp[list->ele_size];
#endif

    middle = list->count >> 1;
    for (size_t i = 0; i < middle; i++)
    {
        val1 = _CRA_ALIST_VALUE_PTR(list, i);
        val2 = _CRA_ALIST_VALUE_PTR(list, list->count - 1 - i);
        memcpy(temp, val1, list->ele_size);
        memcpy(val1, val2, list->ele_size);
        memcpy(val2, temp, list->ele_size);
    }

#ifdef CRA_COMPILER_MSVC
    cra_free(temp);
#endif
}

CraAList *cra_alist_clone(CraAList *list, cra_deep_copy_val_fn deep_copy_val)
{
    CraAList *ret = cra_alloc(CraAList);
    cra_alist_init(ret, list->ele_size, list->count, list->zero_memory, list->remove_val);

    ret->count = list->count;
    if (!!deep_copy_val)
    {
        for (size_t i = 0; i < list->count; i++)
        {
            deep_copy_val(_CRA_ALIST_VALUE_PTR(list, i),
                          _CRA_ALIST_VALUE_PTR(ret, i));
        }
    }
    else
    {
        memcpy(ret->array, list->array, list->ele_size * list->count);
    }
    return ret;
}

static size_t cra_alist_partition(CraAList *list, cra_compare_fn compare, size_t begin, size_t end)
{
    size_t left, middle, right;
    void *val1, *val2;

#ifdef CRA_COMPILER_MSVC
    unsigned char *temp = cra_malloc(list->ele_size);
#else
    unsigned char temp[list->ele_size];
#endif

    left = begin;
    middle = end - 1;
    right = end - 2;

#define _CRA_ALIST_SWAP_VAL(_i, _j)        \
    val1 = _CRA_ALIST_VALUE_PTR(list, _i); \
    val2 = _CRA_ALIST_VALUE_PTR(list, _j); \
    memcpy(temp, val1, list->ele_size);    \
    memcpy(val1, val2, list->ele_size);    \
    memcpy(val2, temp, list->ele_size)

    while (left < right)
    {
        while (left < right && compare(_CRA_ALIST_VALUE_PTR(list, left), _CRA_ALIST_VALUE_PTR(list, middle)) <= 0)
            left++;
        while (left < right && compare(_CRA_ALIST_VALUE_PTR(list, right), _CRA_ALIST_VALUE_PTR(list, middle)) >= 0)
            right--;
        if (left != right)
        {
            _CRA_ALIST_SWAP_VAL(left, right);
        }
    }

    if (compare(_CRA_ALIST_VALUE_PTR(list, left), _CRA_ALIST_VALUE_PTR(list, middle)) > 0)
    {
        _CRA_ALIST_SWAP_VAL(left, middle);
    }

#undef _CRA_ALIST_SWAP_VAL

#ifdef CRA_COMPILER_MSVC
    cra_free(temp);
#endif

    return left + 1;
}

static void cra_alist_quick_sort(CraAList *list, cra_compare_fn compare, size_t begin, size_t end)
{
    if (begin >= end - 1)
        return;

    size_t middle = cra_alist_partition(list, compare, begin, end);
    cra_alist_quick_sort(list, compare, begin, middle);
    cra_alist_quick_sort(list, compare, middle, end);
}

void cra_alist_sort(CraAList *list, cra_compare_fn compare)
{
    assert(compare != NULL);
    if (list->count > 1)
        cra_alist_quick_sort(list, compare, 0, list->count);
}

static size_t cra_alist_binary_seach(CraAList *list, cra_compare_fn compare, void *val)
{
    int res;
    size_t left, middle, right;

    left = 0;
    middle = list->count;
    right = list->count - 1;
    while (left <= right)
    {
        middle = (left + right) >> 1;
        res = compare(_CRA_ALIST_VALUE_PTR(list, middle), val);
        if (res == 0)
        {
            return middle;
        }
        else if (res > 0)
        {
            if (middle == 0)
                break;
            right = middle - 1;
        }
        else
        {
            left = ++middle;
        }
    }
    return middle;
}

bool cra_alist_add_sort(CraAList *list, cra_compare_fn compare, void *val)
{
    size_t index;

    assert(compare != NULL);

    index = list->count == 0
                ? 0
                : cra_alist_binary_seach(list, compare, val);
    return cra_alist_insert(list, index, val);
}

// ===============

static void cra_alist_ser_iter_init(void *obj, void *const it, size_t itbufsize)
{
    assert(sizeof(CraAListIter) <= itbufsize);
    CraAList *list = (CraAList *)obj;
    CraAListIter _it = cra_alist_iter_init(list);
    memcpy(it, &_it, sizeof(CraAListIter));
}

static bool cra_alist_ser_iter_append(void *obj, void *val)
{
    return cra_alist_append((CraAList *)obj, val);
}

static void cra_alist_ser_init(void *obj, void *args)
{
    assert_always(args != NULL);

    CraAList *list = (CraAList *)obj;
    CraAListSerInitArgs *params = (CraAListSerInitArgs *)args;

    cra_alist_init(list, params->element_size, 8, params->zero_memory, params->remove_val_fn);
}

const ICraTypeIter g_alist_ser_iter_i = {
    .list.init = cra_alist_ser_iter_init,
    .list.next = (bool (*)(void *, void **))cra_alist_iter_next,
    .list.append = cra_alist_ser_iter_append,
};

const ICraTypeInit g_alist_ser_init_i = {
    .alloc = NULL,
    .dealloc = NULL,
    .init = cra_alist_ser_init,
    .uinit = (void (*)(void *))cra_alist_uninit,
};
