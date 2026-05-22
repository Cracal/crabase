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
#include "cra_malloc.h"
#include <time.h>

void
test_new_delete(void)
{
    CraAList *list, list2;

    list = cra_alloc(CraAList);
    assert_always(list);
    assert_always(cra_alist_init_with_size(int, list, 100));
    assert_always(list->count == 0);
    assert_always(list->array != NULL);
    assert_always(list->capacity == 100);
    assert_always(list->itemsize == sizeof(int));

    assert_always(cra_alist_init(double, &list2));
    assert_always(list2.count == 0);
    assert_always(list2.array != NULL);
    assert_always(list2.capacity == CRA_ALIST_DEFAULT_CAPACITY);
    assert_always(list2.itemsize == sizeof(double));

    cra_alist_uninit(list);
    assert_always(list->count == 0);
    assert_always(list->array == NULL);
    assert_always(list->capacity == 0);
    assert_always(list->itemsize == 0);
    cra_dealloc(list);

    cra_alist_uninit(&list2);
    assert_always(list2.count == 0);
    assert_always(list2.array == NULL);
    assert_always(list2.capacity == 0);
    assert_always(list2.itemsize == 0);

    CRA_ALIST_INITIALIZABLE_PARAM_DEF(param, int);
    assert_always(cra_initializable_init(CRA_ALIST_INITIALIZABLE_I, &list2, 16, &param));
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
    CraAList *list;
    int       i, val;

    list = cra_alloc(CraAList);
    assert_always(cra_alist_init(int, list));

    assert_always(cra_alist_append(list, &(int){ 100 }) && cra_alist_get(list, 0, &val) && val == 100);
    cra_alist_clear(list);
    assert_always(cra_alist_prepend(list, &(int){ 200 }) && cra_alist_get(list, 0, &val) && val == 200);
    cra_alist_clear(list);
    assert_always(cra_alist_insert(list, 0, &(int){ 300 }) && cra_alist_get(list, 0, &val) && val == 300);
    cra_alist_clear(list);

    for (i = 0; i < 1000; i++)
        assert_always(cra_alist_append(list, &i));
    assert_always(list->count == 1000);

    assert_always(cra_alist_prepend(list, &(int){ -1 }));
    assert_always(cra_alist_prepend(list, &(int){ -2 }));
    assert_always(cra_alist_insert(list, 4, &(int){ 4000 }));
    assert_always(cra_alist_insert(list, 4, &(int){ 40000 }));
    CraTwoVals vals;
    vals.val1_ref = &(int){ 11 };
    assert_always(cra_appendable_append(CRA_ALIST_APPENDABLE_I, list, &vals));
    assert_always(cra_alist_get(list, list->count - 1, &val) && val == 11);
    vals.val1_ref = &(int){ 12 };
    assert_always(cra_appendable_append(CRA_ALIST_APPENDABLE_I, list, &vals));
    assert_always(cra_alist_get(list, list->count - 1, &val) && val == 12);
    assert_always(list->count == 1006);

    assert_always(!cra_alist_insert(list, 10000, &(int){ 1 }));

    i = -2;
    for (int j = 0; j < list->count; j++, i++)
    {
        memcpy(&val, CRA_ALIST_PVAL(list, j), sizeof(int));
        // printf("%d  ", val);
        if (i == 2)
            i = 40000;
        else if (i == 40001)
            i = 4000;
        else if (i == 4001)
            i = 2;
        else if (i == 1000)
            i = 11;
        assert_always(i == val);
    }
    // printf("\n");

    cra_alist_clear(list);

    for (i = 0; i < 1000; i++)
        assert_always(cra_alist_prepend(list, &i));
    assert_always(list->count == 1000);

    i = 999;
    for (int j = 0; j < list->count; j++, i--)
    {
        memcpy(&val, CRA_ALIST_PVAL(list, j), sizeof(int));
        // printf("%d  ", val);
        assert_always(i == val);
    }
    // printf("\n");

    cra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_remove(void)
{
    CraAList *list = cra_alloc(CraAList);
    assert_always(cra_alist_init_with_size(int, list, 100));

    assert_always(!cra_alist_remove_back(list));
    assert_always(!cra_alist_remove_front(list));
    assert_always(!cra_alist_remove_at(list, 100));

    for (int i = 0; i < 100; i++)
        assert_always(cra_alist_append(list, &i));
    assert_always(list->count == 100);

    for (int i = 0; i < 100; i++)
        assert_always(cra_alist_remove_back(list));
    assert_always(list->count == 0);

    for (int i = 1; i <= 100; i++)
        assert_always(cra_alist_append(list, &i));
    assert_always(list->count == 100);

    int val, val2;
    assert_always(cra_alist_pop_at(list, 70 - 1, &val) && val == 70);
    assert_always(cra_alist_pop_at(list, 60 - 1, &val) && val == 60);
    cra_alist_get(list, 38 - 1, &val);
    assert_always(cra_alist_remove_at(list, 38 - 1) && cra_alist_get(list, 38 - 1, &val2) && val != val2);
    cra_alist_get(list, 34 - 1, &val);
    assert_always(cra_alist_remove_at(list, 34 - 1) && cra_alist_get(list, 34 - 1, &val2) && val != val2);
    assert_always(cra_alist_pop_front(list, &val) && val == 1);
    assert_always(cra_alist_pop_front(list, &val) && val == 2);
    cra_alist_get(list, 0, &val);
    assert_always(cra_alist_remove_front(list) && cra_alist_get(list, 0, &val2) && val != val2);
    cra_alist_get(list, 0, &val);
    assert_always(cra_alist_remove_front(list) && cra_alist_get(list, 0, &val2) && val != val2);
    assert_always(cra_alist_pop_back(list, &val) && val == 100);
    assert_always(cra_alist_pop_back(list, &val) && val == 99);
    cra_alist_get(list, list->count - 1, &val);
    assert_always(cra_alist_remove_back(list) && cra_alist_get(list, list->count - 1, &val2) && val != val2);
    cra_alist_get(list, list->count - 1, &val);
    assert_always(cra_alist_remove_back(list) && cra_alist_get(list, list->count - 1, &val2) && val != val2);
    assert_always(list->count == 88);

    for (size_t i = 0; i < list->count; i++)
    {
        memcpy(&val, CRA_ALIST_PVAL(list, i), sizeof(int));
        printf("%d  ", val);
    }
    printf("\n");

    cra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_set(void)
{
    int       val, i;
    CraAList *list = cra_alloc(CraAList);
    assert_always(cra_alist_init(int, list));

    for (i = 0; i < 1000; i++)
        assert_always(cra_alist_append(list, &i));
    assert_always(list->count == 1000);

    assert_always(cra_alist_set(list, 3, &(int){ 3000 }));
    assert_always(cra_alist_get(list, 3, &val) && val == 3000);
    assert_always(cra_alist_get_and_set(list, 6, &(int){ 6000 }, &val) && val == 6);
    assert_always(cra_alist_get(list, 6, &val) && val == 6000);
    assert_always(!cra_alist_set(list, 1000, &(int){ 10000 }));
    assert_always(!cra_alist_get_and_set(list, 1000, &(int){ 10000 }, &val));

    i = 0;
    for (int j = 0; j < (int)list->count; j++, i++)
    {
        memcpy(&val, CRA_ALIST_PVAL(list, j), sizeof(int));
        // printf("%d  ", val);
        if (i == 3)
            i = 3000;
        else if (i == 3001)
            i = 4;
        else if (i == 6)
            i = 6000;
        else if (i == 6001)
            i = 7;
        assert_always(i == val);
    }
    // printf("\n");

    cra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_get(void)
{
    CraAList list;
    int      val, *pval;

    assert_always(cra_alist_init(int, &list));
    for (int i = 0; i < 1000; i++)
        assert_always(cra_alist_prepend(&list, &i));

    size_t i = 0;
    for (i = 0; i < list.count; i++)
    {
        cra_alist_get(&list, i, &val);
        pval = cra_alist_get_ref(&list, i);
        assert_always(val == *pval);
        assert_always(val == *(int *)CRA_ALIST_PVAL(&list, i)); // UB?
        // printf("%d  ", val);
    }
    // printf("\n");

    cra_alist_uninit(&list);
}

void
test_reverse(void)
{
    CraAList list, list2;
    int      val, val2, i;

    cra_alist_init_with_size(int, &list, 1000);
    cra_alist_init_with_size(int, &list2, 1000);

    for (i = 0; i < 1000; i++)
    {
        cra_alist_append(&list, &i);
        cra_alist_prepend(&list2, &i);
    }

    cra_alist_reverse(&list);

    i = 0;
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, vals)
    {
        assert_always(cra_alist_get(&list2, i, &val2));
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(val == val2);
        i++;
    }

    cra_alist_uninit(&list);
    cra_alist_uninit(&list2);
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
    int      i, val;
    CraAList list;

    assert_always(cra_alist_init(int, &list));

    assert_always(cra_alist_append(&list, &(int){ 8 }));
    assert_always(cra_alist_append(&list, &(int){ 7 }));
    assert_always(cra_alist_append(&list, &(int){ 1 }));
    assert_always(cra_alist_append(&list, &(int){ 3 }));
    assert_always(cra_alist_append(&list, &(int){ 0 }));
    assert_always(cra_alist_append(&list, &(int){ 9 }));
    assert_always(cra_alist_append(&list, &(int){ 6 }));
    assert_always(cra_alist_append(&list, &(int){ 4 }));
    assert_always(cra_alist_append(&list, &(int){ 5 }));
    assert_always(cra_alist_append(&list, &(int){ 2 }));

    printf("before sort     : ");
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, list.itemsize);
        printf("%d  ", val);
    }
    printf("\n");

    assert_always(cra_alist_sort(&list, comare_int1));
    printf("after sort(ASC) : ");
    i = 0;
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, list.itemsize);
        assert_always(i == val);
        printf("%d  ", val);
        i++;
    }
    printf("\n");

    assert_always(cra_alist_sort(&list, comare_int2));
    printf("after sort(DESC): ");
    i = 9;
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, list.itemsize);
        assert_always(i == val);
        printf("%d  ", val);
        i--;
    }
    printf("\n");

    // add sort

    cra_alist_clear(&list);
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 8 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 7 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 1 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 3 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 0 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 9 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 6 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 4 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 5 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 2 }));
    printf("add sort(ASC)   : ");
    i = 0;
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, list.itemsize);
        assert_always(i == val);
        printf("%d  ", val);
        i++;
    }
    printf("\n");

    cra_alist_clear(&list);
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 8 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 7 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 1 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 3 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 0 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 9 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 6 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 4 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 5 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 2 }));
    printf("add sort(DESC)  : ");
    i = 9;
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, list.itemsize);
        assert_always(i == val);
        printf("%d  ", val);
        i--;
    }
    printf("\n");

    cra_alist_clear(&list);

    srand((unsigned int)time(NULL));
    for (i = 0; i < 10000; i++)
        cra_alist_append(&list, &(int){ rand() });

    assert_always(cra_alist_sort(&list, comare_int1));

    i = 0;
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, list.itemsize);
        assert_always(i <= val);
        i = val;
    }

    cra_alist_uninit(&list);
}

void
test_foreach(void)
{
    int       i, val;
    CraAList *list = cra_alloc(CraAList);
    assert_always(cra_alist_init(int, list));

    // foreach(empty)
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, list, vals) assert_always(false);
    CRA_FOREACH_REVERSE(CRA_ALIST_ITERABLE_I, list, vals) assert_always(false);

    for (i = 0; i < 10; i++)
        cra_alist_append(list, &i);
    assert_always(list->count == 10);

    i = 0;
    printf("foreach        : ");
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, list, vals)
    {
        memcpy(&val, vals.val1_ref, list->itemsize);
        assert_always(i == val);
        printf("%d  ", val);
        i++;
    }
    printf("\n");

    i = 9;
    printf("foreach reverse: ");
    CRA_FOREACH_REVERSE(CRA_ALIST_ITERABLE_I, list, vals)
    {
        memcpy(&val, vals.val1_ref, list->itemsize);
        assert_always(i == val);
        printf("%d  ", val);
        i--;
    }
    printf("\n");

    cra_alist_clear(list);

    // foreach(empty)
    CRA_FOREACH(CRA_ALIST_ITERABLE_I, list, vals) assert_always(false);
    CRA_FOREACH_REVERSE(CRA_ALIST_ITERABLE_I, list, vals) assert_always(false);

    cra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_test(void)
{
    CraAList *list = cra_alloc(CraAList);
    assert_always(cra_alist_init(int, list));

    int i, j, n, v;
    srand((unsigned int)time(NULL));
    for (i = 0; i < 10; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            cra_alist_append(list, &j);

        n = list->count == 0 ? 0 : (rand() + 1) % list->count;
        for (j = 0; j < n; j++)
        {
            cra_alist_pop_front(list, &v);
            assert_always(v == j);
        }

        for (; cra_alist_pop_front(list, &v); j++)
            assert_always(v == j);
    }
    assert_always(list->count == 0);

    for (i = 0; i < 100; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            cra_alist_prepend(list, &j);

        n = list->count == 0 ? 0 : (rand() + 1) % list->count;
        for (j = 0; j < n; j++)
        {
            cra_alist_pop_back(list, &v);
            assert_always(v == j);
        }

        for (; cra_alist_pop_back(list, &v); j++)
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
            cra_alist_insert(list, idx, &j);
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
        CRA_FOREACH(CRA_ALIST_ITERABLE_I, list, vals)
        {
            memcpy(&v, vals.val1_ref, list->itemsize);
            assert_always(v == check[j++]);
        }

        while (true)
        {
            idx = list->count == 0 ? 0 : (rand() % list->count);
            if (!cra_alist_pop_at(list, idx, &j))
                break;

            assert_always(check[idx] == j);
            memmove(check + idx, check + idx + 1, (last_idx - idx + 1) * sizeof(int));
            last_idx--;
        }
    }
    assert_always(list->count == 0);

    cra_alist_uninit(list);
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
    test_reverse();
    test_sort();
    test_foreach();
    test_test();

    cra_memory_leak_report();
    return 0;
}
