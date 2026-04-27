/**
 * @file test-list.c
 * @author Cracal
 * @brief test array list
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "collections/cra_alist.h"
#include "cra_assert.h"
#include "cra_malloc.h"
#include <time.h>

CRA_ALIST_DEF(int);

void
test_new_delete(void)
{
    CRA_ALIST(int) * list, list2;

    list = cra_alloc(CRA_ALIST(int));
    assert_always(list);
    assert_always(newcra_alist_init_with_size(list, 4));
    assert_always(list->count == 0);
    assert_always(list->array != NULL);
    assert_always(list->capacity == 4);
    assert_always(list->itemsize == sizeof(int));

    assert_always(newcra_alist_init(&list2));
    assert_always(list2.count == 0);
    assert_always(list2.array != NULL);
    assert_always(list2.capacity == CRA_ALIST_DEFAULT_CAPACITY);
    assert_always(list2.itemsize == sizeof(int));

    newcra_alist_uninit(list);
    assert_always(list->count == 0);
    assert_always(list->array == NULL);
    assert_always(list->capacity == 0);
    assert_always(list->itemsize == 0);
    cra_dealloc(list);

    newcra_alist_uninit(&list2);
    assert_always(list2.count == 0);
    assert_always(list2.array == NULL);
    assert_always(list2.capacity == 0);
    assert_always(list2.itemsize == 0);

    CraAListInitializableParam param = CRA_ALIST_INITIALIZABLE_PARAM_INIT(CRA_ALIST(int), 16);
    assert_always(cra_initializable_init(CRA_ALIST_INITIALIZABLE_I, &list2, &param));
    assert_always(list2.count == 0);
    assert_always(list2.array != NULL);
    assert_always(list2.capacity == 16);
    assert_always(list2.itemsize == sizeof(int));

    cra_initializable_uninit(CRA_ALIST_INITIALIZABLE_I, &list2);
    assert_always(list2.count == 0);
    assert_always(list2.array == NULL);
    assert_always(list2.capacity == 0);
    assert_always(list2.itemsize == 0);
}

void
test_add(void)
{
    int i;
    CRA_ALIST(int) * list;

    list = cra_alloc(CRA_ALIST(int));
    assert_always(newcra_alist_init(list));

    assert_always(newcra_alist_append(list, 100) && (i = newcra_alist_get(list, 0, -1), i == 100));
    newcra_alist_clear(list);
    assert_always(newcra_alist_prepend(list, 200) && (i = newcra_alist_get(list, 0, -1), i == 200));
    newcra_alist_clear(list);
    assert_always(newcra_alist_insert(list, 0, 300) && (i = newcra_alist_get(list, 0, -1), i == 300));
    newcra_alist_clear(list);

    for (i = 0; i < 1000; i++)
        assert_always(newcra_alist_append(list, i));
    assert_always(list->count == 1000);

    assert_always(newcra_alist_prepend(list, -1));
    assert_always(newcra_alist_prepend(list, -2));
    assert_always(newcra_alist_insert(list, 4, 4000));
    assert_always(newcra_alist_insert(list, 4, 40000));
    CraTwoVals vals;
    vals.val1_ref = &(int){ 11 };
    assert_always(cra_appendable_append(CRA_ALIST_APPENDABLE_I, list, &vals));
    assert_always((i = list->array[list->count - 1], i == 11));
    vals.val1_ref = &(int){ 12 };
    assert_always(cra_appendable_append(CRA_ALIST_APPENDABLE_I, list, &vals));
    assert_always((i = list->array[list->count - 1], i == 12));
    assert_always(list->count == 1006);

    assert_always(!newcra_alist_insert(list, 10000, 1));

    i = -2;
    for (int j = 0; j < list->count; j++, i++)
    {
        // printf("%d  ", list->array[j]);
        if (i == 2)
            i = 40000;
        else if (i == 40001)
            i = 4000;
        else if (i == 4001)
            i = 2;
        else if (i == 1000)
            i = 11;
        assert_always(i == list->array[j]);
    }
    // printf("\n");

    newcra_alist_clear(list);

    for (i = 0; i < 1000; i++)
        assert_always(newcra_alist_prepend(list, i));

    i = 999;
    for (int j = 0; j < list->count; j++, i--)
    {
        // printf("%d  ", list->array[j]);
        assert_always(i == list->array[j]);
    }
    // printf("\n");

    newcra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_remove(void)
{
    CRA_ALIST(int) *list = cra_alloc(CRA_ALIST(int));
    assert_always(newcra_alist_init_with_size(list, 100));

    assert_always(!newcra_alist_remove_back(list));
    assert_always(!newcra_alist_remove_front(list));
    assert_always(!newcra_alist_remove_at(list, 100));

    for (int i = 0; i < 100; i++)
        assert_always(newcra_alist_append(list, i));
    assert_always(list->count == 100);

    for (int i = 0; i < 100; i++)
        assert_always(newcra_alist_remove_back(list));
    assert_always(list->count == 0);

    for (int i = 1; i <= 100; i++)
        assert_always(newcra_alist_append(list, i));
    assert_always(list->count == 100);

    int val, val2;
    assert_always(newcra_alist_pop_at(list, 70 - 1, &val) && val == 70);
    assert_always(newcra_alist_pop_at(list, 60 - 1, &val) && val == 60);
    val = newcra_alist_get(list, 38 - 1, -1);
    assert_always(newcra_alist_remove_at(list, 38 - 1) && (val2 = newcra_alist_get(list, 38 - 1, -1), val != val2));
    val = newcra_alist_get(list, 34 - 1, -1);
    assert_always(newcra_alist_remove_at(list, 34 - 1) && (val2 = newcra_alist_get(list, 34 - 1, -1), val != val2));
    assert_always(newcra_alist_pop_front(list, &val) && val == 1);
    assert_always(newcra_alist_pop_front(list, &val) && val == 2);
    val = newcra_alist_get(list, 0, -1);
    assert_always(newcra_alist_remove_front(list) && (val2 = newcra_alist_get(list, 0, -1), val != val2));
    val = newcra_alist_get(list, 0, -1);
    assert_always(newcra_alist_remove_front(list) && (val2 = newcra_alist_get(list, 0, -1), val != val2));
    assert_always(newcra_alist_pop_back(list, &val) && val == 100);
    assert_always(newcra_alist_pop_back(list, &val) && val == 99);
    val = newcra_alist_get(list, list->count - 1, -1);
    assert_always(newcra_alist_remove_back(list) && (val2 = newcra_alist_get(list, list->count - 1, -1), val != val2));
    val = newcra_alist_get(list, list->count - 1, -1);
    assert_always(newcra_alist_remove_back(list) && (val2 = newcra_alist_get(list, list->count - 1, -1), val != val2));

    for (size_t i = 0; i < list->count; i++)
        printf("%d  ", list->array[i]);
    printf("\n");

    newcra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_set(void)
{
    int val, i;
    CRA_ALIST(int) *list = cra_alloc(CRA_ALIST(int));
    assert_always(newcra_alist_init(list));

    for (i = 0; i < 1000; i++)
        assert_always(newcra_alist_append(list, i));
    assert_always(list->count == 1000);

    assert_always(newcra_alist_set(list, 3, 3000));
    assert_always((val = newcra_alist_get(list, 3, -1), val == 3000));
    assert_always(newcra_alist_get_and_set(list, 6, 6000, &val) && val == 6);
    assert_always((val = newcra_alist_get(list, 6, -1), val == 6000));
    assert_always(!newcra_alist_set(list, 1000, 10000));
    assert_always(!newcra_alist_get_and_set(list, 1000, 10000, &val));

    i = 0;
    for (int j = 0; j < (int)list->count; j++, i++)
    {
        // printf("%d  ", list->array[j]);
        if (i == 3)
            i = 3000;
        else if (i == 3001)
            i = 4;
        else if (i == 6)
            i = 6000;
        else if (i == 6001)
            i = 7;
        assert_always(i == list->array[j]);
    }
    // printf("\n");

    newcra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_get(void)
{
    CRA_ALIST(int) list;
    int val, *pval;

    assert_always(newcra_alist_init(&list));
    for (int i = 0; i < 1000; i++)
        assert_always(newcra_alist_prepend(&list, i));

    size_t i = 0;
    for (i = 0; i < list.count; i++)
    {
        val = newcra_alist_get(&list, i, -1);
        pval = newcra_alist_get_ref(&list, i);
        assert_always(val == *pval);
        assert_always(val == list.array[i]);
        // printf("%d  ", val);
    }
    // printf("\n");

    newcra_alist_uninit(&list);
}

static int
comare_int1(const int *a, const int *b)
{
    return *a - *b;
}

static int
comare_int2(const int *a, const int *b)
{
    return *b - *a;
}

void
test_sort(void)
{
    int i;
    CRA_ALIST(int) list;

    assert_always(newcra_alist_init(&list));

    assert_always(newcra_alist_append(&list, 8));
    assert_always(newcra_alist_append(&list, 7));
    assert_always(newcra_alist_append(&list, 1));
    assert_always(newcra_alist_append(&list, 3));
    assert_always(newcra_alist_append(&list, 0));
    assert_always(newcra_alist_append(&list, 9));
    assert_always(newcra_alist_append(&list, 6));
    assert_always(newcra_alist_append(&list, 4));
    assert_always(newcra_alist_append(&list, 5));
    assert_always(newcra_alist_append(&list, 2));

    printf("before sort     : ");
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, val)
    {
        printf("%d  ", *(int *)val.val1_ref);
    }
    printf("\n");

    assert_always(newcra_alist_sort(&list, comare_int1));
    printf("after sort(ASC) : ");
    i = 0;
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, val)
    {
        printf("%d  ", *(int *)val.val1_ref);
        assert_always(i == *(int *)val.val1_ref);
        i++;
    }
    printf("\n");

    assert_always(newcra_alist_sort(&list, comare_int2));
    printf("after sort(DESC): ");
    i = 9;
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, val)
    {
        printf("%d  ", *(int *)val.val1_ref);
        assert_always(i == *(int *)val.val1_ref);
        i--;
    }
    printf("\n");

    // add sort

    newcra_alist_clear(&list);
    assert_always(newcra_alist_add_sort(&list, comare_int1, 8));
    assert_always(newcra_alist_add_sort(&list, comare_int1, 7));
    assert_always(newcra_alist_add_sort(&list, comare_int1, 1));
    assert_always(newcra_alist_add_sort(&list, comare_int1, 3));
    assert_always(newcra_alist_add_sort(&list, comare_int1, 0));
    assert_always(newcra_alist_add_sort(&list, comare_int1, 9));
    assert_always(newcra_alist_add_sort(&list, comare_int1, 6));
    assert_always(newcra_alist_add_sort(&list, comare_int1, 4));
    assert_always(newcra_alist_add_sort(&list, comare_int1, 5));
    assert_always(newcra_alist_add_sort(&list, comare_int1, 2));
    printf("add sort(ASC) : ");
    i = 0;
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, val)
    {
        printf("%d  ", *(int *)val.val1_ref);
        assert_always(i == *(int *)val.val1_ref);
        i++;
    }
    printf("\n");

    newcra_alist_clear(&list);
    assert_always(newcra_alist_add_sort(&list, comare_int2, 8));
    assert_always(newcra_alist_add_sort(&list, comare_int2, 7));
    assert_always(newcra_alist_add_sort(&list, comare_int2, 1));
    assert_always(newcra_alist_add_sort(&list, comare_int2, 3));
    assert_always(newcra_alist_add_sort(&list, comare_int2, 0));
    assert_always(newcra_alist_add_sort(&list, comare_int2, 9));
    assert_always(newcra_alist_add_sort(&list, comare_int2, 6));
    assert_always(newcra_alist_add_sort(&list, comare_int2, 4));
    assert_always(newcra_alist_add_sort(&list, comare_int2, 5));
    assert_always(newcra_alist_add_sort(&list, comare_int2, 2));
    printf("add sort(DESC): ");
    i = 9;
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, val)
    {
        printf("%d  ", *(int *)val.val1_ref);
        assert_always(i == *(int *)val.val1_ref);
        i--;
    }
    printf("\n");

    newcra_alist_clear(&list);

    srand((unsigned int)time(NULL));
    for (i = 0; i < 10000; i++)
        newcra_alist_append(&list, rand());

    assert_always(newcra_alist_sort(&list, comare_int1));

    i = 0;
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, val)
    {
        assert_always(i <= *(int *)val.val1_ref);
        i = *(int *)val.val1_ref;
    }

    newcra_alist_uninit(&list);
}

void
test_foreach(void)
{
    CRA_ALIST(int) *list = cra_alloc(CRA_ALIST(int));
    assert_always(newcra_alist_init(list));

    printf("foreach(    empty): ");
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, list, val)
    {
        printf("%d ", *(int *)val.val1_ref);
        assert_always(false);
    }
    printf("\n");

    for (int i = 0; i < 10; i++)
        newcra_alist_append(list, i);
    assert_always(list->count == 10);

    printf("foreach(not empty): ");
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, list, val)
    {
        printf("%d ", *(int *)val.val1_ref);
    }
    printf("\n");

    newcra_alist_clear(list);

    printf("foreach(    empty): ");
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, list, val)
    {
        printf("%d ", *(int *)val.val1_ref);
        assert_always(false);
    }
    printf("\n");

    newcra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_test(void)
{
    CRA_ALIST(int) *list = cra_alloc(CRA_ALIST(int));
    assert_always(newcra_alist_init(list));

    int i, j, n, v;
    srand((unsigned int)time(NULL));
    for (i = 0; i < 10; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            newcra_alist_append(list, j);

        n = (rand() + 1) % list->count;
        for (j = 0; j < n; j++)
        {
            newcra_alist_pop_front(list, &v);
            assert_always(v == j);
        }

        for (; newcra_alist_pop_front(list, &v); j++)
            assert_always(v == j);
    }
    assert_always(list->count == 0);

    for (i = 0; i < 100; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            newcra_alist_prepend(list, j);

        n = (rand() + 1) % list->count;
        for (j = 0; j < n; j++)
        {
            newcra_alist_pop_back(list, &v);
            assert_always(v == j);
        }

        for (; newcra_alist_pop_back(list, &v); j++)
            assert_always(v == j);
    }
    assert_always(list->count == 0);

    int idx, last_idx;
    int check[10000];
    bzero(check, sizeof(check));
    for (i = 0; i < 100; i++)
    {
        last_idx = 0;
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
        {
            idx = list->count == 0 ? 0 : (rand() % list->count);
            newcra_alist_insert(list, idx, j);
            if (idx > last_idx)
            {
                last_idx = idx;
            }
            else
            {
                memmove(check + idx + 1, check + idx, (last_idx - idx) * sizeof(int));
                if (list->count > 1)
                    last_idx++;
            }
            check[idx] = j;
        }
        j = 0;
        CRA_FOREACH(CRA_ALIST_ITERABLE_I, list, val)
        {
            assert_always(*(int *)val.val1_ref == check[j++]);
        }

        while (true)
        {
            idx = list->count == 0 ? 0 : (rand() % list->count);
            if (!newcra_alist_pop_at(list, idx, &j))
                break;

            assert_always(check[idx] == j);
            memmove(check + idx, check + idx + 1, (last_idx - idx + 1) * sizeof(int));
            last_idx--;
        }
    }
    assert_always(list->count == 0);

    newcra_alist_uninit(list);
    cra_dealloc(list);
}

int
main(void)
{
    test_new_delete();
    test_add();
    test_remove();
    test_set();
    test_get();
    test_sort();
    test_foreach();
    test_test();

    cra_memory_leak_report();
    return 0;
}
