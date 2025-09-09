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
#include "collections/cra_alist.h"
#include "cra_assert.h"
#include "cra_malloc.h"

#define _CRA_ALIST_VALUE_PTR(_list, _index) ((_list)->array + (_index) * (_list)->ele_size)

static inline bool
__cra_alist_pop__(CraAList *list, size_t index, void *retval)
{
    if (index >= list->count)
        return false;

    if (retval)
        memcpy(retval, _CRA_ALIST_VALUE_PTR(list, index), list->ele_size);

    size_t move_count = list->count - index;
    if (move_count > 1)
    {
        memmove(
          _CRA_ALIST_VALUE_PTR(list, index), _CRA_ALIST_VALUE_PTR(list, index + 1), --move_count * list->ele_size);
    }
    --list->count;
    if (list->zero_memory)
        bzero(list->array + list->count * list->ele_size, list->ele_size);
    return true;
}

static inline bool
__cra_alist_set_and_pop_old__(CraAList *list, size_t index, void *newval, void *retoldval)
{
    if (index >= list->count)
        return false;
    if (retoldval)
        memcpy(retoldval, _CRA_ALIST_VALUE_PTR(list, index), list->ele_size);
    memcpy(_CRA_ALIST_VALUE_PTR(list, index), newval, list->ele_size);
    return true;
}

static inline void
__cra_alist_init_size(CraAList *list, size_t element_size, size_t init_capacity, bool zero_memory)
{
    assert(!!list && element_size > 0);
    list->zero_memory = zero_memory;
    list->count = 0;
    list->ele_size = element_size;
    list->capacity = init_capacity == 0 ? 8 : init_capacity;
    list->array = (unsigned char *)cra_malloc(element_size * list->capacity);
    if (zero_memory)
        bzero(list->array, element_size * list->capacity);
}

void
cra_alist_iter_init(CraAList *list, CraAListIter *it)
{
    it->index = 0;
    it->list = list;
}

bool
cra_alist_iter_next(CraAListIter *it, void **retvalptr)
{
    if (it->index < it->list->count)
    {
        if (retvalptr)
            *retvalptr = _CRA_ALIST_VALUE_PTR(it->list, it->index++);
        return true;
    }
    return false;
}

void
cra_alist_init_size(CraAList *list, size_t element_size, size_t init_capacity, bool zero_memory)
{
    __cra_alist_init_size(list, element_size, init_capacity, zero_memory);
}

void
cra_alist_init(CraAList *list, size_t element_size, bool zero_memory)
{
    __cra_alist_init_size(list, element_size, CRA_ALIST_INIT_CAPACITY, zero_memory);
}

void
cra_alist_uninit(CraAList *list)
{
    cra_alist_clear(list);
    cra_free(list->array);
    list->array = NULL;
}

void
cra_alist_clear(CraAList *list)
{
    if (list->zero_memory)
        bzero(list->array, list->ele_size * list->count);
    list->count = 0;
}

bool
cra_alist_resize(CraAList *list, size_t new_capacity)
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

bool
cra_alist_insert(CraAList *list, size_t index, void *val)
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
        memmove(_CRA_ALIST_VALUE_PTR(list, index + 1), _CRA_ALIST_VALUE_PTR(list, index), move_count * list->ele_size);
    memcpy(_CRA_ALIST_VALUE_PTR(list, index), val, list->ele_size);
    list->count++;
    return true;
}

bool
cra_alist_remove_at(CraAList *list, size_t index)
{
    return __cra_alist_pop__(list, index, NULL);
}

bool
cra_alist_pop_at(CraAList *list, size_t index, void *retval)
{
    return __cra_alist_pop__(list, index, retval);
}

size_t
cra_alist_remove_match(CraAList *list, cra_match_fn match, void *arg)
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

bool
cra_alist_set(CraAList *list, size_t index, void *newval)
{
    return __cra_alist_set_and_pop_old__(list, index, newval, NULL);
}

bool
cra_alist_set_and_pop_old(CraAList *list, size_t index, void *newval, void *retoldval)
{
    return __cra_alist_set_and_pop_old__(list, index, newval, retoldval);
}

bool
cra_alist_get(CraAList *list, size_t index, void *retval)
{
    if (index >= list->count)
        return false;
    memcpy(retval, _CRA_ALIST_VALUE_PTR(list, index), list->ele_size);
    return true;
}

bool
cra_alist_get_ptr(CraAList *list, size_t index, void **retvalptr)
{
    if (index >= list->count)
        return false;
    *retvalptr = _CRA_ALIST_VALUE_PTR(list, index);
    return true;
}

void
cra_alist_reverse(CraAList *list)
{
    size_t         middle;
    unsigned char *val1, *val2;

#ifdef CRA_COMPILER_MSVC
    unsigned char *temp = (unsigned char *)cra_malloc(list->ele_size);
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

CraAList *
cra_alist_clone(CraAList *list, cra_deep_copy_val_fn deep_copy_val)
{
    CraAList *ret = cra_alloc(CraAList);
    cra_alist_init_size(ret, list->ele_size, list->count, list->zero_memory);

    ret->count = list->count;
    if (!!deep_copy_val)
    {
        for (size_t i = 0; i < list->count; i++)
        {
            deep_copy_val(_CRA_ALIST_VALUE_PTR(list, i), _CRA_ALIST_VALUE_PTR(ret, i));
        }
    }
    else
    {
        memcpy(ret->array, list->array, list->ele_size * list->count);
    }
    return ret;
}

static size_t
cra_alist_partition(CraAList *list, cra_compare_fn compare, size_t begin, size_t end)
{
    size_t left, right;

#ifdef CRA_COMPILER_MSVC
    unsigned char *temp = (unsigned char *)cra_malloc(list->ele_size);
#else
    unsigned char temp[list->ele_size];
#endif

    left = begin;
    right = end;
    // temp = array[right]
    memcpy(temp, _CRA_ALIST_VALUE_PTR(list, right), list->ele_size);

    while (left < right)
    {
        //                     array[left] <= temp
        while (left < right && compare(_CRA_ALIST_VALUE_PTR(list, left), temp) <= 0)
            left++;
        // array[right] = array[left];
        memcpy(_CRA_ALIST_VALUE_PTR(list, right), _CRA_ALIST_VALUE_PTR(list, left), list->ele_size);

        //                     array[right] >= temp
        while (left < right && compare(_CRA_ALIST_VALUE_PTR(list, right), temp) >= 0)
            right--;
        // array[left] = array[right];
        memcpy(_CRA_ALIST_VALUE_PTR(list, left), _CRA_ALIST_VALUE_PTR(list, right), list->ele_size);
    }
    assert(left == right);
    // array[left] = temp;
    memcpy(_CRA_ALIST_VALUE_PTR(list, left), temp, list->ele_size);

#ifdef CRA_COMPILER_MSVC
    cra_free(temp);
#endif

    return left;
}

static void
cra_alist_quick_sort(CraAList *list, cra_compare_fn compare, size_t begin, size_t end)
{
    size_t middle = cra_alist_partition(list, compare, begin, end);
    if (middle > 0 && middle - 1 > begin)
        cra_alist_quick_sort(list, compare, begin, middle - 1);
    if (middle < end && middle + 1 < end)
        cra_alist_quick_sort(list, compare, middle + 1, end);
}

void
cra_alist_sort(CraAList *list, cra_compare_fn compare)
{
    assert(compare != NULL);
    if (list->count > 1) // count(array) >= 2
        cra_alist_quick_sort(list, compare, 0, list->count - 1);
}

static size_t
cra_alist_binary_seach(CraAList *list, cra_compare_fn compare, void *val)
{
    int    res;
    size_t left, middle, right;

    assert(list->count > 0);

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

bool
cra_alist_add_sort(CraAList *list, cra_compare_fn compare, void *val)
{
    size_t index;

    assert(compare != NULL);

    index = list->count == 0 ? 0 : cra_alist_binary_seach(list, compare, val);
    return cra_alist_insert(list, index, val);
}

// ===============

static void
cra_alist_szer_iter_init(void *obj, void *it, size_t itsize)
{
    CRA_UNUSED_VALUE(itsize);
    assert(sizeof(CraAListIter) <= itsize);
    cra_alist_iter_init((CraAList *)obj, (CraAListIter *)it);
}

static void
cra_alist_dzer_init(void *obj, size_t count, size_t element_size, const void *arg)
{
    assert(obj);
    assert(element_size > 0);
    CRA_UNUSED_VALUE(arg);
    cra_alist_init_size((CraAList *)obj, element_size, count, false);
}

static bool
cra_alist_dzer_append(void *obj, void *val)
{
    return cra_alist_append((CraAList *)obj, val);
}

const CraSzer_i __g_cra_alist_szer_i = {
    .get_count = (size_t (*)(void *))cra_alist_get_count,
    .iter_init = cra_alist_szer_iter_init,
    .iter_next1 = (bool (*)(void *, void **))cra_alist_iter_next,
};

const CraDzer_i __g_cra_alist_dzer_i = {
    .init1 = cra_alist_dzer_init,
    .uninit = (void (*)(void *))cra_alist_uninit,
    .append1 = cra_alist_dzer_append,
};
