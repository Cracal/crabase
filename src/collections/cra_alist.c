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
#include "cra_malloc.h"

// 动态数组容量扩展策略:
// 小于1024时，每次扩展一倍, 否则每次扩展1.5倍
#define CRA_ALIST_EXPEND(_old_capacity)                                                          \
    ((_old_capacity) < 1024 ? ((_old_capacity) << 1) : (_old_capacity) + ((_old_capacity) >> 1))

bool(cra_alist_init_with_size)(CraAList *list, size_t itemsize, size_t init_capacity)
{
    assert(list);
    assert(itemsize > 0);

    if (init_capacity < CRA_ALIST_DEFAULT_CAPACITY)
        init_capacity = CRA_ALIST_DEFAULT_CAPACITY;

    list->array = cra_malloc(init_capacity * itemsize);
    if (!list->array)
        return false;

    list->count = 0;
    list->capacity = init_capacity;
    list->itemsize = itemsize;

    return true;
}

void(cra_alist_uninit)(CraAList *list)
{
    assert(list);
    assert(list->array);

    cra_free(list->array);
    bzero(list, sizeof(*list));
}

static inline bool
cra_alist_resize(CraAList *list, size_t new_capacity)
{
    unsigned char *new_array;
    if (!(new_array = cra_realloc(list->array, new_capacity * list->itemsize)))
        return false;
    list->array = new_array;
    list->capacity = new_capacity;
    return true;
}

bool(cra_alist_ensure)(CraAList *list, size_t nspare, bool shrink2fit)
{
    size_t spare;
    size_t capacity;

    assert(list);
    assert(list->array);

    spare = list->capacity - list->count;
    if (shrink2fit && spare != nspare)
    {
        capacity = list->count + nspare;
        if (capacity == 0)
            capacity = CRA_ALIST_DEFAULT_CAPACITY;
        goto resize;
    }
    else if (spare < nspare)
    {
        capacity = CRA_ALIST_EXPEND(list->capacity);
        if (capacity < list->count + nspare)
            capacity = list->count + nspare;

    resize:
        return cra_alist_resize(list, capacity);
    }
    return true;
}

bool(cra_alist_insert)(CraAList *list, size_t index, void *val)
{
    size_t nmoving;

    assert(list);
    assert(list->array);
    assert(list->itemsize > 0);

    if (index > list->count)
        return false;

    if (list->count == list->capacity)
    {
        if (!cra_alist_resize(list, CRA_ALIST_EXPEND(list->capacity)))
            return false;
    }

    nmoving = list->count - index;
    if (nmoving > 0)
        memmove(CRA_ALIST_PVAL(list, index + 1), CRA_ALIST_PVAL(list, index), nmoving * list->itemsize);
    memcpy(CRA_ALIST_PVAL(list, index), val, list->itemsize);
    ++list->count;
    return true;
}

bool(cra_alist_pop_at)(CraAList *list, size_t index, void *retval)
{
    size_t nmoving;

    assert(list);
    assert(list->array);
    assert(list->itemsize > 0);

    if (index >= list->count)
        return false;

    if (retval)
        memcpy(retval, CRA_ALIST_PVAL(list, index), list->itemsize);

    nmoving = list->count - index - 1;
    if (nmoving > 0)
        memmove(CRA_ALIST_PVAL(list, index), CRA_ALIST_PVAL(list, index + 1), nmoving * list->itemsize);
    --list->count;
    return true;
}

static bool
cra_alist_partition(CraAList *list, cra_compare_fn compare, size_t begin, size_t end, size_t *middle, char *temp)
{
    size_t left, right;
    char  *array;

    left = begin;
    right = end;
    array = (char *)list->array;
    // temp = array[right]
    memcpy(temp, array + right * list->itemsize, list->itemsize);

    while (left < right)
    {
        //                     array[left] <= temp
        while (left < right && compare(array + left * list->itemsize, temp) <= 0)
            left++;
        // array[right] = array[left];
        memcpy(array + right * list->itemsize, array + left * list->itemsize, list->itemsize);

        //                     array[right] >= temp
        while (left < right && compare(array + right * list->itemsize, temp) >= 0)
            right--;
        // array[left] = array[right];
        memcpy(array + left * list->itemsize, array + right * list->itemsize, list->itemsize);
    }
    assert(left == right);
    // array[left] = temp;
    memcpy(array + left * list->itemsize, temp, list->itemsize);

    *middle = left;
    return true;
}

static bool
cra_alist_quick_sort(CraAList *list, cra_compare_fn compare, size_t begin, size_t end, char *temp)
{
    size_t middle;

    if (!cra_alist_partition(list, compare, begin, end, &middle, temp))
        return false;
    if (middle > 0 && middle - 1 > begin)
        cra_alist_quick_sort(list, compare, begin, middle - 1, temp);
    if (middle < end && middle + 1 < end)
        cra_alist_quick_sort(list, compare, middle + 1, end, temp);
    return true;
}

bool(cra_alist_sort)(CraAList *list, cra_compare_fn compare)
{
    assert(list);
    assert(compare);
    assert(list->array);
    if (list->count > 1) // count(array) >= 2
    {
#ifdef CRA_COMPILER_MSVC
        char *temp = cra_malloc(list->itemsize);
        if (!temp)
            return false;
#else
        char temp[list->itemsize];
#endif
        bool ret = cra_alist_quick_sort(list, compare, 0, list->count - 1, temp);
#ifdef CRA_COMPILER_MSVC
        cra_free(temp);
#endif
        return ret;
    }
    return true;
}

static size_t
cra_alist_binary_seach(CraAList *list, cra_compare_fn compare, void *val)
{
    int    res;
    size_t left, mid, right;

    assert(list->count > 0);

    left = 0;
    right = list->count - 1;
    while (left <= right)
    {
        mid = left + ((right - left) >> 1);
        res = compare((char *)list->array + mid * list->itemsize, val);
        if (res < 0)
        {
            left = mid + 1; // val in [mid + 1, right]
        }
        // else if (res > 0)
        // {
        //     if (mid == 0)
        //         break;
        //     right = mid - 1; // val in [left, mid - 1]
        // }
        // else
        // {
        //     if (mid == 0)
        //         break;
        //     right = mid - 1; // first (array[mid] < val) in [left, mid - 1]
        // }
        else
        {
            if (mid == 0)
                break;
            right = mid - 1;
        }
    }
    return left;
}

bool(cra_alist_add_sort)(CraAList *list, cra_compare_fn compare, void *val)
{
    size_t index;

    assert(val);
    assert(list);
    assert(compare);
    assert(list->array);

    index = list->count == 0 ? 0 : cra_alist_binary_seach(list, compare, val);
    return (cra_alist_insert)(list, index, val);
}

// ====================================== interfaces ======================================

// initializable

static CRA_INITIALIZABLE_INIT_FN(cra_alist_initializable_init)
{
    assert(obj);
    assert(params);
    CraAListInitializableParam *param = (CraAListInitializableParam *)params;
    return (cra_alist_init_with_size)((CraAList *)obj, param->itemsize, param->init_capacity);
}

CRA_INITIALIZABLE_DEF(cra_g_alist_initializable_i) = {
    .init = cra_alist_initializable_init,
    .uninit = (CRA_INITIALIZABLE_UNINIT_FN((*)))cra_alist_uninit,
};

// appendable

static CRA_APPENDABLE_APPEND_FN(cra_alist_appendable_append)
{
    assert(obj);
    assert(vals);
    assert(vals->val1_ref);
    CraAList *list = (CraAList *)obj;
    return (cra_alist_insert)(list, list->count, vals->val1_ref);
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
    if (list->count > 0)
    {
        it->idx = reverse ? list->count : 0;
        it->obj = obj;
        return true;
    }
    return false;
}

static CRA_ITERABLE_NEXT_FN(cra_alist_iterable_next)
{
    assert(it);
    assert(vals);
    assert(it->obj);

    CraAList *list = (CraAList *)it->obj;
    if (it->idx < list->count)
    {
        vals->val1_ref = CRA_ALIST_PVAL(list, it->idx++);
        return true;
    }
    return false;
}

static CRA_ITERABLE_PREV_FN(cra_alist_iterable_prev)
{
    assert(it);
    assert(vals);
    assert(it->obj);

    CraAList *list = (CraAList *)it->obj;
    if (it->idx > 0)
    {
        vals->val1_ref = CRA_ALIST_PVAL(list, --it->idx);
        return true;
    }
    return false;
}

CRA_ITERABLE_DEF(cra_g_alist_iterable_i) = {
    .init = cra_alist_iterable_init,
    .next = cra_alist_iterable_next,
    .prev = cra_alist_iterable_prev,
};
